#include <Arduino.h>
#include "USB.h"
#include "USBMIDI.h"

// Freenove ESP32-S3
// Viscount 32-note pedalboard: 8 rows x 4 columns

USBMIDI MIDI("Viscount Pedalboard");

const uint8_t rowPins[8] = {
  4, 5, 6, 7, 15, 16, 17, 18
};

const uint8_t colPins[4] = {
  9, 10, 11, 12
};

const uint8_t MIDI_CHANNEL = 3;
const uint8_t FIRST_NOTE = 36;  // Lowest C
const uint8_t VELOCITY = 100;

const unsigned long DEBOUNCE_MS = 15;

// true: current conducts from column to row.
// false: current conducts from row to column.
const bool COLUMN_TO_ROW = true;

bool stableState[4][8] = {};
bool lastReading[4][8] = {};
unsigned long lastChange[4][8] = {};

void configureMatrix() {
  if (COLUMN_TO_ROW) {
    for (int r = 0; r < 8; r++) {
      pinMode(rowPins[r], INPUT_PULLDOWN);
    }

    for (int c = 0; c < 4; c++) {
      pinMode(colPins[c], INPUT);
    }
  } else {
    for (int c = 0; c < 4; c++) {
      pinMode(colPins[c], INPUT_PULLDOWN);
    }

    for (int r = 0; r < 8; r++) {
      pinMode(rowPins[r], INPUT);
    }
  }
}

void scanMatrix(bool result[4][8]) {
  if (COLUMN_TO_ROW) {
    for (int c = 0; c < 4; c++) {
      pinMode(colPins[c], OUTPUT);
      digitalWrite(colPins[c], HIGH);

      delayMicroseconds(100);

      for (int r = 0; r < 8; r++) {
        result[c][r] = digitalRead(rowPins[r]) == HIGH;
      }

      pinMode(colPins[c], INPUT);
    }
  } else {
    for (int r = 0; r < 8; r++) {
      pinMode(rowPins[r], OUTPUT);
      digitalWrite(rowPins[r], HIGH);

      delayMicroseconds(100);

      for (int c = 0; c < 4; c++) {
        result[c][r] = digitalRead(colPins[c]) == HIGH;
      }

      pinMode(rowPins[r], INPUT);
    }
  }
}

void setup() {
  configureMatrix();

  MIDI.begin();
  USB.begin();

  delay(1000);
}

void loop() {
  bool current[4][8];
  scanMatrix(current);

  unsigned long now = millis();

  for (int c = 0; c < 4; c++) {
    for (int r = 0; r < 8; r++) {
      if (current[c][r] != lastReading[c][r]) {
        lastReading[c][r] = current[c][r];
        lastChange[c][r] = now;
      }

      if ((now - lastChange[c][r]) >= DEBOUNCE_MS &&
          current[c][r] != stableState[c][r]) {
        stableState[c][r] = current[c][r];

        uint8_t note = FIRST_NOTE + c * 8 + r;

        if (stableState[c][r]) {
          MIDI.noteOn(note, VELOCITY, MIDI_CHANNEL);
        } else {
          MIDI.noteOff(note, 0, MIDI_CHANNEL);
        }
      }
    }
  }

  delay(1);
}
