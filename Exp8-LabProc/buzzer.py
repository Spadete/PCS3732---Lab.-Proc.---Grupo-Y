import RPi.GPIO as GPIO
import time

BUZZER_PIN = 4

GPIO.setmode(GPIO.BCM)
GPIO.setup(BUZZER_PIN, GPIO.OUT)

buzzer = GPIO.PWM(BUZZER_PIN, 440)

try:
    buzzer.start(5)

    notes = [
        (130, "C"),
        (164, "E"),
        (196, "G"),
        (220, "A"),
        (261, "High C"),
        (329, "High E")
    ]

    for frequency, name in notes:
        print(f"{name} ({frequency} Hz)")
        buzzer.ChangeFrequency(frequency)
        time.sleep(2)

finally:
    buzzer.stop()
    GPIO.cleanup()
    print("Test finished.")