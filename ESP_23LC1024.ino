/*
   Used the following components and wire routing:
   (1) ESP8266 - WeMOS D1 Mini
   (2) Microchip 23LC1024
   (3) 10K Resistor pull-up for HOLD/WP
   Connections:
     WeMOS  - 23LC1024
  GPIO14  D5  - 6 - SCK
  GPIO12  D6  - 2 - MISO
  GPIO13  D7  - 5 - MOSI
  GPIO0   D3  - 1 - CS
  (D3 0n WeMos has 10k pull-up)
  (https://www.wemos.cc/product/d1-mini.html)
*/
#include <SPI.h>
#define CS D3

//SRAM opcodes
#define RDSR    0x05
#define WRSR    0x01
#define READ    0x03
#define WRITE   0x02

//Byte transfer functions
uint8_t Spi23LC1024ReadR() {
  uint8_t read_byte;
  //  PORTB &= ~(1<<PORTB2);        //set SPI_SS low
  digitalWrite(CS, LOW);
  SPI.transfer(RDSR);
  read_byte = SPI.transfer(0x00);
  //  PORTB |= (1<<PORTB2);         //set SPI_SS high
  digitalWrite(CS, HIGH);
  return read_byte;
}

uint8_t Spi23LC1024Read8(uint32_t address) {
  uint8_t read_byte;
  //  PORTB &= ~(1<<PORTB2);        //set SPI_SS low
  digitalWrite(CS, LOW);
  SPI.transfer(READ);
  SPI.transfer((uint8_t)(address >> 16) & 0xff);
  SPI.transfer((uint8_t)(address >> 8) & 0xff);
  SPI.transfer((uint8_t)address);
  read_byte = SPI.transfer(0x00);
  //  PORTB |= (1<<PORTB2);         //set SPI_SS high
  digitalWrite(CS, HIGH);
  return read_byte;
}

void Spi23LC1024Write8(uint32_t address, uint8_t data_byte) {
  //  PORTB &= ~(1<<PORTB2);        //set SPI_SS low
  digitalWrite(CS, LOW);
  SPI.transfer(WRITE);
  SPI.transfer((uint8_t)(address >> 16) & 0xff);
  SPI.transfer((uint8_t)(address >> 8) & 0xff);
  SPI.transfer((uint8_t)address);
  SPI.transfer(data_byte);
  //  PORTB |= (1<<PORTB2);         //set SPI_SS high
  digitalWrite(CS, HIGH);
}

void setup(void) {
  uint32_t i;
  uint8_t value;
  uint8_t RDMR;
  pinMode(CS, OUTPUT);
  digitalWrite(CS, HIGH);
  Serial.begin(74880);
  Serial.println(" ");
  Serial.println("Serial Start.");
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);
  SPI.setFrequency(1000000);

  RDMR = Spi23LC1024ReadR();
  Serial.print("  Read Mode Register: ");
  print_binary(RDMR, 8);
  Serial.println("");
  switch (RDMR) {
    case 0:
      Serial.println("  RDMR Byte mode.");
      break;
    case 1:
      Serial.println("  RDMR Sequential mode.");
      break;
    case 2:
      Serial.println("  RDMR Page mode.");
      break;
    case 3:
      Serial.println("  Reserved");
      break;
  }

  for (i = 0; i < 16; i++) {
    Spi23LC1024Write8(i, (uint8_t)i);
    value = Spi23LC1024Read8(i);
    Serial.print("Address: ");
    Serial.println(i, HEX);
    Serial.print("  DEC: ");
    Serial.print(value, DEC);
    Serial.print(", HEX: ");
    Serial.println(value, HEX);
  }
}

void loop() {
}

void print_binary(int v, int num_places)
{
  int mask = 0, n;

  for (n = 1; n <= num_places; n++)
  {
    mask = (mask << 1) | 0x0001;
  }
  v = v & mask;  // truncate v to specified number of places

  while (num_places)
  {

    if (v & (0x0001 << num_places - 1))
    {
      Serial.print("1");
    }
    else
    {
      Serial.print("0");
    }

    --num_places;
    if (((num_places % 4) == 0) && (num_places != 0))
    {
      Serial.print("_");
    }
  }
}
