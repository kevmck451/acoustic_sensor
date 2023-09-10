## Acoustic Sensor Operation Guide

This guide is an overview of how to operate the Sony Audio Recorder and mics 
in its current configuration. It is not a substitute for experience, 
practice, tutelage, or the official documentation of involved projects.

This guide (and other files) must be updated if the configuration is changed.
Do not make unauthorized or undocumented configuration changes.

### Equipment List
* Recorder
* Microphone Array
* 5V Battery with Cable
* Anometer
* Small phillipshead screwdriver 

### Operation Procedure

#### Ground Setup
* Make sure battery has charge (press button on battery)
* Attach battery to drone using velcro
* Verify mount components are in good condition (vibration dampeners)
* Slide recorder in mount (mic cables on backside of drone / screen on front)
* Plug in battery and mic cables into recorder. Mics A/B should be Channel 1/2
* Take wind reading with anometer and log for sample library

#### Power Up
* Press silver button on the side. Outer ring of the button should light up
* After start up, screen status mode should display "Standing By"
* Temp, Humid, and Pressure can be read on the screen
* Press blue botton to start recording. Status should change to "Recording..."

#### Landing
* Once landed, press the blue button again. Status should change to "Finished"
* Recording time information shoud be displayed along with filename
* Turn recorder off and repeat "Power Up" section to start another recording
* Run any additional flights if desired

#### Finished
* After stopping the recording, press silver button to turn device off


### Data Retrieval

Plug in usb from computer to micro usb cable on recorder. Power on device by pressing the silver button and hold down the blue button during start up and recorder will go into "Data Transfer Mode". Once in this mode, an Acoustic drive will appear on your computer and files can be viewed. Eject the drive from computer and turn device off by pressing silver button. After you've ejected the drive from you computer, the recorder will remount the drive in about 10 seconds so try to turn the device off immediate after ejection to avoid possible corruptions.


### Reprogramming the Recorder

Open arduino software will all sony spresense board plugins installed and serial monitor communicating at 115200. Steps to do this can be found on the sony spresense website. Plug in usb from computer to micro usb cable on recorder. Power on device by pressing the silver button. Arduino IDE should recognize the device and beginning displaying information starting with the software version information. From here, uploading code to the recorder is like dealing with any other common microcontroller.














