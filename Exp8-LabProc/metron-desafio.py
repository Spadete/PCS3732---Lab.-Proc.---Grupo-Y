import RPi.GPIO as GPIO
import time

SERVO_PIN = 18
BUZZER_PIN = 4
BTN_UP = 20
BTN_DOWN = 21

bpm = 60
BPM_INTERVAL = 60.0 / bpm

GPIO.setmode(GPIO.BCM)
GPIO.setup(SERVO_PIN, GPIO.OUT)
GPIO.setup(BUZZER_PIN, GPIO.OUT)

GPIO.setup(BTN_UP, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.setup(BTN_DOWN, GPIO.IN, pull_up_down=GPIO.PUD_UP)


def aumentar_bpm(channel):
    global bpm, BPM_INTERVAL
    bpm += 5
    BPM_INTERVAL = 60.0 / bpm
    print(f"BPM: {bpm}")


def diminuir_bpm(channel):
    global bpm, BPM_INTERVAL
    if bpm > 5:
        bpm -= 5
        BPM_INTERVAL = 60.0 / bpm
        print(f"BPM: {bpm}")


GPIO.add_event_detect(BTN_UP, GPIO.FALLING, callback=aumentar_bpm, bouncetime=300)
GPIO.add_event_detect(BTN_DOWN, GPIO.FALLING, callback=diminuir_bpm, bouncetime=300)

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
