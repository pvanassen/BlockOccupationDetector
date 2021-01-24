
#include "BlockOccupationDetector.h"

BlockOccupationDetector::BlockOccupationDetector(byte pinA, byte pinB, byte pinC, byte pinRead) {
    selectPinArray[0] = pinA;
    selectPinArray[1] = pinB;
    selectPinArray[2] = pinC;
    this->pinRead = pinRead;
    pinMode(pinA, OUTPUT);
    pinMode(pinB, OUTPUT);
    pinMode(pinC, OUTPUT);
    pinMode(pinRead, INPUT);
    if (debugStream == NULL) {
    #ifdef BLOCKOCCUPATIONDETECTOR_DEBUG
        exit(0);
    #endif
    }
}

BlockOccupationDetector::BlockOccupationDetector(byte pinA, byte pinB, byte pinC, byte pinRead,
                                                 void (*blockOccupied)(byte), void (*blockReleased)(byte)):
        BlockOccupationDetector(pinA, pinB, pinC, pinRead) {
    this->blockOccupied = blockOccupied;
    this->blockReleased = blockReleased;
}

BlockOccupationDetector::BlockOccupationDetector(byte pinA, byte pinB, byte pinC, byte pinRead, Stream *debugStream):
        BlockOccupationDetector(pinA, pinB, pinC, pinRead){
    this->debugStream = debugStream;
}

BlockOccupationDetector::BlockOccupationDetector(byte pinA, byte pinB, byte pinC, byte pinRead, Stream *debugStream,
                                                 void (*blockOccupied)(byte), void (*blockReleased)(byte)):
                                                 BlockOccupationDetector(pinA, pinB, pinC, pinRead, debugStream) {
    this->blockOccupied = blockOccupied;
    this->blockReleased = blockReleased;
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
    if (block == 8) {
        block = 0;
    }
    if (detector == detectors) {
        detector = 0;
    }

    #ifdef BLOCKOCCUPATIONDETECTOR_DEBUG
        debugStream->print("Checking block: ");
        debugStream->print(block);
        debugStream->print(", detector: ");
        debugStream->println(detector);
    #endif

    for (int pin=0; pin<3; pin++) {
        digitalWrite(selectPinArray[pin], block & (1<<pin) ? HIGH : LOW);
    }
    digitalWrite(pinEnable[detector], LOW);

    lastBlock.changed = false;

    int value = 0;
    for (int sample=0;sample!=samples;sample++) {
        value += analogRead(this->pinRead);
    }

    #ifdef BLOCKOCCUPATIONDETECTOR_DEBUG
        debugStream->print("Value from analog read: ");
        debugStream->println(value);
    #endif

    bool occupied = (value / samples) < 150;
    if (lastState[detector][block] != occupied) {
        lastStateChange[detector][block] = now;
        lastState[detector][block] = occupied;
    }
    else if ((now - lastStateChange[detector][block]) > debounceDelay) {
        if (blockStates[detector][block] != occupied) {
            blockStates[detector][block] = occupied;

            int absoluteBlock = (detector * 8) + block;
            lastBlock.changed = true;
            lastBlock.absoluteBlock = absoluteBlock;
            lastBlock.occupied = occupied;

            if (blockOccupied != NULL && blockReleased != NULL) {
                if (occupied) {
                    blockOccupied(absoluteBlock);
                } else {
                    blockReleased(absoluteBlock);
                }
            }
        }
    }

    digitalWrite(pinEnable[detector], HIGH);
}

byte BlockOccupationDetector::firstAvailableSensor() {
    return (detectors * 8);
}

bool* BlockOccupationDetector::getLastKnownStates() {
    return (bool*)lastState;
}