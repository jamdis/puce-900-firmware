void initTFT(){
  tft.initR(INITR_BLACKTAB);  //initialise screen
  tft.setRotation(SCREEN_ROTATION); //set screen rotation
  tft.fillScreen(0b100110100001110011); //Background colour. note that red and blue seem to be reversed vs what is on adafruit docs
  tft.fillScreen(0);
  tft.setTextColor(0xFFFF, 0x0000);
}

void readyTFT(){
  //call this to get the screeen ready after using other SPI devices
  //otherwise drawing to the screen will be slow
  //SPI.setClockDivider( SPI_CLOCK_DIV2 );
  SPI.setFrequency(80000000);
}

void dumpToScreen(){
  
 
  tft.startWrite();
  tft.setAddrWindow(0, 0, ADNS3080_PIXELS_X * DISP_SCALE, ADNS3080_PIXELS_Y * DISP_SCALE);
  //digitalWrite(TFT_DC, HIGH);
  //digitalWrite(TFT_CS, LOW);
  tft.writePixels(screen_buffer, ADNS3080_PIXELS_X * DISP_SCALE * ADNS3080_PIXELS_Y * DISP_SCALE, true, false);
  tft.endWrite();
  //digitalWrite(TFT_CS, HIGH);  
}


void drawFrameToScreen(){
  //Draw frame to screen
  for(int x = 0; x < ADNS3080_PIXELS_X; x += 1 ) {
    for(int y = 0; y < ADNS3080_PIXELS_Y; y += 1 ) {
      uint8_t pix_value = frame[x][y];
      pucePixel(ADNS3080_PIXELS_X - x - 1,y,frame[x][y],PIXEL_SIZE); //flipping horizontally for the screen.  TODO: fix this for real.
    }
  }
}

void drawHud(){
  int old_cursor_x = tft.getCursorX();
  int old_cursor_y = tft.getCursorY();
  
  tft.setCursor(121,20);
  tft.print("ME:");
  if( getMeSwitch() ){
    tft.write(0x18);
  }
  else{
    tft.write(0x19);
  }

  if( ss.manual_exposure ){
    tft.write(0x18);
  }
  else{
    tft.write(0x19);
  }


  tft.setCursor(121,28);
  tft.print("Sh:");
  if( getShutterButton() ){
    tft.write(0x0f);
  }
  else{
    tft.write(0xEF);
  }
  tft.setCursor(121,36);
  tft.print("SC:");
  tft.print(ui.shutter_count);
  tft.setCursor(121,44);
  tft.print("EP:");
  tft.print(getExpPot(), HEX);

  barGraph(125,55,10,60,getExpPot(),0xFFFF);



  tft.setCursor(old_cursor_x, old_cursor_y);

}

void barGraph( uint x, uint y, uint w, uint h, uint value, uint max ){
  tft.fillRect(x, y, w, h, 0x0000);
  tft.drawRect(x, y, w, h, 0xFFFF);
  int barh = map(value, 0, 4096, 0, h);
  tft.fillRect(x, y + ( h - barh ), w, barh, 0xFFFF);



  
}


uint16_t grayToColor(byte gray){
  //Convert an 8 bit gray value to the 16 bit color expected by Adafruit Graphics library
  uint16_t red = gray >> 3;
  uint16_t green = (gray >>2) << 5;
  uint16_t blue = (gray >>3) << 11;
  uint16_t output =  blue | red | green;
  return output;  
}

void pucePixel(uint16_t x0, uint16_t y0, byte value, byte px_size){
  //draw a pixel to the screen
  tft.fillRect(x0 * px_size, y0 * px_size, px_size, px_size,grayToColor(value));
}