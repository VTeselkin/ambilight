// LIBRARIES

#include <Wire.h>
//#include <LiquidCrystal_I2C.h>
#include "Adafruit_NeoPixel.h"

// DEFINITIONS

#define STARTCOLOR 0xff8000  // LED color at startup 0xff8000 is orange in hex code. pick your own here: http://www.w3schools.com/colors/colors_picker.asp
#define BLACK      0x000000  // LED color BLACK

#define DATAPIN    6         // Datapin
#define LEDCOUNT   107       //  Number of LEDs used in your system
#define SHOWDELAY  20        // Delay in micro seconds before showing
#define BAUDRATE   500000    // Serial port speed

#define BRIGHTNESS 100        // Max. brightness in %

const char prefix[] = { 0x41, 0x64, 0x61, 0x00, 0x6B, 0x3E };  // Prefix at the start of the transmission
char buffer[sizeof(prefix)];                                 // Temporary buffer for receiving the prefix data

// to calculate your prefix, the first 4 bytes will never change: const char prefix[] = {0x41, 0x64, 0x61, 0x00, this never changes.
// the next byte is equal to the number of LED - 1 --> (232-1)=231. 231 transformed in HEX. 231 in hex is E7 (use google)
// the last byte is equal to the XOR value of the calculated value just above and 0x55 (byte just calculated (E7) XORED with 0x55) = B2     use this link http://xor.pw/? and in input 1 put 55 and in input 2 put your HEX value.

// some precalculated values to save some time: 178 leds gives B1 and E4, 180 B3E6, 181 B4E1, 182 B5E0 232 E7B2 230 E5B0

Adafruit_NeoPixel strip = Adafruit_NeoPixel(LEDCOUNT, DATAPIN, NEO_GRB + NEO_KHZ800);
//LiquidCrystal_I2C lcd(0x20,16,2);                            // set the LCD address to 0x20 for a 16 chars and 2 line display

int state;                                                   // Define current state
#define STATE_WAITING   1                                    // - Waiting for prefix
#define STATE_DO_PREFIX 2                                    // - Processing prefix
#define STATE_DO_DATA   3                                    // - Handling incoming LED colors

int readSerial;                                              // Read Serial data (1)
int currentLED;                                              // Needed for assigning the color to the right LED
const int led_button = 11;                                   // ON/OFF button input
const int status_led = 8;                                    // LED of button
const int ch_2 = A0;                                         // Inputs from HDMI switcher
const int ch_3 = A1;
const int ch_4 = A2;
const int ch_5 = A3;

int curr_source = 0;                                         // Used to store which source is currently displayed
boolean strip_status = true;                                 // Used to chose if the LED strip is ON or OFF (depending on button choice)
unsigned long timeout = 0;                                   // Timeout used to turn LED strip off if no new data has come thorugh after some time

void setup() {
	pinMode(led_button, INPUT);
	pinMode(ch_2, INPUT);
	pinMode(ch_3, INPUT);
	pinMode(ch_4, INPUT);
	pinMode(ch_5, INPUT);
	pinMode(status_led, OUTPUT);
	digitalWrite(led_button, HIGH);
	digitalWrite(status_led, HIGH);

	delay(15000);                                             // 15 seconds delay at startup to avoid strange behaviours as the PI boots up etc
	strip.begin();                                             // Init LED strand, set all black, then all to startcolor

	strip.setBrightness((255 / 100) * 25);

	setAllLEDs(BLACK, 0);
	setAllLEDs(STARTCOLOR, 10);                               // Will turn ON all LEDS with a 5ms delay between each turn ON creating a snake increasing pattern

	Serial.begin(BAUDRATE);                                   // Init serial speed

	state = STATE_WAITING;                                    // Initial state: Waiting for prefix

	//lcd.init();                                                // Initialize the lcd
	//lcd.backlight();                                           // Turn ON LCD backlight
	//lcd.print(" waiting for PI");                              // Wait for PI to boot up for 5 sec (avoids the Arduino rebooting randomly)
	delay(5000);
	//lcd.clear();
	//lcd.print("**** SOURCE ****");
	strip.setBrightness((255 / 100) * BRIGHTNESS);           // Set the brightness we chose above
}


void loop() {
	do_strip();                                               // Main part of the code where we look at the data incoming from the PI
	if (state == STATE_WAITING)                               // Only if we are in WAITING state we check the source inputs and our ON/OFF button.
	{
		check_source();
		check_button();
	}
}



