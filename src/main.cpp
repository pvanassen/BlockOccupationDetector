#include <Arduino.h>
#include "Blokdetectie.h"

Blokdetectie* block1;

void blockOccupied(byte id, byte block) {
    Serial.print("Train entered block ");
    Serial.print(block);
    Serial.print(" on detector ");
    Serial.println(id);
}

void blockReleased(byte id, byte block) {
    Serial.print("Train exitted block ");
    Serial.print(block);
    Serial.print(" on detector ");
    Serial.println(id);
}

void setup() {
    Serial.begin(9600);
    Serial.println("Initializing block 1");
    block1 = new Blokdetectie(0, 2, 3, 4, A0, 6, &blockOccupied, blockReleased);
    Serial.println("Done setup");
}

void loop() {
    block1->tick();
    delay(750);
}
