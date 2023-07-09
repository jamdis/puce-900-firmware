void sensorInit(){
  sensor.setSPI(&SPI2);
  bool sensor_ok = sensor.setup(false, false); //initialise sensor
  bool id_test = sensor.testProductID();
  uint8_t rev_id = sensor.getRevisionID();
  
  

  if( sensor_ok ){
    tPrintln("Sensor OK.");
  }
  else{
    tPrintln("Sensor NOT OK.");
  }
  tPrint("ID: ");
  tPrint(sensor.getProductID());

  if( id_test ){
   tPrint(" id OK.");
  }
  else{
    tPrintln(" id NOT OK.");
  }
  tPrint( " rev: " );
  tPrintln( sensor.getRevisionID() );


}

void readySensor(){
  Serial.println("hello");
  //set SPI stuff so as not to confuse the poor sensor
  //this must be called whenever you switch from using other SPI devices to using the sensor
  //otherwise sensor may read back garbage
  SPI2.setClockDivider( SPI_CLOCK_DIV8); //Faster than SPI_CLOCK_DIV32 is overclocking past what the ADNS library wants. faster speeds seem stable so far...
  SPI2.setDataMode( SPI_MODE3 );
  SPI2.setBitOrder( MSBFIRST );
  //SPI2.setFrequency(100);// set speed?
  Serial.print("spi clock speed: ");
  Serial.println( SPI2.getClockDivider() );
  
  
  
}

void scaleBuffer(){
  uint screen_x_size = ADNS3080_PIXELS_X * DISP_SCALE;
  uint screen_y_size = ADNS3080_PIXELS_Y * DISP_SCALE;
  
  
  for( int i = 0; i < ADNS3080_PIXELS_X *DISP_SCALE * ADNS3080_PIXELS_Y * DISP_SCALE; i ++){
    int i_x = i % screen_y_size;
    int i_y = i / screen_x_size;
    int buffer_x = i_x / DISP_SCALE;
    int buffer_y = i_y / DISP_SCALE;
    
    screen_buffer[i] = grayToColor( sensor_buffer[ buffer_y * ADNS3080_PIXELS_X + buffer_x ] );
  }

}


void rotateFrame( int x ){
  //rotate frame clockwise in place x times
  for (int i =0; i < x; i++){
    rotateClockWise(frame);
  }
}

void flipBuffer(){
  uint8_t temp_buffer[ADNS3080_PIXELS_X][ADNS3080_PIXELS_Y];
  //read into temp buffer while flipping
  int i = 0;
  for( int x = 29; x >= 0; x--){
    for( int y =0; y < 30; y++){
      temp_buffer[x][y] = sensor_buffer[i];
      i++;
    }
    
  }

  //read back into sensor buffer
  i = 0;
  for( int x = 0; x < 30; x++){
    for( int y =0; y < 30; y++){
      sensor_buffer[i] = temp_buffer[x][y];
      i++;
    }
    
  }

}


void rotateBuffer180(){
  uint8_t temp_buffer[ADNS3080_PIXELS_X * ADNS3080_PIXELS_Y];
  
  for (int i = 0; i < ADNS3080_PIXELS_X * ADNS3080_PIXELS_Y; i ++){
    temp_buffer[i] = sensor_buffer[ADNS3080_PIXELS_X * ADNS3080_PIXELS_Y - i];
  }

}

void rotateBufferClockwise(){
  uint8_t temp_buffer[ADNS3080_PIXELS_X][ADNS3080_PIXELS_Y];
  //read into temp buffer
  int i = 0;
  for( int x = 0; x < 30; x++){
    for( int y =0; y < 30; y++){
      temp_buffer[x][y] = sensor_buffer[i];
      i++;
    }
    
  }

  rotateClockWise(temp_buffer); //rotate

  //read back into sensor buffer
  i = 0;
  for( int x = 0; x < 30; x++){
    for( int y =0; y < 30; y++){
      sensor_buffer[i] = temp_buffer[x][y];
      i++;
    }
    
  }

}


void rotateClockWise(uint8_t arr[ADNS3080_PIXELS_X][ADNS3080_PIXELS_Y]){
  //rotates a frame clockwise in place
   for(int i = 0; i < ADNS3080_PIXELS_X; i++){
      for(int j = 0; j < ADNS3080_PIXELS_X - i; j++){
         int ptr = arr[i][j];
         arr[i][j] = arr[ADNS3080_PIXELS_X - 1 - j][ADNS3080_PIXELS_X - 1 - i];
         arr[ADNS3080_PIXELS_X - 1 - j][ADNS3080_PIXELS_X - 1 - i] = ptr;
      }
   }
   for(int i = 0; i < ADNS3080_PIXELS_X / 2; i++){
      for(int j = 0; j < ADNS3080_PIXELS_X; j++){
         int ptr = arr[i][j];
         arr[i][j] = arr[ADNS3080_PIXELS_X - 1 - i][j];
         arr[ADNS3080_PIXELS_X - 1 - i][j] = ptr;
      }
   }
}


void updateSensorStatus(){
  //update global vars from acutal info from sensor
  ss.manual_exposure = sensor.getManualShutter();
  ss.shutter = sensor.getShutter();
  ss.suttter_max_bound = sensor.getShutterMaxBound();
}

void setExposure(){
  sensor.reset(); // seems to prevent perma-busy if we reset before we start changing exposure.
  sensor.setManualShutter(ui.me_switch); //set auto/manual based upon the position of the switch
  sensor.setManualFrameRate(ui.me_switch); //same for frame period

  if(ui.me_switch){
    //manual mode. set shutter based upon setting on the potentiometer
    uint16_t set_shutter_to = map( ui.exposure_pot, 0x000, 0xFFF, 0x00, 0xFFFF);
    sensor.setShutterMaxBound(set_shutter_to);
  }
  else{
    //auto mode. set shutter to max to allow full range.
    sensor.setShutterMaxBound(0xFFFF);
  }
  //frame period min and max set to widest possible range regardless of manual or auto:
  sensor.setFramePeriodMinBound(0xB54); //lowest possible
  sensor.setFramePeriodMaxBound(0xFFFF);//highest possible. Setting this activates the other settings


}



