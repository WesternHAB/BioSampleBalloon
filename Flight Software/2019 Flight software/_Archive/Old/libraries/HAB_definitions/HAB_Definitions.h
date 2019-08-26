//--------------------------------------------------------------------------------\
//General-------------------------------------------------------------------------|

	#define READINGS_TIME_STEP 1000	
	#define STOP_ALTITUDE 5000


//--------------------------------------------------------------------------------\
//Ethernet & UDP------------------------------------------------------------------|

	#define UDP_TX_PACKET_MAX_SIZE 300 //Is this a safe size?
	#define HEARTBEAT_TIMEOUT 10000
	#define RECONNECT_DELAY 1000
	#define ARGUMENT_MAX_LENGTH 20
	#define DELIMITER " "

	//Local MAC, IP, port
	#define MAC {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED}
	#define LOCAL_IP_O1 192
	#define LOCAL_IP_O2 168
	#define LOCAL_IP_O3 0
	#define LOCAL_IP_O4 10	
	#define LOCAL_PORT 54443
	
	//Remote IP, port (groundstation)
	#define GS_IP_O1 99
	#define GS_IP_O2 242
	#define GS_IP_O3 100
	#define GS_IP_O4 155
	#define GS_PORT 54442
	
	//UWO TEST
	/*#define LOCAL_IP_O1 172
	#define LOCAL_IP_O2 30
	#define LOCAL_IP_O3 83
	#define LOCAL_IP_O4 155	
	#define LOCAL_PORT 59662*/	
	
	//UWO TEST
	/*#define GS_IP_O1 172
	#define GS_IP_O2 30
	#define GS_IP_O3 83
	#define GS_IP_O4 154
	#define GS_PORT 59661*/
	
	//Remote IP, port (PRISM)
	#define PRISM_IP_O1 192
	#define PRISM_IP_O2 168
	#define PRISM_IP_O3 1
	#define PRISM_IP_O4 1
	#define PRISM_PORT 10000
	

//--------------------------------------------------------------------------------\
//GPS-----------------------------------------------------------------------------|
	
	#define GPS_BAUD 9600
	#define GPS_MAX_AGE 110
	#define GPS_RX_PIN 53
	#define GPS_TX_PIN 52


//--------------------------------------------------------------------------------\
//SD card-------------------------------------------------------------------------|
	
	#define SD_CHIPSELECT 4


//--------------------------------------------------------------------------------\
//Camera--------------------------------------------------------------------------|

	//Camera 1
	#define CAM1_RX_PIN A15 //MUST FIND THE CAMERA ITS OWN SPOT
	#define CAM1_TX_PIN A14
	
	
//--------------------------------------------------------------------------------\
//BME280--------------------------------------------------------------------------|	

	//SPI (Interferes with SD card)
	//#define BME_SCK 13
    //#define BME_MISO 12
    //#define BME_MOSI 11
    //#define BME_CS 10
	
	//I2C (Set up wiring for this, no software config required)
	//SDI to 20
	//SCK to 21
	

//--------------------------------------------------------------------------------\
//Thermistors---------------------------------------------------------------------|	

	#define SERIESRESISTOR 10000  
	#define THERMISTORNOMINAL 10000   
	#define TEMPERATURENOMINAL 25 
	#define BCOEFFICIENT 3950


//--------------------------------------------------------------------------------\
//Actuator, heaters, and thermistor pins------------------------------------------|

	#define MIN_ACTUATOR_TEMP -10
	#define MAX_ACTUATOR_TEMP 0

	//Pod 1
	#define HEAT1_EN 22
	#define ACT1_EN 23
	#define ACT1_PUSH 24
	#define ACT1_PULL 25
	#define THERMISTOR1 A8
	#define ACT1_POS A9
	
	//Pod 2
	#define HEAT2_EN 28
	#define ACT2_EN 29
	#define ACT2_PUSH 30
	#define ACT2_PULL 31
	#define THERMISTOR2 A10
	#define ACT2_POS A11
	
	//Pod 3
	#define HEAT3_EN 34
	#define ACT3_EN 35
	#define ACT3_PUSH 36
	#define ACT3_PULL 37
	#define THERMISTOR3 A12
	#define ACT3_POS A13
	
	//Pod 4
	#define HEAT4_EN 40
	#define ACT4_EN 41
	#define ACT4_PUSH 42
	#define ACT4_PULL 43
	#define THERMISTOR4 A14
	#define ACT4_POS A15