void sensorInit(){
    bool sensor_ok = sensor.setup(false, false); //initialise sensor
    bool id_test = sensor.testProductID();
    uint8_t rev_id = sensor.getRevisionID();
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
  sensor.setFramePeriodMaxBound(0x12C0);
  //sensor.setExposure(true, true, 0x2EE0, 0x7E0E, 0x8C20);
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

void compareAndChangeSensor(){
  // if (ss.manual_exposure != ui.me_switch){
  //   sensor.setManualShutter(ui.me_switch);
  // }

  uint16_t new_max_shutter;
  //'max_shutter' is the max in auto mode, but the actual shutter speed when in manual.
  //We set it to 0xFFFF here because we want to default to the slowest speed possible.
  if(ui.me_switch){
    new_max_shutter = map(ui.exposure_pot, 0, 0xFFF, 0, 0xFFFF);
  }
  else{
    new_max_shutter = 0xFFFF;
  }

  //sensor.setExposure(true,ui.me_switch,0x2EE0, 0x0B55, new_max_shutter);
  

  //sensor.setManualShutter(ui.me_switch);
  //sensor.setExposure(bool manual_fp, bool manual_shutter, uint16_t frame_period_max, uint16_t frame_period_min, uint16_t shutter_max)

}
