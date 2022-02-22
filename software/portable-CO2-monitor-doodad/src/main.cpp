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
#include <Fonts/FreeMonoBold24pt7b.h>

GxEPD2_BW<GxEPD2_290, GxEPD2_290::HEIGHT> display(GxEPD2_290(/*CS=D8*/ 10, /*DC=D3*/ 4, /*RST=D4*/ 3, /*BUSY=D2*/ 2)); // GDEM029T94

void setup()
{
  Serial.begin(9600);
  co2Sensor.begin(9600);
  display.init(115200);
  delay(100);

  display.setRotation(1);
  display.setFont(&FreeMonoBold24pt7b);
  display.setTextColor(GxEPD_BLACK);
  display.setFullWindow();
  display.firstPage();
  display.fillScreen(GxEPD_WHITE);

  display.fillScreen(GxEPD_WHITE);
  display.setCursor(50, 100);
  std::stringstream ss;
  ss << "Welcome";

  display.setPartialWindow(0, 0, display.width(), display.height());
  int16_t tbx, tby;
  uint16_t tbw, tbh;
  display.getTextBounds(ss.str().c_str(), 0, 0, &tbx, &tby, &tbw, &tbh);
  uint16_t utx = ((display.width() - tbw) / 2) - tbx;
  uint16_t uty = ((display.height() / 2) - tbh / 2) - tby;
  do
  {
    display.setCursor(utx, uty);
    display.print(ss.str().c_str());
    display.nextPage();
  } while (display.nextPage());

  delay(2000);
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

      display.fillScreen(GxEPD_WHITE);
      display.setCursor(50, 100);
      std::stringstream ss;
      ss << co2Ppm << "ppm";

      display.setPartialWindow(0, 0, display.width(), display.height());
      int16_t tbx, tby;
      uint16_t tbw, tbh;
      display.getTextBounds(ss.str().c_str(), 0, 0, &tbx, &tby, &tbw, &tbh);
      uint16_t utx = ((display.width() - tbw) / 2) - tbx;
      uint16_t uty = ((display.height() / 2) - tbh / 2) - tby;
      do
      {
        display.setCursor(utx, uty);
        display.print(ss.str().c_str());
        display.nextPage();
      } while (display.nextPage());
    }
    else
    {
      DEBUG_PRINT("Corrupted CO2 sensor frame\n");

      co2SensorStatus = -1;
      co2SensorPpm = -1;

      display.fillScreen(GxEPD_WHITE);
      display.setCursor(50, 100);
      std::stringstream ss;
      ss << "broken";

      display.setPartialWindow(0, 0, display.width(), display.height());
      int16_t tbx, tby;
      uint16_t tbw, tbh;
      display.getTextBounds(ss.str().c_str(), 0, 0, &tbx, &tby, &tbw, &tbh);
      uint16_t utx = ((display.width() - tbw) / 2) - tbx;
      uint16_t uty = ((display.height() / 2) - tbh / 2) - tby;
      do
      {
        display.setCursor(utx, uty);
        display.print(ss.str().c_str());
        display.nextPage();
      } while (display.nextPage());
    }
  }
  else
  {
    DEBUG_PRINT("Less than 13 bytes available: ");
    DEBUG_PRINT_2(co2Sensor.available(), HEX);
    DEBUG_PRINT("\n");
  }

  delay(4000);
}