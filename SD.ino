bool fileExists( fs::FS &fs, const char * path ){
  File file = fs.open(path);
  Serial.print( path );
  if(!file){
    Serial.println(" does not exist");
    return false;
  }
  Serial.println(" exists");
  return true;
}

void fileNameFromNumber(char *str, int number){
  char out[12];
  sprintf(out, "/%d.jpg", number);
  str = out;
}

void readFile(fs::FS &fs, const char * path){
  File file = fs.open(path);
  if(!file){
    tPrintln("Failed to open file for reading");
    return;
  }
  while(file.available()){
    tWrite(file.read());
  }
  file.close();
}

void writeFile(fs::FS &fs, const char * path, const char * message){

  File file = fs.open(path, FILE_WRITE);
  if(!file){
    tPrintln("Failed to open file for writing");
    return;
  }
  if(! file.print(message)){
    tPrintln("Write failed");
  }
  file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message){
  File file = fs.open(path, FILE_APPEND);
  if(!file){
    tPrintln("Failed to open file for appending");
    return;
  }
  if(!file.print(message)){
    tPrintln("Append failed");
  }
  file.close();
}

void deleteFile(fs::FS &fs, const char * path){
  if(!fs.remove(path)){
    tPrintln("Delete failed");
  }
}

void miniSDTest(){
  tPrint("SD Card: ");
  if(!SD.begin(5)){
    tPrintln("Mount Failed.");
    return;
  }
  uint8_t cardType = SD.cardType();

  if(cardType == CARD_NONE){
    tPrintln("No SD card.");
    return;
  }
  
  writeFile(SD, "/card_test.txt", "Ok");
  readFile(SD, "/card_test.txt");
  deleteFile(SD, "/card_test.txt");
}