#include <Camera.h>
#include <Logging.h>


#include <Wire.h>
#include <SPI.h>


Camera newCam;
//Log newLog;


void setup(void) {    
    //Serial setup
    Serial.begin(9600);

    //Set camera
    newCam = Camera(4, A14, 47);

    //Set up logging
    Logging::setChip(4);

    //Start message
    printHeader(); //Print the header
    
    //Wait for GPS lock
    //if(GPS::waitForLock){
        Logging::printLog("\r\n                     *!GPS lock obtained!*\r\n", "");
    /*}
    else{
        Logging::printLogln("GPS lock could not be obtained, aborting.);
    }*/

    //Print all other info
    printInfo();




    
    newCam.captureImage("tester6.jpg", 2);
    newCam.emptyImageBuffer();
    newCam.emptyImageBuffer();
    newCam.emptyImageBuffer();
    newCam.captureImage("tester6.jpg", 2);
    newCam.emptyImageBuffer();
    newCam.captureImage("tester6.jpg", 2);
    newCam.captureImage("tester6.jpg", 2);
    newCam.captureImage("tester6.jpg", 2);
    newCam.captureImage("tester2.jpg", 2);
     newCam.emptyImageBuffer();
    newCam.captureImage("tester3.jpg", 2);
     newCam.emptyImageBuffer();
     delay (4000);
      newCam.emptyImageBuffer();

     newCam.captureImage("largePic.jpg", 0);
}
 

void loop(void) {   
    newCam.writeImage();
    //delay(1000);
}

void printHeader(){
    Logging::printLogln(" ############################################################", "");
    Logging::printLogln("##                                                          ##", "");
    Logging::printLogln("#               Western University HAB Project               #", "");
    Logging::printLogln("#                                                            #", "");
    Logging::printLogln("#                    Flight Software Logs                    #", "");
    Logging::printLogln("##                                                          ##", "");
    Logging::printLogln(" ############################################################", "");
}

void printInfo(){
    //Print author and team info
    Logging::printLogln("\r\nAuthor:", ""); Logging::printLogln("---------------", ""); Logging::printLogln("Stephen Amey", "");
    //Add rest of info here

    //Print the date from the GPS
    //Logging::printLogln("\r\n\r\nFlight date:", ""); Logging::printLogln("---------------", ""); Logging::printLogln(GPS::getDateStamp(), "");

    //Print out the camera info
    Logging::printLogln("\r\n\r\nCamera info:", ""); Logging::printLogln("---------------", ""); Logging::printLogln(newCam.getInfo(), "");

    //Print out GPS info, e.g. initial coordinates

    //Print out final header
    Logging::printLogln("\r\n##############################################################", "");
    Logging::printLogln(    "#                       Logging begins                       #", "");
    Logging::printLogln(    "##############################################################\r\n\r\n", "");
}
