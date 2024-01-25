/**
This code was published and developed by Anusha on Electronicshub.org
Here a link to the complete article:
https://www.electronicshub.org/arduino-capacitance-meter/#Capacitance_Meter_for_Range_20_pF_to_1000_nF

It also has a very good explanation on how this all works.

I modified the code so it works woithout a display
**/
const int analog_charge = A2;
const int analogPin = A0;

float ck = 24.42;
int voltage;
float cu;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(analogPin, OUTPUT);
  pinMode(analog_charge, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
abc:
  pinMode(analogPin, INPUT);
  digitalWrite(analog_charge, HIGH);
  voltage = analogRead(analogPin);
  digitalWrite(analog_charge, LOW);
  //analog_charge = 998 || 999 || 1000 || 1001 and ! 1024
  pinMode(analogPin, OUTPUT);
  if (voltage < 1000)
  {
    cu = ((ck * voltage) / (1024.0 - voltage));
    if (cu > 20.0)
    {
      Serial.print(cu, 2);
      Serial.println("pF");
    }
    else
    {
      Serial.println("place capacitor ");
      delay(200);
      goto abc;
    }
  }
  else
  {
    voltage = 0;
    pinMode(analogPin, OUTPUT);
    delay(1);
    pinMode(analog_charge, INPUT_PULLUP);
    unsigned long start_time = micros();
    unsigned long final_time = 0;

    while ((voltage < 1) && (final_time < 400000L))
    {
      voltage = digitalRead(analog_charge);
      unsigned long stop_time = micros();
      final_time = stop_time > start_time ? stop_time - start_time : start_time - stop_time;
    }

    pinMode(analog_charge, INPUT);
    voltage = analogRead(analog_charge);
    digitalWrite(analogPin, HIGH);
    int delay_T = (int)(final_time / 1000L) * 5;
    delay(delay_T);
    pinMode(analog_charge, OUTPUT);
    digitalWrite(analog_charge, LOW);
    digitalWrite(analogPin, LOW);

    cu = -(float)final_time / 34.8 ;
    cu /= log(1.0 - (float)voltage / (float)1023);
    if (cu < 1000.0)
    {
      Serial.print(cu, 2);
      Serial.println("nF");
    }
    else
    {
      Serial.println("**Out of Range**");
    }
  }
  delay(1000);
}
