#ifndef HAB_Structs_h
#define HAB_Structs_h

struct actuatorReadings {
	float position; //signed int
    float temperature;
    char actuatorStatusPtr[10] = "AUTO"; //OVR_CLOSE
    char heaterStatusPtr[13] = "AUTO"; //OVR_DISABLE
};
typedef struct actuatorReadings ActuatorReadings;

struct bmeReadings {
	float temperature; //signed int
    float pressure;
    float humidity;
};
typedef struct bmeReadings BMEReadings;

struct gpsReadings {  
    float second; //Seconds
    float minute; //Seconds
    float hour; //Seconds
    float speed; //Meters per second
    float altitude; //Meters
	float latitude; //Degrees
	float longitude; //Degrees
};
typedef struct gpsReadings GPSReadings;

#endif