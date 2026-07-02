#include <wiringPi.h>
#include <wiringPiI2C.h>

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

int main() {
    wiringPiSetup();
    fd = wiringPiI2CSetup(LCD_ADDR);

    lcd_byte(0x33, LCD_CMD);
    lcd_byte(0x32, LCD_CMD);
    lcd_byte(0x06, LCD_CMD);
    lcd_byte(0x0C, LCD_CMD);
    lcd_byte(0x28, LCD_CMD);
    lcd_byte(0x01, LCD_CMD);
    delayMicroseconds(500);

    char *text = "3+3=6";
    while (*text) {
        lcd_byte(*(text++), LCD_CHR);
    }

    return 0;
}