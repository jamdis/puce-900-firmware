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

  //Serial.println("SD card opened");

  int i; //counter for main loop.  prefix 'i' used to denote other variables in the outer loop.
  int iMCUCount; //the number of MCUs in this image.  MCUs are "minimum coded unit" that is, one block of 8x8 pixels.
  int rc; //the result of the last JPEG operation
  int iDataSize; //the size of the resultant file
  JPEGENCODE jpe; //the encoder object
  uint8_t ucMCU[64]; //holds one MCU at a time. 1 dimensional array of 64 bytes, but represents 2 dimensional matrix of 8x8 bytes-- one byte per pixel.

  rc = jpg.open(filename, myOpen, myClose, myRead, myWrite, mySeek);  //start a new jpeg file
  if(rc == JPEG_SUCCESS){ //did it work?
    //Serial.println("Opened new Jpeg file successfully");
  }
  else{
    return false;
  }
  rc = jpg.encodeBegin(&jpe, width, height, JPEG_PIXEL_GRAYSCALE, JPEG_SUBSAMPLE_444, JPEG_Q_BEST); //create the encoder
  if(rc == JPEG_SUCCESS){ //did it work?
    //Serial.println("Created encoder successfully");
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
  //Serial.print("All done!  Output file size = ");
  //Serial.println( iDataSize );

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


//================JPEG Decoding==========================
//based on JPEG decoding library example by Bodmer
void drawFSJpeg(const char *filename, int xpos, int ypos) {

  //Serial.println("=====================================");
  //Serial.print("Drawing file: "); Serial.println(filename);
  //Serial.println("=====================================");

  // Open the file (the Jpeg decoder library will close it)
  //fs::File jpgFile = SPIFFS.open( filename, "r");    // File handle reference for SPIFFS
  File jpgFile = SD.open( filename, FILE_READ);  // or, file handle reference for SD library
 
  if ( !jpgFile ) {
    Serial.print("ERROR: File \""); Serial.print(filename); Serial.println ("\" not found!");
    return;
  }

  // To initialise the decoder and provide the file, we can use one of the three following methods:
  //boolean decoded = JpegDec.decodeFsFile(jpgFile); // We can pass the SPIFFS file handle to the decoder,
  boolean decoded = JpegDec.decodeSdFile(jpgFile); // or we can pass the SD file handle to the decoder,
  //boolean decoded = JpegDec.decodeFsFile(filename);  // or we can pass the filename (leading / distinguishes SPIFFS files)
                                                     // The filename can be a String or character array
  if (decoded) {
    // print information about the image to the serial port
    //jpegInfo();

    // render the image onto the screen at given coordinates
    jpegRender(xpos, ypos, DISP_SCALE);
  }
  else {
    Serial.println("Jpeg file format not supported!");
  }
}

void jpegRender(int xpos, int ypos, int scale) {

  // retrieve infomration about the image
  
  uint16_t  *pImg;
  uint16_t mcu_w = JpegDec.MCUWidth;
  uint16_t mcu_h = JpegDec.MCUHeight;
  uint32_t max_x = JpegDec.width;
  uint32_t max_y = JpegDec.height;
  

  // Jpeg images are draw as a set of image block (tiles) called Minimum Coding Units (MCUs)
  // Typically these MCUs are 16x16 pixel blocks
  // Determine the width and height of the right and bottom edge image blocks
  uint32_t min_w = minimum(mcu_w, max_x % mcu_w);
  uint32_t min_h = minimum(mcu_h, max_y % mcu_h);

  // save the current image block size
  uint32_t win_w = mcu_w;
  uint32_t win_h = mcu_h;
  uint16_t scaledImg[win_w * scale * win_h * scale];

  // record the current time so we can measure how long it takes to draw an image
  uint32_t drawTime = millis();

  // save the coordinate of the right and bottom edges to assist image cropping
  // to the screen size
  max_x += xpos;
  max_y += ypos;

  // read each MCU block until there are no more
  while ( JpegDec.read()) {

    // save a pointer to the image block
    pImg = JpegDec.pImage; 

    // calculate where the image block should be drawn on the screen
    int mcu_x = (JpegDec.MCUx * mcu_w) + xpos;
    int mcu_y = (JpegDec.MCUy * mcu_h) + ypos;

    // check if the image block size needs to be changed for the right edge
    if (mcu_x + mcu_w <= max_x) win_w = mcu_w;
    else win_w = min_w;

    // check if the image block size needs to be changed for the bottom edge
    if (mcu_y + mcu_h <= max_y) win_h = mcu_h;
    else win_h = min_h;

    // copy pixels into a contiguous block
    if (win_w != mcu_w)
    {
      for (int h = 1; h < win_h-1; h++)
      {
        memcpy(pImg + h * win_w, pImg + (h + 1) * mcu_w, win_w << 1);
      }
    }

    //copy and scale pixels
    for(int i = 0; i < win_w * scale * win_w * scale; i++){    
     
      int x = (i % (win_w * scale)) / scale; //x coord of the pixel we want
      int y = (i / (win_h * scale)) / scale; //y coord of the pixel we want
      
      
      int pimg_index = x + y * win_w;
      scaledImg[i] = pImg [pimg_index];
      
      //scaledImg[i] = pImg [1];
      //scaledImg[i] = grayToColor(i);
    }

    // draw image MCU block 
    tft.drawRGBBitmap(mcu_x * scale, mcu_y * scale, scaledImg, win_w * scale, win_h *scale);
  }

  // calculate how long it took to draw the image
  drawTime = millis() - drawTime;

  // print the results to the serial port
  //Serial.print  ("Total render time was    : "); Serial.print(drawTime); Serial.println(" ms");
  //Serial.println("=====================================");

}
