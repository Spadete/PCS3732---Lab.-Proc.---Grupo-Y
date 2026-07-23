#include <stdio.h>
#include <string>
#include <sys/time.h>

#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <pcf8574.h>
#include <lcd.h>
#include "Keypad.hpp"

#define BUZZER_PIN 4
#define TRIG_PIN 14
#define ECHO_PIN 15

#define MAX_DISTANCE 220
#define TIMEOUT (MAX_DISTANCE * 60)

int pcf8574Address = 0x27;
#define BASE 64
#define RS   (BASE + 0)
#define RW   (BASE + 1)
#define EN   (BASE + 2)
#define LED  (BASE + 3)
#define D4   (BASE + 4)
#define D5   (BASE + 5)
#define D6   (BASE + 6)
#define D7   (BASE + 7)

const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {16, 20, 21, 26};
byte colPins[COLS] = {19, 13, 6, 5};

Keypad keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

enum State {
    TRANCADA,
    ABERTA,
    ALERTA
};

State currentState = TRANCADA;
std::string password = "";
const std::string VALID_PASSWORD = "123456";
const float LIMIT_DISTANCE = 10.0f;
bool doorWasOpened = false;
int lcdHandle = -1;

void playToneLowVolume(int frequency, int duration_ms)
{
    if (frequency == 0) {
        delay(duration_ms);
        return;
    }

    long period = 1000000L / frequency;
    long highTime = period * 0.02;
    
    if (highTime == 0) {
        highTime = 1;
    }
    
    long lowTime = period - highTime;
    long cycles = (duration_ms * 1000L) / period;

    for (long i = 0; i < cycles; i++) {
        digitalWrite(BUZZER_PIN, HIGH);
        delayMicroseconds(highTime);
        digitalWrite(BUZZER_PIN, LOW);
        delayMicroseconds(lowTime);
    }
}

void successSound()
{
    playToneLowVolume(1200, 100);
    playToneLowVolume(1500, 100);
    playToneLowVolume(2000, 200);
}

void errorSound()
{
    playToneLowVolume(300, 300);
    playToneLowVolume(200, 400);
}

int pulseIn(int pin, int level, int timeout)
{
    struct timeval currentTime;
    struct timeval initialTime;
    struct timeval pulseStartTime;
    long micros;

    gettimeofday(&initialTime, NULL);
    micros = 0;

    while (digitalRead(pin) != level) {
        gettimeofday(&currentTime, NULL);
        if (currentTime.tv_sec > initialTime.tv_sec) {
            micros = 1000000L;
        } else {
            micros = 0;
        }
        micros += currentTime.tv_usec - initialTime.tv_usec;
        if (micros > timeout) {
            return 0;
        }
    }

    gettimeofday(&pulseStartTime, NULL);

    while (digitalRead(pin) == level) {
        gettimeofday(&currentTime, NULL);
        if (currentTime.tv_sec > initialTime.tv_sec) {
            micros = 1000000L;
        } else {
            micros = 0;
        }
        micros += currentTime.tv_usec - initialTime.tv_usec;
        if (micros > timeout) {
            return 0;
        }
    }

    if (currentTime.tv_sec > pulseStartTime.tv_sec) {
        micros = 1000000L;
    } else {
        micros = 0;
    }
    micros += currentTime.tv_usec - pulseStartTime.tv_usec;

    return micros;
}

float getDistance()
{
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);

    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    long echoTime = pulseIn(ECHO_PIN, HIGH, TIMEOUT);

    if (echoTime == 0) {
        return -1.0f;
    }

    return static_cast<float>(echoTime) * 340.0f / 2.0f / 10000.0f;
}

int detectI2C(int address)
{
    int fd = wiringPiI2CSetup(address);
    if (fd < 0) return 0;
    return wiringPiI2CWrite(fd, 0) >= 0;
}

