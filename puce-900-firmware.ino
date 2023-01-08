#include "puce-900-firmware.h"



//------------------Globals---------------------------
uint8_t frame[ADNS3080_PIXELS_X][ADNS3080_PIXELS_Y]; //2d array that will hold pixel data

struct user_input {
  bool me_switch;
  bool shutter_button_down;
  bool shutter_button_pressed;
  unsigned long shutter_ready_at;
  uint exposure_pot;
  uint shutter_count;
};
user_input ui = {0,0,0,0,0,1};

struct sensor_status{
  bool manual_exposure;
  uint16_t shutter;
  uint16_t suttter_max_bound;
};
sensor_status ss = { false, 0, 0 };



char next_filename[12] = "/000001.jpg";

//------------------Setup---------------------------

void setup() {
  Serial.begin(500000); //initialise serial
  Serial.println();

  initUserInput();
  initTFT(); //initialise TFT
  tft.println("_Puce 900_");
  sensorInit();//initialise sensor
  miniSDTest();//test the SD card
  nextAvailableFileName(); //find the next filename
  tPrint(next_filename);


  delay(2000);

  //

}

//------------------Loop---------------------------

void loop() {
  //----ui---------
  updateUserInput();

  //----Sensor-----
  readySensor();
  updateSensorStatus(); //get status from the sensor
  
  //change settings on the sensor
  exposureDump();
  sensor.frameCapture( frame ); // dump frame from sensor
  rotateFrame( SENSOR_ROTATION );
  compareAndChangeSensor();//compare current status to user input
  
  //----TFT--------
  //draw frame to screen
  readyTFT();
  drawFrameToScreen();
  drawHud();

  //----OUTPUT-----
  if( getShutterButton() ){
    //shutter has been pressed. Save JPEG to the SD card

    nextAvailableFileName(); //find the next available filename  
    saveFrame(frame, next_filename );
  }
  

  

}
