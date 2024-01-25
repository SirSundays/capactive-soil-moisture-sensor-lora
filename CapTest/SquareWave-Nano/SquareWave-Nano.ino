void setup() {

TCCR0B = TCCR0B & B11111000 | B00000001; // for PWM frequency of 62500.00 Hz
pinMode(5,OUTPUT);

}

void loop() {

analogWrite(5,127);

}