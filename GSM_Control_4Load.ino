#include <EEPROM.h>
#include <SoftwareSerial.h> //Create software serial object to communicate with SIM800L
SoftwareSerial GSM(8, 9);   //SIM800L Tx & Rx is connected to Arduino #8 & #9

String phone_no1 = "+923378655465"; //change +92 with country code and 3378655465 with phone number to sms
String phone_no2 = "Enter Number2"; 

String        RxString  = ""; // Will hold the incoming String  from the GSM shield
char          RxChar    = ' ';
int           Counter   = 0;
String        GSM_Nr    = "";
String        GSM_Msg   = "";

#define Relay1 2 // Load1 Pin Out
#define Relay2 3 // Load2 Pin Out
#define Relay3 4 // Load3 Pin Out
#define Relay4 5 // Load4 Pin Out

int load1, load2, load3, load4;

void setup(){ // put your setup code here, to run once

pinMode(Relay1, OUTPUT); digitalWrite(Relay1, 1); 
pinMode(Relay2, OUTPUT); digitalWrite(Relay2, 1); 
pinMode(Relay3, OUTPUT); digitalWrite(Relay3, 1); 
pinMode(Relay4, OUTPUT); digitalWrite(Relay4, 1); 

Serial.begin(9600);//Begin serial communication with Arduino and Arduino IDE (Serial Monitor)  
GSM.begin(9600);   //Begin serial communication with Arduino and SIM800L

Serial.println("Initializing....");
initModule("AT","OK",1000);                //Scan for GSM Module
initModule("AT+CPIN?","READY",1000);       //this command is used to check whether SIM card is inserted in GSM Module or not
initModule("AT+CMGF=1","OK",1000);         //Set SMS mode to ASCII
initModule("AT+CNMI=2,2,0,0,0","OK",1000); //Set device to read SMS if available and print to serial
Serial.println("Initialized Successfully"); 
  
load1 = EEPROM.read(1);
load2 = EEPROM.read(2);
load3 = EEPROM.read(3);
load4 = EEPROM.read(4);

relays();

delay(100);
}

void loop(){
  
  // scan for data from software serial port
  //-----------------------------------------------
  RxString = "";
  Counter = 0;
  while(GSM.available()){
    delay(1);  // short delay to give time for new data to be placed in buffer
    // get new character
    RxChar = char(GSM.read());
    //add first 200 character to string
    if (Counter < 200) {
      RxString.concat(RxChar);
      Counter = Counter + 1;
    }
  }
 
  // Is there a new SMS?
  //-----------------------------------------------
  if (Received(F("CMT:")) ) GetSMS();

if(GSM_Nr==phone_no1 || GSM_Nr==phone_no2){
  
if(GSM_Msg=="load1on") {load1=0; sendSMS(GSM_Nr,"Ok Load 1 is On");}
if(GSM_Msg=="load1off"){load1=1; sendSMS(GSM_Nr,"Ok Load 1 is Off");}

if(GSM_Msg=="load2on") {load2=0; sendSMS(GSM_Nr,"Ok Load 2 is On");}
if(GSM_Msg=="load2off"){load2=1; sendSMS(GSM_Nr,"Ok Load 2 is Off");}

if(GSM_Msg=="load3on") {load3=0; sendSMS(GSM_Nr,"Ok Load 3 is On");}
if(GSM_Msg=="load3off"){load3=1; sendSMS(GSM_Nr,"Ok Load 3 is Off");}

if(GSM_Msg=="load4on") {load4=0; sendSMS(GSM_Nr,"Ok Load 4 is On");}
if(GSM_Msg=="load4off"){load4=1; sendSMS(GSM_Nr,"Ok Load 4 is Off");}

if(GSM_Msg=="allon") {load1=0, load2=0, load3=0, load4=0; sendSMS(GSM_Nr,"Ok All Load is On");}
if(GSM_Msg=="alloff"){load1=1, load2=1, load3=1, load4=1; sendSMS(GSM_Nr,"Ok All Load is Off");}

if(GSM_Msg=="loadstatus"){
String loadst = "";

if(load1==0){loadst="Load1 On\r\n";}
        else{loadst="Load1 Off\r\n";}
        
if(load2==0){loadst=loadst + "Load2 On\r\n";}
        else{loadst=loadst + "Load2 Off\r\n";}
        
if(load3==0){loadst=loadst + "Load3 On\r\n";}
        else{loadst=loadst + "Load3 Off\r\n";}
        
if(load4==0){loadst=loadst + "Load4 On";}
        else{loadst=loadst + "Load4 Off";}
        
sendSMS(GSM_Nr,loadst);
}

eeprom_write();
relays();
}


GSM_Nr="";
GSM_Msg="";
}

void eeprom_write(){
EEPROM.write(1,load1);
EEPROM.write(2,load2);
EEPROM.write(3,load3);
EEPROM.write(4,load4);  
}

void relays(){  
digitalWrite(Relay1, load1); 
digitalWrite(Relay2, load2); 
digitalWrite(Relay3, load3); 
digitalWrite(Relay4, load4); 
}

// Send SMS 
void sendSMS(String number, String msg){
GSM.print("AT+CMGS=\"");GSM.print(number);GSM.println("\"\r\n"); //AT+CMGS=”Mobile Number” <ENTER> - Assigning recipient’s mobile number
delay(500);
GSM.println(msg); // Message contents
delay(500);
GSM.write(byte(26)); //Ctrl+Z  send message command (26 in decimal).
delay(5000);  
}

// Get SMS Content
void GetSMS() {
  //Get SMS number
  //================================================
  GSM_Nr  = RxString;
  //get number
  int t1 = GSM_Nr.indexOf('"');
  GSM_Nr.remove(0,t1 + 1);
  t1 = GSM_Nr.indexOf('"');
  GSM_Nr.remove(t1);
   
  // Get SMS message
  //================================================
  GSM_Msg = RxString;
  t1 = GSM_Msg.indexOf('"');
  GSM_Msg.remove(0,t1 + 1);
  t1 = GSM_Msg.indexOf('"');
  GSM_Msg.remove(0,t1 + 1);
  t1 = GSM_Msg.indexOf('"');
  GSM_Msg.remove(0,t1 + 1);
  t1 = GSM_Msg.indexOf('"');
  GSM_Msg.remove(0,t1 + 1);
  t1 = GSM_Msg.indexOf('"');
  GSM_Msg.remove(0,t1 + 1);
  t1 = GSM_Msg.indexOf('"');
  GSM_Msg.remove(0,t1 + 1);
  GSM_Msg.remove(0,1);
  GSM_Msg.trim();

Serial.print("Number:"); Serial.println(GSM_Nr);
Serial.print("SMS:"); Serial.println(GSM_Msg);
}

// Search for specific characters inside RxString 
boolean Received(String S) {
  if (RxString.indexOf(S) >= 0) return true; else return false;
}


// Init GSM Module 
void initModule(String cmd, char *res, int t){
while(1){
    Serial.println(cmd);
    GSM.println(cmd);
    delay(100);
    while(GSM.available()>0){
       if(GSM.find(res)){
        Serial.println(res);
        delay(t);
        return;
       }else{Serial.println("Error");}}
    delay(t);
  }
}
