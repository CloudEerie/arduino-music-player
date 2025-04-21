// Final project: Play music notes from Serial Monitor input with Arduino
#include <avr/pgmspace.h>

const char hotCrossBuns[] PROGMEM =
  "E5 400, D5 400, C5 400, NA 200, E5 400, D5 400, C5 400, "
  "C5 200, C5 200, C5 200, C5 200, NA 200, D5 200, D5 200, D5 200, D5 200, NA 200, "
  "E5 300, D5 300, C5 300";

const char happyBirthday[] PROGMEM =
  "G4 400, G4 400, A4 600, G4 600, C5 600, B4 800, NA 200, "
  "G4 400, G4 400, A4 600, G4 600, D5 600, C5 800, NA 200, "
  "G4 400, G4 400, G5 600, E5 600, C5 600, B4 600, A4 800, NA 200, "
  "F5 400, F5 400, E5 600, C5 600, D5 600, C5 800";


const int buzzerPin = A0;
bool debugSerialMode = false;

const int noteHz[] = {
  440, 493, 525, 588, 658, 701, 786, 880
};

const char* noteNames[] = { // Default frequencies, A minor scale
  "A4", "B4", "C5", "D5", "E5", "F5", "G5", "A5"
};

class Note {
public:
  int frequency;
  int durationMs;
  String noteName;

  Note(int freq, int durMs, String label) {
    frequency = freq;
    durationMs = durMs;
    noteName = label;
  }

  void play(int pin) {
    Serial.print("\nPlayed ");
    Serial.print(noteName);
    Serial.print(" for ");
    Serial.print(durationMs);
    Serial.println(" milliseconds");

    if (frequency == 0) {
      delay(durationMs);
      return;
    }

    if (debugSerialMode == false) {
      tone(pin, frequency, durationMs); // Start playing
      delay(durationMs);               
      noTone(pin); // Stop playing                    
    }
  }

};

struct Song {
  const char* name;
  const char* data;
};

const Song songLibrary[] = {
  { "Hot Cross Buns", hotCrossBuns },
  { "Happy Birthday", happyBirthday}
};

int getFrequency(const String& noteStr) {
  if (noteStr == "NA") {
    return 0;
  }
  for (int i = 0; i < 8; i++) {
    if (noteStr == noteNames[i]) {
      return noteHz[i];
    }
  }
  return 0;
}

void playNoteSequence(const String& input) {
  String useableInput = input;
  useableInput.replace(',', '\n');
  useableInput.replace('-', '\n');
  useableInput.replace('–', '\n');
  useableInput.replace('—', '\n');
  useableInput.trim();

  int start = 0;
  while (start < useableInput.length()) {
    int endIndex = useableInput.indexOf('\n', start);
    if (endIndex == -1) endIndex = useableInput.length();

    String noteEntry = useableInput.substring(start, endIndex);
    noteEntry.trim();

    int spaceIndex = noteEntry.indexOf(' ');
    if (spaceIndex == -1) {
      Serial.println("Invalid format, skipping: " + noteEntry);
      start = endIndex + 1;
      continue;
    }

    String currNote = noteEntry.substring(0, spaceIndex);
    String durationStr = noteEntry.substring(spaceIndex + 1);
    currNote.trim();
    durationStr.trim();

    int durationMs = durationStr.toInt();
    int freq = getFrequency(currNote);
    Note note(freq, durationMs, currNote);
    note.play(buzzerPin);

    delay(100);
    start = endIndex + 1;
  }

  Serial.println("Finished playing input.");
}

void playSongByNumber(int songNumber) {
  const int songCount = sizeof(songLibrary) / sizeof(songLibrary[0]);
  if ((songNumber > 0) && (songNumber <= songCount)) {
    char buffer[512];
    strcpy_P(buffer, songLibrary[songNumber - 1].data);
    Serial.print("Playing: ");
    Serial.println(songLibrary[songNumber - 1].name);
    playNoteSequence(String(buffer));
  }
  else {
    Serial.println("Error: Invalid song number.");
  }
}


void setup() {
  Serial.begin(9600);
  if (debugSerialMode == false) {
    pinMode(buzzerPin, OUTPUT);
  }

  Serial.println("\nEnter notes as NOTE then DURATION, with commas or dashes as delimiters.\n(Example: E5 400, D5 400, C5 400, NA 200)");
  Serial.println("Or enter: PLAY SONG #\n(Example: play song 1 for Hot Cross Buns)");
}

void loop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    input.toUpperCase();

    if (input.startsWith("PLAY SONG")) {
      input.replace("PLAY SONG", "");
      input.trim();
      int songNumber = input.toInt();
      playSongByNumber(songNumber);
    }
    else {
      playNoteSequence(input);
    }
  }
}
