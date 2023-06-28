#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>


/*******************************************
* Screen Defaults & Setup                  *
*******************************************/

#define SCREEN_WIDTH 160  // OLED display width, in pixels
#define SCREEN_HEIGHT 128 // OLED display height, in pixels
#define TFT_CS     -1     // no CS pin on this screen
#define TFT_RST     3     // define reset pin, or set to -1 and connect to Arduino RESET pin
#define TFT_DC      2     // define data/command pin


/*******************************************
* Midi Defaults & Setup                    *
*******************************************/

#define MIDI_CHANNEL 1
#define MIDI_UPDATE_PERIOD 1                                                   // The number of milliseconds to wait between
unsigned long next_midi_update   = 0;                                          // The time when the next midi update should occur

#define MIDI_CMD_NOTEON  0x90
#define MIDI_CMD_NOTEOFF 0x80



/*******************************************
* Hardware Input Tracking                  *
*******************************************/

// INPUT PINS:
#define PIN_MODE     PIN_PD5         // Mode Control
#define PIN_NOTE_CV  PIN_PD0         // Note CV Input 1v/Octave

#define PIN_CV_1     PIN_PD6         // CV Control Input 1
#define PIN_CV_2     PIN_PD7         // CV Control Input 2
#define PIN_CV_3     PIN_PD1         // CV Control Input 3
#define PIN_OFFSET_1 PIN_PD2         // CV Control Offset 1
#define PIN_OFFSET_2 PIN_PD3         // CV Control Offset 2
#define PIN_OFFSET_3 PIN_PD4         // CV Control Offset 3

#define PIN_GATE_1   PIN_PC0         // Gate Input 1
#define PIN_GATE_2   PIN_PC1         // Gate Input 2
#define PIN_LED_1    PIN_PC2         // Gate 1 LED
#define PIN_LED_2    PIN_PC3         // Gate 2 LED
#define PIN_LED_MIDI PIN_PA7         // MIDI Activity LED

// ANALOG INPUT BUFFER: (Separate from settings variables so we can make them more solid)
#define ANALOG_READ_THRESHOLD 5                    // Delta that input has to change by in order to update a setting

#define UPDATE_PERIOD 1


class Hardware{
  private:
    uint16_t analogIn[8] = {0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF}; // Buffer containing all of the analog inputs
    uint16_t next_update = 0;

    bool     analogReadFiltered( uint16_t &val, uint8_t pin, uint8_t threshold );
    uint8_t  getNoteNumber( uint16_t CV );
    void     sendMIDI(uint8_t cmd, uint8_t d1, uint8_t d2);
    
    void (*cb_gate1On)()  = NULL;
    void (*cb_gate1Off)() = NULL;
    void (*cb_gate2On)()  = NULL;
    void (*cb_gate2Off)() = NULL;

    // Handle NoteCV Non-linearity:
    // noteCVs contains empirically measured note values
    uint16_t noteCVs[121] = {
      26,   44,   62,   80,   97,   115,  133,  151,  169,  187,  204,  222,
      240,  264,  288,  312,  336,  360,  385,  409,  433,  457,  481,  505,
      529,  559,  590,  620,  650,  681,  711,  741,  772,  802,  832,  863,
      893,  929,  964,  1000, 1035, 1071, 1106, 1142, 1177, 1213, 1248, 1284,
      1319, 1357, 1395, 1433, 1471, 1509, 1547, 1585, 1623, 1661, 1699, 1737,
      1775, 1814, 1853, 1892, 1930, 1969, 2008, 2047, 2086, 2125, 2163, 2202,
      2241, 2280, 2319, 2358, 2396, 2435, 2474, 2513, 2552, 2591, 2629, 2668,
      2707, 2746, 2785, 2824, 2863, 2902, 2941, 2980, 3019, 3058, 3097, 3136,
      3175, 3214, 3253, 3292, 3331, 3370, 3409, 3447, 3486, 3525, 3564, 3603,
      3642, 3680, 3718, 3756, 3793, 3831, 3869, 3907, 3945, 3983, 4020, 4058, 4096
    };
    uint16_t CV_mins[3] = { 1718, 1742, 1739 };                                       // Values read at 0 volts on the CV input 
    uint16_t CV_maxs[3] = { 3055, 3157, 3112 };                                       // Values read at 5 volts on the CV input

    char _buffer[20];



  public:
    uint16_t  cv[3]     = {0,0,0};  // Current CV values
    uint16_t  offset[3] = {0,0,0};  // Current Offset values
    uint16_t  mode      = 0;        // Current Mode value
    uint16_t  noteCV    = 0;        // Current noteCV value
    uint8_t   note      = 0;        // Current ModiNote (based on noteCV)
    uint8_t   gate1     = 0;        // Current State of Gate 1
    uint8_t   gate2     = 0;        // Current State of Gate 2


    char notes[12]  = {'C','C','D','D','E','F','F','G','G','A','A','B'};
    char sharps[12] = {' ','#',' ','#',' ',' ','#',' ','#',' ','#',' '};

