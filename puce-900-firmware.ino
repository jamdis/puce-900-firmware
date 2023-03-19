#include "puce-900-firmware.h"
#define minimum(a,b)     (((a) < (b)) ? (a) : (b))


//------------------Globals---------------------------
uint8_t frame[ADNS3080_PIXELS_X][ADNS3080_PIXELS_Y]; //2d array that will hold pixel data

struct user_input {
  bool me_switch;
  bool playback_switch;
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

uint8_t mode = MODE_STILL_CAMERA;

uint loops = 0;

char next_filename[12] = "/000001.jpg";

//------------------Setup---------------------------

void setup() {
  Serial.begin(500000); //initialise serial
  Serial.println("PUCE 900");

  initUserInput(); //set up user interface stuff
  initTFT(); //initialise TFT
  tft.println("_Puce 900_");
  sensorInit();//initialise sensor
  miniSDTest();//test the SD card
  EEPROM.begin(2); //setup eeprom to store the number of pics taken
  ui.shutter_count = EEPROM.read(0);


  delay(500); //delay so that debug info can be read

  //

}

//------------------Loop---------------------------

void loop() {
  loops ++;
    //----ui---------
  updateUserInput();

  if( getPlaybackSwitch() ){
    mode = MODE_STILL_CAMERA;
    cameraLoop();
  }
  else{
    int this_image = 1; //current image slectected, represented as the nth image on the card, ignoring irrelevant files 
    int image_count = countImages(SD, "/");
    if( mode != MODE_PLAYBACK ){
      
      //we have just now switched into playback mode. Need to set it up.
      readyTFT();
      tft.fillScreen(0);
      tft.setCursor(10,10);
      tft.println("PLAYBACK MODE");
      tPrint(image_count);
      tPrint(getFileNameByCount(SD, "/", 3));
      mode = MODE_PLAYBACK;
    }

    this_image = map(ui.exposure_pot, 0, 0xFFF, 0, image_count - 1);

    drawFSJpeg( getFileNameByCount( SD, "/", this_image),0,0 );

    tft.setCursor(0, 120);
    tft.print(this_image + 1);
    tft.print(" / ");
    tft.print(image_count);
    tft.print (" : ");
    tft.print(getFileNameByCount( SD, "/", this_image) );
    tft.print("                    "); //clear the rest of the row
    
  }
}

void cameraLoop(){
  //----Sensor-----
  readySensor();
  updateSensorStatus(); //get status from the sensor
  
  //change settings on the sensor
  sensor.frameCapture( frame ); // dump frame from sensor
  rotateFrame( SENSOR_ROTATION ); 
  setExposure();//update the exposure settings
  
  //----TFT--------
  //draw frame to screen
  readyTFT();
  drawFrameToScreen();
  drawHud();

  //----OUTPUT-----
  if( getShutterButton() ){
    //shutter has been pressed. Save JPEG to the SD card

    fileNameFromNumber(ui.shutter_count);
    saveFrame(frame, next_filename );
    ui.shutter_count ++;
    EEPROM.write(0,ui.shutter_count);
    EEPROM.commit();
  }  
}


