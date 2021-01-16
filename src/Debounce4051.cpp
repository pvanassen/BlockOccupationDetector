//
// Created by paul on 15-1-2021.
//

#include "Debounce4051.h"
#include "Arduino.h"

Debounce4051::Debounce4051(byte pinA, byte pinB, byte pinC, byte pinRead, void (*blockOccupied)(byte),
                           void (*blockReleased)(byte)) {
    this->selectPinArray[0] = pinA;
    this->selectPinArray[1] = pinB;
    this->selectPinArray[2] = pinC;
    this->pinRead = pinRead;
    this->blockOccupied = blockOccupied;
    this->blockReleased = blockReleased;
    pinMode(pinA, OUTPUT);
    pinMode(pinB, OUTPUT);
    pinMode(pinC, OUTPUT);
    pinMode(pinRead, INPUT);
}

Debounce4051::Debounce4051(byte pinA, byte pinB, byte pinC, byte pinRead, byte pinEnable, void (*blockOccupied)(byte),
                           void (*blockReleased)(byte)) {
    Debounce4051(pinA, pinB, pinC, pinRead, blockOccupied, blockReleased);
    this->pinEnable = pinEnable;
    pinMode(pinEnable, OUTPUT);
}


void Debounce4051::tick() {
    // Enable device
    if (pinEnable > -1) {
        digitalWrite(pinEnable, LOW);
    }
    unsigned long now = millis();

    for (byte pin=0; pin<=7; pin++) {
        for (int i=0; i<3; i++) {
            digitalWrite(this->selectPinArray[i], pin & (1<<i) ? HIGH : LOW);
        }
        // 0 -> block occupied, 1 -> block free
        bool occupied = digitalRead(this->pinRead) == 0;
        if (lastState[pin] != occupied) {
            lastStateChange[pin] = now;
        }
        else if ((now - lastStateChange[pin]) > debounceDelay) {
            if (blockStates[pin] != occupied) {
                blockStates[pin] = occupied;
                if (occupied) {
                    blockOccupied(pin);
                }
                else {
                    blockReleased(pin);
                }
            }
        }

    }

    // Disable device
    if (pinEnable > -1) {
        digitalWrite(pinEnable, HIGH);
    }
}