//
//  Tastenblinky.c
//  Tastenblinky
//
//  Created by Sysadmin on 03.10.07.
//  Copyright Ruedi Heimlihcer 2007. All rights reserved.
//



#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include "defines.h"

#include "lcd.c"
#include "adc.c"
#include "sevenseg.c"

uint16_t loopCount0=0;
uint16_t loopCount1=0;
uint16_t loopCount2=0;

#define OSZIPORT   PORTB      // Ausgang fuer Servo
#define OSZIDDR   DDRB

#define OSZIA 4            // 
#define OSZIB 5            // 
#define OSZIALO OSZIPORT &= ~(1<<OSZIA)
#define OSZIAHI OSZIPORT |= (1<<OSZIA)
#define OSZIATOG OSZIPORT ^= (1<<OSZIA)

#define OSZIBLO OSZIPORT &= ~(1<<OSZIB)
#define OSZIBHI OSZIPORT |= (1<<OSZIB)
#define OSZIBTOG OSZIPORT ^= (1<<OSZIB)


#define LOOPLED_PORT   PORTB
#define LOOPLED_DDR   DDRB
#define LOOPLED_PIN   4

#define  IMPULS_MAX 5
#define  IMPULS_MIN 5
#define ANZAHL_IMPULSE 3

#define MITTELWERTBREITE 8

uint16_t mittelwertarray[MITTELWERTBREITE] = {};
uint8_t ringpos = 0;
uint16_t mittelwert = 0;

volatile uint8_t   INT0status=0x00;   

volatile uint8_t INT0counter = 0; // Anzahl impulse auf Spule

volatile uint8_t impulscounter = 0; // Anzahl impulse an relais: Sicher ist sicher


volatile uint16_t loopcounter = 0; // Anzahl impulse an relais: Sicher ist sicher

volatile uint16_t adcspannung = 0;
volatile uint16_t spannung = 0;

void slaveinit(void)
{
   LOOPLED_DDR |= (1<<LOOPLED_PIN);
   
   OSZIDDR |= (1<<OSZIA);
   OSZIDDR |= (1<<OSZIB);

   DDRC &= ~(1<<0);
   //LCD
   LCD_DDR |= (1<<LCD_RSDS_PIN);   //Pin 5 von PORT B als Ausgang fuer LCD
   LCD_DDR |= (1<<LCD_ENABLE_PIN);   //Pin 6 von PORT B als Ausgang fuer LCD
   LCD_DDR |= (1<<LCD_CLOCK_PIN);   //Pin 7 von PORT B als Ausgang fuer LCD


    
   
   ADCSRA = 0;
//   MCUCR = 0;
   SMCR |= (1<<SE);
   SMCR |= (1<<SM1);
   SMCR |= (1<<SM0);
   
}

//MARK  Timer2
void timer2 (uint8_t wert) 
{ 
   TCCR2B |= (1<<CS20);            //8-Bit Timer, Timer clock = system clock/256
   TCCR2B |= (1 << CS21); 
   TCCR2B |= (1 << CS22); 
   TCCR2A |= (1<<WGM21);      //   ClearTimerOnCompareMatch CTC

   TIFR2 |= (1<<TOV2);             //Clear TOV2 Timer/Counter Overflow Flag. clear pending interrupts
   TIMSK2 |= (1<<OCIE2A);         //CTC Interrupt aktivieren
   TIMSK2 |= (1 << TOIE2);
   TCNT2 = 0x00;               //Zaehler zuruecksetzen
   
   //;
   OCR2A = 10;               //Setzen des Compare Registers auf HIimpulsdauer
} 

ISR(TIMER2_OVF_vect)
{
   OSZIBTOG;
}

//MARK  ISR Timer2
ISR(TIMER2_COMPA_vect) // Schaltet Impuls an MOTOROUT LO wenn speed
{
 //  OSZIATOG;
   //return;
    if (INT0status & (1<<1))  // interrupt am laufen
    {
       INT0counter++;
       
       if (INT0counter > IMPULS_MAX)// Ereignis fertig
       {
//          INT0status &= ~(1<<1);
          
 //         RELAIS_PORT |= (1<<RELAIS_ENABLE); // enable OFF
          
          INT0counter = 0; // reset auf WAIT, Pause zwischen impulsen
          impulscounter ++;
       }
       
       
       if (impulscounter >= ANZAHL_IMPULSE)
       {
          INT0status &= ~(1<<1);
          impulscounter = 0;
          //        MCUCR |= (1<<SM1);
          //        MCUCR |= (1<<SM0);
          //MCUCR = 0;
       }
    }
} // TIMER0_COMPA_vect

