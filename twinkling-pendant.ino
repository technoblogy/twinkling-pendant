 /* Twinkling Pendant - see http://www.technoblogy.com/show?3RMX

   David Johnson-Davies - www.technoblogy.com - 23rd February 2022
   ATtiny10 @ 1MHz (internal oscillator)
   
   CC BY 4.0
   Licensed under a Creative Commons Attribution 4.0 International license: 
   http://creativecommons.org/licenses/by/4.0/
*/

#include <avr/sleep.h>
#include <avr/interrupt.h>

uint8_t Power __attribute__ ((section (".noinit")));

// LEDs:                426 153
const uint8_t Mask = 0b011101110;         // Valid light positions
volatile uint8_t Lights = 0;              // All off
volatile uint8_t Ticks;

// Delay in 1/200ths of a second
void delay (uint8_t msec5) {
  Ticks = 0;
  while (Ticks < msec5);
}

// Multiplex LEDs
ISR(TIM0_COMPA_vect) {
  static uint8_t Count;
  PORTB = 0;                              // All bits low
  DDRB = 0;                               // All pins inputs
  Count = (Count + 1) % 3;
  uint8_t bits = Lights>>(3*Count) & 0b111;
  DDRB = 1<<Count | bits;                 // Make outputs
  PORTB = bits;                           // Take bits high
  Ticks++;
}

void setup () {
  sei();
  TCCR0A = 0<<WGM00;                      // No outputs, CTC mode, top=OCR0A
  TCCR0B = 1<<WGM02 | 2<<CS00;            // Divide clock by 8
  OCR0A = 624;                            // Gives 200Hz interrupt
  TIMSK0 = 1<<OCIE0A;                     // Enable compare interrupt
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  delay(20);                              // 100 msec
  Power = ~Power & 1;
}

void loop () {
  if (Power) {
    for (int n=0; n<=32767; n++) {
      for (int i=1; i<8; i++) {
        int k = n % (30 + i);
        if (k == 0 && i != 4) Lights = Lights | 1<<i;
        else Lights = Lights & ~(1<<i);
      }
    delay(20);                            // 100 msec
    }
  }
  PORTB = 0;
  TIMSK0 = 0;                             // Disable compare interrupt
  sleep_enable();
  sleep_cpu();
}
