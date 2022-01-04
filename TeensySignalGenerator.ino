// Experiment with Teensy

// Audio stuff...
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

AudioSynthWaveform       waveformL; 
AudioSynthWaveform       waveformR; 
AudioOutputUSB           usb;  
AudioOutputI2S           i2s;
AudioConnection          patchCord1(waveformL, 0, usb, 0); 
AudioConnection          patchCord2(waveformR, 0, usb, 1); // right
AudioConnection          patchCord3(waveformL, 0, i2s, 0);
AudioConnection          patchCord4(waveformR, 0, i2s, 1);
AudioControlSGTL5000     sgtl5000;

#include <dmcTermMenu.h>
class dmcTermMenu TMenu;

#define HELP_LIST {  "Terminal line commands: (Teensy Signal Generator)", \
                     "A = n.nn  -  Left  Gain (0..1)"      , \
                     "B = n.nn  -  Right Gain (0..1)"       , \
                     "C = nnnn  -  Left  Freq (100 ..2000)" , \
                     "D = nnnn  -  Right Freq (100 ..2000)" , \
                     "E = n  Waveform Left  1=Sin 2=Sq"      , \
                     "F = n  Waveform Right 3=Saw1 4=Saw2 5=Tri" }
#define MENU_ITEMS 6
float MenuItem[MENU_ITEMS+1];

void UpDateMenuItems(){
  float f;
  int i;
  for(i=1;i<=MENU_ITEMS;i++){
    f=TMenu.DataGet(i); 
    switch(i){
    default:
      break;
    case 1:
      if(f!=MenuItem[i]){
        MenuItem[i]=f;
        waveformL.amplitude(f);
      }
      break;
    case 2:
      if(f!=MenuItem[i]){
        MenuItem[i]=f;
        waveformR.amplitude(f);
      }
      break;
    case 3:
      if(f!=MenuItem[i]){
        MenuItem[i]=f;
        waveformL.frequency(f);
      }
      break;
    case 4:
      if(f!=MenuItem[i]){
        MenuItem[i]=f;
        waveformR.frequency(f);
      }
      break;
    case 5:
      if(f!=MenuItem[i]){
        MenuItem[i]=f;
        if (f<2) {
          waveformL.begin(WAVEFORM_SINE);
        } else if (f<3){
          waveformL.begin(WAVEFORM_SQUARE);
        } else if (f<4){
          waveformL.begin(WAVEFORM_SAWTOOTH);
        } else if (f<5){
          waveformL.begin(WAVEFORM_SAWTOOTH_REVERSE);
        } else {
          waveformL.begin(WAVEFORM_TRIANGLE);
        } 
      }
      break;
    case 6:
      if(f!=MenuItem[i]){
        MenuItem[i]=f;
        if (f<2) {
          waveformR.begin(WAVEFORM_SINE);
        } else if (f<3){
          waveformR.begin(WAVEFORM_SQUARE);
        } else if (f<4){
          waveformR.begin(WAVEFORM_SAWTOOTH);
        } else if (f<5){
          waveformR.begin(WAVEFORM_SAWTOOTH_REVERSE);
        } else {
          waveformR.begin(WAVEFORM_TRIANGLE);
        } 
      } 
      break;
    }
  }
}
#define LED 13
void LEDstate(bool s){
  digitalWrite(LED,s);
}

void setup() {
  Serial.begin(115200);     // Serial port used for debug
  delay(1000);
  AudioMemory(10);
  delay(250);
  
  Serial.print("Teensy Setup..");
  Serial.println(__DATE__ " " __TIME__ );
  const char * NewMenu[7]=HELP_LIST ;
  TMenu.Setup(6,(char **)NewMenu);
  TMenu.DataSet(0,1);// running
  TMenu.DataSet(1,0.2,0,1);// gain 0.. 1
  TMenu.DataSet(2,0.2,0,1);// gain 0.. 1
  TMenu.DataSet(3,500,100,2000); // freq 100 .. 2000
  TMenu.DataSet(4,1500,100,2000); // freq 100 .. 2000
  TMenu.DataSet(5,1,1,5); // waveform select
  TMenu.DataSet(6,1,1,5);
  pinMode(LED,OUTPUT);
  LEDstate(false);
 
  waveformL.begin(0,1150,WAVEFORM_SQUARE);
  waveformR.begin(0,1150,WAVEFORM_SQUARE);
  sgtl5000.enable();
  sgtl5000.volume(0.4);
  
  LEDstate(true);
  Serial.println("..Finished");
}

void loop() {
  long m,mlast=0;
  char buff[16]; // line only 16 chars long
  float RunStop=-1;
  bool NewData=false;
  
  while(true){
    m=millis();
    NewData=false;
    if(TMenu.TerminalIRQ()){
      TMenu.CommandDecode(buff);// screen redraw
      NewData=true;
    }
        
    if(mlast<(m-500)){// every 500ms
      mlast=m;
      
      if(TMenu.DataGet(0)!=RunStop){
        RunStop=TMenu.DataGet(0);
        LEDstate(RunStop!=0);
        // Set Audio Gain to zero
        if(RunStop==0)TMenu.DataSet(1,0); 
        if(RunStop==0)TMenu.DataSet(2,0); 
        NewData=true;
      }
      
    }
    if(NewData){
      UpDateMenuItems();// read menu and implement changes
      TMenu.ShowScreen();// append run/stop mess
     // TMenu.ShowScreen((char *)__DATE__ " " __TIME__ );// append run/stop mess
    }
  }
  
}
