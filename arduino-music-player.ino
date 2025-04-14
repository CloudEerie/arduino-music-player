// Final project: Play music notes from Serial Monitor input with Arduino
#include <avr/pgmspace.h>

// Example melody stored in PROGMEM (Hot Cross Buns)
const char hotCrossBuns[] PROGMEM =
  "E5 400, D5 400, C5 400, E5 400, D5 400, C5 400, "
  "C5 400, C5 400, C5 400, C5 400, D5 400, D5 400, D5 400, D5 400, "
  "E5 400, D5 400, C5 400";

// const int speakerPin = A5;            
// const int ledPin = 13;                

const int noteHz[] = {  // Default frequencies, A minor scale
  440, 493, 525, 588, 658, 701, 786, 880
};

const char* noteNames[] = {
  "A4", "B4", "C5", "D5", "E5", "F5", "G5", "A5"
};

class Note {
public:
  int frequency;
  long durationMicros;
  String noteName;

  Note(int freq, long durUs, String label) {
    frequency = freq;
    durationMicros = durUs;
    noteName = label;
  }

  void play(int pin) {
    Serial.print("\nPlayed ");
    Serial.print(noteName);
    Serial.print(" for ");
    Serial.print(durationMicros);
    Serial.println(" microseconds");

    if (frequency == 0) {
      delayMicroseconds(durationMicros);
      return;
    }
    long startMicros = micros();
    long elapsed;
    while ((elapsed = micros() - startMicros) < durationMicros) {
      int toneVal = (elapsed * frequency / 500000) % 2;
      // digitalWrite(pin, toneVal);
    }
  }
};

int getFrequency(const String& noteStr) {
  if (noteStr == "NA") return 0; // NA = pause/rest
  for (int i = 0; i < 8; i++) {
    if (noteStr == noteNames[i]) {
      return noteHz[i];
    }
  }
  return 0; // Silence if note is not found
}

void setup() {
  Serial.begin(9600);
  // pinMode(speakerPin, OUTPUT);
  // pinMode(ledPin, OUTPUT);

  Serial.println("Enter notes as NOTE then DURATION, with commas or other punctuation to distinguish between notes.");
  Serial.println("Example: E5 400, D5 400, C5 400, NA 200");
}

void loop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.replace(',', '\n');
    input.replace(';', '\n');
    input.replace('-', '\n');
    input.replace('.', '\n');
    input.trim();

    int start = 0;
    while (start < input.length()) {
      int spaceIndex = input.indexOf(' ', start);
      if (spaceIndex == -1) break;
      int endIndex = input.indexOf('\n', spaceIndex);

      String currentNote = input.substring(start, spaceIndex);
      int durationEnd = (endIndex != -1) ? endIndex : input.length();
      String durationStr = input.substring(spaceIndex + 1, durationEnd);
      int durationMs = durationStr.toInt();

      int freq = getFrequency(currentNote);
      Note note(freq, (long)durationMs * 1000, currentNote);
      // note.play(speakerPin);
      note.play(0);

      start = durationEnd + 1;
    }
    Serial.println("Finished playing input.");
  }
}
