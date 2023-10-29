

uint8_t setBit( uint8_t number, uint8_t n, bool set_to ) {
  //set the nth bit of a byte
  number ^= ( -set_to ^ number ) & (1U << n );
  return number;
}


bool getBit( uint8_t number, uint8_t n ) {
  //Get bit n from a byte
  bool bit;
  
  bit = (number >> n) & 1U;
  
  return bit;
}

uint8_t countDigits(int num){
  //count the number of base 10 digits in a number
  uint8_t count = 0;
  while(num)
  {
    num = num / 10;
    count++;
  }
  return count;
}


//
// Writes an uint32_t in Big Endian at current file position
//
static void inline print_quartet(unsigned long i, File fd) {

  uint8_t y[4];
  y[0] = i % 0x100;
  y[1] = (i >> 8) % 0x100;
  y[2] = (i >> 16) % 0x100;
  y[3] = (i >> 24) % 0x100;
  size_t i1_err = fd.write(y , 4);
}