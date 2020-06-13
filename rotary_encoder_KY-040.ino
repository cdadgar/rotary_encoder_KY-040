/*
 * KY-040 rotary encoder
 * 
 * use library:
 * https://github.com/Erriez/ErriezRotaryEncoderFullStep
 *
 * GND - ground
 * + - VCC (3.3 or 5 VDC)
 * SW - button switch
 * DT - encoder pin B
 * CLK - encoder pin A 
 */


#include <ErriezRotaryFullStep.h>


// Connect rotary pins to the DIGITAL pins of the Arduino board with interrupt
// support:
//
// +-----------------------------------+----------------------------+
// |              Board                |    DIGITAL interrupt pins  |
// +-----------------------------------+----------------------------+
// | Uno, Nano, Mini, other 328-based  |  2, 3                      |
// | Mega, Mega2560, MegaADK           |  2, 3, 18, 19, 20, 21      |
// | Micro, Leonardo, other 32u4-based |  0, 1, 2, 3, 7             |
// | WeMos D1 R2 & mini (ESP8266)      |  12 = D6; 13 = D7; 14 = D5 |
// +-----------------------------------+----------------------------+


// use interrupt pins
#define ROTARY_PIN1   12
#define ROTARY_PIN2   13

/*
 * if not using interupts, the encoder must be read during loop
 */


// Initialize full step rotary encoder with internal pull-up pins enabled
// and default sensitivity=100
RotaryFullStep rotary(ROTARY_PIN1, ROTARY_PIN2);




int rotaryButton = 14;

// the following variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long lastDebounceTime = 0;  // the last time the output pin was toggled
const long debounceDelay = 50;    // the debounce time

const int defaultButtonState = HIGH;
int buttonState = defaultButtonState;
int lastButtonState = defaultButtonState;



/*
 * note: for esp8266, the interrupt handler must be in ram, not in flash
 * the ICACHE_RAM_ATTR forces the function to be placed into ram.
 * without it, you'll get "ISR not in IRAM!" error messages.
 */
// Forward declaration
void ICACHE_RAM_ATTR rotaryInterrupt();


void setup()
{
  // start serial port
  Serial.begin(115200);
  Serial.print(F("\n\n rotary encoder test\n\n"));

  pinMode(rotaryButton, INPUT_PULLUP);

  // Initialize pin change interrupt on both rotary encoder pins
  attachInterrupt(digitalPinToInterrupt(ROTARY_PIN1), rotaryInterrupt, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ROTARY_PIN2), rotaryInterrupt, CHANGE);
}

void loop()
{
  // Wait for interrupt
  handleEncoderButton();
}


void ICACHE_RAM_ATTR rotaryInterrupt()
{
  // Read rotary state
  int rotaryState = rotary.read();
  
  // rotaryState = -3: Counter clockwise turn, multiple notches fast
  // rotaryState = -2: Counter clockwise turn, multiple notches
  // rotaryState = -1: Counter clockwise turn, single notch
  // rotaryState = 0:  No change
  // rotaryState = 1:  Clockwise turn, single notch
  // rotaryState = 2:  Clockwise turn, multiple notches
  // rotaryState = 3:  Clockwise turn, multiple notches fast
  
  // Print count value when count value changed
  if (rotaryState != 0) {
    Serial.println(rotaryState);
  }
}


void handleEncoderButton() {
  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH),  and you've waited
  // long enough since the last press to ignore any noise:
  int reading = digitalRead(rotaryButton);

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;
      if (reading != defaultButtonState)
        doButton();
    }
  }

  // save the reading.  Next time through the loop,
  // it'll be the lastButtonState:
  lastButtonState = reading;
}

void doButton() {
  Serial.println("button");
}
