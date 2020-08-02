#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <BH1750.h>
#include <RF24.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

// define pin for Tx20
#define WINDpin 6
// define pins for nRF24L01+PA LNA
#define CE_PIN 8             
#define CSN_PIN 7
                 
const uint64_t transmit_pipe = 0xEEFDFDF0A2; // transmit pipe
RF24 radio(CE_PIN, CSN_PIN);

struct WeatherData {
    uint8_t lum;
    float temp;
    float hum;
    float press;
    float wind_speed;
    String wind_dir;
} weather;

Adafruit_BME280 bme; // utworzenie instancji klasy czujnika bme (cisnienia, temp i wilg)
BH1750 lightMeter;   // utworzenie instancji klasy czujnika BH1750 (jasnosci)

unsigned long delayTime = 1000;
bool status;         // BME280 availability state ('true' if the sensor is available)
int bitlenght = -1;
bool data[41];
int watchdogIterations = 450;                  // number of WDT cycles to get ~1 h interval between data read
int currentIteration = watchdogIterations;     // enables data read right after powering on
byte adcsra = 0;


void delay2(unsigned long ms);
void getdata(void);
void interpretdata();

ISR(WDT_vect) {
   sleep_disable();               
   ADCSRA = adcsra;  // turn on ADC
}

void radiosetup()
{
  radio.powerUp();  
  radio.begin();
  radio.setCRCLength( RF24_CRC_16 ) ;
  radio.setDataRate( RF24_1MBPS );
  radio.setPayloadSize(32);
  radio.setChannel(0x4c);
  radio.setAutoAck( true ) ;
  radio.setPALevel( RF24_PA_HIGH ) ;
  radio.setRetries(5,15);
  radio.stopListening();
}

void setupSensors()
{
    radio.powerUp();
    void radiosetup();
    pinMode(WINDpin, INPUT);     
    Wire.begin();
    
    // set BH1750 sensor to read data once and go into sleep mode; set resolution to 4lx
    lightMeter.begin(BH1750::ONE_TIME_LOW_RES_MODE);
   
    status = bme.begin(0x76);
    if (!status) {
        while (1);
    }
}
  

void setup() { 
  // If reset occurs, check whether it was caused by WDT
  if(MCUSR & _BV(WDRF)) {
    MCUSR &= ~_BV(WDRF);              // reset reset-due-to-watchdog flag
    WDTCSR |= (_BV(WDCE) | _BV(WDE));  // enter watchdog configuration mode
    WDTCSR = 0x00;                     // disable WDT
  }  
  
  cli(); //disable interruptions
     
   WDTCSR |= (_BV(WDCE) | _BV(WDE));               // enter watchdog configuration mode
   WDTCSR =  _BV(WDIE) | _BV(WDP3) | _BV(WDP0);    // enable interrupts and set timer to ~8 sec
   wdt_reset();
  //re-enable interrupts    
  sei(); 
}

void loop() { 
  
  gotoSleep();
  
  if(++currentIteration >= watchdogIterations) {
    setupSensors();
    getdata();                                    
    interpretdata();                   
    weather.lum = lightMeter.readLightLevel(); 
    weather.temp = bme.readTemperature();        
    weather.press = bme.readPressure()/100.0;
    weather.hum = bme.readHumidity();

    radio.openWritingPipe(transmit_pipe);
    radio.write( &weather, sizeof(WeatherData)); 
    currentIteration = 1;
  }
   
}

// function to get datagram from Tx20
void getdata(){
  if (bitlenght <0){
     bitlenght = 1220; // duration of 1 bit [ms]
  }
 
   pinMode(WINDpin, OUTPUT);
   digitalWrite(WINDpin, HIGH);
   delay(100);
   
   digitalWrite(WINDpin, LOW );
   delay(500);
   
   pinMode(WINDpin, INPUT);
  
  
   while(digitalRead(WINDpin) == LOW){}  
   while(digitalRead(WINDpin) == HIGH){}
   while(digitalRead(WINDpin) == LOW){}

   for (int i=0 ; i<41 ; i++){  
     data[i] = (digitalRead(WINDpin) == LOW)? 0 : 1 ; 
     delay2(bitlenght);
 }

 interpretdata();
}

// function to retrieve wind speed and direction
void interpretdata(){
 
  // wind direction
  char DirBuffer = 0;
  for (int i =8 ; i> 4 ; i--){ 
    DirBuffer = DirBuffer << 1;
    DirBuffer |= (char)(data[i]^1);;
  }

   switch(DirBuffer) {
     case 0: weather.wind_dir="N"; break;
     case 1: weather.wind_dir="NNE"; break;;
     case 2: weather.wind_dir="NE"; break;
     case 3: weather.wind_dir="ENE"; break;
     case 4: weather.wind_dir="E"; break;
     case 5: weather.wind_dir="ESE"; break;
     case 6: weather.wind_dir="SE"; break;
     case 7: weather.wind_dir="SSE"; break;
     case 8: weather.wind_dir="S"; break;
     case 9: weather.wind_dir="SSW"; break;
     case 10: weather.wind_dir="SW"; break;
     case 11: weather.wind_dir="WSW"; break;
     case 12: weather.wind_dir="W"; break;
     case 13: weather.wind_dir="WNW"; break;
     case 14: weather.wind_dir="NW"; break;
     case 15: weather.wind_dir="NNW"; break;
   }

 
 int ws_buffer = 0;
 // wind speed data
 for (int i =9 ; i> 21 ; i++){
      ws_buffer |= (data[i]^1)<<(i-9);     
 }

 ws_buffer &=~(1<<11|1<<10|1<<9); 
 weather.wind_speed = ws_buffer * 0.1;  
}

void gotoSleep(void)
{
   adcsra = ADCSRA;                       // save ADC state
   ADCSRA = 0;                            // turn off ADC 
   set_sleep_mode(SLEEP_MODE_PWR_DOWN);   // use power down sleep mode
   sleep_enable();                        // enable sleep mode
   sleep_cpu();                   
}

void delay2(unsigned long ms){
  unsigned long start = micros();
  while (micros() - start <= ms);
}