    Adafruit_ST7735 tft;

    Hardware();                                                                   // Constructor
    void setup();                                                                 // Setup function
    void processEvents();                                                         // Process Events Function
    void setGate1OnHandler(  void (*fn)() ){ cb_gate1On  = fn; }
    void setGate1OffHandler( void (*fn)() ){ cb_gate1Off = fn; }
    void setGate2OnHandler(  void (*fn)() ){ cb_gate2On  = fn; }
    void setGate2OffHandler( void (*fn)() ){ cb_gate2Off = fn; }
    uint16_t getCV( uint8_t cvNum, uint16_t max );                                    // Return a value between 0 and max based on hardware constraints
    uint16_t getOffset( uint8_t osNum, uint16_t max );                              // Return a value between 0 and max based on hardware constraints

    // MIDI Functions
    void noteOn( uint8_t ch, uint8_t n, uint8_t velocity){ sendMIDI( MIDI_CMD_NOTEON  | (ch & 0xF), n, velocity ); }
    void noteOff(uint8_t ch, uint8_t n, uint8_t velocity){ sendMIDI( MIDI_CMD_NOTEOFF | (ch & 0xF), n, velocity ); }

    // TFT Functions
    void print( char buffer );                     // Print char string to the tft screen 
    void print( char* buffer );                    // Print char string to the tft screen 
    void print( uint16_t buffer );                 // Print char string to the tft screen 
    void print( uint8_t val ){ print(uint16_t(val)); }
    void print( const char* str );
    void printNote( uint8_t midiNote );            // Print the name of the midiNote to the tft screen
};

Hardware::Hardware() : tft(TFT_CS, TFT_DC, TFT_RST){}


/*******************************************
* SETUP                                    *
*******************************************/

void Hardware::setup(){
  // Screen Setup
  tft.initR(INITR_BLACKTAB);                       // If random colors around edge of screen, use BLACKTAB vs. GREEN
  tft.fillScreen(   ST77XX_BLACK  );               // Fill the screen with black
  tft.setTextColor( ST77XX_WHITE, ST77XX_BLACK );  // Setup the text colors
	tft.setRotation(1);                              // Put screen in proper orientation

  // Setup Pin Modes
  pinMode(PIN_GATE_1,   INPUT);                    // Setup the record input as a pull-up input
  pinMode(PIN_GATE_2,   INPUT);                    // Setup the record input as a pull-up input
  pinMode(PIN_LED_1,    OUTPUT);                   // Setup the record input as a pull-up input
  pinMode(PIN_LED_2,    OUTPUT);                   // Setup the record input as a pull-up input
  pinMode(PIN_LED_MIDI, OUTPUT);                   // Setup the record input as a pull-up input

  // Setup Analog Sampling
  analogReference(INTERNAL4V096);                  // Set analog reference to 4.096v
  ADC0.SAMPCTRL = 4;                               // Increase the time spent sampling to 4 clock cycles
  analogReadResolution(12);                        // Set resolution of sample to 12 bits
}


/*******************************************
* MIDI                                     *
*******************************************/

// MIDI Note Functions
void Hardware::sendMIDI(uint8_t cmd, uint8_t d1, uint8_t  d2) {
  digitalWrite( PIN_LED_MIDI, 1 );
  Serial2.write(cmd);                                                          // command byte (should be > 127)
  Serial2.write(d1);                                                           // data byte 1 (should be < 128)
  Serial2.write(d2);                                                           // data byte 2 (should be < 128)
  digitalWrite( PIN_LED_MIDI, 0 );
}


/*******************************************
* TFT Passthrough Functions                *
*******************************************/

void Hardware::print( char buffer ) {
  tft.print(buffer);                                                           // Print the character to the screen
  processEvents();                                                             // See if any midi events need to be processed
}

void Hardware::print( char* buffer ) {
  for( uint8_t i=0; i<strlen(buffer); i++ ){                                   // Loop through the buffer
    tft.print(buffer[i]);                                                      // Print the character to the screen
    processEvents();                                                           // See if any midi events need to be processed
  }
}

void Hardware::print( const char* buffer ) {
  for( uint8_t i=0; i<strlen(buffer); i++ ){                                   // Loop through the buffer
    tft.print(buffer[i]);                                                      // Print the character to the screen
    processEvents();                                                           // See if any midi events need to be processed
  }
}

void Hardware::print( uint16_t val ){
  char _buffer[20];
  sprintf( _buffer, "%3d", val );
  print( _buffer );
}


void Hardware::printNote( uint8_t midiNote ){
  print( notes[midiNote % 12] );
  print( sharps[midiNote % 12] );
  print( uint8_t (midiNote / 12) );
}


/*******************************************
* PIN READING                              *
*******************************************/

// getNoteNumber returns 
uint8_t Hardware::getNoteNumber( uint16_t CV ){
  CV -= 10;
  for( uint8_t n = 0; n<121; n++ ){
    if( CV <= noteCVs[n] ) return n;
  }
  return( 121 );
}

