void sensorInit(){
  bool sensor_ok = sensor.setup(false, false); //initialise sensor
  bool id_test = sensor.testProductID();
  uint8_t rev_id = sensor.getRevisionID();
  
  // sensor.setManualFrameRate(true);
  // sensor.setManualShutter(true);
  // sensor.setShutterMaxBound(0x0010); 
  // sensor.setFramePeriodMinBound(0x0B54); //lowest possible
  // sensor.setFramePeriodMaxBound(0xFFFF);  //highest possible

  
  if( sensor_ok ){
    tPrintln("Sensor OK.");
  }
  else{
    tPrintln("Sensor NOT OK.");
  }

  if( id_test ){
   tPrint(" id OK.");
  }
  else{
    tPrint(" id NOT OK.");
  }
  tPrint( " rev: " );
  tPrintln( sensor.getRevisionID() );

}

void readySensor(){
  //set SPI stuff so as not to confuse the poor sensor
  //this must be called whenever you switch from using other SPI devices to using the sensor
  //otherwise sensor may read back garbage
  SPI.setClockDivider( SPI_CLOCK_DIV32 ); 
  SPI.setDataMode( SPI_MODE3 );
}

void rotateFrame( int x ){
  //rotate frame clockwise in place x times
  for (int i =0; i < x; i++){
    rotateClockWise(frame);
  }
}

void rotateClockWise(uint8_t arr[ADNS3080_PIXELS_X][ADNS3080_PIXELS_X]){
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



