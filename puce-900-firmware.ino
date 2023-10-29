#include "puce-900-firmware.h"
#define minimum(a,b)     (((a) < (b)) ? (a) : (b))


//------------------Globals---------------------------

File avi_file;
File y4m_file;

uint exposure_table[16][2] = {
  {366, 0xFFFF},
  {500, 0xBB80},
  {1000, 0x5DC0},
  {2000, 0x2EE0},
  {4000, 0x1770},
  {8000, 0x0BB8},
  {16000, 0x05DC},
  {32000, 0x02EE},
  {128000, 0x00BC},
  {256000, 0x05E},
  {512000, 0x02F},
  {1024000, 0x017},
  {2048000, 0x000C},
  {4096000, 0x0006},
  {8192000, 0x0003},
  {16384000, 0x0001},
};

const char *exposure_pretty[] = {
  "366",
  "500",
  "1000",
  "2000",
  "4000",
  "8000",
  "16k",
  "32k",
  "128k",
  "256k",
  "512k",
  "1m",
  "2m",
  "4m",
  "8m",
  "16m"
};

uint8_t test_pattern[16] = 
{
0xFF,0   ,0   ,0  ,
0   ,0xFF,0   ,0  ,
0   ,0   ,0xFF,0  ,
0   ,0   ,0   ,0xFF
};
uint8_t  sensor_buffer[ADNS3080_PIXELS_X * ADNS3080_PIXELS_Y]; //1d array that will hold pixel data
uint16_t screen_buffer[ADNS3080_PIXELS_X * DISP_SCALE * ADNS3080_PIXELS_Y * DISP_SCALE]; //1d array that will hold screen buffer
uint8_t frame[ADNS3080_PIXELS_X][ADNS3080_PIXELS_Y]; //2d array that will hold pixel data

struct user_input {
  bool me_switch;
  bool playback_switch;
  bool video_mode_switch;
  bool shutter_button_down;
  bool shutter_button_pressed;
  unsigned long shutter_ready_at;
  uint exposure_pot;
  uint exposure_index;
  uint exposure_display;
  uint exposure_period;
  uint shutter_count;
};
user_input ui = {0,0,0,0,0,1};

struct sensor_status{
  bool manual_exposure;
  uint16_t shutter;
  uint16_t suttter_max_bound;
};
sensor_status ss = { false, 0, 0 };

uint8_t mode = 0;

uint loops = 0;

char next_filename[12] = "/000001.jpg";

//timer
bool start_frame_signal = false;
bool late_frame_alarm = false;
hw_timer_t *Frame_timer = NULL;

void IRAM_ATTR onTimer(){
  if(start_frame_signal){
    //last frame hasn't completed
    late_frame_alarm = true;
  }
  start_frame_signal = true;
}

Stopwatch sw = Stopwatch();


//------------------Setup---------------------------

void setup() {
  Serial.begin(500000); //initialise serial
  
  //setup timer. this will trigger 24x per second.
  Frame_timer = timerBegin(0, 80, true); //setup timer zero
  timerAttachInterrupt(Frame_timer, &onTimer, true); // attach timer to function
  timerAlarmWrite(Frame_timer, 41666, true); // set timer for every 1/24 second
  timerAlarmEnable(Frame_timer); //enable it.
  
  //interrupts for rotary encoder



  Serial.println("PUCE 900");
  setCpuFrequencyMhz(240);
  Serial.print("Clock Speed: ");
  Serial.println( getCpuFrequencyMhz() );
  initUserInput(); //set up user interface stuff
  initTFT(); //initialise TFT
  tft.println("_Puce 900_");
  sensorInit();//initialise sensor
  Serial.println("1.0");
  exposureDump();
  miniSDTest();//test the SD card
  EEPROM.begin(2); //setup eeprom to store the number of pics taken
  ui.shutter_count = EEPROM.read(0);


  delay(500); //delay so that debug info can be read

  //
  Stopwatch sw = Stopwatch();
  sw.start("OneK_Test");
  oneKTest(SD);
  sw.stop();
  Serial.println("2.0");
  exposureDump();
  tft.fillScreen(0);

}

