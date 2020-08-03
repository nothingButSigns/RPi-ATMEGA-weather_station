# RPi-ATMEGA weather station
Project of weather station involving RPi, AVR microcontroller ATMEGA329P and radio transmission

The project concerns electronical and programming aspect of weather station.
The station itself is based on AVR microcontroller - ATMEGA 328p and set of sensors which allow to collect following data:
* temperature [°C]
* humidity [%RH]
* pressure [hPa]
* luminosity [lux]
* wind direction and wind speed [m/s]

The data is read every 1 h and sent via nRF24L01+PA radio tranceiver to Raspberry Pi 3B+ and saved in mysql database.

The repository contains eagle project of PCB board (THT) for the station and schematic. Libraries of parts which could not be found in EAGLE are included.


The project requires mySQL database to be installed on RPi.
MySQL database schema can be then imported with following terminal command:
```sh
$ mysql -u root -p [DB_NAME] < \path\user_name\Downloads\file.sql
```

C program (meteo_station.ino) was written in Arduino IDE 1.8.9.
Arduino bootloader was uploaded to ATMEGA 328P as instructed by [Nick Gammon](http://www.gammon.com.au/forum/?id=11637).
Following libraries are necessary:
* [Adafruit Sensor](https://github.com/adafruit/Adafruit_Sensor)
* [Wire](https://github.com/esp8266/Arduino/tree/master/libraries/Wire)
* [Adafruit BME280](https://github.com/adafruit/Adafruit_BME280_Library)
* [BH1750](https://github.com/claws/BH1750)
* [RF24](http://tmrh20.github.io/RF24)

C++ program written for RPi requires [RF24 library](https://github.com/nRF24/RF24) which contains essential dependencies (WiringPi & BCM2835) and [mySQL-cpp-connector] (https://dev.mysql.com/downloads/connector/cpp/)

The station is powered with 3.7 V NCR18650B connected to circuit via JACK socket. The battery is not included in schematic.

#BOM
| Q-ty | Part name | Value | Package | Description |
| --- | --------- | ----- | ------- | ----------- |
|1|ATMEGA328P|-|DIP|AVR Microcontroller|
|1|WIRELESS-NRF24L01-PA-EXT|-|-|2.4 GHz Wireless Module based on NRF24L01|
|1|BME280|-|-|Digital Humidity-Temperature-Pressure Sensor|
|1|BH1750|-|-|Digital Light Sensor|
|1|Lacrosse TX20|-|-|Wind speed and direction sensor|
|1|Quartz Crystal|8MHz|HC49/S|-|
|2|Electrolytic capacitor|0.1 uF|C2.5-3|-|
|1|Electrolytic capacitor|0.22 uF|C2.5-3|-|
|1|Electrolytic capacitor|4.7 uF|C2.5-3|-|
|2|Ceramic capacitor|22 pF|C2.5-3|-|
|3|Resistor|0 Ω |0204/5|-|
|1|Resistor|10 kΩ|0204/5|-|
|1|LFXXCVS|LF33CVS|TO220 S|Voltage regulator|
|1|JACK-PLUG|-|SPC4077|Plug to enable circuit powering with DC|
|1|4-pin terminal block|3.5 mm|-|Terminal block with round pad|



