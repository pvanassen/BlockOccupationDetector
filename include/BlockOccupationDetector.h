#ifndef BLOCKOCCUPATIONDETECTOR_H
#define BLOCKOCCUPATIONDETECTOR_H

#include <Arduino.h>

class BlockOccupationDetector {
private:
    byte pinRead = -1;
    byte selectPinArray[3] = {0};
    byte *pinEnable;
    byte detectors = 0;
    const byte samples = 20;
    bool **lastState;
    unsigned long ** lastStateChange;
    bool **blockStates;
    byte debounceDelay = 50;
    void (*blockOccupied)(byte block);
    void (*blockReleased)(byte block);
    unsigned long now;

public:

    // Constructors
    BlockOccupationDetector(byte pinA, byte pinB, byte pinC, byte pinRead, void (*blockOccupied)(byte block), void (*blockReleased)(byte block));

    void addDetector(byte pinEnable);

    // Checking logic
    void tick();

    byte firstAvailableSensor();
};

#endif //BLOCKOCCUPATIONDETECTOR_H

