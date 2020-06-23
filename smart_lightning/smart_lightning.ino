#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char ssid[] = "ssid";
char pass[] = "science@78";
const int led1= D5;
const int trigPin = D3;
const int echoPin = D4;
const int buttonPin = D7;
bool detect=true;
int intensity=300;      
// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "79p68zcowk5XLc3LvY-D3B-k34KswWTO";
BlynkTimer timer; // Create a Timer object called "timer"!
WidgetLED blynkled1(V1);

int triggerRadar(int trigPin, int echoPin){
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH);
  int d = duration*0.0343/2;
  return d;
}

// This function will be called every time Slider Widget
// in Blynk app writes values to the Virtual Pin 1
BLYNK_WRITE(V3)
{
  intensity = param.asInt(); // assigning incoming value from pin V1 to a variable
  Serial.println("Led intensity is set to: "+ String(intensity));
}

BLYNK_WRITE(V2)                    //Controlling street light from Blynk app manually
{
  int pinValue = param.asInt(); // assigning incoming value from pin V2 to a variable
  if(pinValue==1){
    detect=false;
    }
    else{
      detect=true;
      }
}
void notifyOnButtonPress()
{
  // Invert state, since button is "Active LOW"
  int isButtonPressed = !digitalRead(buttonPin);
  if (isButtonPressed) {
    Blynk.notify("Panic button pressed!");
  }
}
void setup() {
  //Deifne output pins for Mux
  Serial.begin(115200);

  pinMode(led1, OUTPUT);
  pinMode(trigPin, OUTPUT);  // Sets the trigPin as an Output
  pinMode(echoPin, INPUT);  // Sets the echoPin as an Input
  pinMode(buttonPin,INPUT_PULLUP); 
  Blynk.begin(auth, ssid, pass);
  // Attach pin 2 interrupt to our handler
  //attachInterrupt(digitalPinToInterrupt(buttonPin), notifyOnButtonPress, CHANGE); 
   timer.setInterval(2000L,updateChart); 
}
void loop() {
  Blynk.run(); 
  timer.run();
}
void updateChart(){
    int d = triggerRadar(trigPin,echoPin);   // read the distance from ultrasonic sensor
    int brightness = analogRead(A0); //Value of the sensor connected Option 4 pin of Mux
   if(brightness<580 && detect){    //At Night
     if(d<10){            //As the car detected
         analogWrite(led1, 1023);  /* set initial 1000% duty cycle of 1023 */
           blynkled1.setValue(255);         /* set initial 1000% of 255 */  
     }
     else{
        analogWrite(led1, intensity);                /* set intensity as 30% of 1023 */  
        int mappedIntensity = map(intensity, 0, 1023, 0, 255);
        blynkled1.setValue(mappedIntensity);         /* set intensity as 30% of 255 */    
      }
   }                      
   else{                   //At Daytime
    blynkled1.off();
    digitalWrite(led1,LOW);
    }
   Serial.println("Dist: "+String(d));
   Serial.println("BRIGHT: "+String(brightness));
   Blynk.virtualWrite(V0, brightness);

   notifyOnButtonPress();     //Panic button
}
