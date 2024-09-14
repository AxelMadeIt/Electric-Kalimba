#include <Servo.h>

// Create Servo objects
Servo ServoPluck; // Servo for plucking mechanism
Servo ServoMove; // Servo for moving to different tines

// Constants for the random song
const int NOTE_PLAY_CHANCE = 70;  // Probability (0-100) of a note being played at each interval

// Define pins for servos and buttons
const int pluckServoPin = 9;      // Pin connected to the pluck servo
const int pluckMovePin = 10;      // Pin connected to the tine-moving servo
const int buttonPins[] = {2, 3}; // Pins connected to buttons for playing predefined songs
const int ButtonRPin = 4;        // Pin connected to button for playing a random song

// Pluck positions
const int PluckUpper = 1300; // Servo position to lift the plucker above the tine
const int PluckLower = 1680; // Servo position to lower the plucker onto the tine
bool plucked = false;       // Current state of the plucker

// Tine positions (servo positions corresponding to different tines)
int tinePositions[] = {1950, 1780, 1650, 1540, 1400, 1290, 1180, 1040};

// Structure to represent a musical note
struct Note {
  int tine;         // Tine number (1-8)
  unsigned long startTime; // Time (in milliseconds) when the note should start playing
};

// Predefined songs with notes and their timings
Note song1[] = { //Vivaldi - Spring
    {1, 0},
    {3, 429},
    {3, 857},
    {3, 1286},
    {2, 1714},
    {1, 1929},
    {5, 2143},
    {5, 3429},
    {4, 3643},
    {3, 3857},
    {3, 4286},
    {3, 4714},
    {2, 5143},
    {1, 5358},
    {5, 5571},
    {5, 6857},
    {4, 7072},
    {3, 7286},
    {4, 7714},
    {5, 7929},
    {4, 8143},
    {3, 8571},
    {2, 9000},

    {1, 10286},
    {5, 10714},
    {4, 11143},
    {3, 11358},
    {4, 11571},
    {5, 12000},
    {6, 12429},
    {5, 12857},
    {1, 13714},
    {5, 14143},
    {4, 14571},
    {3, 14786},
    {4, 15000},
    {5, 15429},
    {6, 15857},
    {5, 16286},
    {4, 17143},
    {3, 17571},
    {2, 18000},
    {1, 18215},
    {2, 18429},
    {1, 19072},
    {1, 19286},
};

Note song2[] = { //Beethoven - Ode to Joy
    {3, 0},
    {3, 429},
    {4, 857},
    {5, 1286},
    {5, 1714},
    {4, 2143},
    {3, 2571},
    {2, 3000},

    {1, 3429},
    {1, 3857},
    {2, 4286},
    {3, 4714},
    {3, 5143},
    {2, 5786},
    {2, 6000},

    {3, 6857},
    {3, 7286},
    {4, 7714},
    {5, 8143},
    {5, 8571},
    {4, 9000},
    {3, 9429},
    {2, 9857},

    {1, 10286},
    {1, 10929},
    {2, 11143},
    {3, 11571},
    {2, 12000},
    {1, 12643},
    {1, 12857},
};

// Array of songs and their lengths
Note *songs[] = {song1, song2};
int songLengths[] = {sizeof(song1) / sizeof(Note), sizeof(song2) / sizeof(Note)};

// Setup function runs once when the Arduino is powered on or reset
void setup() {
  // Initialize Serial communication
  Serial.begin(9600);

  // Attach servos to their respective pins
  ServoPluck.attach(pluckServoPin);
  ServoMove.attach(pluckMovePin);

  // Move the pluck servo to the upper position initially
  ServoPluck.writeMicroseconds(PluckUpper);
  plucked = false; // Ensure the plucker state is updated

  // Initialize button pins as input with internal pull-up resistors
  pinMode(buttonPins[0], INPUT_PULLUP);
  pinMode(buttonPins[1], INPUT_PULLUP);
  pinMode(ButtonRPin, INPUT_PULLUP);

  // Seed the random number generator with a value from analog pin A0
  randomSeed(analogRead(A0));
}

// Main loop function runs repeatedly
void loop() {
  // Check if any of the predefined song buttons are pressed
  for (int i = 0; i < 2; i++) {
    if (digitalRead(buttonPins[i]) == LOW) {
      playSong(i); // Play the selected song
    }
  }

  // Check if the random song button is pressed
  if (digitalRead(ButtonRPin) == LOW) {
    playRandom(); // Play a random song
  }
}

// Function to play a random sequence of notes
void playRandom() {
  const int numNotes = 16;  // Number of possible notes in a random song
  const int interval = 500; // Interval between notes in milliseconds

  unsigned long startTime = millis(); // Record the start time

  for (int i = 0; i < numNotes; i++) {
    int playNote = random(100); // Generate a random number between 0 and 99
    int randomTine = random(1, 9); // Pick a random tine (1 to 8)

    if (playNote < NOTE_PLAY_CHANCE) { // Check if the note should be played
      while (millis() - startTime < i * interval) {
        // Wait until it's time to play the note
      }
      playNoteRandom(randomTine); // Play the random note
    }
  }
}

// Function to move to a specific tine and play a note
void playNoteRandom(int tine) {
  if (tine < 1 || tine > 8) return; // Ensure the tine number is valid

  // Move the servo to the selected tine
  ServoMove.write(tinePositions[tine - 1]);
  delay(100); // Wait for the servo to move into position

  pluck(); // Trigger the plucker
}

// Function to play a predefined song
void playSong(int songIndex) {
  if (songIndex >= sizeof(songs) / sizeof(songs[0])) return; // Check if the song index is valid

  Note *song = songs[songIndex];  // Get the selected song
  int length = songLengths[songIndex]; // Get the length of the song

  unsigned long startTime = millis(); // Record the start time

  for (int i = 0; i < length; i++) {
    while (millis() - startTime < song[i].startTime) {
      // Wait until it's time to play the note
    }
    playNote(song[i].tine); // Play the note
  }
}

// Function to move to a specific tine and play a note
void playNote(int tine) {
  if (tine < 1 || tine > 8) return; // Ensure the tine number is valid

  // Move the servo to the selected tine
  ServoMove.write(tinePositions[tine - 1]);
  delay(150); // Wait for the servo to move into position

  pluck(); // Trigger the plucker
}

// Function to trigger the plucker
void pluck() {
  if (plucked == false) {
    ServoPluck.writeMicroseconds(PluckLower);
    plucked = true;
  } else {
    ServoPluck.writeMicroseconds(PluckUpper);
    plucked = false;
  }
}
