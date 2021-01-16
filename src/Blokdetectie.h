//
// Created by paul on 15-1-2021.
//

#ifndef BLOKDETECTIE_BLOKDETECTIE_H
#define BLOKDETECTIE_BLOKDETECTIE_H

#include <Arduino.h>

class Blokdetectie {
private:
    byte id, pinRead, pinEnable = -1;
    byte selectPinArray[3] = {0};
    bool lastState[8] = {false};
    unsigned long lastStateChange[8] = {0};
    bool blockStates[8] = {false};
    byte debounceDelay = 50;
    void (*blockOccupied)(byte id, byte block);
    void (*blockReleased)(byte id, byte block);

public:

    // Constructors
    Blokdetectie(byte id, byte pinA, byte pinB, byte pinC, byte pinRead, void (*blockOccupied)(byte id, byte block), void (*blockReleased)(byte id, byte block));
    Blokdetectie(byte id, byte pinA, byte pinB, byte pinC, byte pinRead, byte pinEnable, void (*blockOccupied)(byte id, byte block), void (*blockReleased)(byte id, byte block));

    // Checking logic
    void tick();
};

#endif //BLOKDETECTIE_BLOKDETECTIE_H

