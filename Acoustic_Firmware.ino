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
    String PROGRAM_NAME = "Acoustic Main V4.0 - 9/12/23-Kevin McKenzie";
    bool first_time = true;
    static const int ButtonPin = 8;
    static const int GainPin = 7;
    static const int resetPin = 13;
    unsigned long sample_millis = millis();
    float samptime = 1.0;  //0.9158

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
    String old_file_time = "00:00";
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
    bool gain_button_pressed = false;
    bool gain_button_released = false;
    int recording_second_1 = 0;
    int recording_second_2 = 0;
    int recording_minute_1 = 0;
    int recording_minute_2 = 0;
    int gain_value = 0;


//----------------------------------------------------------------------------
//-------------- SETUUUUUUP --------------------------------------------------
//----------------------------------------------------------------------------
void setup() {
    /* Setup */
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
          display.clearDisplay(); // Clear the buffer
          delay(1000); // Pause for 2 seconds
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
          display.clearDisplay(); // Clear the buffer
          delay(1000); // Pause for 2 seconds
          drawScreen_TempError();
          if (first_time) {
            Serial.println("Temp Sensor Not Found"); 
            Serial.println("Restart Device");
            first_time = false;
            }
          error_thrown = true;
          
          while (digitalRead(ButtonPin)) {
            insertSDLEDS();
            }
        } // if
      if (error_thrown) {
        Serial.println("Continuing without Temp Sensor");
        first_time = true;
        error_thrown = false;
        delay(1000);
        } // if

      temp = temp_sensor(bme);
      humid = humid_sensor(bme);
      pressure = press_sensor(bme);

    /* Stand By */
      Serial.println("Standing By");
      Serial.println("Press Button to Start Recording");
      unsigned long sb_current_millis = millis(); 
      int sbst = samptime * 1000;
      uint32_t mic_gain = readGain();
      unsigned long gain_millis;
      
      
      while (digitalRead(ButtonPin)) {
          bool default_set = false;
          int gain = mic_gain / 10;
          gain_value = gain;
          temp = temp_sensor(bme);
          humid = humid_sensor(bme);
          pressure = press_sensor(bme);
          drawScreen_Standby(temp, humid, pressure, gain); 

          if (!digitalRead(GainPin)) {
            gain_millis = millis();

            while (!digitalRead(GainPin)) {
              unsigned long current_millis = millis();

              if (current_millis - gain_millis > 5000) {
                setGainDefault(gain);
                drawScreen_GainDefault(gain);
                Serial.println("New Default Set!");
                default_set = true;
                delay(3000);
                break;
                } // if
              } // while

            if (!default_set) {
              if (mic_gain > 210) { mic_gain = 0;}
              else {mic_gain += 20;} 
            } // if 
          } // if button
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
      drawScreen_Recording(temp, humid, pressure, RECORD_FILE_NAME.c_str(), file_time.c_str(), gain_value);
      

} // setup

//----------------------------------------------------------------------------
//-------------- LOOOOOOOOP --------------------------------------------------
//----------------------------------------------------------------------------
void loop() {
    /* Setup */
      int recording_button = digitalRead(ButtonPin);
      int gain_button = digitalRead(GainPin);
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
          // drawScreen_Recording_Time(temp, humid, pressure, RECORD_FILE_NAME.c_str(), file_time.c_str());
          // logData(temp_sensor(bme), humid_sensor(bme), press_sensor(bme));
          sample_millis = millis();
          } // if



    /* Update Record Time */
      if (gain_button == 0) {gain_button_pressed = true;}
      if (gain_button_pressed && gain_button == 1) {gain_button_released = true;}
      if (gain_button_pressed) {
        if (old_file_time != file_time.c_str()) {
          drawScreen_Recording_Time(temp, humid, pressure, RECORD_FILE_NAME.c_str(), file_time.c_str(), gain_value);
          Serial.println("Time Updated");
          old_file_time = file_time.c_str();
            }
        } // if

      if (gain_button_released) {
        gain_button_pressed = false;
        gain_button_released = false;
        drawScreen_Recording(temp, humid, pressure, RECORD_FILE_NAME.c_str(), file_time.c_str(), gain_value);
        }


    /* Stop Recording */
      if (recording_in_progress && !recording_button) {
          Serial.println("Recording Button Pressed");
          theAudio->stopRecorder();
          sleep(1);
          err = theAudio->readFrames(myFile);
          drawScreen_Recording_Suc(temp, humid, pressure, RECORD_FILE_NAME.c_str(), file_time.c_str(), gain_value);
          goto exitRecording;
          }

      if (theAudio->getRecordingSize() > recoding_size) {
          theAudio->stopRecorder();
          sleep(1);
          err = theAudio->readFrames(myFile);
          drawScreen_Recording_Suc(temp, humid, pressure, RECORD_FILE_NAME.c_str(), file_time.c_str(), gain_value);
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
          delay(100);
          average_weather_data();


          /* Wait for Reset */
          if (!error_thrown) {
              drawScreen_Recording_Suc(temp, humid, pressure, RECORD_FILE_NAME.c_str(), file_time.c_str(), gain_value);
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
  drawScreen_TempError();
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

  // Serial.println(gainValue);
  return gainValue;

}

void setGainDefault(int gain) {
  
  // remove current gain.txt file
  theSD.remove("BIN/gain.txt");

  File logFile = theSD.open("BIN/gain.txt", FILE_WRITE);

  logFile.println(gain * 10);
  logFile.close();
  
}

void average_weather_data() {
  String record_filename = String(fileNumber) + ".csv";
  File logFile = theSD.open(record_filename, FILE_READ);

  if (logFile) {
    long totalTemperature = 0;
    long totalHumidity = 0;
    long totalPressure = 0;
    int rowCount = 0;

    // Skip header line
    String header = logFile.readStringUntil('\n');
    
    while (logFile.available()) {
      // Read a line from the CSV file
      String line = logFile.readStringUntil('\n');
      
      // Split the line to get temperature, humidity, and pressure values
      int firstCommaIndex = line.indexOf(",");
      int secondCommaIndex = line.lastIndexOf(",");
      
      int temp = line.substring(0, firstCommaIndex).toInt();
      int hum = line.substring(firstCommaIndex + 1, secondCommaIndex).toInt();
      int pres = line.substring(secondCommaIndex + 1).toInt();

      totalTemperature += temp;
      totalHumidity += hum;
      totalPressure += pres;

      rowCount++;
    }
    logFile.close();

    // Calculate the averages and round them
    int avgTemperature = (totalTemperature + rowCount / 2) / rowCount; // Rounding
    int avgHumidity = (totalHumidity + rowCount / 2) / rowCount;       // Rounding
    int avgPressure = (totalPressure + rowCount / 2) / rowCount;       // Rounding

    theSD.remove(record_filename);
    logFile = theSD.open(record_filename, FILE_WRITE);

    if (logFile) {
      logFile.println("Temperature,Humidity,Pressure");
      logFile.print(avgTemperature);
      logFile.print(",");
      logFile.print(avgHumidity);
      logFile.print(",");
      logFile.println(avgPressure);
      logFile.close();
    } else {
      Serial.println("Error: Could not open log file for writing");
    }

  } else {
    Serial.println("Error: Could not open log file for reading");
  }
}











