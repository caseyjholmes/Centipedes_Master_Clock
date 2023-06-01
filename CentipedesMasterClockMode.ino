//HOLMES MODULAR - CASEY HOLMES - "CENTIPEDES VOLTAGE CONTROLLED MASTER CLOCK MODE" 5-31-23

// pin definitions
const int enableLed = 5;                      //green input led
const int buttonPin = 2;                      //button is here
const int enablePin = 4;                      //clock input transistor pin
const int outputLed = 6;                      //red output LED
const int output = 3;                         //output transistor on pin 3

//variables
int toggleState;                              //variable to record button presses
int lastButtonState = 1;                      //variable to record previous button states
long unsigned int lastPress;                  //timestamp for working with button
volatile int buttonFlag;                      //global button flaggin variable for ISR
int debounceTime = 50;                        //variable for storing debounce time
bool isOn = true;                             //variable to store state of master duty clock cycle
unsigned long dutypot = 0;                    //duty pot read variable
unsigned long frequency = 0;                  //frequncy read variable for master clock
unsigned long offDuration = 0;                //variable to calculate duty cycle off time length
unsigned long previousTime = 0;               //stores previous time stamps from millis
unsigned long currentTime = 0;                //stores current time stamps from millis
volatile int enableState = 0;                 //variable to store current clock status

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);           // intilize button as input with internal pullup resistor
  attachInterrupt(digitalPinToInterrupt(buttonPin), ISR_button, CHANGE); //trigger interrupt on change of button
  pinMode(output, OUTPUT);                    //initialize output transistor
  pinMode(outputLed, OUTPUT);                 //initialize output LED as output
  digitalWrite(outputLed, 0);                 //initialize output LED as off
  pinMode(enableLed, OUTPUT);                 //initialize the LED pin as an output:
  digitalWrite(enableLed, 0);                 //initialize cock LED as off
  pinMode(enablePin, INPUT);                  //initialize clock pin as input
}

void loop() {
  enableState = digitalRead(enablePin);                           //read the enable input pin
  if((millis() - lastPress) > debounceTime && buttonFlag){        //if button was pressed for longer than debounce time
    lastPress = millis();                                         //update lastPress                                                     
    if(digitalRead(buttonPin) == 0 && lastButtonState == 1){      //if button is pressed and was released last change   
      toggleState =! toggleState;                                 //turn on master clock with button by flipping variable state
      lastButtonState = 0;                                        //record the lastButtonState
    }    
    else if(digitalRead(buttonPin) == 1 && lastButtonState == 0){ //if button is not pressed, and was pressed last change   
      lastButtonState = 1;                                        //record the lastButtonState
    }
    buttonFlag = 0;                                               //reset button flag variable
  }

if (toggleState){                                                 //if button is toggled on
   digitalWrite(outputLed, 0);                                    //turn off the red LED
   frequency = map(analogRead(A0), 0, 1023, 20, 3000);            //read and map pot on A into speed pot variable 1000 is max delay time(slow) 20 is minimum (fast)
   dutypot = map(analogRead(A1), 0, 1023,10, frequency);          //read and map pot on A into speed pot variable
   offDuration = frequency - dutypot;                             //calculation to get off time from duty cycle on time              
   currentTime = millis();                                        //calculate current time since program started (in milliseconds)
   if (isOn && (currentTime - previousTime >= dutypot)) {         //check if it's time to change the wave and LED signals by comparing isON variable with current and prvious time stamps and duty pot value
      digitalWrite (enableLed, 1);                                //turn on red output led
      digitalWrite (output, 1);                                   //turn on output
      isOn = false;                                               //update boolean is on with false value so next time through the loop it turns off instead of on
      previousTime = currentTime;                                 //set previous time to equal current time
   }
   else if (!isOn && (currentTime - previousTime >= offDuration)){//check if it's time to change the wave and LED signals by comparing isON variable with current and prvious time stamps and off duration value
      digitalWrite(enableLed, 0);                                 //turn off red output led
      digitalWrite(output, 0);                                    //turn off output
      isOn = true;                                                //update boolean is on with false value so next time through the loop it turns on instead of off
      previousTime = currentTime;                                 //set previous time to equal current time
   }
}

else if (!enableState){                                           //if trigger input "HIGH" is detected (pullup goes low)
   digitalWrite(enableLed, 0);                                    //turn off the green LED
   frequency = map(analogRead(A0), 0, 1023, 20, 3000);            //read and map pot on A into speed pot variable 1000 is max delay time(slow) 20 is minimum (fast)
   dutypot = map(analogRead(A1), 0, 1023,10, frequency);          //read and map pot on A into speed pot variable
   offDuration = frequency - dutypot;                             //calculation to get off time from duty cycle on time              
   currentTime = millis();                                        //calculate current time since program started (in milliseconds)
   if (isOn && (currentTime - previousTime >= dutypot)) {         //check if it's time to change the wave and LED signals by comparing isON variable with current and prvious time stamps and duty pot value
      digitalWrite(outputLed, 1);                                 //turn on red output led
      digitalWrite (output, 1);                                   //turn on output
      isOn = false;                                               //update boolean is on with false value so next time through the loop it turns off instead of on
      previousTime = currentTime;                                 //set previous time to equal current time
   }
   else if (!isOn && (currentTime - previousTime >= offDuration)){//check if it's time to change the wave and LED signals by comparing isON variable with current and prvious time stamps and off duration value
      digitalWrite(outputLed, 0);                                 //turn off red output led
      digitalWrite(output, 0);                                    //turn off output
      isOn = true;                                                //update boolean is on with false value so next time through the loop it turns on instead of off
      previousTime = currentTime;                                 //set previous time to equal current time
   }
}else{                                                            //else to manage shutting everything off if the enable isn't high and the button toggle isn't on
digitalWrite(enableLed, 0);                                       //keep the red enable output led off
digitalWrite(output, 0);                                          //keep the output off
digitalWrite(outputLed, 0);                                       //keep green output led off
isOn = true;                                                      //setting this boolean to true here makes sure the clock loop start up with a high condition each time enabled
} 
}

void ISR_button()                                                 //interrupt service routine
{
  buttonFlag = 1;                                                 //let buttonFlag = 1
}