int initializeLCD()
{
    if (detectI2C(0x27)) {
        pcf8574Address = 0x27;
    } else if (detectI2C(0x3F)) {
        pcf8574Address = 0x3F;
    } else {
        return -1;
    }

    pcf8574Setup(BASE, pcf8574Address);

    for (int i = 0; i < 8; i++) {
        pinMode(BASE + i, OUTPUT);
    }

    digitalWrite(LED, HIGH);
    digitalWrite(RW, LOW);

    return lcdInit(2, 16, 4, RS, EN, D4, D5, D6, D7, 0, 0, 0, 0);
}

void updateDisplay()
{
    lcdClear(lcdHandle);
    lcdPosition(lcdHandle, 0, 0);

    if (currentState == TRANCADA) {
        lcdPrintf(lcdHandle, "TRANCADA");
        lcdPosition(lcdHandle, 0, 1);
        lcdPrintf(lcdHandle, "Senha: ");
        for (size_t i = 0; i < password.length(); i++) {
            lcdPutchar(lcdHandle, '*');
        }
    } else if (currentState == ABERTA) {
        lcdPrintf(lcdHandle, "ACESSO LIBERADO");
        lcdPosition(lcdHandle, 0, 1);
        lcdPrintf(lcdHandle, "Abra a porta");
    } else if (currentState == ALERTA) {
        lcdPrintf(lcdHandle, "ALERTA INVASAO!");
        lcdPosition(lcdHandle, 0, 1);
        lcdPrintf(lcdHandle, "Senha: ");
        for (size_t i = 0; i < password.length(); i++) {
            lcdPutchar(lcdHandle, '*');
        }
    }
}

void checkDoorState(float dist)
{
    bool doorOpen = (dist > LIMIT_DISTANCE);

    if (currentState == TRANCADA) {
        if (doorOpen) {
            currentState = ALERTA;
            updateDisplay();
        }
    } else if (currentState == ABERTA) {
        if (doorOpen) {
            if (!doorWasOpened) {
                doorWasOpened = true;
                lcdClear(lcdHandle);
                lcdPosition(lcdHandle, 0, 0);
                lcdPrintf(lcdHandle, "Porta aberta");
                lcdPosition(lcdHandle, 0, 1);
                lcdPrintf(lcdHandle, "Aguardando fech.");
            }
        } else if (!doorOpen && doorWasOpened) {
            currentState = TRANCADA;
            doorWasOpened = false;
            playToneLowVolume(1000, 200);
            updateDisplay();
        }
    } else if (currentState == ALERTA) {
        playToneLowVolume(2500, 100);
    }
}

void handleKey(char key)
{
    if (key >= '0' && key <= '9') {
        if (password.length() < 6) {
            password += key;
            updateDisplay();
        }
    } else if (key == '*') {
        if (!password.empty()) {
            password.pop_back();
            updateDisplay();
        }
    } else if (key == '#') {
        if (currentState == TRANCADA || currentState == ALERTA) {
            if (password == VALID_PASSWORD) {
                currentState = ABERTA;
                password = "";
                successSound();
            } else {
                errorSound();
                password = "";
            }
            updateDisplay();
        }
    }
}

int main()
{
    if (wiringPiSetupGpio() == -1) {
        printf("Erro ao inicializar wiringPi.\n");
        return 1;
    }

    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);
    
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    digitalWrite(TRIG_PIN, LOW);

    lcdHandle = initializeLCD();
    if (lcdHandle == -1) {
        printf("LCD nao encontrado.\n");
        return 1;
    }

    keypad.setDebounceTime(50);
    updateDisplay();

    long lastSensorCheck = millis();

    while (true) {
        char key = keypad.getKey();
        if (key) {
            handleKey(key);
        }

        if (millis() - lastSensorCheck > 200) {
            float dist = getDistance();
            lastSensorCheck = millis();

            if (dist > 0) {
                checkDoorState(dist);
            }
        }

        delay(10);
    }

    return 0;
}