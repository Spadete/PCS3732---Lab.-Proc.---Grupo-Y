#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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

int rows[4] = {16, 20, 21, 26};
int cols[4] = {19, 13, 6, 5};

char keys[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

void setup_keypad() {
    for (int i = 0; i < 4; i++) {
        pinMode(rows[i], OUTPUT);
        digitalWrite(rows[i], HIGH);
        pinMode(cols[i], INPUT);
        pullUpDnControl(cols[i], PUD_UP);
    }
}

char get_key() {
    for (int r = 0; r < 4; r++) {
        digitalWrite(rows[r], LOW);
        for (int c = 0; c < 4; c++) {
            if (digitalRead(cols[c]) == LOW) {
                delay(50);
                while (digitalRead(cols[c]) == LOW);
                digitalWrite(rows[r], HIGH);
                return keys[r][c];
            }
        }
        digitalWrite(rows[r], HIGH);
    }
    return '\0';
}

int main() {
    wiringPiSetupGpio();
    lcd_init();
    setup_keypad();

    char key;
    char buffer[16];
    int num1 = 0, num2 = 0, result = 0;
    char op = '\0';
    int state = 0;

    lcd_print("Calculadora");
    delay(2000);
    lcd_clear();

    while (1) {
        key = get_key();
        if (key != '\0') {
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
                    sprintf(buffer, "%d %c %d", num1, op, num2);
                    lcd_print(buffer);
                }
            } else if (key == 'A' || key == 'B' || key == 'C' || key == 'D') {
                if (state == 1) {
                    op = key;
                    state = 2;
                    lcd_clear();
                    sprintf(buffer, "%d %c", num1, op);
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
                lcd_clear();
            }
        }
    }
    return 0;
}