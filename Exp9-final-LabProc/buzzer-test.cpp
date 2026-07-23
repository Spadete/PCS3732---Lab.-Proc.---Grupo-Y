#include <stdio.h>
#include <wiringPi.h>
#include <softTone.h>

#define BUZZER_PIN 4

void stopBuzzer()
{
    softToneWrite(BUZZER_PIN, 0);
}

void successSound()
{
    softToneWrite(BUZZER_PIN, 1200);
    delay(100);
    softToneWrite(BUZZER_PIN, 1500);
    delay(100);
    softToneWrite(BUZZER_PIN, 2000);
    delay(200);
    stopBuzzer();
}

void errorSound()
{
    softToneWrite(BUZZER_PIN, 300);
    delay(400);
    softToneWrite(BUZZER_PIN, 200);
    delay(400);
    stopBuzzer();
}

void alertSound()
{
    for (int i = 0; i < 5; i++) {
        softToneWrite(BUZZER_PIN, 2500);
        delay(100);
        stopBuzzer();
        delay(100);
    }
}

int main()
{
    printf("Iniciando teste do buzzer...\n");

    if (wiringPiSetupGpio() == -1) {
        printf("Erro ao inicializar wiringPi.\n");
        return 1;
    }

    pinMode(BUZZER_PIN, OUTPUT);

    if (softToneCreate(BUZZER_PIN) != 0) {
        printf("Erro ao configurar softTone.\n");
        return 1;
    }

    printf("Som de sucesso...\n");
    successSound();

    delay(1000);

    printf("Som de erro...\n");
    errorSound();

    delay(1000);

    printf("Som de alerta...\n");
    alertSound();

    stopBuzzer();

    printf("Teste concluido.\n");

    return 0;
}