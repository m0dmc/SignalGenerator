// My library for encoder and button polling - dmc - 2021 Dec

#include "dmcEncoder.h"

int dmcEncoder::EncoderInc(int x){
  if(x!=0){
    EncoderCount+=x;
    if(EncoderCount>=EncoderMax){
      EncoderCount-=(EncoderMax-EncoderMin);
    } else if(EncoderCount<EncoderMin){
      EncoderCount+=(EncoderMax-EncoderMin);
      }
#ifdef DEBUG
    Serial.print("Count=");
    Serial.println(EncoderCount);
#endif
  }
  return(EncoderCount);
}

void dmcEncoder::Setup(int p1,int p2,int p3){
  pinMode(ButtonPin=p1,INPUT);
  pinMode(CosPin=p2,INPUT);
  pinMode(SinPin=p3,INPUT);
}

int dmcEncoder::EncoderGet(){
  return(EncoderCount/ScaleDiv);
}
void dmcEncoder::EncoderSet(int Val,int Min,int Max){
  EncoderCount=Val*ScaleDiv;
  EncoderMin=Min*ScaleDiv;
  EncoderMax=Max*ScaleDiv;
}

int dmcEncoder::EncoderIRQ(){
  bool c,s;
  byte EncoderNow;
  int UpDown=0;// could use byte if concerned about speed
  c=digitalRead(CosPin);
  s=digitalRead(SinPin);
  //EncoderNow=(s?2:0)+((s^c)?1:0);// gray code to int
  EncoderNow=(s?1:0)+(c?2:0);// gray code to byte
  if(EncoderNow!=EncoderLast){
    switch (EncoderNow){
    default:
    case 0b00 :
      switch (EncoderLast){
      default:
      case 0b00 :
	break;
      case 0b01 :
	UpDown=1;
	UpDownLast=true;
	break;
      case 0b11 :
	UpDown=(UpDownLast?2:-2);
	break;
      case 0b10 :
	UpDown=-1;
	UpDownLast=false;
	break;
      }
      break;
    case 0b01 :
      switch (EncoderLast){
      default:
      case 0b01 :
	break;
      case 0b11 :
	UpDown=1;
	UpDownLast=true;
	break;
      case 0b10 :
	UpDown=(UpDownLast?2:-2);
	break;
      case 0b00 :
	UpDown=-1;
	UpDownLast=false;
	break;
      }
      break;
    case 0b11 :
      switch (EncoderLast){
      default:
      case 0b11 :
	break;
      case 0b10 :
	UpDown=1;
	UpDownLast=true;
	break;
      case 0b00 :
	UpDown=(UpDownLast?2:-2);
	break;
      case 0b01 :
	UpDown=-1;
	UpDownLast=false;
	break;
      }
      break;
    case 0b10 :
      switch (EncoderLast){
      default:
      case 0b10 :
	break;
      case 0b00 :
	UpDown=1;
	UpDownLast=true;
	break;
	case 0b01 :
	  UpDown=(UpDownLast?2:-2);
	  break;
	case 0b11 :
	  UpDown=-1;
	  UpDownLast=false;
	  break;
      }
      break;
    }
    EncoderLast=EncoderNow;// remember state of encoder inputs
#ifdef DEBUG
    Serial.print("Encoder=");
    //      Serial.print(c);
    //      Serial.print(s);
    Serial.println(UpDown);
#endif
    EncoderInc(UpDown);// update encoder counter
  }
  return(UpDown);// return 0,+/-1,+/-2
}

byte dmcEncoder::ButtonIRQ(){
  bool x;
  byte ButtonReturn=0; // Not Pressed
  unsigned long m;// Temporary Time value
  x=!digitalRead(ButtonPin);
  m=millis();
  if(ButtonLast!=x){
    if (!x){
      if((m-ButtonTime)>500){
	ButtonFlag=ButtonReturn=2; // return long press
      } else {
	ButtonFlag=ButtonReturn=1; // return short press
      }
    }
    ButtonLast=x;
    ButtonTime=m;
#ifdef DEBUG
    Serial.print("Button=");
    Serial.println(ButtonReturn);
#endif
  }
  return(ButtonReturn);
}
byte dmcEncoder::ButtonGet(){
  byte b=ButtonFlag;
  ButtonFlag=0;
  return(b);
}
