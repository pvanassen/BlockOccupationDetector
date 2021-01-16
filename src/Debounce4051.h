//
// Created by paul on 15-1-2021.
//

#ifndef BLOKDETECTIE_DEBOUNCE4051_H
#define BLOKDETECTIE_DEBOUNCE4051_H

#include <Arduino.h>

class Debounce4051 {
private:
    byte pinRead, pinEnable = -1;
    byte selectPinArray[3] = {0};
    bool lastState[8] = {false};
    unsigned long lastStateChange[8] = {0};
    bool blockStates[8] = {false};
    byte debounceDelay = 50;
    void (*blockOccupied)(byte block);
    void (*blockReleased)(byte block);

public:

    // Constructors
    Debounce4051(byte pinA, byte pinB, byte pinC, byte pinRead, void (*blockOccupied)(byte block), void (*blockReleased)(byte block));
    Debounce4051(byte pinA, byte pinB, byte pinC, byte pinRead, byte pinEnable, void (*blockOccupied)(byte block), void (*blockReleased)(byte block));

    // Checking logic
    void tick();
};

#endif //BLOKDETECTIE_DEBOUNCE4051_H

