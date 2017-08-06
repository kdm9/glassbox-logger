// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <DHT.h>
#include <SdFat.h>
#include <LowPower.h>
#include <RTClib.h>


#define PIN_PWR     9
#define PIN_CS_SD   4
#define PIN_LED     13
#define PIN_VBAT    A0

#define DELAY_SECS  10
#define RTC_CLASS RTC_DS1307

/* WIRING
# DHT
Connect pin 1 (on the left) of the sensor to +5V (or 3v3 if logic is 3v3)
Connect pin 2 of the sensor to PIN_DHT
Connect pin 4 (on the right) of the sensor to GROUND

# SD card
For the etherten: Pin is pin4
Otherwise, connect SCL, MISO, MOSI to (, and CS/SS to PIN_CS_SD
*/


RTC_CLASS rtc;
SdFat sd;

void iso8601(char *buf, const DateTime &t);
void iso8601dt(char *buf, const DateTime &t);
void pwrDown();
void pwrUp();


void setup() {
    Serial.begin(115200);
    Serial.print("#Initializing... ");

    pinMode(10, OUTPUT); // SS pin must be kept high
    pinMode(PIN_CS_SD, OUTPUT);
    pinMode(PIN_PWR, OUTPUT);

    digitalWrite(PIN_PWR, HIGH);

    while (!rtc.begin()) {
        #ifndef GLB_PRODUCTION
        Serial.println("Couldn't find RTC");
        #endif
        delay(10);
    }
    if (!rtc.isrunning()) {
        #ifndef GLB_PRODUCTION
        Serial.println("Couldn't find RTC");
        #endif
        // Set clock if unset
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }


    #ifndef GLB_PRODUCTION
    Serial.println("done!");
    Serial.println("time\thumidity_1\ttemperature_1\thumidity_2\ttemperature_2\tvbat");
    #endif

}

void loop() {
    pwrUp();

    uint32_t start = millis();
    float secs = (float)start / 1000.0;

    DateTime now = rtc.now();
    char ts[25] = "";
    iso8601(ts, now);

    char filename[32] = "";
    iso8601dt(filename, now);
    strcat(filename, ".tsv");

    SdFile log_file;
    if (!log_file.open(filename, FILE_WRITE)) {
#ifndef GLB_PRODUCTION
        Serial.println("# error opening file");
        delay(1000);
#endif
        return;
    }

    analogRead(PIN_VBAT);
    float vbat = analogRead(PIN_VBAT) * (3.3 / 1024.0) * 2;

    log_file.print(ts);
    log_file.print("\t");
    log_file.println(vbat);
    log_file.close();
#ifndef GLB_PRODUCTION
    Serial.print(ts);
    Serial.print("\t");
    Serial.println(vbat);
#endif

    delay(100);


    // Power off
    pwrDown();
    deepSleep();
}

void deepSleep()
{
    // To be replaced with RTC interrupt and SLEEP_FOREVER
    int to_sleep = DELAY_SECS;
    while (to_sleep >= 8) {
        LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
        to_sleep -= 8;
    }
    while (to_sleep > 0) {
        LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF);
        to_sleep -= 1;
    }
}

void pwrDown()
{
    digitalWrite(PIN_PWR, LOW);
}

void pwrUp()
{
    digitalWrite(PIN_PWR, HIGH);
    delay(50); // Wait for pwr on
    while(!sd.begin(PIN_CS_SD, SPI_HALF_SPEED)) {
        Serial.println("Card init failed");
        delay(1000);
    }
}

void iso8601(char *buffer, const DateTime &t)
{
    sprintf(buffer, "%04u-%02u-%02u_%02u:%02u:%02u", t.year(), t.month(),
            t.day(), t.hour(), t.minute(), t.second());
}

void iso8601dt(char *buffer, const DateTime &t)
{
    sprintf(buffer, "%04u-%02u-%02u", t.year(), t.month(), t.day());
}
