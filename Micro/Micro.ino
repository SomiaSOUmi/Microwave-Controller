#include <Adafruit_Circuit_Playground.h> 
#include <SD.h>
#include <Wire.h>
#include <SPI.h>


#define CAP_THRESHOLD   50
#define DEBOUNCE        250
  int CT = 0 ; 
  String CPL; 
  int C = 0 ; // associated to CPL, helps to control CPL value
  unsigned long time_now = 0; // used to delay the display of the messages in the serial monitor screen 
  unsigned long t = 0;
  int m = 0 ; // used to control the messages display 
  int DS = 1 ; // door sensor 
  int CS = 0 ; // cooking status 
  int start = 0 ; // start and stop button
  int tr = 0 ; // magnetron temperature, it is added to the current temperature and we assumed that it is the magnetron teperature

// a simple function used to iddentify the press of a button 
boolean capButton(uint8_t pad) {
  if (CircuitPlayground.readCap(pad) > CAP_THRESHOLD) {
    return true;  
  } else {
    return false;
  }
}

void setup() {
    CircuitPlayground.begin();  
}

void loop() {
   if (CircuitPlayground.slideSwitch() and CS == 0 ) // if there is a request to open the door by switching the switch and the cooking has not started 
       {
    if (DS==0 ){ // the controller checks if the door was closed previously before the controller opens it 
      Serial.println("Door is open\n");}
      DS = 1 ; // the door is opened 
       }
    else if (CS == 1 and CircuitPlayground.slideSwitch()) // if the request to open the door is done when the cooking has started 
          { 
            Serial.println("Door is loocked and cannot be opened  \n");
            DS = 0 ; 
          }
    else 
      {
        DS = 0 ; 
       }
  if (capButton(12) and start == 0) // when the start sensor is touched asking to start cooking 
    {
    if ( DS == 1 ) // if the door is opened the cooking cannot start 
    {
    Serial.println("The Start button is pressed and the cooking hasn't started because the door is opened \n");
    }
    else if ((CPL != "LOW" and CPL != "MID" and CPL != "HIGH") or CT < 10 or CT >60 ) // bot inputs should be given 
       {
      Serial.println("The Start button is pressed but the cooking hasnt started because CT or CPL are not selected ") ; 
       }
      else 
        {
        start = 1 ; 
        Serial.println ("The start button is pressed ") ; 
         }
      time_now = millis();
    while(millis() < (time_now + 500)){}
    
         }
   if (capButton(12) and start == 1){
    start = 0 ;
    Serial.println(" The stop button is pressed") ;  
            time_now = millis();
    while(millis() < (time_now + 500)){}} // a smale delay is used to take some time before considering the sensor touch as a start or a stop 
    
    if (CT>=60) 
    {
      CT= 0;
      }
    if (CircuitPlayground.leftButton())
     {
        CT = CT +10;
    Serial.print(" Cooking time is: ") ;  
    Serial.println (CT) ;
    time_now = millis();
    while(millis() < (time_now + 300)){} // this delay is used to keep some time before increasing or restarting the cooking time
 
     }

   if (CircuitPlayground.rightButton()) 
     {
        time_now = millis();
        while(millis() < (time_now + 200)){} // same delay is used for the cookig power level changing 
        if (C == 3 ) { C = 0 ; }
        C = C+1 ; 
        if (C == 1){
        CPL = "LOW" ; }
        else if (C == 2)
        {CPL = "MID";}
        else {CPL = "HIGH";}
        Serial.print(" Cooking Power level is: ") ;  
        Serial.println ( CPL) ; 
         }

        
   if ( CT<= 60 and CT >=10 )
    {
    if (CPL == "LOW" or CPL == "MID" or CPL == "HIGH")  {
     if (start == 1  and DS == 0) // the door should be closed to start cooking 
     {
    CS = 1 ; // cooking status to show that the cooking has started 
    
    CircuitPlayground.setPixelColor(2,255, 0, 255);// the bulb is turned ON
    CircuitPlayground.setPixelColor(7, 255, 0,255);
    Serial.println("\n ");
    Serial.println("The Start button is pressed ");
    Serial.print(" The Cooking power Level is ") ; 
    Serial.print(CPL) ; 
    Serial.print(" and the cooking time is ") ; 
    Serial.println(CT); 
      time_now = millis() + CT*1000; 

      t = ((time_now - millis() )/1000) -1 ; // used to control the display of the messages in the screen 
    Serial.print(" Cooking time left is:" ); 
    Serial.println( t+1 ); 
    while(millis() <= (time_now) and start == 1 ){
        if (CS == 1 and CircuitPlayground.slideSwitch() and m == 0) // if a request to open the door is asked 
         { 
          Serial.println("Door is loocked and cannot be opened  \n");
          m = 1 ; 
          }

  
       if (capButton(12) and start == 1){
    start = 0 ;
    Serial.println(" The stop button is pressed") ;  
            time_now = millis();
   tr= 0 ; 
    while(millis() < (time_now + 500)){}}

    if (t == ((time_now - millis() )/1000)  ){
 
    if (C==1) {tr = tr + 2 ; } // the magnetron temperature is increased every second, and according to the cooking power level 
    else if (C==2) {tr = tr + 3 ; }
    else if (C==3){ tr = tr+ 5 ; }  

       Serial.print(" Cooking time left is: " ); 
        Serial.print( (time_now - millis())/1000 ); 
    Serial.print (" and the magnetron temperature is "); 
  Serial.println (CircuitPlayground.temperature()+ tr);
        
        }
        t = ((time_now - millis() )/1000) -1 ; 
  // the fan is ON
    CircuitPlayground.setPixelColor(0, 255, 0, 0);
    CircuitPlayground.setPixelColor(1, 255, 0, 0);
    // lock the door 
    CircuitPlayground.setPixelColor(3, 0, 255, 0);
    CircuitPlayground.setPixelColor(4, 0, 255, 0);
    // turntable is ON
    CircuitPlayground.setPixelColor(5,0, 0, 255);
    CircuitPlayground.setPixelColor(6, 0, 0, 255);
    // before turning the magnetron ON we should check the temperature
    // the magnetron's temperature is the temperature that we get from the controller temperature sensor plus the tr values that increases according to CPL and CT
  
   if ( (CircuitPlayground.temperature()+ tr) < 100)
       {
   CircuitPlayground.setPixelColor(9, 255, 255, 255);
    CircuitPlayground.setPixelColor(8, 255, 255, 255); 
       
       }
      else // when the magnetron's temperature is more than 100 it is turned OFF 
       {       
    CircuitPlayground.setPixelColor(8, 0,0,0);
    CircuitPlayground.setPixelColor(9, 0,0,0);      
       }

  }

    
    Serial.println("\n ");
    Serial.println("The Fan stays ON for extra 10 seconds");
    time_now = millis() + 10*1000;
    t = ((time_now - millis())/1000 )- 1 ;
    Serial.print(" Fan is ON for " ); 
    Serial.println(  t+1 );
     while(millis() <= (time_now )){

        if (CircuitPlayground.slideSwitch() and DS == 0  ) 
             { 
                DS = 1 ; 
             }
      if (DS == 0) 
            {
                CircuitPlayground.playTone(440,10 ) ;    }
      if (t == ((time_now - millis())/1000) ){
                Serial.print(" Fan is ON for " ); 
                Serial.println(  (time_now - millis())/1000 );  
            }
                t = ((time_now - millis())/1000) -1 ; 


     // when the cooking is done, the turntable is stopped and the door is unlocked, the fan stays on for more 10 seconds
     // unlock the door 
    CircuitPlayground.setPixelColor(3, 0, 0, 0);
    CircuitPlayground.setPixelColor(4, 0, 0, 0);
    // turntable is off
    CircuitPlayground.setPixelColor(5,0, 0, 0);
    CircuitPlayground.setPixelColor(6, 0, 0, 0);
    // the fan is ON 
    CircuitPlayground.setPixelColor(0, 255, 0, 0);
    CircuitPlayground.setPixelColor(1, 255, 0, 0);
    // the magnetron is off 
     CircuitPlayground.setPixelColor(8, 0,0,0);
    CircuitPlayground.setPixelColor(9, 0,0,0);  
     }
              // the fan turned off 
        CircuitPlayground.setPixelColor(0, 0,0, 0);
    CircuitPlayground.setPixelColor(1,0, 0, 0);

    
    time_now = millis() + 10*1000;
    t = ((time_now - millis())/1000 )- 1 ;
   
     while(millis() <= (time_now ) ){
      
        if (CircuitPlayground.slideSwitch() and DS == 0  ) 
          { 
                DS = 1 ; 
           }
        if (DS == 0) {
               CircuitPlayground.playTone(440,10 ) ;    }

        if (t == ((time_now - millis())/1000) and DS == 0 ){
              Serial.print(" Buzzer is ON for " ); 
              Serial.println(  (time_now - millis())/1000 );  
           }
            t = ((time_now - millis())/1000) -1 ; 
           }
    // after cooking all the variables should be set to the start values to allow a new cooking to start 
    CS = 0 ; 
    start = 0 ; 
    CT = 0 ; 
    DS = 1 ; 
    tr = 0 ; 
    C = 0 ; 
     }
      }     
    }
    // keep the bulb off when the door is closed and no cooking. 
     if ( DS ==0 and CS == 0 ) { 
    CircuitPlayground.setPixelColor(2,0, 0, 0);
    CircuitPlayground.setPixelColor(7, 0, 0, 0);
      }
      else {
    CircuitPlayground.setPixelColor(2,255, 0, 255);
    CircuitPlayground.setPixelColor(7, 255, 0,255);
        }
 
}



