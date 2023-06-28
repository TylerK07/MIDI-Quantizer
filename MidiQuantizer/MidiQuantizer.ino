#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include "definitions.h"
#include "ALG_Quantizer.h"





Hardware hw;                         // Create a structure to track the current state of the hardware







/*******************************************
* UI Styles                                *
*******************************************/
#define UI_MIDIVIZ_HEAD_LINE     0x2E56
#define UI_MIDIVIZ_SCROLL_NORMAL 0x66E9
#define UI_MIDIVIZ_SCROLL_OVER   0xE246
#define UI_MIDIVIZ_SCROLL_BG     0x52AA
#define UI_MIDIVIZ_FADE          0x2E56



/*******************************************
* Global Variables                         *
*******************************************/
ALG_Quantizer quantizer(&hw);


/*******************************************
* Event Handler Functions                  *
*******************************************/
void handleGate1On(){
  quantizer.gate1On();
}
void handleGate1Off(){
  quantizer.gate1Off();
}



/*******************************************
* Main Setup Function                      *
*******************************************/

void setup(){
  // Initiate MIDI communications, listen to all channels
  Serial2.begin(31250);
  while( !Serial2 ){}

  hw.setup();
  hw.setGate1OnHandler( handleGate1On );
  hw.setGate1OffHandler( handleGate1Off );
}





/*******************************************
* Main Loop Function                       *
*******************************************/
void loop(){

  quantizer.debug();

}


