/* University of Memphis Acoustic Sensing */ 
/* Audio Recorder for MUAS Project */
/* Kevin McKenzie - 2023 */

/* Included Libraries */
    #include <arch/board/board.h>
    #include <Arduino.h>
    #include <SDHCI.h>
    #include <Audio.h>
    #include <string>
    #include <sstream>
    #include <iostream>
    #include "screen.h"
    #include "temp.h"


/* Global Variables */
    String PROGRAM_NAME = "Acoustic Main V4.0 - 9/8/23-Kevin McKenzie";
    bool first_time = true;
    static const int ButtonPin = 8;
    static const int GainPin = 7;
    static const int resetPin = 13;
    unsigned long sample_millis = millis();
    float samptime = 0.9158; 

/* Screen */
    int error_thrown = false;
    String error_type = "";

/* SD Card */
    SDClass theSD;
    File myFile;
    #define FILENAME_LENGTH 23  
    String RECORD_FILE_NAME;
    int fileNumber;
    String file_time = "00:00";
    bool data_transfer_mode = false;

/* Temp Sensor */
    Adafruit_BME280 bme;
    int temp = 0;
    int humid = 0;
    int pressure = 0;

/* Audio */
    AudioClass *theAudio;
    bool ErrEnd = false;
    bool recording_in_progress;

    static const uint32_t recoding_sampling_rate = 48000;  
    static const uint8_t recoding_channel_number = 4;
    static const uint8_t recoding_bit_length = 16;  
    static const uint32_t recoding_time = 3600;     // 1800 seconds is 30min
    static const int32_t recoding_byte_per_second = recoding_sampling_rate * recoding_channel_number * recoding_bit_length / 8;
    static const uint64_t recoding_size = recoding_byte_per_second * recoding_time;
    static const uint32_t buff_size = 500 * 1024;  // default = 160k
    int recording_second_1 = 0;
    int recording_second_2 = 0;
    int recording_minute_1 = 0;
    int recording_minute_2 = 0;