//MARK INT0
void int0_init(void)
{
   
   EICRA |= (1<<ISC00 ); // raise int0 on any Change
   EIMSK |= (1<<INT0); // enable external int0
   INT0status = 0;
   INT0counter = 0; // begin erster impuls vor WAIT
   
}


ISR(INT0_vect) 
{
   INT0status |=(1<<1);
   OSZIBTOG;

}

void watchdog_init(void)
{
   cli();
  // watchdog konfigurieren
  WDTCSR = (1 << WDCE)|(1 << WDE);  // zunaechst Schutz des Registers aufheben gemaess Datasheet
 // WDTCSR = (1 << WDIE)|(1 << WDP3)|(1 << WDP0);  // Interruptmode an, Zeit auf 8s einstellen
   WDTCSR = (1 << WDIE)|(1 << WDP2)|(1 << WDP1) |(1 << WDP0);  // Interruptmode an, Zeit auf 1s einstellen
   sei();
}

ISR(WDT_vect)
{
   
   //OSZIBTOG;
   INT0status |=(1<<1);
 }

int main (void) 
{
   /* INITIALIZE */
   //watchdog_init();
   slaveinit();
   _delay_ms(100);
   lcd_initialize(LCD_FUNCTION_8x2, LCD_CMD_ENTRY_INC, LCD_CMD_ON);
   lcd_puts("Guten Tag\0");
   _delay_ms(100);
   lcd_cls();
   lcd_puts("READY\0");
   
//   timer2(4); // 128 ca. 130ms
   
//   int0_init();
   

   initADC(0);
//   lcd_gotoxy(16,0);
//   _delay_ms(100);
//   int i=0;
 //  set_sleep_mode(SLEEP_MODE_PWR_DOWN);  //  SLEEP_MODE_PWR_SAVE SLEEP_MODE_PWR_DOWN
   init7segment();
   sei();
   unsigned int voltage=0;
   //MARK  while
   lcd_clr_line(0);
   while (1) 
   {
      loopCount0++;
      if (loopCount0 > 0x4F)
      {
         
         LOOPLED_PORT ^= (1<<LOOPLED_PIN);
         loopCount0 = 0;
         /*
         lcd_gotoxy(0,0);
         lcd_putint12(adcspannung);
         lcd_gotoxy(0,1);
         //voltage = 43;
         lcd_putint12(voltage);
         lcd_putc(' ');
         lcd_putint12(mittelwert);
*/
      }
      
      loopcounter++;
      if (loopcounter == 2)
      {
         loopCount1++;
         adcspannung = readKanal(1);
         mittelwertarray[(ringpos & 0x03)] = adcspannung;
         ringpos ++;
         mittelwert = 0;
         for(uint8_t i=0;i<MITTELWERTBREITE;i++)
         {
            mittelwert += mittelwertarray[i];
         }
         mittelwert /= MITTELWERTBREITE;
         
         if ((adcspannung > voltage) && (adcspannung -voltage > 20))
         {
                 voltage=adcspannung + adcspannung;
         }else if ((adcspannung < voltage) && (voltage -adcspannung > 20))
         {
                 voltage=adcspannung + adcspannung;
         }else{
                 // just build average (avoid toggle):
                 voltage=voltage + adcspannung;
         }
         voltage=voltage/2;

         #if (DP_DIG == 1)    // runden, by RWCooper 2007/11/25
    //       voltage = (voltage + 5) / 10;
         #endif
  //       lcd_putc(' ');
  //       lcd_putint12(voltage);
       
         
          
         OSZIALO;
 
         OSZIAHI;
      }
      if (loopcounter > 0x1F)
      {
         loopcounter = 0;
      }
     // loopcounter = loopcounter & 0x4F;
      //voltage = 45;
      upd7segment(mittelwert);
      
   }
   
   
   return 0;
}
