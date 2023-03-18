  void tPrint(char* message ){
  Serial.print(message);
  tft.print(message);
}

void tPrint(int message ){
  Serial.print(message);
  tft.print(message);
}

void tPrintln(char* message ){
  Serial.println(message);
  tft.println(message);
}

void tPrintln(int message ){
  Serial.println(message);
  tft.println(message);
}

void tWrite(int message ){
  Serial.write(message);
  tft.write(message);
}

void dumpVar( char* message, int var, int type = DEC ){
  Serial.print(message);
  Serial.print(": ");
  Serial.print(var, type);
  Serial.print("\t");
}


void exposureDump(){
  //dump registers relating to exposure
  unsigned long t = millis();
  dumpVar("ext_config", sensor.getExtConfig(), BIN);
  dumpVar("busy", sensor.getBusy() );
  dumpVar("manual_fr", sensor.getManualFrameRate());
  dumpVar("manual_shut", sensor.getManualShutter());
  dumpVar("fp", sensor.getFramePeriod(), HEX );
  dumpVar("fp_min", sensor.getFramePeriodMinBound(), HEX);
  dumpVar("fp_max", sensor.getFramePeriodMaxBound(), HEX );
  dumpVar("shut", sensor.getShutter(), HEX );
  dumpVar("shut_max", sensor.getShutterMaxBound(), HEX );
  dumpVar("Δt", millis() - t );
  Serial.println();

}

class Stopwatch{
  public:
    Stopwatch();
    void start( char* message );
    void stop();
    long start_time;
    char message[126]; 
  private:
};

Stopwatch::Stopwatch(){
}

void Stopwatch::start( char* message ){  
}