//----------------------------------------------------------------------------
//-------------- SETUUUUUUP --------------------------------------------------
//----------------------------------------------------------------------------
void setup() {
    Serial.begin(115200);
    Serial.println(PROGRAM_NAME);

    pinMode(LED0, OUTPUT);
    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);
    pinMode(LED3, OUTPUT);
    pinMode(ButtonPin, INPUT_PULLUP); // Recording & Data Transfer Mode Button
    pinMode(GainPin, INPUT_PULLUP); // Gain Level & Auxiliary Function Button
    pinMode(resetPin, OUTPUT); // To Reset Board


    /* Data Transfer Mode */
      if (!digitalRead(ButtonPin)) {
        delay(1000);
        if (!digitalRead(ButtonPin)) {
            data_transfer_mode = true;        
        }
      }

    /* Screen */
      if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
          Serial.println("Screen Not Found");
          for(;;); // Don't proceed, loop forever
          }

      display.clearDisplay(); // Clear the buffer
      delay(1000); // Pause for 2 seconds
      drawBM(bitmap_title);
      delay(3000);

    /* SD Card */
      while (!theSD.begin()) {
          /* wait until SD card is mounted. */
          if (first_time) {Serial.println("Insert SD card"); first_time = false;}
          String text = "Insert SD Card";
          error_type = "No SD Card";
          error_thrown = true;
          drawScreen_Error(temp, humid, pressure, text.c_str(), error_type.c_str(), file_time.c_str());
          insertSDLEDS();
          }
      if (data_transfer_mode) {
        delay(1000);
        theSD.beginUsbMsc(); 
        drawScreen_DT();
        while(true) {}
        }
      if (error_thrown) {
        Serial.println("SD Card Entered");
        error_thrown = false;
        first_time = true;
        delay(100);
        }
  
    /* Temp Sensor */
      if (!bme.begin(0x76)) {
          if (first_time) {
            Serial.println("Temp Sensor Not Found"); 
            Serial.println("Restart Device");
            first_time = false;
            }
          error_thrown = true;
          drawScreen_TempError();
          while (digitalRead(ButtonPin)) {
            insertSDLEDS();
            }
          }
      if (error_thrown) {
        Serial.println("Continuing without Temp Sensor");
        first_time = true;
        error_thrown = false;
        delay(1000);
      }

      temp = temp_sensor(bme);
      humid = humid_sensor(bme);
      pressure = press_sensor(bme);

    /* Stand By */
      Serial.println("Standing By");
      Serial.println("Press Button to Start Recording");
      unsigned long sb_current_millis = millis(); 
      int sbst = samptime * 1000;

      // Get default value from text file
      // static const uint32_t mic_gain = 160;  // 0 - 210 (+21dB)
      uint32_t mic_gain = readGain();
      
      while (digitalRead(ButtonPin)) {
          int gain = mic_gain / 10;
          temp = temp_sensor(bme);
          humid = humid_sensor(bme);
          pressure = press_sensor(bme);
          drawScreen_Standby(temp, humid, pressure, gain); 

          if (!digitalRead(ButtonPin)) {
            unsigned long gain_millis = millis();
            while (!digitalRead(ButtonPin)) {
                unsigned long current_millis = millis(); 
                if (current_millis - gain_millis > 5) {
                  mic_gain += 20;
                  
                } // if


            } // while
            // thing that happens when pressed: start timer


            // thing that happens when released
            if (mic_gain > 210) {
                    mic_gain = 0;
                  }
                  else {
                    mic_gain += 20;
                  }
          } // if button

          

            // If gain button == 1 for 5s, set current gain value as default
              // by writing it to file
              // screen updates with message default has been set
          
            // else gain button released, cycle through gain values

          
          } // while stand by

    /* Audio */
      // Initialize
        theAudio = AudioClass::getInstance();
        theAudio->begin(audio_attention_cb);
        // theAudio->setRenderingClockMode(AS_CLKMODE_HIRES); 
        theAudio->setRecorderMode(AS_SETRECDR_STS_INPUTDEVICE_MIC, mic_gain, buff_size); 
        theAudio->initRecorder(AS_CODECTYPE_WAV,
                           "/mnt/sd0/BIN",
                           recoding_sampling_rate,
                           recoding_bit_length,
                           recoding_channel_number);

    /* File */

      fileNumber = 1;
      String record_filename = String(fileNumber) + ".wav";
      RECORD_FILE_NAME = record_filename;

      while (theSD.exists(RECORD_FILE_NAME)) {
        fileNumber++;
        record_filename = String(fileNumber) + ".wav";
        RECORD_FILE_NAME = record_filename;
        }

      Serial.print("Filename: ");
      Serial.println(RECORD_FILE_NAME);
      File file = theSD.open(RECORD_FILE_NAME, FILE_WRITE);

      if (!file){
        Serial.println("Error creating file");
        }
        else {
        Serial.println("File created successfully");
        file.close();
        }

      myFile = theSD.open(RECORD_FILE_NAME, FILE_WRITE);

      if (!myFile) {
        printf("File open error\n");
        exit(1);
        } 

    /* Start Recording */
      logData(temp_sensor(bme), humid_sensor(bme), press_sensor(bme));
      Serial.println("Recording Started!");
      recording_in_progress = true;
      lightsON();
      theAudio->writeWavHeader(myFile);
      theAudio->startRecorder();
      drawScreen_Recording(temp, humid, pressure, RECORD_FILE_NAME.c_str(), file_time.c_str());

} // setup