uint16_t Hardware::getCV( uint8_t cvNum, uint16_t max ){                              // Return a value between 0 and max based on hardware constraints
  return( constrain( map( cv[cvNum], CV_mins[cvNum], CV_maxs[cvNum], 0, max ), 0, max-1 ) );
}
uint16_t Hardware::getOffset( uint8_t osNum, uint16_t max ){                              // Return a value between 0 and max based on hardware constraints
  return( constrain( map( offset[osNum], 0, 255, 0, max ), 0, max-1 ) );
}


// Note for the analog reads, we do things in a couple of steps. First we check the value of the analog pot
// Then we compare it to the prior value to see if it has changed by a threshold. If it has, then analogReadFiltered
// will return true and we will update the setting_XXX value. If it didn't, then analogReadFiltered will still update
// the value in the analogIn array (even though that value won't carry to the setting variable). This allows us to
// accommodate drift in the analog inputs.

bool Hardware::analogReadFiltered( uint16_t &val, uint8_t pin, uint8_t threshold ){
  if( val==0xFFFF ){                               // If system just initialized, val will be set to 0xFFFF
    val = analogRead(pin);                         // Overwrite val with the current analog value and
    return( true );                                // Tell the system that we need to update
  }                                                // Otherwise, if everything is already initialized...
  uint16_t vNew = (analogRead(pin) + val) >> 1;    // Average the current value with the new one
  if( abs( val - vNew ) > threshold ){             // See if the change is larger than threshold
    val = vNew;                                    // Update the value
    return( true );                                // Return true since we updated the value
  }                                                // If not larger than threshold, still update the value
  //val = vNew;                                      // to allow for some level of drift over time
  return( false );                                 // but return false to indicate that the value didn't change enough
}


// === processEvents === //
// This is the main event loop for checking the midi serial input lines for data
// and then sending that data to the appropriate places. This function also handles
// Updating our position in the midi loop and seeing if there are any events there
// that need to be dealt with.

void Hardware::processEvents(){

  // Manage timer so midi events only get updated every MIDI_UPDATE_PERIOD milliseconds
  unsigned long t = millis();                                               // Capture the current time
  if(t < next_update) return;                                               // See if it is time to check for midi updates again
  next_update = t + UPDATE_PERIOD;                                          // Set the next time we need to check for midi updates

  // Read current Note CV's current voltage level and calculate the current note number
  if( analogReadFiltered( analogIn[0], PIN_NOTE_CV, 0 ) ){                  // Get value of Note CV analog pin
    noteCV = 4096 - analogIn[0];                                            // Invert value of analog in and Update setting_length
    note   = getNoteNumber(noteCV);
  }

  // Read the other settings
  if( analogReadFiltered( analogIn[1], PIN_MODE, ANALOG_READ_THRESHOLD ) ){ mode  = analogIn[1] >> 4;   }
  if( analogReadFiltered( analogIn[2], PIN_CV_1, ANALOG_READ_THRESHOLD ) ){ cv[0] = 4096 - analogIn[2]; }
  if( analogReadFiltered( analogIn[3], PIN_CV_2, ANALOG_READ_THRESHOLD ) ){ cv[1] = 4096 - analogIn[3]; }
  if( analogReadFiltered( analogIn[4], PIN_CV_3, ANALOG_READ_THRESHOLD ) ){ cv[2] = 4096 - analogIn[4]; }
  if( analogReadFiltered( analogIn[5], PIN_OFFSET_1, ANALOG_READ_THRESHOLD ) ){ offset[0] = analogIn[5] >> 4; }
  if( analogReadFiltered( analogIn[6], PIN_OFFSET_2, ANALOG_READ_THRESHOLD ) ){ offset[1] = analogIn[6] >> 4; }
  if( analogReadFiltered( analogIn[7], PIN_OFFSET_3, ANALOG_READ_THRESHOLD ) ){ offset[2] = analogIn[7] >> 4; }


  uint8_t _gate1 = gate1;                                                   // Store old value of gate 1
  uint8_t _gate2 = gate2;                                                   // Store old value of gate 2
  gate1 = digitalRead(PIN_GATE_1);                                          // Read current value of Gate 1
  gate2 = digitalRead(PIN_GATE_2);                                          // Read current value of Gate 2

  digitalWrite( PIN_LED_1, gate1 );                                         // Set LED for gate 1
  digitalWrite( PIN_LED_2, gate2 );                                         // Set LED for gate 2

  if( _gate1 != gate1 ){                                                    // If gate 1 changed
    if( gate1 ){                                                            // Trigger the On or Off callback
      if( cb_gate1On )  cb_gate1On();
    } else {
      if( cb_gate1Off ) cb_gate1Off();
    }
  }

  if( _gate2 != gate2 ){                                                    // If gate 2 changed
    if( gate2 ){                                                            // Trigger the On or Off callback
      if( cb_gate2On )  cb_gate2On();
    } else {
      if( cb_gate2Off ) cb_gate2Off();
    }
  }

}



#endif