#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "Keypad.hpp"

#define LCD_ADDR 0x27
#define LCD_CHR 1
#define LCD_CMD 0
#define LCD_BACKLIGHT 0x08
#define LCD_ENABLE 0b00000100

int fd;

void lcd_toggle_enable(int bits) {
    delayMicroseconds(500);
    wiringPiI2CWrite(fd, (bits | LCD_ENABLE));
    delayMicroseconds(500);
    wiringPiI2CWrite(fd, (bits & ~LCD_ENABLE));
    delayMicroseconds(500);
}

void lcd_byte(int bits, int mode) {
    int bits_high = mode | (bits & 0xF0) | LCD_BACKLIGHT;
    int bits_low = mode | ((bits << 4) & 0xF0) | LCD_BACKLIGHT;

    wiringPiI2CWrite(fd, bits_high);
    lcd_toggle_enable(bits_high);
    wiringPiI2CWrite(fd, bits_low);
    lcd_toggle_enable(bits_low);
}

void lcd_init() {
    fd = wiringPiI2CSetup(LCD_ADDR);
    lcd_byte(0x33, LCD_CMD);
    lcd_byte(0x32, LCD_CMD);
    lcd_byte(0x06, LCD_CMD);
    lcd_byte(0x0C, LCD_CMD);
    lcd_byte(0x28, LCD_CMD);
    lcd_byte(0x01, LCD_CMD);
    delayMicroseconds(500);
}

void lcd_print(const char *s) {
    while (*s) {
        lcd_byte(*(s++), LCD_CHR);
    }
}

void lcd_clear() {
    lcd_byte(0x01, LCD_CMD);
    delay(2);
}

const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {16, 20, 21, 26};
byte colPins[COLS] = {19, 13, 6, 5};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

int main() {
    wiringPiSetupGpio();
    lcd_init();

    char key;
    char buffer[16];
    int num1 = 0, num2 = 0, result = 0;
    char op = '\0';
    char display_op = '\0';
    int state = 0;

    keypad.setDebounceTime(50);

    lcd_print("Calculadora");
    delay(2000);
    lcd_clear();

    while (1) {
        key = keypad.getKey();
        if (key) {
            if (key >= '0' && key <= '9') {
                if (state == 0 || state == 1) {
                    num1 = num1 * 10 + (key - '0');
                    state = 1;
                    lcd_clear();
                    sprintf(buffer, "%d", num1);
                    lcd_print(buffer);
                } else if (state == 2 || state == 3) {
                    num2 = num2 * 10 + (key - '0');
                    state = 3;
                    lcd_clear();
                    sprintf(buffer, "%d %c %d", num1, display_op, num2);
                    lcd_print(buffer);
                }
            } else if (key == 'A' || key == 'B' || key == 'C' || key == 'D') {
                if (state == 1) {
                    op = key;
                    
                    if (op == 'A') display_op = '+';
                    else if (op == 'B') display_op = '-';
                    else if (op == 'C') display_op = '*';
                    else if (op == 'D') display_op = '/';
                    
                    state = 2;
                    lcd_clear();
                    sprintf(buffer, "%d %c", num1, display_op);
                    lcd_print(buffer);
                }
            } else if (key == '#') {
                if (state == 3) {
                    if (op == 'A') result = num1 + num2;
                    else if (op == 'B') result = num1 - num2;
                    else if (op == 'C') result = num1 * num2;
                    else if (op == 'D') {
                        if (num2 != 0) result = num1 / num2;
                    }
                    
                    lcd_clear();
                    if (op == 'D' && num2 == 0) {
                        lcd_print("Erro Div 0");
                    } else {
                        sprintf(buffer, "Res: %d", result);
                        lcd_print(buffer);
                    }
                    state = 0;
                    num1 = 0;
                    num2 = 0;
                }
            } else if (key == '*') {
                state = 0;
                num1 = 0;
                num2 = 0;
                op = '\0';
                display_op = '\0';
                lcd_clear();
            }
        }
    }
    return 0;
}