//----------------------------------------------------------------------------
//-------------- LOOOOOOOOP --------------------------------------------------
//----------------------------------------------------------------------------
void loop() {
 
    int recording_button = digitalRead(ButtonPin);
    unsigned long current_millis = millis(); 
    int st = samptime * 1000;
    err_t err;

    /* Read frames to record in file */
      err = theAudio->readFrames(myFile);

    /* Screen Timer */
      if ((current_millis - sample_millis) > st) {

          recording_second_1++;
          if (recording_second_1 > 9) {
            recording_second_1 = 0;
            recording_second_2++;
            if (recording_second_2 > 5) {
              recording_second_2 = 0;
              recording_minute_1++;
              if (recording_minute_1 > 9) {
                recording_minute_1 = 0;
                recording_minute_2++;
                if (recording_minute_2 > 5) {
                  recording_minute_2 = 0;
                  }
                }
              }          
            }

          std::stringstream ss;
          ss << recording_minute_2 << recording_minute_1 << ":" << recording_second_2 << recording_second_1;
          std::string time_str = ss.str();
          file_time = ss.str().c_str();
          
          // display.display();
          // display.clearDisplay();   // Clear the buffer
          // drawScreen_Recording(temp, humid, pressure, RECORD_FILE_NAME.c_str(), file_time.c_str());
          sample_millis = millis();
          }

    /* Data Log Sampling */
      // if ((current_millis - sample_millis) > st/2) {
      //     logData(temp_sensor(bme), humid_sensor(bme), press_sensor(bme));
      //   }      

    /* Stop Recording */
      if (recording_in_progress && !recording_button) {
          Serial.println("BUTTON PRESSED");
          theAudio->stopRecorder();
          sleep(1);
          err = theAudio->readFrames(myFile);
          drawScreen_Recording_Suc(temp, humid, pressure, RECORD_FILE_NAME.c_str(), file_time.c_str());
          goto exitRecording;
          }

      if (theAudio->getRecordingSize() > recoding_size) {
          theAudio->stopRecorder();
          sleep(1);
          err = theAudio->readFrames(myFile);
          drawScreen_Recording_Suc(temp, humid, pressure, RECORD_FILE_NAME.c_str(), file_time.c_str());
          goto exitRecording;
          }

    /* Errors */
      if (err != AUDIOLIB_ECODE_OK) {
          Serial.printf("File End! =%d\n", err);
          error_thrown = true;
          theAudio->stopRecorder(); 
          Serial.println("Error Area 1");
          error_type = "Error 1";
          drawScreen_Error(temp, humid, pressure, RECORD_FILE_NAME.c_str(), error_type.c_str(), file_time.c_str());
          goto exitRecording;
          }

      if (ErrEnd) {
          Serial.printf("Error End\n");
          error_thrown = true;
          theAudio->stopRecorder(); 
          Serial.println("Error Area 2");
          error_type = "Error 2: FIFO";
          drawScreen_Error(temp, humid, pressure, RECORD_FILE_NAME.c_str(), error_type.c_str(), file_time.c_str());
          goto exitRecording;
          }

      return;

    /* End Recording Procedure */
      exitRecording:
          theAudio->closeOutputFile(myFile);
          myFile.close();
          theAudio->setReadyMode();
          theAudio->end();
          Serial.println("Finished");
          lightsOFF();
          recording_in_progress = false;
          logData(temp_sensor(bme), humid_sensor(bme), press_sensor(bme));

          /* Wait for Reset */
          if (!error_thrown) {
              drawScreen_Recording_Suc(temp, humid, pressure, RECORD_FILE_NAME.c_str(), file_time.c_str());
              Serial.println("Press Button to Enter DT Mode");
              while (digitalRead(ButtonPin)) {}
              delay(1000);
              theSD.beginUsbMsc(); 
              drawScreen_DT();
              while(true) {}
            }
          else {
            drawScreen_Error(temp, humid, pressure, RECORD_FILE_NAME.c_str(), error_type.c_str(), file_time.c_str());
          }        
          
          // digitalWrite(resetPin, LOW);
          // delay(200);
          exit(0);

} // loop

// -------------------------------- FUNCTIONS

static void audio_attention_cb(const ErrorAttentionParam *atprm) {
  puts("Attention!");
  lightsOFF();

  if (atprm->error_code >= AS_ATTENTION_CODE_WARNING) {
    ErrEnd = true;
    logError("Error code " + String(atprm->error_code));
  }
}

void insertSDLEDS() {

  for (int i = 0; i < 5; i++) {
    lightsON();
    delay(100);
    lightsOFF();
    delay(100);
  }
}

void lightsON() {
  digitalWrite(LED0, HIGH);
  digitalWrite(LED1, HIGH);
  digitalWrite(LED2, HIGH);
  digitalWrite(LED3, HIGH);
}

void lightsOFF() {
  digitalWrite(LED0, LOW);
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);
}

void logError(String errorMsg) {
  File logFile = theSD.open("error_log.txt", FILE_WRITE);
  if (logFile) {
    // write error message to log file
    logFile.print("Error: ");
    logFile.println(errorMsg);
    logFile.close();

    // print error message to Serial monitor
    Serial.println("Error: " + errorMsg);
  } else {
    // if log file cannot be opened, print error message to Serial monitor
    Serial.println("Error: Could not create log file");
  }
}

void logData(int temperature, int humidity, int pressure) {
  
  String record_filename = String(fileNumber) + ".csv";
  File logFile = theSD.open(record_filename, FILE_WRITE);

  if (logFile) {
    // Check if the CSV file is empty
    bool isNewFile = logFile.size() == 0;

    // Write the CSV header if the file is empty
    if (isNewFile) {
      logFile.println("Temperature,Humidity,Pressure");
    }

    // Write the temperature, humidity, and pressure values to the CSV file
    logFile.print(temperature);
    logFile.print(",");
    logFile.print(humidity);
    logFile.print(",");
    logFile.println(pressure); 

    logFile.close();

  } else {
    // If log file cannot be opened, print error message to Serial monitor
    Serial.println("Error: Could not create or open log file");
  }
}

uint32_t readGain() {
  File logFile = theSD.open("BIN/gain.txt", FILE_READ);
  int gainValue = 0;

  if (logFile) {
    String gainString = "";
    // Read the gain value from the file
    while (logFile.available()) {
        char c = logFile.read();
        gainString += c;
          }
    gainValue = gainString.toInt();  // convert string to integer
    logFile.close();
  } 
  
  else {
      Serial.println("Error opening gain.txt");
  }

  return gainValue;

}














