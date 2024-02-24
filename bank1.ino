#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Servo.h>

#define SERVO_INIT 30  // Hold coin position
#define SERVO_COIN1 50  // drop small coin
#define SERVO_COIN5 70  // drop middle coin
#define SERVO_COIN10 80  // drop big coin

#define DELAY_CHECK 200

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define state_init 1
#define state_wait 2
#define state_coin_in 3
#define state_check_coin1 4 
#define state_check_coin5 5
#define state_check_coin10 6  
#define state_coin_drop 7
#define state_check_error 8

// Declaration for SSD1306 display connected using software SPI (default case):
#define OLED_MOSI   9
#define OLED_CLK   10
#define OLED_DC    11
#define OLED_CS    12
#define OLED_RESET 13
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
  OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

Servo myservo;

int balance = 0;
int coin1 =100;
int coin5 =50;
int coin10=10;

const int input_pin = 2;
const int servo_pin = 3;
int state = state_init;
int current_coin = 0;

void setup() {
Serial.begin(9600);
pinMode(input_pin,INPUT);
display.begin(SSD1306_SWITCHCAPVCC);
display.setRotation(3);
display.display();

state = state_init;
  scrolltext();
}
void init_servo()
{
  myservo.attach(servo_pin);
  delay(100);
  myservo.write(SERVO_INIT);
  delay(500);
  myservo.detach();

}
void control_servo(){
     int icheck = SERVO_COIN1;
     if( state == state_check_coin5){
          icheck = SERVO_COIN5;
     }
     else if(state == state_check_coin10){
          icheck = SERVO_COIN10;
     }
     myservo.write(icheck);
     delay(200);
}
void loop() {
   // printtext(); 
   
   switch(state){
    case state_init:init_servo();
                    drawBalance();
                    delay(3000);
                    display.clearDisplay();
                    display.ssd1306_command(SSD1306_DISPLAYOFF);
                    state = state_wait;
    break;                
    case state_wait:  waitUser();
    break; 
    case state_coin_in : myservo.attach(servo_pin);
                         display.ssd1306_command(SSD1306_DISPLAYON); 
                         drawBalance();
                        // delay(500);
                         state = state_check_coin1;
    break;  
    case state_check_coin1 :  control_servo();
                              if( digitalRead(input_pin)){
                                  state = state_check_coin5;
                              }else{
                                 current_coin = 1;
                                 coin1++;
                                 state = state_coin_drop;
                              } 
    break;  
    case state_check_coin5 : control_servo();
                              if( digitalRead(input_pin)){
                                  state = state_check_coin10;
                              }else{
                                 current_coin = 5;
                                 coin5++;
                                 state = state_coin_drop;
                              } 
    break;
    case state_check_coin10 : control_servo();
                              
                              if( digitalRead(input_pin)){
                                  state = state_check_error;
                              }else{
                                 current_coin = 10;
                                 coin10++;
                                 state = state_coin_drop;
                              } 
    break;                          
    case state_coin_drop: coinDropAnimation();
    break;  
    case state_check_error:
    break;                         
   }
 
}
int px=SCREEN_HEIGHT/2,py=20;
void printtext(void){
display.clearDisplay();

py+=5;

if( py > 200) py = 20;

drawCoin(px,py,1);
display.display();
delay(50);
}
 void waitUser(){
       int coin_in = digitalRead(input_pin);
       if(coin_in == 1){
           state = state_coin_in;
       }
 }
void drawCoin(int16_t x,int16_t y,int num)
{
      
      if(num== 10){
        display.fillCircle(x, y, 30, 1);
      }else{
          display.fillCircle(x, y, 20, 1);
      }

      display.setTextSize(3); 
      display.setTextColor(SSD1306_BLACK);
      display.setCursor(x-6, y-8);
      if(num == 10){
        display.setCursor(x-17, y-8);
        display.setTextSize(3); 
      }
      
      display.print(current_coin);

     // display.startscrollright(0x00, 0x0F);
 // delay(3000);
}
void coinDropAnimation()
{
    display.clearDisplay();

    py+=5;

    if( py > 160){
      py = 20;
      state = state_init;
    } 

    drawCoin(px,py,current_coin);
    display.display();
    delay(50);
}
void drawBalance(void) {

balance = coin1 + (coin5*5)+(coin10*10);  
display.clearDisplay();
display.setTextSize(1.6); 
display.setTextColor(SSD1306_WHITE);
display.setCursor(0, 0);
display.cp437(true);
display.write("Balance");
display.write('\n');
display.setTextSize(2);  
display.setCursor(0, 13);
display.print(String(balance));

display.drawFastHLine(0, 30, 64, 1);
// sum coin 1
display.setTextSize(1.5);  
display.setCursor(0, 38);
display.write("COIN 1");

display.setTextSize(2);  
display.setCursor(0, 50);
display.print(String(coin1));



// sum coin 5
display.setTextSize(1.5);  
display.setCursor(0, 68);
display.write("COIN 5");

display.setTextSize(2);  
display.setCursor(0, 80);
display.print(String(coin5));

// sum coin 10
display.setTextSize(1.5);  
display.setCursor(0, 100);
display.write("COIN 10");

display.setTextSize(2);  
display.setCursor(0, 110);
display.print(String(coin10));



display.display();
delay(2000);
}

void scrolltext(void) {
  display.clearDisplay();
  display.setTextSize(1.5); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("MakerTutor"));
  display.display();      // Show initial text
  delay(100);
  // Scroll in various directions, pausing in-between:
  display.startscrollright(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrollleft(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrolldiagright(0x00, 0x07);
  delay(2000);
  display.startscrolldiagleft(0x00, 0x07);
  delay(2000);
  display.stopscroll();
  delay(1000);
}


