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
user_input ui = {0,0,0,0,0,1};


//------------------Setup---------------------------

void setup() {
  Serial.begin(500000); //initialise serial
  Serial.println();

  initUserInput();
  initTFT(); //initialise TFT
  tft.println("_Puce 900_");
  sensorInit();//initialise sensor
  miniSDTest();//test the SD card
  char file_name[12];
  fileNameFromNumber(file_name, ui.shutter_count);
  Serial.print("fn: ");
  Serial.println(file_name);
  while ( fileExists(SD, file_name ) ){
    ui.shutter_count ++;
    //fn = fileNameFromNumber(ui.shutter_count);
  }
  delay(1000);

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
    char file_name[12];
    fileNameFromNumber(file_name, ui.shutter_count);
    while ( fileExists(SD, file_name ) ){
      ui.shutter_count ++;
      fileNameFromNumber(file_name, ui.shutter_count);
    }
    saveFrame(frame, file_name ); 
  }
  

}
