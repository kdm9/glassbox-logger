BOARD_TAG        := mini328
MONITOR_PORT     := /dev/ttyACM?
MONITOR_PORT     := /dev/ttyUSB?
ISP_PORT         := /dev/ttyS0
ISP_PROG         := usbasp
ARDUINO_LIBS     := DHT Adafruit_Sensor SD LowPower RTClib Wire
USER_LIB_PATH    := $(realpath deps/)

include /usr/share/arduino/Arduino.mk
