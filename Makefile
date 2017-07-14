BOARD_TAG        := mini328
MONITOR_PORT     := /dev/ttyACM?
ISP_PORT         := /dev/ttyS0
ISP_PROG         := usbasp
MONITOR_BAUDRATE := 115200
ARDUINO_LIBS     := DHT Adafruit_Sensor SD LowPower
USER_LIB_PATH    := $(realpath deps/)

include /usr/share/arduino/Arduino.mk
