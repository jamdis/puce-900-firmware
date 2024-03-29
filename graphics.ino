void initTFT(){
  tft.initR(SCREEN_COLORTAB);  //initialise screen
  tft.setRotation(SCREEN_ROTATION); //set screen rotation
  tft.fillScreen(0b100110100001110011); //Background colour. note that red and blue seem to be reversed vs what is on adafruit docs
  tft.fillScreen(0);
  tft.setTextColor(0xFFFF, 0x0000);
}

void readyTFT(){
  //call this to get the screeen ready after using other SPI devices, otherwise drawing to the screen will be slow
  SPI2.setFrequency(TFT_SPI_SPEED);
}

void dumpToScreen(){
  tft.startWrite();
  tft.setAddrWindow(0, 0, ADNS3080_PIXELS_X * DISP_SCALE, ADNS3080_PIXELS_Y * DISP_SCALE);
  tft.writePixels(screen_buffer, ADNS3080_PIXELS_X * DISP_SCALE * ADNS3080_PIXELS_Y * DISP_SCALE, true, false);
  tft.endWrite();
 
}

void drawAdvancedMonitor(){

  
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

}

void setupHud(){
  tft.drawRect(125,2,28,24,0xFFFF);
  tft.setCursor(127,26);
  tft.print("pics");

  tft.drawRect(125,40,28,24,0xFFFF);
  tft.setCursor(130,64);
  tft.print("exp");

}

uint16_t bob = 0;

void drawHud(){
  int old_cursor_x = tft.getCursorX();
  int old_cursor_y = tft.getCursorY();
  if(ADVANCED_MONITOR){
     drawAdvancedMonitor();
  }
  else{
    int center = 139 - (countDigits( ui.shutter_count ) * 3 );
    tft.setCursor(center,10);
    tft.print(ui.shutter_count);

    
    if(ss.manual_exposure){
      tft.fillRect(126,41,26,22,0);
      center = 139 - (sizeof(exposure_pretty[ui.exposure_index]) * 3 );
      //center = 130;
      tft.setCursor(center,48);
      tft.print( exposure_pretty[ui.exposure_index] );
    }
    else{
      tft.setCursor(127,48);
      tft.print("auto");
    }
  }
  tft.setCursor(old_cursor_x, old_cursor_y);
  bob ++;

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