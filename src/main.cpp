#include <Arduino.h>
#include "Debounce4051.h"

Debounce4051* block1;

void blockOccupied(byte block) {
    Serial.println("Train entered block ");
    Serial.println(block);
}

void blockReleased(byte block) {
    Serial.print("Train exitted block ");
    Serial.println(block);
}

void setup() {
    Serial.begin(9600);
    Serial.println("Initializing block 1");
    block1 = new Debounce4051(2, 3, 4, 6, 8, &blockOccupied, blockReleased);
    Serial.println("Done setup");
}

void loop() {
    block1->tick();
    delay(10);
}
