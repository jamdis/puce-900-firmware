//--------------------------Config------------------------------------
#define SCREEN_ROTATION    1
#define SENSOR_ROTATION    0
#define SHUTTER_COOLDOWN   500


//--------------------------Libraries---------------------------------
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h> //SPI library
#include <ADNS3080.h> //ADNS3080 library
#include "FS.h" //Filestystem library
#include "SD.h" //SD card library
#include <JPEGENC.h> //JPEG Encoder library

//--------------------------Pins--------------------------------------
//for the tft screen
#define TFT_CS             15
#define TFT_RST            12
#define TFT_DC             2
#define ADNS_PIN_RESET     13        
#define ADNS_PIN_CS        4
#define AE_SWITCH          32
#define SHUTTER_BUTTON     34
#define EXPOSURE_POT       35

//-------------------------Core Objects--------------------------------
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST); //Create the TFT screen object
ADNS3080 <ADNS_PIN_RESET, ADNS_PIN_CS> sensor; //Create the ADNS sensor object
JPEG jpg; //jpeg encoder
static File myfile; //file system
