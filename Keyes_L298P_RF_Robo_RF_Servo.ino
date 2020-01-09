/* JoyStick module receiver code
 - CONNECTIONS: nRF24L01 Modules See:
   1 - GND
   2 - VCC 3.3V !!! NOT 5V
   3 - CE to Arduino pin 6
   4 - CSN to Arduino pin 8
   5 - SCK to Arduino pin 13
   6 - MOSI to Arduino pin 11
   7 - MISO to Arduino pin 12
   8 - UNUSED
 */
//-----( Import needed libraries )-----
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>
Servo myservo;
/*-----( Declare Constants and Pin Numbers )-----*/
#define CE_PIN 8
#define CSN_PIN 7

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16
static const unsigned char PROGMEM logo_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000 };
//NOTE: the "LL" at the end of the constant is "LongLong" type
const uint64_t pipe = 0xE8E8F0F0E1LL; // Define the transmit pipe
/*
It's connected to the rest of the digital pins: D10, D11, D12 and D13. 
The two PWM pins connect to the enable functions for the two motors, 
and act as speed controls via the PWM duty cycle, D10 for Motor A, 
and D11 for Motor B. The corresponding GPIO - D12 for Motor A and D13 for motor B - 
are connected to each motors two input controls, once directly and once through 
an inverter, and thus act as direction controls: the motor spins one direction 
when it's high, the other when it's low, and stops - by coasting - 
when the speed is zero.
*/
int right_motor_PWM=10;          // PWM Motor A
int left_motor_PWM=11;          // PWM Motor B
int right_motor_direction=12;          // Motor A  
int left_motor_direction=13;          // Motor B
/*-----( Declare objects )-----*/
RF24 radio(CE_PIN, CSN_PIN); // Create a Radio
/*-----( Declare Variables )-----*/
int joystick[8];  // 6 element array holding Joystick readings
int speed = 0;
int speedb = 0;
int  xAxis, yAxis;
// the four button variables from joystick
int buttonUp;
int buttonRight;
int buttonDown;
int buttonLeft;
int servo_angle;
int set_speed = 140;

void setup()
{
  pinMode(right_motor_PWM, OUTPUT);
  pinMode(left_motor_PWM, OUTPUT);
  pinMode(right_motor_direction, OUTPUT);
  pinMode(left_motor_direction, OUTPUT);
  Serial.begin(9600);
  //Serial.println("Nrf24L01 Receiver Starting");
  myservo.attach(9);
  radio.begin();
  radio.setPALevel(RF24_PA_MIN);
  radio.openReadingPipe(1,pipe);
  radio.startListening();


 // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

  // Draw a single pixel in white
  display.drawPixel(10, 10, SSD1306_WHITE);

  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  display.display();
  delay(2000);
 // testdrawstyles();    // Draw 'stylized' characters
  // Invert and restore display, pausing in-between
  //display.invertDisplay(true);
  // delay(1000);
  display.invertDisplay(false);
  delay(1000);
  
}