//------------------Loop---------------------------

void loop() {
  loops ++;
    //----ui---------
  updateUserInput();
  
  if( getPlaybackSwitch() ){


    if ( getVideoModeSwitch() ){
      videoLoop();
    }
    else{
      cameraLoop();
    }
  }
  else{
    playbackLoop();
  }
  
}

int vid_frames = 0;
void videoLoop(){
  if(mode != MODE_VIDEO_CAMERA){
    //need to init video camera mode
    tft.fillScreen(0);
    tft.setCursor(3, 120);
    tft.print("video mode not yet enabled");

    vid_frames = 0;
    start_y4m("/test.y4m");
    Serial.println("Started video file!");

    mode = MODE_VIDEO_CAMERA;
  }

  //wait for the next frame
  while(!start_frame_signal){
    delayMicroseconds(1);
  }
  
  sw.start("video frame");
  //sensor
  readySensor();
  updateSensorStatus();
  sensor.frameCapture( sensor_buffer );
  flipBuffer();
  rotateBufferClockwise();
  setExposure();
  //tft
  readyTFT();
  scaleBuffer();
  dumpToScreen();
  sw.stop();

  if(vid_frames < 10000){
    y4m_add_frame();
    Serial.println("added frame!");
  }
  else if(vid_frames == 72){
    end_y4m();
  }
  vid_frames ++;

  if(late_frame_alarm){
    Serial.println("Late Frame!");
    late_frame_alarm = false;
  }
  start_frame_signal = false; //reset the flag

}

void playbackLoop(){
  int this_image = 1; //current image slectected, represented as the nth image on the card, ignoring irrelevant files 
  int image_count = countImages(SD, "/");

  Serial.print("There are ");
  Serial.print(image_count);
  Serial.println(" images.");
  /*
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
  */
}

void cameraLoop(){
  
  if(mode != MODE_STILL_CAMERA){
    //need to init still camera mode
    tft.fillScreen(0);
    setupHud();
    mode = MODE_STILL_CAMERA;
  }
  
  //----Sensor-----
  
  
  sw.start("prep_sensor");
  readySensor();
  updateSensorStatus(); //get status from the sensor
  sw.stop();

  
  sw.start("frame_capture");
  sensor.frameCapture( sensor_buffer ); // dump frame from sensor to 1-d array
  sensor.frameCapture( frame ); //dump frame from sensor to 2d array
  sw.stop();
  sw.start("frame_post-processing");
  //rotateFrame( SENSOR_ROTATION );
  
  flipBuffer();
  rotateBufferClockwise();

  setExposure();//update the exposure settings
  sw.stop();

  //----TFT--------
  //draw frame to screen
  sw.start("prep TFT");
  readyTFT();
  sw.stop();
  drawHud();
  
  
  // for (int i = 0; i < 900; i++){
  //   screen_buffer[i] = grayToColor(sensor_buffer[i]);
  // }
  sw.start("scale up");
  scaleBuffer();
  sw.stop();

  sw.start("dump to screen");
  //tft.drawRGBBitmap(0,0,screen_buffer, ADNS3080_PIXELS_X * DISP_SCALE, ADNS3080_PIXELS_X * DISP_SCALE );
  dumpToScreen();
  sw.stop();

  //----OUTPUT-----
  if( getShutterButton() ){
    //shutter has been pressed. Save JPEG to the SD card
    SPI.setClockDivider(SPI_CLOCK_DIV2 );
    sw.start("saving");
    fileNameFromNumber(ui.shutter_count);
    saveFrame(frame, next_filename );
    ui.shutter_count ++;
    EEPROM.write(0,ui.shutter_count);
    EEPROM.commit();
    sw.stop();
    
  }
  Serial.println();  
}


