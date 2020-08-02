#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/statement.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
#include <inttypes.h>

#include <RF24.h>
#include <wiringPi.h>

/* Create instance of class RF24 to receive radio data.
 * 	Connection of nRF24L01 transceiver module to RPi:
 *  CE -> RPI_V2_GPIO_P1_22
 *	CSN -> RPI_V2_GPIO_P1_24
 *	SPI speed [MHZ] -> BCM2835_SPI_SPEED_8MHZ
 */

RF24 radio(RPI_V2_GPIO_P1_22, RPI_V2_GPIO_P1_24, BCM2835_SPI_SPEED_8MHZ);
/* Writing/receiving pipes created according to nRF24L01 specification:
 * pipes[0] -> writing pipe
 * pipes[1] -> reading pipe; it's address is the same as address
 * of writing pipe in weather station
 */
const uint64_t pipes[2] = { 0xeefdfdf0ec, 0xa2ll};

/* Data to connect to mySQL database in order to save received data.
 * MYSQL_USER and MYSQL_PASSWD defines should be replaced with
 * Your own user data
 */
#define HOST "localhost"
#define MYSQL_USER "randomuser"
#define MYSQL_PASSWD "password"
#define MYSQL_DATABASE "sensor_data"

struct __attribute__((__packed__)) WeatherData {
	uint8_t lum;
    float temp;
    float hum;
    float press;
    float w_speed;
    const char* w_dir;
} weather;

void setup_radio()
{
  radio.begin();
  /* data below can be adjusted to acquire efficient
   * transmission in case-specific conditions
   * such as distance between weather station and receiver
   * or presence of any physical obstacles (e.g. walls)
   */
  radio.setPayloadSize(32);
  radio.setRetries(5,15);
  radio.setAutoAck(true);
  radio.setPALevel(RF24_PA_HIGH);
  radio.setChannel(0x4c);
  radio.setDataRate(RF24_1MBPS);
  radio.setCRCLength(RF24_CRC_16);
  delay(100);

      radio.openWritingPipe(pipes[0]);
      radio.openReadingPipe(1,pipes[1]);
}


void saveData(WeatherData *weather)
{
    sql::Driver * driver;
    sql::Connection *conn;
    sql::Statement *stat;
    sql::PreparedStatement *prep_stat;


    driver = get_driver_instance();
    conn = driver->connect(HOST, MYSQL_USER, MYSQL_PASSWD);
    stat = conn->createStatement();
    stat->execute("USE sensor_data");


            prep_stat = conn->prepareStatement("INSERT INTO weather_data(Date, Time, Temperature, Humidity, Pressure, Luminosity, Wind_dir, Wind_speed) VALUES(NOW(), NOW(),?, ?, ?, ?, ?, ?)");
            prep_stat->setDouble(1,(*weather).temp);
            prep_stat->setDouble(2,(*weather).hum);
            prep_stat->setDouble(3,(*weather).press);
            prep_stat->setInt(4,(*weather).lum);
            prep_stat->setString(5,(*weather).w_dir);
            prep_stat->setDouble(6,(*weather).w_speed);


            prep_stat->execute();

    delete prep_stat;
    delete stat;
    delete conn;
    delete driver;

}


void waitForData()
{
    uint8_t pipeNum;
    setup_radio();
    radio.startListening();

 while(1) {

    if ( radio.available(&pipeNum) ) {

      while (radio.available())
      {
        if (pipeNum) {
			radio.read(&weather, sizeof(WeatherData) );
			saveData(&weather);
        }
      }
    }
    }
}


int main()
{

	waitForData();


	return 0;
}
