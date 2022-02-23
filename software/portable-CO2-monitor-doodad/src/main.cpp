#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <string>
#include <sstream>

#define DEBUG_PRINT(X) Serial.print(X)
#define DEBUG_PRINT_2(X, Y) Serial.print(X, Y)

SoftwareSerial co2Sensor(7, 8);

#define ENABLE_GxEPD2_GFX 0
#include <GxEPD2.h>
#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>
#include <GxEPD2_7C.h>
#include <PlusJakartaSans_Medium26pt7b.h>
#include <PlusJakartaSans_Medium6pt7b.h>

GxEPD2_BW<GxEPD2_290, GxEPD2_290::HEIGHT> display(GxEPD2_290(/*CS=D8*/ 10, /*DC=D3*/ 4, /*RST=D4*/ 3, /*BUSY=D2*/ 2)); // GDEM029T94

uint32_t previous4Sec = 0;
uint32_t previous2Min = 0;
uint16_t historyPer4Sec[296];
uint16_t historyPer4SecTapePosition = 295;
uint16_t historyPer2Min[296];
uint16_t historyPer2MinTapePosition = 295;

bool firstDataEntry = true;
bool show2Min = true;

void setup()
{
  Serial.begin(9600);
  co2Sensor.begin(9600);
  display.init(115200);
  delay(100);

  display.setRotation(1);
  display.setFont(&PlusJakartaSans_Medium26pt7b);
  display.setTextColor(GxEPD_BLACK);
  display.setFullWindow();
  display.firstPage();
  display.fillScreen(GxEPD_WHITE);

  display.fillScreen(GxEPD_WHITE);
  display.setCursor(50, 100);
  std::stringstream ss;
  ss << "Welcome";

  int16_t tbx, tby;
  uint16_t tbw, tbh;
  display.getTextBounds(ss.str().c_str(), 0, 0, &tbx, &tby, &tbw, &tbh);
  uint16_t utx = ((display.width() - tbw) / 2) - tbx;
  uint16_t uty = ((display.height() * 0.5) - tbh / 2) - tby;
  do
  {
    display.setCursor(utx, uty);
    display.print(ss.str().c_str());
    display.nextPage();
  } while (display.nextPage());

  for (int i = 0; i < 296; i++)
  {
    historyPer4Sec[i] = 400;
    historyPer2Min[i] = 400;
  }

  delay(1000);

  display.clearScreen(GxEPD_WHITE);
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.nextPage();
  } while (display.nextPage());

  display.setPartialWindow(0, 0, display.width(), display.height());
}

