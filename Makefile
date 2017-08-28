BOARD_TAG        := pro
BOARD_SUB		 := 8MHzatmega328
MONITOR_PORT     := /dev/ttyUSB?
ISP_PORT         := /dev/ttyS0
ISP_PROG         := usbasp
ARDUINO_LIBS     := OneWire DallasTemperature DHT Adafruit_Sensor SdFat LowPower RTClib Wire SPI
USER_LIB_PATH    := $(realpath deps/)

include $(ARDMK_DIR)/Arduino.mk
