// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <DHT.h>
#include <SD.h>

#define PIN_CS_SD   4
#define PIN_DHT     2

char LOG_FILENAME[] = "temp.tsv";
#define DELAY_SECS  5

/* WIRING
# DHT
Connect pin 1 (on the left) of the sensor to +5V (or 3v3 if logic is 3v3)
Connect pin 2 of the sensor to PIN_DHT
Connect pin 4 (on the right) of the sensor to GROUND

# SD card
For the etherten: Pin is pin4
Otherwise, connect SCL, MISO, MOSI to common bus, and CS/SS to PIN_CS_SD
*/

DHT dht(PIN_DHT, DHT22);

void setup() {
    Serial.begin(115200);
    pinMode(PIN_CS_SD, OUTPUT);

    Serial.print("#Initializing SD card... ");
    if (!SD.begin(PIN_CS_SD)) {
        Serial.println("FAILED -- suspending");
        while(1);
    }
    Serial.println("done!");
    dht.begin();
    if (!SD.exists(LOG_FILENAME)) {
        File log_file = SD.open(LOG_FILENAME, FILE_WRITE);
        if (!log_file) {
            Serial.println("# error opening file");
            while(1);
        }
        log_file.println("humidity\ttemerature\ttime");
        log_file.close();
    }
    Serial.println("humidity\ttemerature\ttime");
}

void loop() {
    uint32_t start = millis();
    float secs = (float)start / 1000.0;

    File log_file = SD.open(LOG_FILENAME, FILE_WRITE);
    if (!log_file) {
        Serial.println("# error opening file");
        delay(1000);
        return;
    }

    float h = dht.readHumidity();
    float t = dht.readTemperature();

    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t)) {
        log_file.println("NA\tNA");
        return;
    }

    log_file.print(h);
    log_file.print("\t");
    log_file.print(t);
    log_file.print("\t");
    log_file.println(secs);
    log_file.close();

    Serial.print(h);
    Serial.print("\t");
    Serial.print(t);
    Serial.print("\t");
    Serial.println(secs);

    delay((DELAY_SECS * 1000) - (millis() - start));
}
