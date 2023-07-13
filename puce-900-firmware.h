//--------------------------Config------------------------------------
#define SCREEN_ROTATION    3
#define SENSOR_ROTATION    3
#define SHUTTER_COOLDOWN   500
#define DISP_SCALE         4
#define SCREEN_COLORTAB    INITR_BLACKTAB 


//--------------------------Libraries---------------------------------
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h> //SPI library
#include <ADNS3080.h> //ADNS3080 library
#include "FS.h" //Filestystem library
//#include "SD.h" //SD card library
#include <SD.h>

#include <JPEGENC.h> //JPEG Encoder library
#include <JPEGDecoder.h> //JPEG Decoder library
#include <EEPROM.h> //Eeprom library to save persistant data

//--------------------------Pins--------------------------------------

#define TFT_CS             15
#define TFT_RST            4         
#define TFT_DC             2 
#define ADNS_PIN_RESET     16 
#define ADNS_PIN_CS        17          

#define ME_SWITCH          21      
#define SHUTTER_BUTTON     27       
#define EXPOSURE_POT       36
#define PLAYBACK_SWITCH    25
#define VIDEO_MODE_SWITCH  26

//--------------------------CONSTS-------------------------------------
#define MODE_STILL_CAMERA       0
#define MODE_VIDEO_CAMERA       1
#define MODE_PLAYBACK           2

#define TFT_SPI_SPEED           80000000
#define SENSOR_SPI_SPEED        2962000

//-------------------------Core Objects--------------------------------
SPIClass SPI2(HSPI);
Adafruit_ST7735 tft = Adafruit_ST7735(&SPI2, TFT_CS, TFT_DC, TFT_RST); //Create the TFT screen object
ADNS3080 <ADNS_PIN_RESET, ADNS_PIN_CS> sensor; //Create the ADNS sensor object

JPEG jpg; //jpeg encoder
static File myfile; //file system

//-------------------------Class declarations--------------------------------
class Stopwatch{
  public:
    Stopwatch();
    void start( char* message );
    void stop();
    long start_time;
    char* message; 
  private:
};