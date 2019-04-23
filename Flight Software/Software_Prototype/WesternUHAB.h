// Pin definition for software SPI if needed
#define BME_SCK 52
#define BME_MISO 50
#define BME_MOSI 51 
#define BME_SS 53

// Chip select pin for SPI interface for the SD card and Ethernet Shield
#define SD_CHIPSELECT 53

// Define RX and TX pin for serial comms with GPS
#define RX_PIN 0
#define TX_PIN 1
#define GPS_BAUD 9600

// Define Altitude Interval
#define ALTITUDE_INTERVAL 5000

// Actuator Direction Pins Definition
#define ACT1_PUSH 22
#define ACT1_PULL 23

#define ACT2_PUSH 24
#define ACT2_PULL 25

#define ACT3_PUSH 26
#define ACT3_PULL 27

#define ACT4_PUSH 28
#define ACT4_PULL 29

#define ACT5_PUSH 30
#define ACT5_PULL 31

#define ACT6_PUSH 32
#define ACT6_PULL 33

#define ACT7_PUSH 34
#define ACT7_PULL 35

// Actuator Enable Pins
#define ACT1_EN 13
#define ACT2_EN 12
#define ACT3_EN 11
#define ACT4_EN 10
#define ACT5_EN 9
#define ACT6_EN 8
#define ACT7_EN 7

typedef struct timeObject {

    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    
} TimeObject;

// bmeReading struct
// temperature reading returns a signed int
// pressure and humidity reading returns a float
typedef struct bmeObject {
    
    signed int temperature;
    float pressure;
    float humidity;

} BMEObject;

// GPS Object Struct
typedef struct gpsObject {
    
    long latitude;
    long longitude;
    long altitude;
    long speed;
    TimeObject time;
    
} GPSObject;