#include "Arduino.h"

/*  for example:
 *
 *	setPWM_Frequency(a);
 * 	setPWM_ComparePeriod(b);
 *	analogWrite(PWM1,c);
 *
 *	the PWM frequency is a/b. 
 *  c used for set dutycycle, it's value can be 0-b;
 */
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Ich bin da");
  /*PWM CLK frequency default is 12M;
   *PWM CLK frequency value:
   *PWM_CLK_FREQ_48M
   *PWM_CLK_FREQ_24M
   *PWM_CLK_FREQ_16M
   *PWM_CLK_FREQ_12M
   *PWM_CLK_FREQ_8M
   *PWM_CLK_FREQ_6M
   *PWM_CLK_FREQ_4M
   *PWM_CLK_FREQ_3M
   *PWM_CLK_FREQ_2M
   *PWM_CLK_FREQ_1M
   */
  setPWM_Frequency(PWM_CLK_FREQ_16M);

  //pwm period can be 0xFF~0xFFFF, default is 0xFFFF
  setPWM_ComparePeriod(0x12C);
  
  pinMode(PWM1,OUTPUT);

  analogWrite(PWM1,150);

  // Generating a 53.3 Mhz (53.13 Mhz measured) squarewave frequency 
}

void loop() {
  Serial.println(analogRead(ADC));
  delay(50);
}
