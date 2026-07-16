import RPi.GPIO as GPIO
import time

PINO_MOTOR = 18

GPIO.setmode(GPIO.BCM)
GPIO.setup(PINO_MOTOR, GPIO.OUT)

motor_pwm = GPIO.PWM(PINO_MOTOR, 50)

try:
    motor_pwm.start(0)

    while True:
        for valor_pwm in range(25, 126):
            motor_pwm.ChangeDutyCycle(valor_pwm / 10)
            time.sleep(0.03)

        for valor_pwm in range(125, 24, -1):
            motor_pwm.ChangeDutyCycle(valor_pwm / 10)
            time.sleep(0.03)

except KeyboardInterrupt:
    print("Interrompido.")

finally:
    motor_pwm.ChangeDutyCycle(0)
    motor_pwm.stop()
    GPIO.cleanup()