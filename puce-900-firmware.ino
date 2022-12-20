#include "puce-900-firmware.h"



//------------------Globals---------------------------
uint8_t frame[ADNS3080_PIXELS_X][ADNS3080_PIXELS_Y]; //2d array that will hold pixel data

struct user_input {
  bool ae_switch;
  bool shutter_button_down;
  bool shutter_button_pressed;
  unsigned long shutter_ready_at;
  uint exposure_pot;
  uint shutter_count;
};
user_input ui = {0,0,0,0,0,0};

char next_filename[12] = "/000000.jpg";

//------------------Setup---------------------------

void setup() {
  Serial.begin(500000); //initialise serial
  Serial.println();

  initUserInput();
  initTFT(); //initialise TFT
  tft.println("_Puce 900_");
  sensorInit();//initialise sensor
  miniSDTest();//test the SD card
  
  tPrint(next_filename);


  delay(10000);

  //

}

//------------------Loop---------------------------

void loop() {
  //----ui---------
  updateUserInput();

  //----Sensor-----
  readySensor();
  sensor.frameCapture( frame ); // dump frame from sensor
  rotateFrame( SENSOR_ROTATION );
  
  //----TFT--------
  //draw frame to screen
  readyTFT();
  drawFrameToScreen();
  drawHud();

  //----OUTPUT-----
  if( getShutterButton() ){
    //shutter has been pressed. Save JPEG to the SD card

    fileNameFromNumber(ui.shutter_count);
    while( fileExists( SD, next_filename )){ //check for next available filename
      ui.shutter_count ++;
      fileNameFromNumber(ui.shutter_count);
    }     
    saveFrame(frame, next_filename );
  }
  

}
