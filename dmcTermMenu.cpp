// My library for terminal entry menu - dmc - 2021 Dec
 
#include "dmcTermMenu.h"

void dmcTermMenu::StringInClear(){ // clear input string
  int i;
  for(i=0;i<BUFF_LEN;i++)StringIn[i]=0;
  StringInLength=0;
  StringComplete=false;
}
void dmcTermMenu::StringInAddChar(char x){ // append char to input string
  if(StringInLength<(BUFF_LEN-1)){
    StringIn[StringInLength]=x;
    StringInLength++;
  }
}
void dmcTermMenu::DataSetCheck(int idx,float f){ // check limits (idx already checked)
  if(f>MaxData[idx])f=MaxData[idx];
  if(f<MinData[idx])f=MinData[idx];
  if(IncData[idx]==1){
    MenuData[idx]=(int)f;
  } else {
    MenuData[idx]=f;
  }
}
void dmcTermMenu::DataSetCheckInc(int idx,int inc){ // check limits (idx already checked)
  DataSetCheck(idx,MenuData[idx]+inc*IncData[idx]);
}
bool dmcTermMenu::StringInToNumber(char * PtrToStr,int idx){// convert input string to number
  char * last;
  float f;
  if(strlen(PtrToStr)>0){    // string length must be greater than 0
    // f=strtol(PtrToStr,&last,10);// integer base 10
    f=strtod(PtrToStr,&last);        // float (arduino only?) Valid number?
    if(*last==0){
      DataSetCheck(idx,f);
      return(true);        // end of string is NULL character
    }
  }
  return(false);
}
bool dmcTermMenu::StringInGet(char * PtrToStr){
  if(StringComplete){             // be careful here sometimes just 
    strcpy(PtrToStr,StringIn);    // pointer not contents copied
    StringInClear();
    return(true);
  }
  return(false);
}
#ifdef USE_ENCODER
void dmcTermMenu::SetEncoderRange(int Menu){
  if(Menu==0){
    MyEnc.EncoderSet(EncSelected,1,DataItems+1);
  } else {
    MyEnc.EncoderSet(EncSelected=(MenuData[Menu]/IncData[Menu]),
		     MinData[Menu]/IncData[Menu],
		     (MaxData[Menu]/IncData[Menu])+1);// scaling for <1 ?
  }
}
void dmcTermMenu::EncoderInterpret(){
  EncSelected=MyEnc.EncoderGet();
      Serial.print("\rEnc=");Serial.print(EncSelected);
  switch(MyEnc.ButtonGet()){
  default:
    break;
  case 1:
    if(MenuSelected==0){
      SetEncoderRange(MenuSelected=EncSelected);// new selection
    } else {
      MenuData[MenuSelected]=EncSelected*IncData[MenuSelected];
    }
    break;
  case 2:
    if(MenuSelected!=0){
      EncSelected=MenuSelected;
      SetEncoderRange(MenuSelected=0);// return to top level select
    }
    break;
  }
}
#endif
void dmcTermMenu::Setup(int n,char *NewMenu[]){
  int i;
  DataItems=((n<=MAX_DATA_ITEMS)?n:(MAX_DATA_ITEMS));
  for(i=0;i<=DataItems;i++){ // i=0 (0 or 1 run/stop)
    MenuData[i] = 1 ;               // Default value 1
    MinData[i]  = (i==0)?  0 : 1 ;  // range 1..1000
    MaxData[i]  = (i==0)?  1 : 1000;// Min and Max are both valid
    IncData[i]  = 1 ;               // step size when incrementing
    MenuList[i] = NewMenu[i];
  }
  StringInClear();
#ifdef USE_ENCODER
  MyEnc.Setup(4,6,5);
  SetEncoderRange(0);
#endif
}
void dmcTermMenu::Setup(){
  Setup(4,(char **)MenuList);
}

bool dmcTermMenu::TerminalIRQ(){ // Poll regularly, returns true for new data
  char inChar;
  bool EncoderIRQ=0;
  while (Serial.available()) {
    inChar = (char)Serial.read();
    if((inChar=='\n')|(inChar=='\r')){
      if(StringIn[0]!=0)StringComplete=true;
    } else {
      if(!StringComplete)StringInAddChar(inChar);
    }
  }
#ifdef USE_ENCODER
  if((0!=MyEnc.ButtonIRQ())|(0!=MyEnc.EncoderIRQ())){
    EncoderInterpret();
    EncoderIRQ=1;
  }
#endif
  return(StringComplete|EncoderIRQ);
}

