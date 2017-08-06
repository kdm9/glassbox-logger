// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

//#define DHT_DEBUG
#include <DHT.h>
#include <SdFat.h>
#include <LowPower.h>
#include <RTClib.h>


// Uncomment the following during final/non-debugging builds
#define GBL_PRODUCTION

#define PIN_PWR     9
#define PIN_DHT     5
#define PIN_DHT2    3
#define PIN_CS_SD   4
#define PIN_LED     13
#define PIN_VBAT    A0
#define Vcc         3.3

#define LOG_FILENAME "temp.tsv"
#define DELAY_SECS  60
#define RTC_MODEL 3231

/* WIRING
# DHT
Connect pin 1 (on the left) of the sensor to +5V (or 3v3 if logic is 3v3)
Connect pin 2 of the sensor to PIN_DHT
Connect pin 4 (on the right) of the sensor to GROUND

# SD card
For the etherten: Pin is pin4
Otherwise, connect SCL, MISO, MOSI to (, and CS/SS to PIN_CS_SD
*/


SdFat sd;
#if RTC_MODEL == 3231
RTC_DS3231 rtc;
#elif RTC_MODEL == 1307
RTC_DS1307 rtc;
#else
#error "RTC must be one of DS3231 or DS1307"
#endif

DHT dht(PIN_DHT, DHT22);
DHT dht2(PIN_DHT2, DHT22);

void iso8601(char *buf, const DateTime &t);
void mkfilename(char *buf, const DateTime &t);
void pwrDown();
void pwrUp();
void deepSleep();
void setupRTC();
float readVbat();


void setup() {
    Serial.begin(9600);
    #ifndef GBL_PRODUCTION
    Serial.print("#Initializing... ");
    #endif

    pinMode(10, OUTPUT); // SS pin must be kept high
    pinMode(PIN_CS_SD, OUTPUT);
    pinMode(PIN_PWR, OUTPUT);

    digitalWrite(PIN_PWR, HIGH);
    delay(20);

    setupRTC();

    #ifndef GBL_PRODUCTION
    Serial.println("done!");
    Serial.println("time\thumidity_1\ttemperature_1\thumidity_2\ttemperature_2\tvbat");
    Serial.flush();
    #endif
}

void loop() {
    pwrUp();

    DateTime now = rtc.now();
    char ts[25] = "";
    iso8601(ts, now);

    char filename[32] = "";
    mkfilename(filename, now);

    SdFile log_file;
    if (!log_file.open(filename, FILE_WRITE)) {
        #ifndef GBL_PRODUCTION
        Serial.println("# error opening file");
        delay(1000);
        #endif
        return;
    }


    float h = dht.readHumidity();
    float t = dht.readTemperature();
    float h2 = dht2.readHumidity();
    float t2 = dht2.readTemperature();
    float vbat = readVbat();


    log_file.print(ts);
    log_file.print("\t");
    log_file.print(h);
    log_file.print("\t");
    log_file.print(t);
    log_file.print("\t");
    log_file.print(h2);
    log_file.print("\t");
    log_file.print(t2);
    log_file.print("\t");
    log_file.println(vbat);
    log_file.sync();
    log_file.close();
    delay(20);

    #ifndef GBL_PRODUCTION
    Serial.print(ts);
    Serial.print("\t");
    Serial.print(h);
    Serial.print("\t");
    Serial.print(t);
    Serial.print("\t");
    Serial.print(h2);
    Serial.print("\t");
    Serial.print(t2);
    Serial.print("\t");
    Serial.println(vbat);
    #endif

    // Power off
    pwrDown();
    deepSleep();
}

float readVbat()
{
    return analogRead(PIN_VBAT) * (Vcc / 1024.0) * 2;
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
    #ifndef GBL_PRODUCTION
    Serial.flush();
    Serial.end();
    #endif
}

void pwrUp()
{
    Serial.begin(9600);
    digitalWrite(PIN_PWR, HIGH);
    delay(50); // Wait for pwr on
    while(!sd.begin(PIN_CS_SD, SPI_HALF_SPEED)) {
        Serial.println("Card init failed");
        delay(1000);
    }

    dht.begin();
    dht2.begin();

    // Allow systems to stabilise before starting measurements
    delay(500);
}

void iso8601(char *buffer, const DateTime &t)
{
    int len = sprintf(buffer, "%04u-%02u-%02u_%02u:%02u:%02u", t.year(), t.month(),
            t.day(), t.hour(), t.minute(), t.second());
    buffer[len] = 0;
}

void mkfilename(char *buffer, const DateTime &t)
{
    int len = sprintf(buffer, "%04u-%02u-%02u.tsv", t.year(), t.month(), t.day());
    buffer[len] = 0;
}

void setupRTC()
{
    while (!rtc.begin()) {
        #ifndef GBL_PRODUCTION
        Serial.println("Couldn't find RTC");
        #endif
        delay(1000);
    }
    #if RTC_MODEL == 1307
    bool needs_setting = !rtc.isrunning();
    #elif RTC_MODEL == 3231
    bool needs_setting = rtc.lostPower();
    #endif
    if (needs_setting) {
        #ifndef GBL_PRODUCTION
        Serial.println("Couldn't find RTC");
        #endif
        // Set clock if unset
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }


}
