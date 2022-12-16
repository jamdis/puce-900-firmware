//uses the JPEG encoder library by Larry Bank
//and is based on his example code

//------------Callback functions for unzipLIB--------------------------
// Callback functions needed by the unzipLIB to access a file system
// The library has built-in code for memory-to-memory transfers, but needs
// these callback functions to allow using other storage media

void * myOpen(const char *filename) {
  myfile = SD.open(filename, FILE_WRITE);
  return (void *)&myfile;
}
void myClose(JPEGFILE *p) {
  File *f = (File *)p->fHandle;
  if (f) f->close();
}
int32_t myRead(JPEGFILE *p, uint8_t *buffer, int32_t length) {
  File *f = (File *)p->fHandle;
  return f->read(buffer, length);
}

int32_t myWrite(JPEGFILE *p, uint8_t *buffer, int32_t length) {
  File *f = (File *)p->fHandle;
  return f->write(buffer, length);
}

int32_t mySeek(JPEGFILE *p, int32_t position) {
  File *f = (File *)p->fHandle;
  return f->seek(position);
}

//--------------saveFrame Function-----------------
//this is what you call when you want to save the current frame as a jpeg.

bool saveFrame( uint8_t frame[ADNS3080_PIXELS_X][ADNS3080_PIXELS_Y], const char* filename ){
  const int width = 30;
  const int height = 30;

  //open the SD card
  if( !SD.begin(5) ){
    Serial.println("Unable to access SD Card");
    return false;
  }
  Serial.println("SD card opened");

  int i; //counter for main loop.  prefix 'i' used to denote other variables in the outer loop.
  int iMCUCount; //the number of MCUs in this image.  MCUs are "minimum coded unit" that is, one block of 8x8 pixels.
  int rc; //the result of the last JPEG operation
  int iDataSize; //the size of the resultant file
  JPEGENCODE jpe; //the encoder object
  uint8_t ucMCU[64]; //holds one MCU at a time. 1 dimensional array of 64 bytes, but represents 2 dimensional matrix of 8x8 bytes-- one byte per pixel.

  rc = jpg.open(filename, myOpen, myClose, myRead, myWrite, mySeek);  //start a new jpeg file
  if(rc == JPEG_SUCCESS){ //did it work?
    Serial.println("Opened new Jpeg file successfully");
  }
  else{
    return false;
  }
  rc = jpg.encodeBegin(&jpe, width, height, JPEG_PIXEL_GRAYSCALE, JPEG_SUBSAMPLE_444, JPEG_Q_BEST); //create the encoder
  if(rc == JPEG_SUCCESS){ //did it work?
    Serial.println("Created encoder successfully");
  }
  else{
    return false;
  }
  
  memset(ucMCU, 0, sizeof(ucMCU)); //fill the current MCU with 0's, thus making it all black
  iMCUCount = ((width + jpe.cx-1)/ jpe.cx) * ((height + jpe.cy-1) / jpe.cy); //figure out how many MCUs we'll have to get

  int x = 0, y = 0; //coordinates in the frame data

  for (i=0; i<iMCUCount && rc == JPEG_SUCCESS; i++) { //for each MCU that we need
    getMCUfromFrame( x, y, frame, ucMCU);                
    rc = jpg.addMCU(&jpe, ucMCU, 8);
    x += 8;
    if( x > width){
      y += 8;
      x = 0;
    }
  }
  iDataSize = jpg.close(); //it is finished.
  Serial.print("All done!  Output file size = ");
  Serial.println( iDataSize );

}

//--------------supporting functions--------------------------
//functions that help break down the 2D array that holds the current frame
//into a series of arrays in the size expected by the JPEG encoder

void getMCUfromFrame(int x, int y, uint8_t frame[ADNS3080_PIXELS_X][ADNS3080_PIXELS_Y], uint8_t mcu[64]){
  //given the upper-left corner's coords, get an MCU from fame data
  //static uint8_t mcu[64];
  //memset(mcu, 0, sizeof(mcu)); //set to all zeroes
  int h = 0;
  for ( int i = y; i < y + 8; i++ ){
    for ( int j = x; j < x + 8; j++ ){
      if( i < ADNS3080_PIXELS_Y && j < ADNS3080_PIXELS_X){
        mcu[ h ] = frame[ j ][ i ];
      }
      h++;      
    }
  }
}