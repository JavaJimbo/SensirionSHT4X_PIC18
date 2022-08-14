# 1 "../DELAY18.c"
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 288 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 "C:/Program Files/Microchip/MPLABX/v6.00/packs/Microchip/PIC18F-K_DFP/1.5.114/xc8\\pic\\include\\language_support.h" 1 3
# 2 "<built-in>" 2
# 1 "../DELAY18.c" 2



# 1 "../DELAY.h" 1
# 12 "../DELAY.h"
extern void DelayMs(unsigned short);
extern void DelayUs (unsigned long i);
# 4 "../DELAY18.c" 2






void DelayMs (unsigned short i)
{
unsigned short j, dummy;

 for (j=0; j<i; j++)
    {
        dummy = 500;
  while (dummy) dummy--;
 }
}

void DelayUs (unsigned long i)
{
unsigned long j;

    j = i;
    while(j--);
}