void dmcTermMenu::ShowScreen(char * EndMessage){
  int i,j,n;
  Serial.print("\x1b[2J");    // clear screen command
  Serial.print("\x1b[H");     // home cursor command
  for (i=0;i<=DataItems;i++){
#ifdef USE_ENCODER
    if(MenuSelected==0){
      Serial.print(((i!=0)&(i==EncSelected)?"*":" "));// Highlight selected line
    }else{
      Serial.print(((i!=0)&(i==MenuSelected)?">":" "));
    }
#else
    Serial.print(((i!=0)&(i==MenuSelected)?">":" "));// Highlight selected line
#endif
    Serial.print(MenuList[i]);
    if(i==0){
      Serial.println();
    } else {
      n=40;//MenuList[i].length();
      n=strlen(MenuList[i]);
      for(j=n;j<45;j++)Serial.print(" ");
      Serial.print("= ");
      if(IncData[i]==1){
	Serial.println((int)MenuData[i]);
      } else {
	Serial.println(MenuData[i]);
      }
    }
  }
  Serial.println((MenuData[0]!=0)?
		 " R = run (Running)\n\r S = stop":
		 " R = run\n\r S = stop (Stopped)");
  if(MenuSelected!=0){
    Serial.print("Enc = ");
    Serial.println(EncSelected*IncData[MenuSelected]);
  }
  Serial.println(EndMessage);
}
void dmcTermMenu::ShowScreen(){
  ShowScreen((char *)"");
}
int dmcTermMenu::CommandDecode(char * PtrToStr){ // call when new data available (pointer set to new data)
  bool ValidCommand=false;
  int idx;
  strcpy(StringTmp,StringIn);
  if(StringComplete){
#ifdef KEYBOARD_SELECT_LINE
    if(MenuSelected!=0){
      if (strcmp(StringTmp,">")==0){
	DataSetCheckInc(MenuSelected,+1);
      } else if (strcmp(StringTmp,"<")==0){
	DataSetCheckInc(MenuSelected,-1);
      } else if (strcmp(StringTmp,">>")==0){
	DataSetCheckInc(MenuSelected,+5);
      } else if (strcmp(StringTmp,"<<")==0){
	DataSetCheckInc(MenuSelected,-5);
      } else {
	MenuSelected=0;
      }
      ValidCommand=true;//Anything goes, some just quit selection
    } else {
#endif
      if(StringTmp[1]==0){        // single character commands (run / stop / show data)
	switch(StringTmp[0]&0x5f){
	default:
#ifdef KEYBOARD_SELECT_LINE
	  idx=(StringTmp[0]&0x5f) - 'A' + 1 ;
	  if((idx>0)&(idx<=DataItems)){
	    MenuSelected=idx;
	    ValidCommand=true;
	  } 
#endif
	  break;
	case 'R':
	  MenuData[0]=1;
	  ValidCommand=true;
	  break;
	case 'S':
	  MenuData[0]=0;
	  ValidCommand=true;
	  break;
	} 
      } else if(StringTmp[1]=='='){ // set value commands
	idx=(StringTmp[0]&0x5f) - 'A' + 1 ;
	if((idx>0)&(idx<=DataItems)){
	  ValidCommand=StringInToNumber( &StringTmp[2] , idx );
	}
      }
#ifdef KEYBOARD_SELECT_LINE
    }
#endif
    StringInGet(PtrToStr); // clears StringComplete flag
    return(ValidCommand?1:0);
  }
  return(0);
}

float dmcTermMenu::DataGet(int i){
  if((i>0)&(i<=DataItems))return(MenuData[i]);
  return(MenuData[0]!=0); // return run/stop if i out of bounds
}
void dmcTermMenu::DataSet(int i,float x){
  if((i>0)&(i<=DataItems))DataSetCheck(i,x);
}
void dmcTermMenu::DataSet(int i,float x,float xmin,float xmax){
  float Range=(xmax-xmin);
  if((i>0)&(i<=DataItems)){
    MinData[i]=xmin;
    MaxData[i]=xmax;
    IncData[i]=(Range>1000)?10:((Range>100)?5:((Range>1)?1:0.01));
  }
  DataSet(i,x);
}
void dmcTermMenu::DataSet(int i,float x,float xmin,float xmax,float inc){
  if((i>0)&(i<=DataItems)){
    MinData[i]=xmin;
    MaxData[i]=xmax;
    IncData[i]=inc;
  }
  DataSet(i,x);
}
