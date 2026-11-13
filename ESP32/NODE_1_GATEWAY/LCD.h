#ifndef LCD_H
#define LCD_H

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "SCAN_KTP.h"
#include "KIRIM.h"
#include "SAVE.h"

#define WIDTH  128
#define HEIGHT 64
#define RES    4
#define DC     2
#define CS     15

int value = 0;

class lcdModule {
  public:
    Adafruit_SSD1306 display;
    lcdModule() : display(WIDTH, HEIGHT, &SPI, DC, RES, CS) {}

    void begin() {
      display.begin(SSD1306_SWITCHCAPVCC);
      display.clearDisplay();
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 0);
      display.display();
    }

    void validation(int isYesSelected) {
      display.setTextWrap(false);
      
      // Judul Utama
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(32, 18);
      display.println("KIRIM DATA?");

      if(isYesSelected == 1) {
        display.fillRect(93, 49, 35, 15, SSD1306_WHITE);
      } else if(isYesSelected == 2) {
        display.fillRect(0, 49, 35, 15, SSD1306_WHITE);
      } else {
        display.fillRect(0, 49, 35, 15, SSD1306_BLACK);
        display.fillRect(93, 49, 35, 15, SSD1306_BLACK);
      }

      if(isYesSelected == 1) {
        display.setTextColor(SSD1306_WHITE);
      } else if(isYesSelected == 2) {
        display.setTextColor(SSD1306_BLACK);
      } else {
        display.setTextColor(SSD1306_WHITE);
      }
      display.setCursor(3, 53);
      display.println("TIDAK");

      if(isYesSelected == 1) {
        display.setTextColor(SSD1306_BLACK);
      } else if(isYesSelected == 2) {
        display.setTextColor(SSD1306_WHITE);
      } else {
        display.setTextColor(SSD1306_WHITE);
      }
      display.setCursor(105, 53);
      display.println("YA");
    }

    void pesanTerkirim() {
      display.fillRect(1, 1, 126, 62, SSD1306_WHITE); 
      display.fillRect(5, 5, 118, 54, SSD1306_BLACK); 
      
      display.setTextColor(SSD1306_WHITE);
      display.setTextSize(2);
      display.setTextWrap(false);
      display.setCursor(41, 13);
      display.println("Data");
      display.setCursor(17, 32);
      display.println("Terkirim");
    }

    void updateAnimations() {
      if(value == 1) {
        portrait_mode_scanning_64_64_28f_frame = millis() / 42 % 28;
      } else if(value == 2) {
        message_sent_64_64_28f_frame = millis() / 42 % 28;
      } else if(value == 3) {
        save_changes_64_64_28f_frame = millis() / 42 % 28;
      }
    }

    void drawAnimation_portrait_mode_scanning_64_64_28f(void) {
      display.fillRect(32, 0, 64, 64, 0x0);
      display.drawBitmap(
        32, 0, 
        portrait_mode_scanning_64_64_28f_frames[portrait_mode_scanning_64_64_28f_frame], 
        64, 64, 
        SSD1306_WHITE,
        SSD1306_BLACK
      );
    }

    void drawScan() {
      value = 1;
      updateAnimations();
      drawAnimation_portrait_mode_scanning_64_64_28f();
    }

    void drawAnimation_message_sent_64_64_28f(void) {
      display.fillRect(32, 0, 64, 64, 0x0);
      display.drawBitmap(
        32, 0, 
        message_sent_64_64_28f_frames[message_sent_64_64_28f_frame], 
        64, 64, 
        SSD1306_WHITE,
        SSD1306_BLACK
      );
    }

    void drawKirim() {
      value = 2;
      updateAnimations();
      drawAnimation_message_sent_64_64_28f();
    }

    void drawAnimation_save_changes_64_64_28f(void) {
      display.fillRect(32, 0, 64, 64, 0x0);
      display.drawBitmap(
        32, 0, 
        save_changes_64_64_28f_frames[save_changes_64_64_28f_frame], 
        64, 64, 
        SSD1306_WHITE,
        SSD1306_BLACK
      );
    }

    void drawSave() {
      value = 3;
      updateAnimations();
      drawAnimation_save_changes_64_64_28f();
    }

    void clear() {
      display.clearDisplay();
    }

    void centerText(String text, int fontSize = 1) {
      int16_t x1, y1;
      uint16_t w, h;

      display.setTextSize(fontSize);
      display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);

      int x = (WIDTH - w) / 2;
      int y = (HEIGHT - h) / 2;

      display.setCursor(x, y -  (h / 2));
      display.print(text);
    }

    void show() {
      display.display();
    }

    void bottomLeft(String text, int fontSize = 1) {
      int16_t x1, y1;
      uint16_t w, h;
      
      display.setTextSize(fontSize);
      display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);

      display.setCursor(0, 64 - h); 
      display.print(text);
    }

    void bottomRight(String text, int fontSize = 1) {
      int16_t x1, y1;
      uint16_t w, h;

      display.setTextSize(fontSize);
      display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);

      display.setCursor(128 - w, 64 - h);
      display.print(text);
    }
};

#endif LCD_H