// Final project: Play music notes from Serial Monitor input with Arduino
#include <avr/pgmspace.h>

const char hotCrossBuns[] PROGMEM = // Hot Cross Buns
  "E5 400, D5 400, C5 400, E5 400, D5 400, C5 400, "
  "C5 400, C5 400, C5 400, C5 400, D5 400, D5 400, D5 400, D5 400, "
  "E5 400, D5 400, C5 400";

const int speakerPin = A0;            
bool debugSerialMode = false;

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
      delayMicroseconds(durationMicros); // rest
      return;
    }

    if (debugSerialMode == false) {
      digitalWrite(pin, HIGH);
      delayMicroseconds(durationMicros);
      digitalWrite(pin, LOW);
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
  if (debugSerialMode == false) {
    pinMode(speakerPin, OUTPUT);
  }

  Serial.println("Enter notes as NOTE then DURATION, with commas or dashes as delimiters.");
  Serial.println("Example: E5 400, D5 400, C5 400, NA 200");
}

void loop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');

    // Dash or comma to delimit notes from another
    input.replace(',', '\n');
    input.replace('-', '\n');
    input.replace('–', '\n');
    input.replace('—', '\n');
    input.trim();

    int start = 0;
    while (start < input.length()) {
      int endIndex = input.indexOf('\n', start);
      if (endIndex == -1) endIndex = input.length();

      String noteEntered = input.substring(start, endIndex);
      noteEntered.trim();  // Trim leading/trailing spaces

      int spaceIndex = noteEntered.indexOf(' ');
      if (spaceIndex == -1) {
        Serial.println("Invalid format, skipping: " + noteEntered);
        start = endIndex + 1;
        continue;
      }

      String currNote = noteEntered.substring(0, spaceIndex);
      String durationStr = noteEntered.substring(spaceIndex + 1);
      currNote.trim();
      durationStr.trim();

      int durationMs = durationStr.toInt();
      int freq = getFrequency(currNote);
      Note note(freq, (long)durationMs * 1000, currNote);
      note.play(speakerPin);

      delay(100);
      start = endIndex + 1;
    }

    Serial.println("Finished playing input.");
  }
}

