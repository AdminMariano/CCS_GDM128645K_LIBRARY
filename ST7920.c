////////////////////////////////////////////////////////////////////////////////// 
//The following are the functions included in this driver file 
// glcd_readbyte(); 
// glcd_instruction( instruction ); 
// glcd_data( data ); - data can be an array of characters! 
// glcd_check_busy(); 
// glcd_update(); -must be called always after writing a pixel or using functions 
//                 from GRAPHICS.C .. Only applicaticable in Graphing mode 
// glcd_fillscreen( ON or OFF); 
// glcd_init_graph(); initialize for graphing mode 
// glcd_init_basic(); initilize for accessing the stored Characters 
//                     you can use glcd_data() for writing text 
// glcd_pixel(x coordinate, y coordinate, ON or OFF);  
//            -WORKS WITH GRAPHIC.C  from CCS Drivers 
// glcd_plot_image(width,height,X coor, Y coor, inverse); 
//            -plots the image[] array. Declare it first before this driver. 
//             or modify this driver 
// 
////////////////////////////////////////////////////////////////////////////////// 


#define  rs  PIN_C0                    //COMMNAD/DATA SELECT 
#define  rw  PIN_C1                    //READ/WRITE SELECT              
#define  e   PIN_C2                    //ENABLE SIGNAL                  
#define  rst PIN_C5                    //RESET SIGNAL    

#define ON   1 
#define OFF   0 
#define XVAL 16  // 16 X 16 or 256 for there is 8 word values for the upper and lower 
#define YVAL 32 

// PSB is tied to Vcc for this driver because this driver uses Parallel 
// operation. 
// data is sent using port B so change output_b() to other ports you 
//want to use. Dont for get to change the Busy pin @ lcd_check_busy 

#define GLCD_WIDTH   128 

typedef union 
{ 
  int16 word; 
  int8 nbyte[2]; 
} Dots; 

typedef struct 
{ 
  int1 refresh; 
  Dots pix[YVAL][XVAL];   // Max dimensions for display (x,y) = (128,32) 
  } GD_RAM;             //  (0,0) corresponds to upper lefthand corner. 

GD_RAM gdram; 


unsigned int8 glcd_readByte (unsigned int1 address) {
  unsigned int8 data;
  set_tris_b(0xFF);  // Configura PORTB como entrada

  if(address==1) output_high(rs);
  else output_low(rs);

  output_high(rw);
  delay_us(1);
  output_high(e);
  delay_us(2);
  data = input_b();    // Lee los datos
  output_low(e);
  output_low(rw);

  set_tris_b(0x00);  // Restaurar como salida
  return data;
}


  
void glcd_check_busy() {
   int1 busy = 1;
   set_tris_b(0xFF);  // Puerto B como entrada
   output_low(rs);
   output_high(rw);
   delay_us(1);

   while(busy) {
      output_high(e);
      delay_us(2);
      busy = input(PIN_B7);
      output_low(e);
      delay_us(2);
   }

   set_tris_b(0x00);  // Puerto B como salida
   output_low(rw);
}
  
void glcd_instruction(unsigned char x) {
   glcd_check_busy();
   output_low(rs);
   output_low(rw);
   output_b(x);
   delay_cycles(1);
   output_high(e);
   delay_us(2);       // Aumentado
   output_low(e);
   delay_us(10);      // Extra margen para seguridad
}

void glcd_data(unsigned char x) {
   glcd_check_busy();
   output_high(rs);
   output_low(rw);
   output_b(x);
   delay_cycles(1);
   output_high(e);
   delay_us(2);       // Aumentado
   output_low(e);
   delay_us(10);
}


void glcd_fillScreen (unsigned int1 color) 
{ 
  int8 v, h; 
  int16 d; 


  d = (color == ON ? 0xFFFFL : 0x0000L); 

  for (v=0; v < YVAL; v++) 
  { 
    for (h=0; h < XVAL; h++) 
    { 
      gdram.pix[v][h].word = d; 
    } 
  } 
  gdram.refresh = TRUE; 
} 


void glcd_update () 
{ 
  int8 v, h; 


  if (gdram.refresh) 
  { 
    for (v=0; v <YVAL; v++) 
    { 
      glcd_instruction( 0x80 | v);   // Set Vertical Address. 
      glcd_instruction( 0x80 | 0);   // Set Horizontal Address. 

      for (h=0; h <XVAL; h++) 
      { 
        glcd_data( gdram.pix[v][h].nbyte[1]);   // Write High Byte. 
        glcd_data(  gdram.pix[v][h].nbyte[0]);   // Write Low Byte. 
      } 
    } 
    gdram.refresh = FALSE; 
  } 
} 

void glcd_init_graph() {
   delay_ms(40);
   output_low(rst);
   delay_ms(2);
   output_high(rst);
   delay_ms(10);

   glcd_instruction(0x30);
   delay_ms(10);
   glcd_instruction(0x30);  // Segundo 0x30 como recomienda el datasheet
   delay_ms(10);

   glcd_instruction(0x0C);
   delay_ms(10);
   glcd_instruction(0x01);
   delay_ms(20);
   glcd_instruction(0x06);
   delay_ms(10);

   glcd_instruction(0x34);  // Extended instruction set
   delay_ms(10);
   glcd_instruction(0x36);  // Enable graphics
   delay_ms(10);

   glcd_fillScreen(OFF);
   glcd_update();
}


void glcd_init_basic(){ 
   delay_ms(40); 
   output_low(rst);         //reset LCD 
   delay_us(1);                      
   output_high(rst);        //LCD normal operation 
   glcd_instruction(0x30);   //set 8 bit operation and basic instruction set 
   delay_us(144); 
   glcd_instruction(0x0C);   //display on cursor off and char blink off 
   delay_us(100); 
   glcd_instruction(0x01);   //display clear 
   delay_ms(10); 
   glcd_instruction(0x06);   //entry mode set 
   delay_us(72);                  
} 

void glcd_DrawPixel(int8 x, int8 y, int1 color) 
{ 
  int8 v, h, b; 
  if(y>31){x += 128; y-= 32;}; 
  v = y; 
  h = x/16; 
  b = 15 - (x%16); 
  
  if (color == ON) bit_set (gdram.pix[v][h].word, b); 
  else bit_clear (gdram.pix[v][h].word, b); 

  gdram.refresh = TRUE; 
} 
//*****************************************************************************
