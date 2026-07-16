import RPi.GPIO as GPIO
import time

SERVO_PIN = 18
BUZZER_PIN = 4
BPM_INTERVAL = 1.0

GPIO.setmode(GPIO.BCM)
GPIO.setup(SERVO_PIN, GPIO.OUT)
GPIO.setup(BUZZER_PIN, GPIO.OUT)

servo = GPIO.PWM(SERVO_PIN, 50)
buzzer = GPIO.PWM(BUZZER_PIN, 880)

try:
    servo.start(5)
    buzzer.start(0)

    is_tick = True

    while True:
        cycle_start_time = time.time()

        if is_tick:
            servo.ChangeDutyCycle(10)
        else:
            servo.ChangeDutyCycle(5)

        buzzer.ChangeDutyCycle(5)
        time.sleep(0.1)
        buzzer.ChangeDutyCycle(0)

        is_tick = not is_tick

        execution_time = time.time() - cycle_start_time
        time.sleep(max(0, BPM_INTERVAL - execution_time))

except KeyboardInterrupt:
    pass

finally:
    servo.ChangeDutyCycle(0)
    buzzer.ChangeDutyCycle(0)
    servo.stop()
    buzzer.stop()
    GPIO.cleanup()