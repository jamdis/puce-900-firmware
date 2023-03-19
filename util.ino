

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