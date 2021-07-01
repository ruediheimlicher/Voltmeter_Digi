//
//  sevenseg.c
//  Voltmeter
//
//  Created by Ruedi Heimlicher on 26.06.2021.
//

#include "sevenseg.h"

void init7segment(void)
{
        // least significant digit, cathode transistor:
   DIGIDDR0 |= 0x3E; // OUTPUT
   DIGIPORT0 |= 0x3E; // bit 1,2,3,4,5 HI
   
   DIGIDDR1 |= 0x06; // OUTPUT
   DIGIPORT1 |= 0x06; // bit 1,1 HI
   
   SEGDDR |= (0<<seg0); // Segment output
   SEGDDR |= (0<<seg1);
   SEGDDR |= (0<<seg2);

   SEGPORT |= (0<<seg0); // HI
   SEGPORT |= (0<<seg1);
   SEGPORT |= (0<<seg2);

}


// Update the sevent segement display.
// You must loop over this function all the time.
void upd7segment(unsigned int number)
{
   // LS digit:
   uint8_t digiwert = d2led[(number%10)]; // modulo
   // wert auf Ports aufteilen
   DIGIPORT0 |= (digiwert & 0x3E);
   DIGIPORT1 |= (digiwert & 0x06);
   //
   SEGPORT |= (1<<seg0);// digit on for a short time:
   _delay_ms(DIGION);
   number/=10; // divide by 10 (shift off right most digit)
   SEGPORT &= ~(1<<seg0);// digit off
   
#if (DP_DIG == 0)
   if (number>0)
   { // chop off the twp left most digits if it is zero
#endif
      // middle digit:
      digiwert = d2led[(number%10)];
      // wert auf Ports aufteilen
      DIGIPORT0 |= (digiwert & 0x3E);
      DIGIPORT1 |= (digiwert & 0x06);
      
      SEGPORT |= (1<<seg1);// digit on for a short time:
      _delay_ms(DIGION);
      number/=10; // divide by 10 (shift off right most digit)
      SEGPORT &= ~(1<<seg1);// digit off
      
#if (DP_DIG == 1 || DP_DIG == 0)
      if (number>0)
      { // chop off left most digit if it is zero
#endif
         //
         // left digit, we assume there that the number is
         // smaller than 999, no modulo here:
         digiwert = d2led[(number)];
         // wert auf Ports aufteilen
         DIGIPORT0 |= (digiwert & 0x3E);
         DIGIPORT1 |= (digiwert & 0x06);
         
         SEGPORT |= (1<<seg2);// digit on for a short time:
         _delay_ms(DIGION);
         number/=10; // divide by 10 (shift off right most digit)
         SEGPORT &= ~(1<<seg2);// digit off
#if (DP_DIG == 1 || DP_DIG == 0)
      }
#endif
#if (DP_DIG == 0)
   }
#endif
   
   
   
}
