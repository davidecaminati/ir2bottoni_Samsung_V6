/*
 * IR-replicator for general purpose
 *
 * Version 0.3 Gen, 2015
 * Copyright 2015 Davide Caminati
 * http://caminatidavide.it/
 */

#include <IRremote.h>

int RECV_PIN = 11;
int BUTTON_PIN1 = 12;
int BUTTON_PIN2 = 7;
int BUTTON_PIN3 = 2;
int STATUS_PIN = 13;

IRrecv irrecv(RECV_PIN);
IRsend irsend;  // pin 3 PWM

decode_results results;

int lastButtonState1;
int lastButtonState2;
int lastButtonState3;
int lastbuttonpressed = 1;


// Storage for the recorded code
unsigned int rawCodesA[RAWBUF]; // The durations if raw
unsigned int rawCodesB[RAWBUF]; // The durations if raw
unsigned int rawCodesC[RAWBUF]; // The durations if raw
unsigned int rawCodesD[RAWBUF]; // The durations if raw


int codeLen; // The length of the code

void setup()
{
  Serial.begin(9600);
  irrecv.enableIRIn(); // Start the receiver
  // set pin and pullup
  pinMode(BUTTON_PIN1, INPUT);
  digitalWrite(BUTTON_PIN1, HIGH);
  pinMode(BUTTON_PIN2, INPUT);
  digitalWrite(BUTTON_PIN2, HIGH);
  pinMode(BUTTON_PIN3, INPUT);
  digitalWrite(BUTTON_PIN3, HIGH);
  pinMode(STATUS_PIN, OUTPUT);

}


void storeCode(decode_results *results) {
  	int count = results->rawlen;
	codeLen = results->rawlen - 1;
	for (int i = 1; i <= codeLen; i++) {
      if (i % 2) {
      	switch (lastbuttonpressed){
      	case 1:	
      		Serial.println("storeCode rawCodesA");
      		rawCodesA[i - 1] = results->rawbuf[i]*USECPERTICK - MARK_EXCESS;	
      		break;
	    case 2:
	    	Serial.println("storeCode rawCodesB");
      		rawCodesB[i - 1] = results->rawbuf[i]*USECPERTICK - MARK_EXCESS;
      		break;
	    case 3:
	    	Serial.println("storeCode rawCodesC");
      		rawCodesC[i - 1] = results->rawbuf[i]*USECPERTICK - MARK_EXCESS;
      		break;
	    case 4:
	    	Serial.println("storeCode rawCodesD");
      		rawCodesD[i - 1] = results->rawbuf[i]*USECPERTICK - MARK_EXCESS;
      		break;
      	}
      } 
      else {
      	switch (lastbuttonpressed){
      	case 1:	
      		rawCodesA[i - 1] = results->rawbuf[i]*USECPERTICK + MARK_EXCESS;	
      		break;
	    case 2:
      		rawCodesB[i - 1] = results->rawbuf[i]*USECPERTICK + MARK_EXCESS;
      		break;
	    case 3:
      		rawCodesC[i - 1] = results->rawbuf[i]*USECPERTICK + MARK_EXCESS;
      		break;
	    case 4:
      		rawCodesD[i - 1] = results->rawbuf[i]*USECPERTICK + MARK_EXCESS;
      		break;

      	}
      }
    }
}


void sendCode(int repeat) {
    // Assume 38 KHz
    switch (lastbuttonpressed){
    case 1:	
    	Serial.println("sendCode rawCodesA");
    	irsend.sendRaw(rawCodesA, codeLen, 38);
    	break;
    case 2:
    	Serial.println("sendCode rawCodesB");
    	irsend.sendRaw(rawCodesB, codeLen, 38);
    	break;
    case 3:
    	Serial.println("sendCode rawCodesC");
    	irsend.sendRaw(rawCodesC, codeLen, 38);
    	break;
    case 4:
    	Serial.println("sendCode rawCodesD");
    	irsend.sendRaw(rawCodesD, codeLen, 38);
    	break;
    }
    	
}


void loop() {
  // If button pressed, send the code.
  int buttonState1 = digitalRead(BUTTON_PIN1);
  int buttonState2 = digitalRead(BUTTON_PIN2);
  int buttonState3 = digitalRead(BUTTON_PIN3);
  
 if (lastButtonState1 == HIGH && buttonState1 == LOW) {
    Serial.println("Pressed 1   lastbuttonpressed += 1");
    sendCode(lastButtonState1 == buttonState1);
    lastbuttonpressed += 1;
    irrecv.enableIRIn(); // Re-enable receiver
    delay(200); //debouncing
  } 

 if (lastButtonState2 == HIGH && buttonState2 == LOW) {
    /*
    Serial.println("Pressed 2    lastbuttonpressed = 1");
    sendCode(lastButtonState2 == buttonState2);
    lastbuttonpressed = 1;
    irrecv.enableIRIn(); // Re-enable receiver
    delay(200); //debouncing
    */
    //store to eprom
  }

  if (lastButtonState3 == HIGH && buttonState3 == LOW) {
  	//simulation
	Serial.println("Pressed 3 start simulation");
  	// sequence for audio
  	int sequence[] = {1, 2, 3, 2, 2, 2, 3, 3, 2, 3, 1 }; 
  	int waitTime = 1000;
  	for (int i = 0; i < sizeof(sequence)/sizeof(int); i++){
  		Serial.println(i);
	  	lastbuttonpressed = sequence[i]; // 
	  	sendCode(lastButtonState3 == buttonState3);
	  	delay(waitTime);
  	}
  	lastbuttonpressed = 1;
  	irrecv.enableIRIn(); // Re-enable receiver
  	// todo  BIP()
  }
    
  // wait for new key
  if (irrecv.decode(&results)) {
      storeCode(&results);
      irrecv.resume(); // resume receiver
  }
  
  lastButtonState1 = buttonState1;
  lastButtonState2 = buttonState2;
  lastButtonState3 = buttonState3;
}
