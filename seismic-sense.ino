// -----------------------------------
// Controlling LEDs over the Internet
// -----------------------------------

/* First, let's create our "shorthand" for the pins
Same as in the Blink an LED example:
led1 is D6, led2 is D7 */

int vibe = D2;
/*int led2 = D1;*/

float mag;
float dis;
unsigned long previousMillis;        // will store last time LED was updated
long interval;
int ledState = LOW;
int vibrationParrtern(int length, int strength);
int dataParse(String incomingData);
// Last time, we only needed to declare pins in the setup function.
// This time, we are also going to register our Particle function

void setup()
{
   Serial.begin(9600);
   pinMode(vibe, OUTPUT);
   Particle.function("data", dataParse);
   analogWrite(vibe, 255);

   Serial.write('setup');
   Serial.println();
}

void loop(){ }

int dataParse(String incomingData) {
    Serial.write("incoming Data: ");
    Serial.println(incomingData);
    int substringIndex = incomingData.indexOf('n');
    int stringLength = incomingData.length();
    String distance = incomingData.substring(substringIndex+1,stringLength);
    String magnitude = incomingData.substring(0,substringIndex);
    float dis = distance.toFloat();
    float mag = magnitude.toFloat();
    Serial.println(mag);
    Serial.println(dis);
    Serial.write("out Going Data: ");
    Serial.println(mag);
    Serial.println(dis);
    Serial.write("vibrationPattern: ");
    Serial.println();
    interval = mag;
    unsigned long currentMillis = millis();
    Serial.printlnf("currentMillis: %f", currentMillis);
    analogWrite(vibe, dis);
    delay(interval);
    analogWrite(vibe, 255);
    ledState = HIGH;
    Serial.write("vibe off");
    Serial.println();
}
