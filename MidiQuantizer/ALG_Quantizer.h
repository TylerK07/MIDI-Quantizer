#ifndef ALG_QUANTIZER_H
#define ALG_QUANTIZER_H

#include "definitions.h"

#define NUM_SCALES 11

/*******************************************
* Header Definitions                       *
*******************************************/

class ALG_Quantizer {
  private:
    uint8_t last_sent_note = 0;
    Hardware* hw;
    uint16_t scales[ NUM_SCALES ] = {
      0b101010110101, // Major
      0b010110101101, // Minor
      0b001001001001, // Diminished
      0b010101010101, // Wholetone
      0b011010101101, // Dorian
      0b010011101001, // Blues
      0b011010110101, // Mixolydian
      0b100111001101, // Hungarian
      0b100110110011, // Bhairav
      0b001010010101, // Pentatonic
      0b100110101101, // Melodic Minor
    };
    uint8_t quantizeToScale( uint8_t midiNote, uint16_t scale );

  public:
    ALG_Quantizer( Hardware* _hw ){ hw = _hw; };                               //Constructor

    void tick();

    void gate1On();
    void gate1Off();
    void gate2On();
    void gate2Off();

    void debug();

};

uint8_t ALG_Quantizer::quantizeToScale( uint8_t midiNote, uint16_t scale ){
  uint16_t n = 1 << (midiNote % 12);                                           // put a 1 in the column associated with the note 
  uint8_t noteOffset = 0;                                                      // Save the current note in the scale for reference later
  while( n > 0 ){                                                              // Ensure that note hasn't hit zero
    if( (scale & n) > 0 ) break;                                               // See if note exists in the scale
    n = n >> 1;                                                                // If it doesn't, move the 1 to the right one column
    noteOffset++;                                                              // And decrement the current note
  }
  return( midiNote - noteOffset );                                             // Shift midiNote down by the amount n decremented
}



void ALG_Quantizer::gate1On(){
  last_sent_note = quantizeToScale(hw->note, scales[hw->getCV(0, NUM_SCALES)]); // Store current note as last_sent
  hw->noteOn( 0, last_sent_note, 127);
}
void ALG_Quantizer::gate1Off(){
  hw->noteOff( 0, last_sent_note, 127);
}

void ALG_Quantizer::debug(){

  hw->tft.setCursor(0, 0);                                                     // Start at bottom-left corner

  hw->print( hw->noteCV );
  hw->print( ' ' );
  hw->print(hw->getCV(0, NUM_SCALES));                                         // Write a number or something
  hw->print( '\n' );
  hw->printNote( hw->note );
  hw->print( "->" );
  hw->printNote( last_sent_note );



  //FOR DEBUGGING:
  hw->tft.println(hw->mode);                                                   // Write a number or something

  hw->print( hw->cv[0] );
  hw->print( ' ' );
  hw->print( hw->cv[1] );
  hw->print( ' ' );
  hw->tft.println( hw->cv[2] );

  hw->print(hw->getCV(0, 255));                                                // Write a number or something
  hw->print( ' ' );
  hw->print(hw->getCV(1, 255));                                                // Write a number or something
  hw->print( ' ' );
  hw->tft.println(hw->getCV(2, 255));                                          // Write a number or something

  hw->print(hw->offset[0]);                                                    // Write a number or something
  hw->print( ' ' );
  hw->print(hw->offset[1]);                                                    // Write a number or something
  hw->print( ' ' );
  hw->tft.print(hw->offset[2]);                                                // Write a number or something

}



#endif