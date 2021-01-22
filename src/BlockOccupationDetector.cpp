
#include "BlockOccupationDetector.h"

BlockOccupationDetector::BlockOccupationDetector(byte pinA, byte pinB, byte pinC, byte pinRead, void (*blockOccupied)(byte),
                               void (*blockReleased)(byte)) {
    selectPinArray[0] = pinA;
    selectPinArray[1] = pinB;
    selectPinArray[2] = pinC;
    this->pinRead = pinRead;
    this->blockOccupied = blockOccupied;
    this->blockReleased = blockReleased;
    pinMode(pinA, OUTPUT);
    pinMode(pinB, OUTPUT);
    pinMode(pinC, OUTPUT);
    pinMode(pinRead, INPUT);
}

void BlockOccupationDetector::addDetector(byte detectorEnablePin) {
    if (detectors > 0) {
        pinEnable = (byte*) realloc(pinEnable, (detectors + 1) * sizeof(byte));
        lastState = (bool**) realloc(lastState, (detectors + 1) * sizeof(bool[8]));
        lastStateChange = (unsigned long**) realloc(lastStateChange, (detectors + 1) * sizeof(long[8]));
        blockStates = (bool**) realloc(blockStates, (detectors + 1) * sizeof(bool[8]));
    }
    else {
        pinEnable = (byte*) malloc((detectors + 1) * sizeof(byte));
        lastState = (bool**) malloc((detectors + 1) * sizeof(bool[8]));
        lastStateChange = (unsigned long**) malloc((detectors + 1) * sizeof(long[8]));
        blockStates = (bool**) malloc((detectors + 1) * sizeof(bool[8]));
    }
    pinEnable[detectors] = detectorEnablePin;
    pinMode(detectorEnablePin, OUTPUT);
    digitalWrite(detectorEnablePin, HIGH);

    for (int i=0;i!=8;i++) {
        lastState[detectors][i] = false;
        lastStateChange[detectors][i] = 0;
        blockStates[detectors][i] = false;
    }

    detectors++;
}

void BlockOccupationDetector::tick() {
    now = millis();
    for (byte block=0; block<=7; block++) {
        for (int pin=0; pin<3; pin++) {
            digitalWrite(selectPinArray[pin], block & (1<<pin) ? HIGH : LOW);
        }
        for (byte detector=0;detector!=detectors;detector++) {
            digitalWrite(pinEnable[detector], LOW);
            delay(1);

            int value = 0;
            for (int sample=0;sample!=samples;sample++) {
                value += analogRead(this->pinRead);
            }
            bool occupied = (value / samples) < 150;
            if (lastState[detector][block] != occupied) {
                lastStateChange[detector][block] = now;
                lastState[detector][block] = occupied;
            }
            else if ((now - lastStateChange[detector][block]) > debounceDelay) {
                if (blockStates[detector][block] != occupied) {
                    blockStates[detector][block] = occupied;
                    int absoluteBlock = (detector * 8) + block;
                    if (occupied) {
                        blockOccupied(absoluteBlock);
                    }
                    else {
                        blockReleased(absoluteBlock);
                    }
                }
            }
            digitalWrite(pinEnable[detector], HIGH);
        }
    }
}

byte BlockOccupationDetector::firstAvailableSensor() {
    return (detectors * 8);
}