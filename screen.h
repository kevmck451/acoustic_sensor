// Code for OLED of Acoustic Module

#include <Arduino.h> 
#include <Wire.h>   //i2c
#include <Adafruit_GFX.h>     //oled 
#include <Adafruit_SSD1306.h> //oled

// -------------------- GLOBAL VARS  -----------------------
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16
#define VERSION "V4.0"


// -------------------- FUNCTIONS -----------------------
  // 10 characters size 2
  // 20 characters size 1
  // 8 lines size 1

/* Stand By Screen */
void drawScreen_Standby(int airtemp, int humid, int pressure, int gain) {
  display.clearDisplay();
  display.setTextSize(1);             
  display.setTextColor(SSD1306_WHITE);        
  display.setCursor(0,0);    

  display.print("Audio Recorder ");
  display.println(VERSION);

  display.print("T:");
  display.print(airtemp);
  display.print(" | ");
  display.print("H:");
  display.print(humid);
  display.print(" | ");
  display.print("P:");
  display.println(pressure);

  display.print("Standing By");
  display.print("   G:");
  display.print(gain);
  display.println("dB");

  display.println("--------------------");

  // display.println("Time: 00:00");
  display.println();
  display.println();

  display.println("Push Button to Start");
  
  display.display();
}

/* Recording Screen */
void drawScreen_Recording(int airtemp, int humid, int pressure, const char* filename, const char* time) {
  display.clearDisplay();
  display.setTextSize(1);             
  display.setTextColor(SSD1306_WHITE);        
  display.setCursor(0,0);    

  display.print("Audio Recorder ");
  display.println(VERSION);

  display.print("T:");
  display.print(airtemp);
  display.print(" | ");
  display.print("H:");
  display.print(humid);
  display.print(" | ");
  display.print("P:");
  display.println(pressure);

  display.print("Recording - ");
  display.println(filename);

  display.println("--------------------");
  
  // display.print("Time: ");
  // display.println(time);

  display.display();
}

/* Recording Screen Sucessful*/
void drawScreen_Recording_Suc(int airtemp, int humid, int pressure, const char* filename, const char* time) {
  display.clearDisplay();
  display.setTextSize(1);             
  display.setTextColor(SSD1306_WHITE);        
  display.setCursor(0,0);    

  display.print("Audio Recorder ");
  display.println(VERSION);

  display.print("T:");
  display.print(airtemp);
  display.print(" | ");
  display.print("H:");
  display.print(humid);
  display.print(" | ");
  display.print("P:");
  display.println(pressure);

  display.print("Rec Done - ");
  display.println(filename);

  display.println("--------------------");
  
  display.print("Time: ");
  display.println(time);

  display.println();

  display.println("Push button to enter");
  display.println("Data Transfer Mode");
  display.display();
}

/* Error Screen */
void drawScreen_Error(int airtemp, int humid, int pressure, const char* filename, const char* error_type, const char* time) {
  display.clearDisplay();
  display.setTextSize(1);             
  display.setTextColor(SSD1306_WHITE);        
  display.setCursor(0,0);    

  display.print("Audio Recorder ");
  display.println(VERSION);

  display.print("T:");
  display.print(airtemp);
  display.print(" | ");
  display.print("H:");
  display.print(humid);
  display.print(" | ");
  display.print("P:");
  display.println(pressure);

  display.print("ERROR: ");
  display.println(filename);

  display.println("--------------------");

  display.print("Time: ");
  display.println(time);

  display.print("Type: ");
  display.println(error_type);
  
  display.println();

  // display.println("Push Button to Reset");
  
  display.display();
}

/* Error Screen Temp*/
void drawScreen_TempError() {
  display.clearDisplay();
  display.setTextSize(1);             
  display.setTextColor(SSD1306_WHITE);        
  display.setCursor(0,0);    

  display.print("Audio Recorder ");
  display.println(VERSION);

  display.print("T: 0");
  display.print(" | ");
  display.print("H: 0");
  display.print(" | ");
  display.println("P: 0");

  display.println("ERROR: Temp Sensor ");

  display.println("--------------------");

  display.println("Type: No Response");

  
  display.println();

  display.println("Restart Recorder");
  
  display.display();
}

