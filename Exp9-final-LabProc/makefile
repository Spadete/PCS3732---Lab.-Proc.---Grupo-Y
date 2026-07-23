CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17

CPPFLAGS = -I/usr/local/include
LDFLAGS = -L/usr/local/lib

WIRINGPI_LIBS = -lwiringPi
LCD_LIBS = -lwiringPi -lwiringPiDev

KEYPAD_FILES = Keypad.cpp Key.cpp

all: \
	lcd-test \
	keypad-test \
	keypad-lcd-test \
	buzzer-test \
	sensor-test

lcd-test: lcd-test.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) \
	lcd-test.cpp \
	-o lcd-test \
	$(LDFLAGS) \
	$(LCD_LIBS)

keypad-test: keypad-test.cpp $(KEYPAD_FILES)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) \
	keypad-test.cpp $(KEYPAD_FILES) \
	-o keypad-test \
	$(LDFLAGS) \
	$(WIRINGPI_LIBS)

keypad-lcd-test: keypad-lcd-test.cpp $(KEYPAD_FILES)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) \
	keypad-lcd-test.cpp $(KEYPAD_FILES) \
	-o keypad-lcd-test \
	$(LDFLAGS) \
	$(LCD_LIBS)

buzzer-test: buzzer-test.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) \
	buzzer-test.cpp \
	-o buzzer-test \
	$(LDFLAGS) \
	$(WIRINGPI_LIBS)

sensor-test: sensor-test.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) \
	sensor-test.cpp \
	-o sensor-test \
	$(LDFLAGS) \
	$(WIRINGPI_LIBS)

clean:
	rm -f \
	lcd-test \
	keypad-test \
	keypad-lcd-test \
	buzzer-test \
	sensor-test

.PHONY: all clean