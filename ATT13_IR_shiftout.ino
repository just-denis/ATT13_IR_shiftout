/**
See README.md for intro.
More in-line comments to be added...
 */

#include "ir.h" // IR_IN_PIN = PB1

// https://docs.arduino.cc/tutorials/communication/guide-to-shift-out/
uint8_t const LATCH_PIN = PB3;
uint8_t const CLOCK_PIN = PB2;
uint8_t const DATA_PIN = PB0;

uint8_t data = 0;

#define USE_LED

#ifdef USE_LED
  #define LED_PIN PB4
  #define BLINK_STEP_MS 50
#else
  #define blink() // a 'dumb' stub in case we don't use led
#endif

#define USE_EEPROM

#ifdef USE_EEPROM
  #include "eeprom_rw.h"
  #define EEPROM_ADDRESS 0
#endif //USE_EEPROM

void send_data();

int main() {
  // setup
  //====================

  // setting up shift out registers to communicate with 595; https://arduino.ru/Reference/ShiftOut
  DDRB |= (1 << LATCH_PIN) | (1 << CLOCK_PIN) | (1 << DATA_PIN); 

  #ifdef USE_LED  
    DDRB |= 1 << LED_PIN; // LED OUTPUT
    PORTB &= ~(1<<LED_PIN); // LED OFF
  #endif

  IR_init();

  #ifdef USE_EEPROM
    data = EEPROM_read(EEPROM_ADDRESS);
  #endif

  send_data();

  // loop
  //====================
  while (1) {
    uint8_t addr, cmd;
    static bool echo = true;
    bool send = true;

    if (IR_read(&addr, &cmd) == IR_SUCCESS ) { //&& old_cmd != cmd
      switch (cmd) {
        case 22: // #0
          data = data << 1; 
          blink();   
          send = echo;
          break;
        case 12: // #1
          data = (data << 1) | 1;  
          blink(); 
          blink(); 
          send = echo;
          break;
        case 9: // #EQ
          echo = !echo; 
          blink(); 
          blink();   
          blink();  
          break;
        case 7: // "-"
          data--;
          break;
        case 21: // "+"
          data++;
          break;
        case 68: // "<<"
          data = ((data & 0b10000000) >> 7) | (data << 1);
          break;
        case 64: // ">>"
          data = ((data & 1) << 7) | (data >> 1);
          break;
        case 67: // ">||"
          data = ~data;
          break;
        case 25: // "FL+"
          data = 0xFF;
          break;
        case 13: // "FL-"
          data = 0;
          break;
        default:
          send = false;
          break;
      }      

      if (send) {
        #ifdef USE_EEPROM
          EEPROM_write(EEPROM_ADDRESS, data);
        #endif
        send_data();
      }
		}
  } //~loop

  return 0;
} //~main

void send_data()
{
  //устанавливаем LOW на latchPin пока не окончена передача байта
  digitalWrite(LATCH_PIN, LOW);
  shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, data);  
  //устанавливаем HIGH на latchPin, чтобы проинформировать регистр, что передача окончена.
  digitalWrite(LATCH_PIN, HIGH);
} //~send_data

#ifdef USE_LED
void blink() {
  //digitalWrite(LED_PIN, HIGH); // interestingly enough, this command is same compile-size as PORTB |=1<<LED_PIN
  PORTB|=1<<LED_PIN; //включаем = compiles more efficiently than XOR = 158 vs 166 (1 XOR) or 170 (2 XOR)
  //PORTB ^= (1 << LED_PIN);
  _delay_ms(BLINK_STEP_MS);

  //digitalWrite(LED_PIN, LOW);
  PORTB&=~(1<<LED_PIN); //отключаем
  //PORTB ^= (1 << LED_PIN);
  _delay_ms(BLINK_STEP_MS);
} //~blink
#endif



