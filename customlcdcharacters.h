

byte chararray[][8] = {
  {0, 0, 0, 0, 0, 0, 0, 0}, //no info char
  { //wifi
    0b00010,
    0b01001,
    0b00101,
    0b10101,
    0b00101,
    0b01001,
    0b00010,
    0b00000
  },
  { //position
    0b00000,
    0b01110,
    0b11111,
    0b11011,
    0b11111,
    0b01110,
    0b00100,
    0b00000
  },
  { //weather
    0b11100,
    0b11000,
    0b10000,
    0b00010,
    0b01001,
    0b01000,
    0b00000,
    0b00000
  },
  { //temperature
    0b00100,
    0b01110,
    0b01010,
    0b01010,
    0b01010,
    0b01110,
    0b01110,
    0b11111
  },
  { //humidity
    0b10010,
    0b01001,
    0b10010,
    0b01000,
    0b00101,
    0b10010,
    0b01000,
    0b10101
  },
  { //wind
    0b00110,
    0b01001,
    0b00001,
    0b11110,
    0b00000,
    0b11110,
    0b00010,
    0b01100
  },
  { //clouds
    0b00000,
    0b00000,
    0b01101,
    0b11111,
    0b11111,
    0b00110,
    0b00000,
    0b00000
  },
  { //rain
    0b00000,
    0b00100,
    0b01110,
    0b11111,
    0b11111,
    0b11111,
    0b01110,
    0b00000
  },
  { //snow
    0b00000,
    0b10101,
    0b01110,
    0b11111,
    0b01110,
    0b10101,
    0b00000,
    0b00000
  },
};