void loop()
{
  uint16_t co2SensorStatus = -1;
  uint16_t co2SensorPpm = -1;

  if (co2Sensor.available())
  {
    while (co2Sensor.available())
    {
      co2Sensor.read();
    }
  }

  if (millis() - previous4Sec > 4000)
  {
    previous4Sec += 4000;

    DEBUG_PRINT("Reading from CO2 sensor\n");
    co2Sensor.listen();
    co2Sensor.write(0xFE);
    co2Sensor.write(0x04);
    co2Sensor.write((uint8_t)0x00);
    co2Sensor.write((uint8_t)0x00);
    co2Sensor.write((uint8_t)0x00);
    co2Sensor.write(0x04);
    co2Sensor.write(0xE5);
    co2Sensor.write(0xC6);
    delay(100);

    // There's a bug in co2Sensor.available(), it won't return the right number of bytes the first time it's called :-/
    if (co2Sensor.available() && co2Sensor.available() >= 13)
    {
      uint8_t sensorPacket[13];
      for (int i = 0; i < 13; ++i)
      {
        sensorPacket[i] = co2Sensor.read();
      }
      if (sensorPacket[0] == 0xFE && sensorPacket[1] == 0x04 && sensorPacket[2] == 0x08)
      {
        uint16_t sensorStatus = (sensorPacket[3] << 8) | sensorPacket[4];
        uint16_t co2Ppm = (sensorPacket[9] << 8) | sensorPacket[10];
        uint16_t checksum = (sensorPacket[11] << 8) | sensorPacket[12];
        DEBUG_PRINT("CO2 sensor status: ");
        DEBUG_PRINT_2(sensorStatus, HEX);
        DEBUG_PRINT("\nCO2 ppm: ");
        DEBUG_PRINT_2(co2Ppm, DEC);
        DEBUG_PRINT("\n");

        co2SensorStatus = sensorStatus;
        co2SensorPpm = co2Ppm;

        if (millis() - previous2Min >= 120000)
        {
          previous2Min += 120000;
          display.setFullWindow();

          historyPer2MinTapePosition++;
          if (historyPer2MinTapePosition == 296)
            historyPer2MinTapePosition = 0;

          historyPer4Sec[historyPer2MinTapePosition] = co2Ppm;
        }
        else
        {
          display.setPartialWindow(0, 0, display.width(), display.height());
        }
        display.fillScreen(GxEPD_WHITE);
        display.setCursor(50, 100);
        std::stringstream ss;
        ss << co2Ppm << "ppm";

        if (firstDataEntry)
        {
          firstDataEntry = false;
          for (int i = 0; i < 296; i++)
          {
            historyPer4Sec[i] = co2Ppm;
            historyPer2Min[i] = co2Ppm;
          }
        }

        historyPer4SecTapePosition++;
        if (historyPer4SecTapePosition == 296)
          historyPer4SecTapePosition = 0;

        historyPer4Sec[historyPer4SecTapePosition] = co2Ppm;

        do
        {
          display.setFont(&PlusJakartaSans_Medium26pt7b);
          int16_t tbx, tby;
          uint16_t tbw, tbh;
          display.getTextBounds(ss.str().c_str(), 0, 0, &tbx, &tby, &tbw, &tbh);
          uint16_t utx = ((display.width() - tbw) / 2) - tbx;
          uint16_t uty = 42;

          display.setCursor(utx, uty);
          display.print(ss.str().c_str());

          if (show2Min)
          {
            show2Min = false;
            uint16_t highestPoint = 0;
            uint16_t mx = 0;

            for (int i = 0; i < 296; i++)
            {
              if (historyPer2Min[((historyPer2MinTapePosition - i) + 296) % 296] > mx)
              {
                mx = historyPer2Min[((historyPer2MinTapePosition - i) + 296) % 296];
                highestPoint = ((historyPer2MinTapePosition - i) + 296) % 296;
              }
            }

            for (int i = 0; i < 296; i++)
            {
              uint16_t pos = map(historyPer2Min[((historyPer2MinTapePosition - i) + 296) % 296], 400, mx, 127, 63);
              display.drawFastVLine(295 - i, pos, 128 - pos, GxEPD_BLACK);
            }

            display.setFont(&PlusJakartaSans_Medium6pt7b);
            display.fillRect(0, 63, 30, 12, GxEPD_WHITE);
            display.setCursor(0, 72);
            display.print(mx);
            display.fillRect(0, 117, 24, 12, GxEPD_BLACK);
            display.setCursor(0, 126);
            display.setTextColor(GxEPD_WHITE);
            display.print("10h");
            display.setTextColor(GxEPD_BLACK);
          }
          else
          {
            show2Min = true;
            uint16_t highestPoint = 0;
            uint16_t mx = 0;

            for (int i = 0; i < 296; i++)
            {
              if (historyPer4Sec[((historyPer4SecTapePosition - i) + 296) % 296] > mx)
              {
                mx = historyPer4Sec[((historyPer4SecTapePosition - i) + 296) % 296];
                highestPoint = ((historyPer4SecTapePosition - i) + 296) % 296;
              }
            }

            for (int i = 0; i < 296; i++)
            {
              uint16_t pos = map(historyPer4Sec[((historyPer4SecTapePosition - i) + 296) % 296], 400, mx, 127, 63);
              display.drawFastVLine(295 - i, pos, 128 - pos, GxEPD_BLACK);
            }

            display.setFont(&PlusJakartaSans_Medium6pt7b);
            display.fillRect(0, 63, 30, 12, GxEPD_WHITE);
            display.setCursor(0, 72);
            display.print(mx);
            display.fillRect(0, 117, 24, 12, GxEPD_BLACK);
            display.setCursor(0, 126);
            display.setTextColor(GxEPD_WHITE);
            display.print("20m");
            display.setTextColor(GxEPD_BLACK);
          }

          display.nextPage();
        } while (display.nextPage());
      }
      else
      {
        DEBUG_PRINT("Corrupted CO2 sensor frame\n");

        co2SensorStatus = -1;
        co2SensorPpm = -1;
      }
    }
    else
    {
      DEBUG_PRINT("Less than 13 bytes available: ");
      DEBUG_PRINT_2(co2Sensor.available(), HEX);
      DEBUG_PRINT("\n");
    }
  }

  delay(100);
}