void do_strip(void) {
	if (strip_status == true) {
		switch (state) {
		case STATE_WAITING:                                    // *** Waiting for prefix ***
			if (Serial.available() > 0) {
				readSerial = Serial.read();                        // Read one character

				if (readSerial == prefix[0])                     // if this character is 1st prefix char
						{
					state = STATE_DO_PREFIX;
				}                     // then set state to handle prefix
			}
			break;

		case STATE_DO_PREFIX:                                  // *** Processing Prefix ***
			timeout = millis();
			if (Serial.available() > sizeof(prefix) - 2) {
				Serial.readBytes(buffer, sizeof(prefix) - 1);

				for (int Counter = 0; Counter < sizeof(prefix) - 1; Counter++) {
					if (buffer[Counter] == prefix[Counter + 1]) {
						state = STATE_DO_DATA;                       // Received character is in prefix, continue
						currentLED = 0;                              // Set current LED to the first one
					} else {
						state = STATE_WAITING;                       // Crap, one of the received chars is NOT in the prefix
						break;                                       // Exit, to go back to waiting for the prefix
					}                                              // end if buffer
				}                                                // end for Counter
			}                                                    // end if Serial
			break;

		case STATE_DO_DATA:                                    // *** Process incoming color data ***
			if (Serial.available() > 2)                         // if we receive more than 2 chars
					{
				Serial.readBytes(buffer, 3);                     // Abuse buffer to temp store 3 charaters
				strip.setPixelColor(currentLED++, buffer[0], buffer[1], buffer[2]);  // and assing to LEDs
			}

			if (currentLED > LEDCOUNT)                          // Reached the last LED? Display it!
			{
				strip.show();                                    // Make colors visible
				delayMicroseconds(SHOWDELAY);                    // Wait a few micro seconds

				state = STATE_WAITING;                           // Reset to waiting ...
				currentLED = 0;                                  // and go to LED one

				break;                                           // and exit ... and do it all over again
			}
			break;
		}                                                        // switch(state)
	}                                                         // if statement
	else {
		setAllLEDs(BLACK, 0);
	}

	if (millis() > timeout + 5000)                             // If no new incoming data after 5seconds, turn the strip OFF.
			{
		setAllLEDs(BLACK, 0);
	}
}                                                          // do_strip

void check_source(void) {
	if (digitalRead(ch_2) == HIGH) {
		if (curr_source != 2) {
			//lcd.setCursor(0,1);
			curr_source = 2;
			//lcd.print("      PS_4  ");                             // Channel 2 of HDMI switcher LCD name
		}
	} else if (digitalRead(ch_3) == HIGH) {
		if (curr_source != 3) {
			//lcd.setCursor(0,1);
			curr_source = 3;
			//lcd.print("    NOT USED");                             // Channel 3 of HDMI switcher LCD name
		}
	} else if (digitalRead(ch_4) == HIGH) {
		if (curr_source != 4) {
			//lcd.setCursor(0,1);
			curr_source = 4;
			//lcd.print("     CD/DVD ");                             // Channel 4 of HDMI switcher LCD name
		}
	} else if (digitalRead(ch_5) == HIGH) {
		if (curr_source != 5) {
			//lcd.setCursor(0,1);
			curr_source = 5;
			//lcd.print("    COMPUTER");                             // Channel 5 of HDMI switcher LCD name
		}
	} else

	{
		if (curr_source != 1) {
			//lcd.setCursor(0,1);
			curr_source = 1;
			//lcd.print("    AV INPUT");                             // Channel 1 of HDMI switcher LCD name
		}
	}
}

void check_button(void)                                  // ON/OFF button routine
		{
	if (digitalRead(led_button) == LOW) {
		if (strip_status == true) {
			strip_status = false;
			digitalWrite(status_led, LOW);
		} else {
			strip_status = true;
			digitalWrite(status_led, HIGH);
		}
		delay(25);
		while (digitalRead(led_button) == LOW) {
		}
	}
}

// Sets the color of all LEDs in the strip
// If 'wait'>0 then it will show a swipe from start to end
void setAllLEDs(uint32_t color, int wait) {
	for (int Counter = 0; Counter < LEDCOUNT; Counter++)    // For each LED
			{
		strip.setPixelColor(Counter, color);                // .. set the color

		if (wait > 0)                                        // if a wait time was set then
				{
			strip.show();                                       // Show the LED color
			delay(wait);                                        // and wait before we do the next LED
		}                                                     // if wait

	}                                                       // for Counter

	strip.show();                                           // Show all LEDs
}                                                         // setAllLEDs

