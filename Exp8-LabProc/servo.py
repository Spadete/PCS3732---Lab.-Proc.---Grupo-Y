import RPi.GPIO as GPIO
import time

SERVO_PIN = 18

GPIO.setmode(GPIO.BCM)
GPIO.setup(SERVO_PIN, GPIO.OUT)

servo = GPIO.PWM(SERVO_PIN, 50)

try:
    servo.start(0)

    while True:
        for duty in range(50, 101):
            servo.ChangeDutyCycle(duty / 10)
            time.sleep(0.03)

        for duty in range(100, 49, -1):
            servo.ChangeDutyCycle(duty / 10)
            time.sleep(0.03)

except KeyboardInterrupt:
    print("Interrompido.")

finally:
    servo.ChangeDutyCycle(0)
    servo.stop()
    GPIO.cleanup()