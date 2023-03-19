//--------------------------Config------------------------------------
#define SCREEN_ROTATION    1
#define SENSOR_ROTATION    3
#define SHUTTER_COOLDOWN   500
#define PIXEL_SIZE         4
#define DISP_SCALE         4


//--------------------------Libraries---------------------------------
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h> //SPI library
#include <ADNS3080.h> //ADNS3080 library
#include "FS.h" //Filestystem library
#include "SD.h" //SD card library
#include <JPEGENC.h> //JPEG Encoder library
#include <JPEGDecoder.h> //JPEG Decoder library
#include <EEPROM.h> //Eeprom library to save persistant data

//--------------------------Pins--------------------------------------

#define TFT_CS             15
#define TFT_RST            4         
#define TFT_DC             2 
#define ADNS_PIN_RESET     16 
#define ADNS_PIN_CS        17          

#define ME_SWITCH          14      
#define SHUTTER_BUTTON     27       
#define EXPOSURE_POT       13
#define PLAYBACK_SWITCH    12

//--------------------------CONSTS-------------------------------------
#define MODE_STILL_CAMERA        0
#define MODE_VIDEO_CAMERA        1
#define MODE_PLAYBACK            2


//-------------------------Core Objects--------------------------------
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST); //Create the TFT screen object
ADNS3080 <ADNS_PIN_RESET, ADNS_PIN_CS> sensor; //Create the ADNS sensor object
JPEG jpg; //jpeg encoder
static File myfile; //file system
