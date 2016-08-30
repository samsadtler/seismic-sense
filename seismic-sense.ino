// -----------------------------------
// Controlling LEDs over the Internet
// -----------------------------------

/* First, let's create our "shorthand" for the pins
Same as in the Blink an LED example:
led1 is D6, led2 is D7 */

int led1 = D2;
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
// Here's the pin configuration, same as last time
    pinMode(led1, OUTPUT);
// pinMode(led2, OUTPUT);[NUMBER_OF_ME

// We are also going to declare a Particle.function so that we can turn the LED on and off from the cloud.
//   Particle.function("led",ledToggle);
   Particle.function("data",dataParse);

   // This is saying that when we ask the cloud for the function "led", it will employ the function ledToggle() from this app.

   // For good measure, let's also make sure both LEDs are off when we start:
  digitalWrite(led1, HIGH);
// digitalWrite(led2, HIGH);
    Serial.write('setup');
    Serial.println();
}


/* Last time, we wanted to continously blink the LED on and off
Since we're waiting for input through the cloud this time,
we don't actually need to put anything in the loop */

void loop()
{
   // Nothing to do here
//   Serial.println('loop');
}

// We're going to have a super cool function now that gets called when a matching API request is sent
// This is the ledToggle function we registered to the "led" Particle.function earlier.

int ledToggle(String command) {
    Serial.print('ledToggle: ');
    Serial.println(command);
    /* Particle.functions always take a string as an argument and return an integer.
    Since we can pass a string, it means that we can give the program commands on how the function should be used.
    In this case, telling the function "on" will turn the LED on and telling it "off" will turn the LED off.
    Then, the function returns a value to us to let us know what happened.
    In this case, it will return 1 for the LEDs turning on, 0 for the LEDs turning off,
    and -1 if we received a totally bogus command that didn't do anything to the LEDs.
    */

    if (command=="on") {
        digitalWrite(led1,LOW);
          Serial.println('on');
        return 1;
    }
    else if (command=="off") {
        digitalWrite(led1,HIGH);
         Serial.println('off');
        return 0;
    }
    else {
        return -1;
    }
}
int vibrationParrtern(int length, int strength){

    Serial.write("vibrationPattern: ");
    Serial.println(length);
    interval = length;
    unsigned long currentMillis = millis();

    if(currentMillis - previousMillis <= interval) {
       Serial.write("currentMillis - previousMillis <= interval");
    // save the last time you blinked the LED
        previousMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
        analogWrite(led1, strength);
        ledState = LOW;
        // return 1;
        // set the LED with the ledState of the variable:
        // digitalWrite(led1, ledState);
        //PWM of vibration motor on pin
    }
    else {
    analogWrite(led1, 0);
    ledState = HIGH;
    Serial.write("vibe off");
    Serial.println();
    }
    return currentMillis;
}

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

    int vibrationLength = map(mag,1,10,500,30000);
    int vibrationStrength = map(dis,0,40075000,255,0);

    /*int vibrationPattern(int vibrationLength, int vibrationStrength);*/
    Serial.write("out Going Data: ");
    Serial.println(vibrationLength);
    Serial.println(vibrationStrength);
    Serial.write("vibrationPattern: ");

    interval = vibrationLength;
    unsigned long currentMillis = millis();

    while(currentMillis - previousMillis <= interval) {
       Serial.write("currentMillis - previousMillis <= interval");
       Serial.println();
        Serial.printlnf("previousMillis: %f",previousMillis);
       Serial.printlnf("currentMillis: %f",currentMillis);
    // save the last time you blinked the LED
        previousMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
        analogWrite(led1, 255);
        ledState = LOW;
        // return 1;
        Serial.write("analogWrite pin D2");
        Serial.println();
        // set the LED with the ledState of the variable:
        /*digitalWrite(led1, ledState );*/
        //PWM of vibration motor on pin
    }
    /*else {*/
    analogWrite(led1, 0);
    ledState = HIGH;
    Serial.write("vibe off");
    Serial.println();
    /*}*/
}
