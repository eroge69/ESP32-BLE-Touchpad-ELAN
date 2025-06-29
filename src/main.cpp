#include "PS2Mouse.h"
#include <Arduino.h>
#include <stdint.h>
#include <BleCombo.h>

BleCombo bleCombo;

#define MDATA 5
#define MCLK 4
#define SCROLL_SENSITIVITY 0.25f

float accumScrollX = 0;
float accumScrollY = 0;
float scrollFactor = 0.25; 

const int LED_PIN = 2;
const int buttonPin1 = 22;
const int buttonPin2 = 23;

bool lastState1 = HIGH;
bool lastState2 = HIGH;
bool toggle1 = true;

PS2Mouse mouse_one(MCLK, MDATA, STREAM);

// Status klik sebelumnya
bool leftPressed = false;
bool rightPressed = false;

void setup() {
  Serial.begin(115200);
  ledcSetup(0, 5000, 8);
  ledcAttachPin(LED_PIN, 0);
  pinMode(buttonPin1, INPUT_PULLUP);
  pinMode(buttonPin2, INPUT_PULLUP);
  
  mouse_one.initialize();
  mouse_one.set_stream_mode();
  mouse_one.set_scaling_1_1();

  Serial.println("BLE started");
  bleCombo.begin();
  Serial.println("BLE started");
}

unsigned long lastUpdate = 0;
int brightness = 0;
int fadeAmount = 5;

void updateLED() {
  unsigned long now = millis();
  if (now - lastUpdate > 40) {
    brightness += fadeAmount;
    if (brightness <= 0 || brightness >= 80) fadeAmount = -fadeAmount;
    ledcWrite(0, brightness);
    lastUpdate = now;
  }
}

void updateLED2() {
  unsigned long now = millis();
  if (now - lastUpdate > 10) {
    brightness += fadeAmount;
    if (brightness <= 0 || brightness >= 80) fadeAmount = -fadeAmount;
    ledcWrite(0, brightness);
    lastUpdate = now;
  }
}

void loop() {
  bool currentState1 = digitalRead(buttonPin1);
  bool currentState2 = digitalRead(buttonPin2);
  
  int16_t data[2];
  mouse_one.report(data);

  int dw = data[0];
  int dx = data[1];
  int dy = data[2];

  if (!(dw == 8 && dx == 0 && dy == 0)) {
    Serial.print(dw); Serial.print(":");
    Serial.print(dx); Serial.print(",");
    Serial.println(dy);
  }
if (lastState1 == HIGH && currentState1 == LOW) {
  toggle1 = !toggle1;
  Serial.print("Toggle aktif: ");
  Serial.println(toggle1);
}

  if (bleCombo.isConnected()) {
    // Deteksi tombol dari status byte
    bool isLeft = dw & 0x01;
    bool isRight = dw & 0x02;

    // Mouse Kiri
    // Jika tombol kiri ditekan dan belum ditekan sebelumnya
    if (isLeft && !leftPressed) {
      if (currentState2 == LOW) {
           bleCombo.press((MouseButton)MOUSE_BACK);     
      }
      else {
         if (toggle1==1){ //scroll mode
          
            bleCombo.press((MouseButton)MOUSE_LEFT);     
        }
      }
      leftPressed = true;
    } 
    
    else if (!isLeft && leftPressed) {
      bleCombo.release((MouseButton)MOUSE_BACK);
      bleCombo.release((MouseButton)MOUSE_LEFT);
      leftPressed = false;
    }
  

    // Mouse Kanan
    // Jika tombol kanan ditekan dan belum ditekan sebelumnya
    if (isRight && !rightPressed) {
      if (currentState2 == LOW) {       
          bleCombo.press((MouseButton)MOUSE_FORWARD);
      }
      else {
        if (toggle1==1){ //scroll mode
           bleCombo.press((MouseButton)MOUSE_RIGHT);
        }
      }
      rightPressed = true;
    } 
    
    else if (!isRight && rightPressed) {
      bleCombo.release((MouseButton)MOUSE_FORWARD);
      bleCombo.release((MouseButton)MOUSE_RIGHT);
      rightPressed = false;
    }


    
    //mouseleft scroll
    if (isLeft) {       
       if (toggle1==0){ //scroll mode         
           bleCombo.move(0,0,-1,0);
           delay(50);
        }
    }
    if (isRight) {       
       if (toggle1==0){ //scroll mode         
           bleCombo.move(0,0,1,0);
           delay(50);
        }
    }

    //LED
    if ((toggle1) && (currentState2 == HIGH)) {
      updateLED(); // Mode gerak - berkedip
    } else if ((!toggle1) && (currentState2 == HIGH)) {
      ledcWrite(0, 80); // Mode scroll - nyala tetap
    }
    else {
      ledcWrite(0, 0); // Matikan LED
    }

    //Tombol1
    if (dx != 0 || dy != 0) {
    if (toggle1) {

      bool xOverflow = dw & 0x40;
      bool yOverflow = dw & 0x80;

      if (xOverflow || yOverflow) {
       dx = 0;
        dy = 0;
      } else {
        dx = constrain(dx, -30, 30);
        dy = constrain(dy, -30, 30);
        bleCombo.move(-dy * 4, -dx * 4);
      }
    } 
    else {

        accumScrollX += dx * SCROLL_SENSITIVITY;
        accumScrollY += dy * SCROLL_SENSITIVITY;

        int scrollX = -(int)accumScrollX;
        int scrollY = -(int)accumScrollY;

       // Kirim scroll horizontal jika ada
        if (dy != 0) {
           bleCombo.move(0, 0, 0, dy);
            accumScrollY -= scrollY;
           delay(5);
        }

        // Kirim scroll vertikal jika ada
        if (dx != 0) {
          bleCombo.move(0, 0, dx, 0);
          accumScrollX -= scrollX;          
        }       
    }
    }
    //Tombol2
    if (currentState2 == LOW) {
         bleCombo.press((uint8_t)KEY_LEFT_CTRL);
         Serial.print("CTRL ");
      } 
    else if (!currentState2 == LOW) {
         bleCombo.release((uint8_t)KEY_LEFT_CTRL);
      } 
  }
  else{
    updateLED2(); // Jika tidak terhubung, LED tetap berkedip
  }
  lastState1 = currentState1;
  
  delay(10);
}
