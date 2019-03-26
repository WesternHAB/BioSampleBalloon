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

#define ALTITUDE_INTERVAL 5000

// Chip select pin for SPI interface for the SD card and Ethernet Shield
#define SD_CHIPSELECT 53

// Define RX and TX pin for serial comms with GPS
#define RX_PIN 0
#define TX_PIN 1
#define GPS_BAUD 9600

// Altitude interval definition.
#define ALTITUDE_INTERVAL 5000

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