void loop()
{
  if ( radio.available() )
  {
      radio.read( joystick, sizeof(joystick) );
      xAxis = joystick[0];
      yAxis = joystick[1];
      
      // the four button variables from joystick array
      int buttonUp    = joystick[2];
      int buttonRight = joystick[3];
      int buttonDown  = joystick[4];
      int buttonLeft  = joystick[5];

      int E_BTN = joystick[6];
      int F_BTN = joystick[7];

//Adjust fixed speed for buttons up/down
      if (E_BTN==0)
      {
      set_speed = set_speed+5;
   
      }
      else if (F_BTN==0)
      {
      set_speed = set_speed-5;
  
      }
 
// Y-axis used for forward and backward control  
 if (yAxis < 500 || buttonUp==0) {
    // Motors backward
    // Convert the declining Y-axis readings for going backward from 470 to 0 into 0 to 255 value for the PWM signal for increasing the motor speed 
    
    if (yAxis < 500)
    {speed  = map(yAxis, 500, 0, 30, 220);}
    else
    {speed=0;}

    if (buttonUp==0)
    {speedb=set_speed-30;}
    else
    {speedb=0;}
    /*
    Serial.print(" Speed reverse = ");  
    Serial.println(speed+speedb);
    */
    digitalWrite(right_motor_direction,LOW);
    analogWrite(right_motor_PWM, speed+speedb); 
    digitalWrite(left_motor_direction, LOW);
    analogWrite(left_motor_PWM, speed+speedb);

    display.clearDisplay();
    display.setTextSize(1);             // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);        // Draw white text
    display.setCursor(0,0);             // Start at top-left corner
  
  // display.println(F("Speed"));
  // display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
  // display.println(3.141592);

  display.setTextSize(1);             // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.print(F("set Speed: ")); display.println(set_speed);
  display.print(F("Speed bw: ")); display.println(speed+speedb);
  display.print(F("servo angle: ")); display.println(servo_angle);
  
  display.display();

  servo_angle = map(xAxis, 0, 1023, 105, 70);
  myservo.write(servo_angle); 
  Serial.println(servo_angle);
  
  }
else if (yAxis > 510 || buttonDown==0) {
    // Motors forward
    // Convert the increasing Y-axis readings for going forward from 550 to 1023 into 0 to 255 value for the PWM signal for increasing the motor speed
    //  speed  = map(yAxis, 510, 1023, 30, 220);
    
    if (yAxis > 510)
    {speed  = map(yAxis, 510, 1023, 30, 220);}
    else
    {speed=0;}
        
    if (buttonDown==0)
    {speedb=set_speed+30;}
    else
    {speedb=0;}
    /*
    Serial.print(" Speed forward = ");  
    Serial.println(speed+speedb);
    */  
    digitalWrite(right_motor_direction,HIGH);
    analogWrite(right_motor_PWM, speed+speedb); 
    digitalWrite(left_motor_direction, HIGH);
    analogWrite(left_motor_PWM, speed+speedb);

  display.clearDisplay();

  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  
  //  display.println(F("Speed"));
  // display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
  // display.println(3.141592);

  display.setTextSize(1);             // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.print(F("set Speed: ")); display.println(set_speed);
  display.print(F("Speed fw: ")); display.println(speed+speedb);
  display.print(F("servo angle: ")); display.println(servo_angle);
  display.display();

      
  servo_angle = map(xAxis, 0, 1023, 105, 70);
  myservo.write(servo_angle); 
  Serial.println(servo_angle);

  }
  // If joystick stays in middle the motors are not moving
  else {
    
    speed  = 0;
    /*
    Serial.print(" Speed zero = ");  
    Serial.println(speed+speedb);
    */
    digitalWrite(right_motor_direction,LOW);
    analogWrite(right_motor_PWM, speed);
    digitalWrite(left_motor_direction, LOW);
    analogWrite(left_motor_PWM, speed);


  display.clearDisplay();

  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  
  //  display.println(F("Speed"));
  // display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
  // display.println(3.141592);

  display.setTextSize(1);             // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.print(F("set Speed: ")); display.println(set_speed);
  display.print(F("Speed stop: ")); display.println(speed);
  display.print(F("servo angle: ")); display.println(servo_angle);
  display.display();
  
  servo_angle = map(xAxis, 0, 1023, 105, 70);
  myservo.write(servo_angle); 
  Serial.println(servo_angle);

  }
    
   /*
   // X-axis used for left and right control
   // Convert the declining X-axis readings from 0 to 1023 into increasing 0 to 255 value
   // servo_angle = xAxis;
   servo_angle = map(xAxis, 0, 1023, 105, 70);
   myservo.write(servo_angle); 
   Serial.println(servo_angle);
   */
  }
  
      /*
      Serial.print(" Servo = ");  
      Serial.println(servo_angle);
      Serial.print(" X = ");
      Serial.print(xAxis);
      Serial.print(" Y = ");  
      Serial.print(yAxis);
      Serial.print(" Speed = ");  
      Serial.print(speed);
      Serial.print(" Up = ");
      Serial.print(joystick[2]);
      Serial.print(" Right = ");  
      Serial.print(joystick[3]);
      Serial.print(" Down = ");
      Serial.print(joystick[4]);
      Serial.print(" Left = ");  
      Serial.println(joystick[5]);
      */
}  
