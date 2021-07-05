//
//  sevenseg.c
//  Voltmeter
//
//  Created by Ruedi Heimlicher on 26.06.2021.
//

#include "sevenseg.h"


#define DP_DIG 1
void init7segment(void)
{
        // least significant digit, cathode transistor:
   
   //DIGIDDR0 |= (1<<diga)|(1<<digb)|(1<<digc)|(1<<digd)|(1<<dige);
   DIGIPORT0 |= 0x3E; // bit 1,2,3,4,5 HI
   DIGIDDR0 |= 0x3E; // OUTPUT
   
   
   DIGIPORT1 |= 0x03; // bit 1,2 HI: CA
   DIGIDDR1 |= 0x03; // OUTPUT
   
   SEGPORT |= (1<<seg0); // HI: PNP
   SEGPORT |= (1<<seg1);
   SEGPORT |= (1<<seg2);

   SEGDDR |= (1<<seg0); // Segment output
   SEGDDR |= (1<<seg1);
   SEGDDR |= (1<<seg2);

 
}
void setportb(uint8_t wert,uint8_t mask)
{
   uint8_t pos = 8;
   
   while(pos--)
   {
    //  if (mask & (1<<pos))
      {
         if (wert & (1<<pos))
         {
            PORTB |= (1<<pos);
         }
         else
         {
            PORTB &= ~(1<<pos);
         }
      }
   }
}
void setportd(uint8_t wert,uint8_t mask)
{
   uint8_t pos = 8;
   
   while(pos--)
   {
   //   if (mask & (1<<pos))
      {
         if (wert & (1<<pos))
         {
            PORTD &= ~(1<<pos);
         }
         else
         {
            PORTD |= (1<<pos);
         }
      }
   }
}
void rotsegment(void)
{
   
   SEGPORT &= ~(1<<seg0);// LO:  digit on for a short time:
   _delay_ms(DIGION);
   SEGPORT |= (1<<seg0);// HI: digit off
   _delay_ms(DIGION);
   SEGPORT &= ~(1<<seg1);// LO:  digit on for a short time:
   _delay_ms(DIGION);
   SEGPORT |= (1<<seg1);// HI: digit off
   _delay_ms(DIGION);
   SEGPORT &= ~(1<<seg2);// LO:  digit on for a short time:
   _delay_ms(DIGION);
   SEGPORT |= (1<<seg2);// HI: digit off
   _delay_ms(DIGION);
}
// Update the sevent segement display.
// You must loop over this function all the time.
void upd7segment(unsigned int number)
{
   lcd_gotoxy(0,1);
 
   // {0x40,0x4F,0x2B,0xA,0x7,0x12,0x10,0x4B,0x1,0x3}
   // LS digit:
   uint8_t digiwert = d2led[(number%10)]; // modulo

   lcd_putint2(number%10);
   lcd_putc(' ');
   lcd_puthex(digiwert);
   
   uint8_t digiwert0 = (digiwert & 0x7C);
   lcd_putc(' ');
   lcd_puthex(digiwert0);

   digiwert0 >>= 1;
   lcd_putc('>');
   lcd_puthex(digiwert0);

   // wert auf Ports aufteilen
   //DIGIPORT0 |= 0x2A;
   lcd_putc(' ');
   lcd_puthex(DIGIPIN0);

   DIGIPORT0 = (digiwert0 & 0x3E);
   //setportb(digiwert0,0x3E);
   lcd_putc(' ');
   lcd_puthex(DIGIPIN0 & 0x3E);
  
   uint8_t digiwert1 = digiwert & 0x03;
   lcd_putc('*');
   lcd_puthex(digiwert1);
   //lcd_putc(' ');
   
   setportd(digiwert1,0x03);
   //DIGIPORT1 |= (digiwert1 & 0x03);
 //  DIGIPORT1 &= ~(1<<0);
 //  DIGIPORT1 &= ~(1<<1);
    
   SEGPORT &= ~(1<<seg0);// LO:  digit on for a short time:
   _delay_ms(DIGION);
   number/=10; // divide by 10 (shift off right most digit)
   SEGPORT |= (1<<seg0);// HI: digit off
   //DIGIPORT0 = 0;
   
   _delay_ms(2);
#if (DP_DIG == 0)
   if (number>0)
   { // chop off the two left most digits if it is zero
#endif
      // middle digit:
      digiwert = d2led[(number%10)];
      
      lcd_gotoxy(0,2);
      lcd_putint2(number%10);
      lcd_putc(' ');
      lcd_puthex(digiwert);
      
      digiwert1 = (digiwert & 0x7C);
      lcd_putc(' ');
      lcd_puthex(digiwert1);

      digiwert1 >>= 1;
      // wert auf Ports aufteilen
      DIGIPORT0 &= (digiwert1);
      //lcd_putc(' ');
      DIGIPORT1 |= (digiwert & 0x06);
      //lcd_puthex(DIGIPIN0 & 0x3E);
      SEGPORT &= ~(1<<seg1);// digit on for a short time:
      _delay_ms(DIGION);
      number/=10; // divide by 10 (shift off right most digit)
      SEGPORT |= (1<<seg1);// digit off
      _delay_ms(2);

#if (DP_DIG == 1 || DP_DIG == 0)
      if (number>0)
      { // chop off left most digit if it is zero
#endif
         //
         // left digit, we assume there that the number is
         // smaller than 999, no modulo here:
         digiwert = d2led[(number)];
    //     lcd_gotoxy(12,1);
    //     lcd_puthex(digiwert);
         digiwert <<= 1;
         // wert auf Ports aufteilen
         
 //        DIGIPORT0 &= (digiwert);
         DIGIPORT1 |= (digiwert & 0x06);
   //      lcd_puthex(PINC & 0x3E);
         SEGPORT &= ~(1<<seg2);// digit on for a short time:
         _delay_ms(DIGION);
         number/=10; // divide by 10 (shift off right most digit)
         SEGPORT |= (1<<seg2);// digit off
#if (DP_DIG == 1 || DP_DIG == 0)
      }
      
      SEGPORT &= ~(1<<seg2);// digit on for a short time:
      _delay_ms(DIGION);
      number/=10; // divide by 10 (shift off right most digit)
      SEGPORT |= (1<<seg2);// digit off
      _delay_ms(2);
#endif
#if (DP_DIG == 0)
   }
#endif
   
  // _delay_ms(DIGION);
   
}
