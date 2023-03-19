void nextAvailableFileName(){
  fileNameFromNumber(ui.shutter_count);
    while( fileExists( SD, next_filename )){ //check for next available filename
      ui.shutter_count ++;
      fileNameFromNumber(ui.shutter_count);
    }    
}

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

void fileNameFromNumber(int number){
  //next_filename = "0000000.jpg";

  int place = 1;
  int position = 6;
  while(number > 0){
    int value = ( number % (place * 10) );
    int digit = value / place;
    next_filename[position] = digit + 48;
    number = number - value;
    place = place * 10;
    position = position - 1;
  }

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

void listDir(fs::FS &fs, const char * path){
  File dir = fs.open(path);
  while (true){
    File next = dir.openNextFile();
    if (! next){
      break; //no more files
      }
    tPrintln(next.name());
  }
}

uint16_t countImages(fs::FS &fs, const char * path){
  File dir = fs.open(path);
  uint16_t count = 0;
  while (true){
    File next = dir.openNextFile();
    if (! next){
      return count;
      }
    
    if( next.name()[1] != 46 && next.name()[10] == 103){ // only files that do not start with "." and which DO end with "g" as in jpg
      count ++;
    }
  }
}

const char* getFileNameByCount(fs::FS &fs, const char * path, uint16_t count_no){
  File dir = fs.open(path);
  uint16_t count = 0;
  while (true){
    File next = dir.openNextFile();
    if (! next){
      return ("No files found");
      }
    if( next.name()[1] != 46 && next.name()[10] == 103){ // only files that do not start with "." and which DO end with "g" as in jpg
      if(count == count_no){
        return next.name();
      }
      count ++;
    }
  }
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
  tPrintln("");
  //deleteFile(SD, "/card_test.txt");
}