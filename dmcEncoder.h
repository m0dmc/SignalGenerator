// My library for encoder and button polling - dmc - 2021 Dec
#ifndef dmcEncoder_h
#define dmcEncoder_h
#include <Arduino.h>

//#define DEBUG

class dmcEncoder{
private:
  int ButtonPin=0;
  int CosPin=0;
  int SinPin=0;
  int ScaleDiv=4;             // Step size
  unsigned long ButtonTime=0; // Time button pressed
  bool ButtonLast=0;          // Button last state
  byte ButtonFlag=0;          // Button state set by IRQ, cleared by reading
  byte EncoderLast=0;         // Encoder last state
  int  EncoderCount=0;        // Default Encoder count, min, max
  int  EncoderMin=0;
  int  EncoderMax=1024*ScaleDiv;
  bool UpDownLast=false;      // last increment direction
  int EncoderInc(int x);
public:
  void Setup(int p1,int p2,int p3);
  void Setup(int p1,int p2,int p3,int ScaleDiv);
  int EncoderGet();
  void EncoderSet(int Val,int Min,int Max);
  int EncoderIRQ();
  byte ButtonIRQ();
  byte ButtonGet();
    // dmcEncoder(void);
};
#endif
