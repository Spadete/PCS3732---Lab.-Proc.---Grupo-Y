import RPi.GPIO as GPIO
import time

LED_PIN = 17

GPIO.setmode(GPIO.BCM)
GPIO.setup(LED_PIN, GPIO.OUT)

pwm = GPIO.PWM(LED_PIN, 1)

try:
    pwm.start(50)

    frequencies = [2, 20, 50, 100, 500]

    for frequency in frequencies:
        print(f"Testando freq {frequency} Hz")
        pwm.ChangeFrequency(frequency)
        time.sleep(5)

finally:
    pwm.stop()
    GPIO.output(LED_PIN, GPIO.LOW)
    GPIO.cleanup()
    print("Funfou.")