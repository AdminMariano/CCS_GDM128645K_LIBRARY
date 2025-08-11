#include <18F25K22.h>
#device ADC=10
#fuses NOWDT, INTRC_IO, NOMCLR, NOPROTECT, NOLVP, PUT, NOCPD
#use delay(internal=64MHz)

#include "ST7920.c" 
#include "lcdgrafics.c" 

char str[]="Hello World!!";

void main(){ 
      glcd_init_graph(); 
      glcd_fillscreen(OFF);
      glcd_text57(3,3,str,2,1);
      glcd_update(); 
   while(1){
 
   } 
} 

