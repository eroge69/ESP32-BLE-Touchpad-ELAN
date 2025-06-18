#include "PS2Mouse.h"
#include <Arduino.h>
#include <stdint.h>
#include <BleMouse.h>
BleMouse bleMouse;

#define MDATA 5
#define MCLK 23
#define SENSITIVITY 5
const int LED_PIN = 2;

PS2Mouse mouse_one(MCLK, MDATA, STREAM);

// Status klik sebelumnya
bool leftPressed = false;
bool rightPressed = false;

void setup() {
  Serial.begin(115200);
  ledcSetup(0, 5000, 8);
  ledcAttachPin(LED_PIN, 0);
  
  mouse_one.initialize();
  mouse_one.set_stream_mode();
  mouse_one.set_scaling_1_1();

  Serial.println("BLE Mouse started");
  bleMouse.begin();
}

unsigned long lastUpdate = 0;
int brightness = 0;
int fadeAmount = 5;

void updateLED() {
  unsigned long now = millis();
  if (now - lastUpdate > 100) {
    brightness += fadeAmount;
    if (brightness <= 0 || brightness >= 80) fadeAmount = -fadeAmount;
    ledcWrite(0, brightness);
    lastUpdate = now;
  }
}

void loop() {
  updateLED();
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

  if (bleMouse.isConnected()) {
    // Deteksi tombol dari status byte
    bool isLeft = dw & 0x01;
    bool isRight = dw & 0x02;

    if (isLeft && !leftPressed) {
      bleMouse.press(MOUSE_LEFT);
      leftPressed = true;
    } else if (!isLeft && leftPressed) {
      bleMouse.release(MOUSE_LEFT);
      leftPressed = false;
    }

    if (isRight && !rightPressed) {
      bleMouse.press(MOUSE_RIGHT);
      rightPressed = true;
    } else if (!isRight && rightPressed) {
      bleMouse.release(MOUSE_RIGHT);
      rightPressed = false;
    }

    // Gerakan
    if (dx != 0 || dy != 0) {
      bleMouse.move(dx, dy);
    }
  }

  delay(10);
}
