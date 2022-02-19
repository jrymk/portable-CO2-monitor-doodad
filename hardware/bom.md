### NDIR CO2 sensor
## Senseair S8 Low Power

**Model number:** S8-004-0-0053\
**Photo:** N/A

**Purchase price:** 51.30 USD including shipping\
[link](https://www.ruten.com.tw/item/show?22125330757377)

---

### temperature and humidity sensor
## SI7021 breakout board

**Model number:** GY-21?\
**Photo:** N/A

**Purchase price:** 4.16 USD including shipping\
[link](https://www.ruten.com.tw/item/show?21906774992668)

---

### ePaper display
## Waveshare 2.9 inch 2 color
*I did not went with the 3 color version because 1. it refreshes in 15 seconds instead of 2 seconds with the 2 color version and 2. it does not support partial refresh which means there is no way around the 15 second refresh.*

**Model number:** 2.9inch e-Paper Module\
**Photo:** N/A

**Purchase price:** 24.11 USD including shipping\
[link](https://www.ruten.com.tw/item/show?21840725853038)

---

### microprocessor
## Arduino Pro Micro (3.3V at 8Mhz)
*It's a Chinese knockoff for sure, but why not?*\
*For current draw, the main consumer would be the NDIR CO2 sensor, so I am not going to underclock the Pro Micro for now. Also I picked the 3.3V version because I initially wanted to add microSD logging to the project. I then abandoned the idea because 1. apparently microSD cards draw quite a some current and 2. the long term CO2 data isn't really that useful.*

**Model number:** Arduino Pro Micro (3.3V/8Mhz variant)\
**Processor:** ATMEGA32U4\
**Photo:** N/A

**Purchase price:** 7.89 USD including shipping\
[link](https://www.ruten.com.tw/item/show?21949999974462)

---

### RTC module
## DS3231 Mini RTC Module for Raspberry Pi
*It is here in case I want to only wake up the processor every minute or so. I might not use that feature, but I would use it to track the time to show more accurate graphs. (I will be storing data withing a day for the graph.)*

**Photo:** N/A

**Purchase price:** 1.61 USD including shipping\
[link](https://www.ruten.com.tw/item/show?21408123500703)

---

### power supply
## Type C 5V 2A li-ion battery charger with constant 5V 2.4A output module
*"I think" the output actually switches to the USB input when plugged in because the description says the output may drop out briefly when power unplugged, which is perfect as drawing current from the battery terminals may prevent the charging IC to switch off and cause the battery to float charge at 4.2V, which is not necessarily dangerous but it will not be good for the battery.*

**Model number:** LX-LBC01/02

**Purchase price:** 2.22 USD including shipping\
[link](https://www.ruten.com.tw/item/show?22151533086321)

---

### battery
## 103450 lithium polymer battery (3.7V 1800mAh)

**Purchase price:** 5.20 USD including shipping *definitely overpaid\
[link](https://www.ruten.com.tw/item/show?22130068217114)

---

### EEPROM
## AT24C1024W
*I want to store data within an hour with 4 seconds interval and data within a day with 20 seconds interval. Plus the temperature and humidity data, each data point will be 6 bytes long. That will take up 30240 bytes, which will fit in the 1Mb EEPROM.*

**Model number:** AT24C1024W-10SI-2.7 (SOP8 package)\
**Purchase price:** 1.90 USD including shipping\
[link](https://www.ruten.com.tw/item/show?21918309065295)