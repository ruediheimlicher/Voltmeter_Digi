//
//  sevenseg.h
//  Voltmeter
//
//  Created by Ruedi Heimlicher on 26.06.2021.
//

#ifndef sevenseg_h
#define sevenseg_h

#include <stdio.h>

//Connections of the seven segment elements: 
//      10 9 7 6 4 2 1 = pin
//   0| -  f a b c d e | 0111111 | 3f
//   1| -  - - b c - - | 0001100 | 0c
//   2| g  - a b - d e | 1011011 | 5b
//   3| g  - a b c d - | 1011110 | 5e
//   4| g  f - b c - - | 1101100 | 6c
//   5| g  f a - c d - | 1110110 | 76
//   6| g  f a - c d e | 1110111 | 77
//   7| -  - a b c - - | 0011100 | 1c
//   8| g  f a b c d e | 1111111 | 7f
//   9| g  f a b c d - | 1111110 | 7e

//    
//
/*
         PORTC                                                PORTD        
 Ziffer  diga     digf     digb     digc     digg     dige     digd
 0        1        1        1        1                 1        1
 1                          1        1                           
 2        1                 1                 1        1        1
 3        1                 1        1        1                 1
 4                 1        1        1        1                  
 5        1        1                 1        1                 1
 6        1        1                 1        1        1        1
 7        1                 1        1                           
 8        1        1        1        1        1        1        1
 9        1        1        1        1        1                 1
 
 
 
 
 */

#define DIGION 4
#define DIGIPORT0 PORTC
#define DIGIDDR0 DDRC
#define DIGIPIN0 PINC
#define diga 1
#define digf 2
#define digb 3
#define digc 4
#define digg 5

#define DIGIPORT1 PORTD
#define DIGIDDR1 DDRD
#define DIGIPIN1 PIND
#define dige 1
#define digd 0

#define dp1 2
#define dp2 3

//static unsigned char d2led[] = {0x7B, 0x18, 0x57, 0x5D, 0x3C, 0x6D, 0x6F, 0x58, 0x7F, 0x7D};
//static unsigned char d2led[] = {0x10, 0x73, 0xA, 0x22, 0x61, 0x24, 0x4, 0x72, 0x0, 0x20};

#define SEGPORT  PORTD
#define SEGDDR   DDRD
#define seg0 5
#define seg1 6
#define seg2 7

static unsigned char d2led[] = {0x40,0x4F,0x2B,0xA,0x7,0x12,0x10,0x4B,0x1,0x3};
#endif /* sevenseg_h */

// where is the decimal point (needed in order to remove leading zeors):
// One digit behind decimal point:

// No decimal point:
//#define DP_DIG 0
// Two digits after decimal point: (do not chop any zeros):
//#define DP_DIG 2
