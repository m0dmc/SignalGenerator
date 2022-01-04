// My library for terminal entry menu - dmc - 2021 Dec
// Use #define to set DATA_ITEMS and HELP_LIST externally
// ToDo: Add rotary encoder interface

#ifndef dmcTermMenu_h
#define dmcTermMenu_h
#include <Arduino.h>

#define USE_ENCODER
#ifdef USE_ENCODER
#include "dmcEncoder.h"
#else
// Use encoder or keyboard not both
#define KEYBOARD_SELECT_LINE
#endif

class dmcTermMenu{
private:
#ifdef USE_ENCODER
class dmcEncoder MyEnc;
#endif
#define BUFF_LEN 100
  char StringIn[BUFF_LEN];   // String from serial in
  char StringTmp[BUFF_LEN];  // Tmp string
  int  StringInLength=0;     // Serial string input length
  bool StringComplete=false; // Line terminator received
  // n variables  + ( run/stop ) = (n+1)
  // run/stop =0, item1, item2 etc
#define MAX_DATA_ITEMS   10
  // array max =DATA_ITEMS + 1
  int DataItems = MAX_DATA_ITEMS ;
  float  MenuData[MAX_DATA_ITEMS+1]; 
  float  MinData[MAX_DATA_ITEMS+1]; 
  float  MaxData[MAX_DATA_ITEMS+1];
  float  IncData[MAX_DATA_ITEMS+1];
  int MenuSelected=0;
  int EncSelected=1;

  const char *MenuList[MAX_DATA_ITEMS+1] = {
    "CLI: (Default)", 
    "A = nnnn",    
    "B = nnnn",  
    "C = nnnn",
    "D = nnnn",
    "E = nnnn",
    "F = nnnn",	
    "G = nnnn",	
    "H = nnnn",	
    "I = nnnn",	
    "J = nnnn"};
    
  void StringInClear();
  void StringInAddChar(char x);
  void DataSetCheck(int i,float f);
  void DataSetCheckInc(int i,int inc);
  bool StringInToNumber(char * PtrToStr,int idx);
  bool StringInGet(char * PtrToStr);
#ifdef USE_ENCODER
  void SetEncoderRange(int Menu);
  void EncoderInterpret();
#endif
public:
  void Setup(int n,char * NewMenu[]);
  void Setup();
  bool TerminalIRQ();
  void ShowScreen(char * EndMessage);
  void ShowScreen();
  int CommandDecode(char * PtrToStr);
  float DataGet(int i);
  void DataSet(int i,float x);
  void DataSet(int i,float x,float xmin,float xmax);
  void DataSet(int i,float x,float xmin,float xmax,float inc);
  // dmcTermMenu(void);
};

#endif
