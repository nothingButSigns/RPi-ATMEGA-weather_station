# RPi-ATMEGA-weather_station
Project of weather station involving RPi, AVR microcontroller ATMEGA329P and radio transmission

The project concerns electronical and programming aspect of weather station.
The station itself is based on AVR microcontroller - ATMEGA 328p and set of sensors which allow to collect temperature [Celcius degs], humidity [%RH], pressure [hPa], luminosity [lux], wind direction and wind speed [m/s] values.
The data is read every 1 h and sent via nRF24L01+PA radio tranceiver to Raspberry Pi 3B+ and saved in mysql database.

The repository contains eagle project of PCB board (THT) for the station and schematic. Libraries of parts which could not be found in EAGLE are included.

The project requires mySQL database to be installed on RPi.
MySQL database schema can be then imported with following terminal command:
 
mysql -u root -p [DB_NAME] < C:\Users\user_name\Downloads\file.sql

C program (meteo_station.ino) was written in Arduino IDE 1.8.9.
Arduino bootloader was uploaded to ATMEGA 328P as presented by Nick Gammon in following link: http://www.gammon.com.au/forum/?id=11637

C++ program written for RPi requires RF24 library (https://github.com/nRF24/RF24) which contains essential dependencies (WiringPi & BCM2835) and mySQL-cpp-connector (https://dev.mysql.com/downloads/connector/cpp/).

The station is powered with 3.7 V NCR18650B connected to circuit via JACK socket. The battery is not included in schematic. 