/* Data Transfer Screen */
void drawScreen_DT() {
  display.clearDisplay();
  display.setTextSize(2);             
  display.setTextColor(SSD1306_WHITE);        
  display.setCursor(0,0);    

  display.println("DT Mode");

  display.setTextSize(1);
  display.println();

  display.println("Eject SD when done");

  display.println("then restart device");
  
  display.display();
}

/* Default Gain Set Screen */
void drawScreen_GainDefault(int gain) {
  display.clearDisplay();
  display.setTextSize(2);             
  display.setTextColor(SSD1306_WHITE);        
  display.setCursor(0,0);    

  display.println("Gain Set!");

  display.setTextSize(1);
  display.println();

  display.println("   Default Gain");

  display.println("   is now set to:");
  display.print("    ");
  display.print(gain);
  display.println("dB");
  
  display.display();
}

void drawBM(const uint8_t* bitmap) {
  
  const uint8_t bitmapWidth = 128;
  const uint8_t bitmapHeight = 64;

  display.clearDisplay();
  display.drawBitmap(0, 0, bitmap, bitmapWidth, bitmapHeight, SSD1306_WHITE);
  display.display();
}



// ---------------- BITMAPS --------------------------

const unsigned char bitmap_title [] PROGMEM = {
	// 'picture, 48x48px
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0xff, 0xf1, 0xdf, 0xe7, 0x1d, 0xc7, 0x7e, 0x3f, 0xfb, 0xf0, 0x7c, 0xff, 0xfc, 0x38, 0x7e, 0x7f, 
	0xff, 0xf1, 0xdf, 0xe7, 0x1d, 0xc7, 0x7e, 0x3f, 0xfb, 0xf8, 0xfe, 0xff, 0xfe, 0x78, 0xff, 0x7f, 
	0xff, 0xf1, 0xdc, 0x07, 0x1d, 0xe7, 0x7e, 0x3f, 0x03, 0xbd, 0xce, 0xff, 0xff, 0xf1, 0xff, 0xf0, 
	0x1c, 0x71, 0xdc, 0x07, 0x1d, 0xf7, 0x7f, 0x7f, 0x03, 0x9d, 0xc0, 0xe3, 0x8f, 0xe1, 0xe7, 0xf0, 
	0x1c, 0x7f, 0xdf, 0xc7, 0x1d, 0xff, 0x77, 0x77, 0xf3, 0xbd, 0xfc, 0xe3, 0x87, 0xc1, 0xc3, 0xff, 
	0x1c, 0x7f, 0xdf, 0xc7, 0x1d, 0xff, 0x77, 0xf7, 0xf3, 0xf8, 0xfe, 0xe3, 0x83, 0xc1, 0xc3, 0xff, 
	0x1c, 0x71, 0xdc, 0x07, 0x1d, 0xdf, 0x73, 0xe7, 0x03, 0xf0, 0x0e, 0xe3, 0x83, 0x81, 0xe7, 0xf0, 
	0x1c, 0x71, 0xdc, 0x07, 0xfd, 0xcf, 0x73, 0xe7, 0x03, 0xf9, 0xce, 0xe3, 0x83, 0x81, 0xff, 0xf0, 
	0x1c, 0x71, 0xdf, 0xe3, 0xf9, 0xc7, 0x71, 0xc7, 0xfb, 0xbd, 0xfc, 0xe3, 0x83, 0x80, 0xff, 0x70, 
	0x1c, 0x71, 0xdf, 0xe1, 0xf1, 0xc7, 0x71, 0xc7, 0xfb, 0x9c, 0xf8, 0xe3, 0x83, 0x80, 0x7e, 0x70, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0xf0, 0x00, 0x3c, 0xff, 0xfe, 0x78, 0x00, 0x1e, 0x7f, 0xfc, 0x0f, 0x80, 0x3e, 0x7c, 0x1f, 0xf8, 
	0xf0, 0x00, 0x3c, 0xff, 0xfe, 0x78, 0x00, 0x1e, 0x7f, 0xff, 0x0f, 0x80, 0x3e, 0x7c, 0x3f, 0xfc, 
	0xf8, 0x00, 0x7c, 0xff, 0xfe, 0x7c, 0x00, 0x3e, 0x7f, 0xff, 0x8f, 0x80, 0x3e, 0x7c, 0x7f, 0xfe, 
	0xf8, 0x00, 0x7c, 0xff, 0xfe, 0x7c, 0x00, 0x3e, 0x7f, 0xff, 0xcf, 0x80, 0x3e, 0x7c, 0xfc, 0x3f, 
	0xfc, 0x00, 0xfc, 0xf8, 0x00, 0x7e, 0x00, 0x7e, 0x7c, 0x0f, 0xcf, 0x80, 0x3e, 0x7c, 0xf8, 0x1f, 
	0xfc, 0x00, 0xfc, 0xf8, 0x00, 0x7e, 0x00, 0x7e, 0x7c, 0x07, 0xcf, 0x80, 0x3e, 0x7c, 0xf0, 0x00, 
	0xfe, 0x01, 0xfc, 0xf8, 0x00, 0x7f, 0x00, 0xfe, 0x7c, 0x07, 0xcf, 0x80, 0x3e, 0x7c, 0xf0, 0x00, 
	0xff, 0x03, 0xfc, 0xf8, 0x00, 0x7f, 0x81, 0xfe, 0x7c, 0x07, 0xcf, 0x80, 0x3e, 0x7c, 0xf8, 0x00, 
	0xff, 0x87, 0xfc, 0xff, 0xfc, 0x7f, 0xc3, 0xfe, 0x7c, 0x0f, 0xcf, 0xff, 0xfe, 0x7c, 0xff, 0xf0, 
	0xff, 0xcf, 0xfc, 0xff, 0xfc, 0x7f, 0xe7, 0xfe, 0x7f, 0xff, 0xcf, 0xff, 0xfe, 0x7c, 0x7f, 0xfc, 
	0xff, 0xff, 0xfc, 0xff, 0xfc, 0x7f, 0xff, 0xfe, 0x7f, 0xff, 0x8f, 0xff, 0xfe, 0x7c, 0x3f, 0xfe, 
	0xfb, 0xff, 0x7c, 0xff, 0xfc, 0x7d, 0xff, 0xbe, 0x7f, 0xff, 0x0f, 0xff, 0xfe, 0x7c, 0x0f, 0xff, 
	0xf9, 0xfe, 0x7c, 0xf8, 0x00, 0x7c, 0xff, 0x3e, 0x7f, 0xfc, 0x0f, 0x80, 0x3e, 0x7c, 0x00, 0x1f, 
	0xf8, 0xfc, 0x7c, 0xf8, 0x00, 0x7c, 0x7e, 0x3e, 0x7c, 0x00, 0x0f, 0x80, 0x3e, 0x7c, 0x00, 0x0f, 
	0xf8, 0x78, 0x7c, 0xf8, 0x00, 0x7c, 0x3c, 0x3e, 0x7c, 0x00, 0x0f, 0x80, 0x3e, 0x7c, 0x00, 0x0f, 
	0xf8, 0x30, 0x7c, 0xf8, 0x00, 0x7c, 0x18, 0x3e, 0x7c, 0x00, 0x0f, 0x80, 0x3e, 0x7c, 0xf8, 0x1f, 
	0xf8, 0x00, 0x7c, 0xff, 0xfe, 0x7c, 0x00, 0x3e, 0x7c, 0x00, 0x0f, 0x80, 0x3e, 0x7c, 0xfc, 0x3f, 
	0xf8, 0x00, 0x7c, 0xff, 0xfe, 0x7c, 0x00, 0x3e, 0x7c, 0x00, 0x0f, 0x80, 0x3e, 0x7c, 0x7f, 0xfe, 
	0xf8, 0x00, 0x7c, 0xff, 0xfe, 0x7c, 0x00, 0x3e, 0x7c, 0x00, 0x0f, 0x80, 0x3e, 0x7c, 0x3f, 0xfc, 
	0xf8, 0x00, 0x7c, 0xff, 0xfe, 0x7c, 0x00, 0x3e, 0x7c, 0x00, 0x0f, 0x80, 0x3e, 0x7c, 0x1f, 0xf8, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x1f, 0xf7, 0xfd, 0xff, 0x71, 0xdf, 0xf7, 0xfd, 0xdf, 0xf0, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x1f, 0xf7, 0xfd, 0xff, 0x71, 0xdf, 0xf7, 0xfd, 0xdf, 0xf0, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x1f, 0xf7, 0xfd, 0xff, 0x71, 0xdf, 0xf7, 0xfd, 0xdf, 0xf0, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x1c, 0x77, 0x01, 0xc7, 0x71, 0xdc, 0x00, 0xe1, 0xdc, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x1c, 0x77, 0x01, 0xc7, 0x71, 0xdc, 0x00, 0xe1, 0xdc, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x1f, 0xf7, 0x01, 0xc7, 0x71, 0xdf, 0xf0, 0xe1, 0xdc, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x1f, 0xf7, 0x01, 0xc7, 0x71, 0xdf, 0xf0, 0xe1, 0xdc, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x1f, 0xf7, 0x01, 0xc7, 0x71, 0xc0, 0x70, 0xe1, 0xdc, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x1c, 0x77, 0x01, 0xc7, 0x71, 0xc0, 0x70, 0xe1, 0xdc, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x1c, 0x77, 0xfd, 0xff, 0x7f, 0xdf, 0xf0, 0xe1, 0xdf, 0xf0, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x1c, 0x77, 0xfd, 0xff, 0x7f, 0xdf, 0xf0, 0xe1, 0xdf, 0xf0, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x1c, 0x77, 0xfd, 0xff, 0x7f, 0xdf, 0xf0, 0xe1, 0xdf, 0xf0, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x3f, 0xef, 0xfb, 0x8e, 0xff, 0xbf, 0xef, 0xf8, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x3f, 0xef, 0xfb, 0x8e, 0xff, 0xbf, 0xef, 0xf8, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x3f, 0xef, 0xfb, 0xce, 0xff, 0xbf, 0xef, 0xf8, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x38, 0x0e, 0x03, 0xce, 0xe0, 0x3f, 0xee, 0x38, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x38, 0x0e, 0x03, 0xee, 0xe0, 0x38, 0xee, 0x38, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x3f, 0xef, 0xe3, 0xfe, 0xff, 0xb8, 0xee, 0x38, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x3f, 0xef, 0xe3, 0xfe, 0xff, 0xb8, 0xef, 0xf0, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x6e, 0x03, 0xbe, 0x03, 0xb8, 0xef, 0xe0, 0x00, 0x00, 0x02, 0x51, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x6e, 0x03, 0x9e, 0x03, 0xb8, 0xef, 0xf0, 0x00, 0x00, 0x02, 0x9b, 
	0x00, 0x00, 0x00, 0x00, 0x3f, 0xef, 0xfb, 0x9e, 0xff, 0xbf, 0xee, 0xf8, 0x00, 0x00, 0x03, 0x15, 
	0x00, 0x00, 0x00, 0x00, 0x3f, 0xef, 0xfb, 0x8e, 0xff, 0xbf, 0xee, 0x78, 0x00, 0x00, 0x02, 0x91, 
	0x00, 0x00, 0x00, 0x00, 0x3f, 0xef, 0xfb, 0x8e, 0xff, 0xbf, 0xee, 0x38, 0x00, 0x00, 0x02, 0x51
};




