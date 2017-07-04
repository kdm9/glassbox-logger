BOARD_TAG     = uno
MONITOR_PORT  = /dev/ttyACM?
MONITOR_BAUDRATE = 115200
ARDUINO_LIBS = DHT Adafruit_Sensor SD
USER_LIB_PATH := $(realpath deps/)

include /usr/share/arduino/Arduino.mk
