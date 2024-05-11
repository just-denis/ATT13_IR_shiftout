/**
 * Copyright (c) 2016, Łukasz Marcin Podkalicki <lpodkalicki@gmail.com>
 * ATtiny13/013
 * NEC proto analyzer. Example of monblocking IR signal reader (38kHz, TSOPxxx) and NEC protocol decoder.
 * Settings:
 *  FUSE_L=0x7A
 *  FUSE_H=0xFF
 *  F_CPU=9600000
 https://github.com/lpodkalicki/blog/tree/master/avr/attiny13/013_ir_receiver_38khz_nec_proto_analyzer
 https://blog.podkalicki.pl/attiny13-ir-receiver-nec-proto-analyzer/
 */

#include "ir.h" // IR_IN_PIN = PB1

// https://docs.arduino.cc/tutorials/communication/guide-to-shift-out/
uint8_t const LATCH_PIN = PB3;
uint8_t const CLOCK_PIN = PB2;
uint8_t const DATA_PIN = PB0;

#define USE_LED

#ifdef USE_LED
#define LED_PIN PB4
#define BLINK_STEP_MS 50
#else
#define blink()
#endif

//const uint16_t TIMER_INC_MS = 50;
#define TIMER_INC_MS 50

//void blink();//(uint16_t on_ms, uint16_t off_ms = 0);

void setup() {

  #ifdef USE_LED  
  DDRB |= 1 << LED_PIN; // LED OUTPUT
  PORTB &= ~(1<<LED_PIN); // LED OFF
  #endif

  DDRB |= (1 << LATCH_PIN) | (1 << CLOCK_PIN) | (1 << DATA_PIN); // https://arduino.ru/Reference/ShiftOut

	IR_init();

  send_data(0);
} //~setup

void loop() {
  uint8_t addr, cmd;
  static uint8_t data = 0;

  #ifdef USE_LED1  
  static uint16_t timer = 500;
  static uint32_t prev_ms = millis();
  #endif

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
          //timer = 0;
          send = false;
          break;
      }      

      if (send) send_data(data);
		} 

  

  #ifdef USE_LED1
    if (timer == 0) {
       PORTB &= ~(1 << LED_PIN); // LED OFF        
    } else {        
      //uint32_t t_ms = millis();
      if (millis() - prev_ms > timer) {
        prev_ms = millis();
        PORTB ^= (1 << LED_PIN); // LED FLIP
      } 
    }   
  #endif 
} //~loop

void send_data(uint8_t data) {
  //устанавливаем LOW на latchPin пока не окончена передача байта
  digitalWrite(LATCH_PIN, LOW);
  shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, data);  
  //устанавливаем HIGH на latchPin, чтобы проинформировать регистр, что передача окончена.
  digitalWrite(LATCH_PIN, HIGH);
}

/*
void blink_delay(uint16_t ms) {
  while (ms >= BLINK_STEP_MS) {
    _delay_ms(BLINK_STEP_MS);
    ms -= BLINK_STEP_MS;
  }
}

void blink(uint16_t on_ms, uint16_t off_ms) {
  //digitalWrite(LED_PIN, HIGH); // interestingly enough, this command is same compile-size as PORTB |=1<<LED_PIN
  PORTB|=1<<LED_PIN; //включаем = compiles more efficiently than XOR = 158 vs 166 (1 XOR) or 170 (2 XOR)
  //PORTB ^= (1 << LED_PIN);
  //_delay_ms(on_ms);
  blink_delay(on_ms);

  //digitalWrite(LED_PIN, LOW);
  PORTB&=~(1<<LED_PIN); //отключаем
  //PORTB ^= (1 << LED_PIN);
  //_delay_ms(off_ms == 0 ? on_ms : off_ms);
  blink_delay(off_ms == 0 ? on_ms : off_ms);
}
*/
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
}
#endif



