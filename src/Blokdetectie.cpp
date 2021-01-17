//
// Created by paul on 15-1-2021.
//

#include "Blokdetectie.h"
#include "Arduino.h"

Blokdetectie::Blokdetectie(byte id, byte pinA, byte pinB, byte pinC, byte pinRead, void (*blockOccupied)(byte, byte),
                           void (*blockReleased)(byte, byte)) {
    this->selectPinArray[0] = pinA;
    this->selectPinArray[1] = pinB;
    this->selectPinArray[2] = pinC;
    this->pinRead = pinRead;
    this->blockOccupied = blockOccupied;
    this->blockReleased = blockReleased;
    this->id = id;
    pinMode(pinA, OUTPUT);
    pinMode(pinB, OUTPUT);
    pinMode(pinC, OUTPUT);
    pinMode(pinRead, INPUT);
}

Blokdetectie::Blokdetectie(byte id, byte pinA, byte pinB, byte pinC, byte pinRead, byte pinEnable, void (*blockOccupied)(byte, byte),
                           void (*blockReleased)(byte, byte)): Blokdetectie(id, pinA, pinB, pinC, pinRead, blockOccupied, blockReleased) {
    this->pinEnable = pinEnable;
    pinMode(pinEnable, OUTPUT);
}


void Blokdetectie::tick() {
    // Enable device
    if (pinEnable < 255) {
        digitalWrite(pinEnable, LOW);
    }
    unsigned long now = millis();

    for (byte pin=0; pin<=7; pin++) {
        for (int i=0; i<3; i++) {
            digitalWrite(this->selectPinArray[i], pin & (1<<i) ? HIGH : LOW);
        }
        delay(1);
        // 0 -> block occupied, 1 -> block free
        Serial.print("Pin ");
        Serial.print(pin);
        Serial.print(" reads: ");
        Serial.print(analogRead(this->pinRead));
        Serial.print("\t");
        bool occupied = digitalRead(this->pinRead) == 0;
        if (lastState[pin] != occupied) {
            lastStateChange[pin] = now;
        }
        else if ((now - lastStateChange[pin]) > debounceDelay) {
            if (blockStates[pin] != occupied) {
                blockStates[pin] = occupied;
                if (occupied) {
                    blockOccupied(id, pin);
                }
                else {
                    blockReleased(id, pin);
                }
            }
        }
    }

    Serial.println();

    // Disable device
    if (pinEnable < 255) {
        digitalWrite(pinEnable, HIGH);
    }
}