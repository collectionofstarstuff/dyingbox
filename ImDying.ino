/*
 * "I'm Dying"
 * Owen Monsma & Blaine Lewis
 * 27 September 2017
 * 
 * Portions of code adapted from Teensy Audio and SD examples
 */

#include <Audio.h>
#include <Wire.h>
#include <SD.h>
#include <SPI.h>
#include <SerialFlash.h>
#include <Bounce.h>

#define SDCARD_CS_PIN BUILTIN_SDCARD
#define BIG_RED_BUTTON 5
#define OPEN_SWITCH 4
#define LIGHT 6
#define AUDIO_DIR "files/press/"
#define OPEN_DIR "files/open/"

AudioPlaySdWav playWav1;
AudioOutputAnalog audioOutput;
AudioConnection patchCord1(playWav1, 0, audioOutput, 0);
AudioConnection patchCord2(playWav1, 1, audioOutput, 1);

Bounce bigRedButton = Bounce (BIG_RED_BUTTON, 20);
Bounce openSwitch = Bounce (OPEN_SWITCH, 20);

uint8_t fileNameArrSize = 0;
char* fileNameArr[256];

bool ledstate = HIGH;

bool diagMode = false;

void setup() {
  Serial.begin(9600);

  // Seed RNG with unconnected analog pin noise
  randomSeed(analogRead(A7));
  
  // Allocate memory for audio
  AudioMemory(8);

  // Set up SD card access
  if (!SD.begin(SDCARD_CS_PIN)){
    Serial.println("SD init failed");
    return;
  }

  // Create array of strings to keep all filenames
  fileNameArrSize = countAndSaveFiles(AUDIO_DIR);
  
  // Set up pins for buttons 
  pinMode(BIG_RED_BUTTON, INPUT_PULLUP);
  pinMode(OPEN_SWITCH, INPUT_PULLUP);
  pinMode(13, OUTPUT);
  pinMode(LIGHT, OUTPUT);

  digitalWrite(13, LOW);
  digitalWrite(LIGHT, HIGH);

  // branch to diagnostics mode if button low at end of setup
  if (digitalRead(BIG_RED_BUTTON) == LOW){
    diagMode = true;
    Serial.println("Welcome to diagnostic mode. I do nothing.");
    diag();
  } else {
    Serial.println("Booting into dyingOS...");
  }
  
} /* end setup() */

void loop() {
  // big red button
  if (bigRedButton.update()){
     if(bigRedButton.fallingEdge()){
      playFile (randFilename());
      Serial.println("Big red button pressed.");
     }
  }

  // lid opened code 
  if (openSwitch.update()){
    if (openSwitch.risingEdge()){
      // playFile (OPEN_DIR + "/OPEN.WAV");
      Serial.println("Lid opened.");
      digitalWrite(13, HIGH);
    } else {
      digitalWrite(13, LOW);
    }
  }
}

int countAndSaveFiles(const char* dirname){
  File dir = SD.open(dirname);
  int i = 0;
  while (true) {
    File f = dir.openNextFile();
    if (!f) {
      break;
    }

    Serial.print("\nChecking file ");
    Serial.println(f.name());

    fileNameArr[i] = strdup(f.name());
    f.close();
    Serial.print("Saved in array: ");
    Serial.print(fileNameArr[i]);
    Serial.print(" at index ");
    Serial.println(i);

     i++;

    Serial.println("\nThe array so far: ");
    for (int j = 0; j < i; j++){
      Serial.println(fileNameArr[j]);
    }
   
  }

  dir.close();

  Serial.print("Number of files: ");
  Serial.println(i);

  Serial.println("\nThe array:");
  for (int j = 0; j < i; j++){
    Serial.println(fileNameArr[j]);
  }
  return i;
}

char* randFilename () {
  // returns a file name from the array randomly
  // Serial.print(random(0, fileNameArrSize));
  return fileNameArr [random (0, fileNameArrSize)];
}

void blinkLight(){
  while (true){
    digitalWrite(LIGHT, HIGH);
    delay(200);
    digitalWrite(LIGHT, LOW);
    delay(200);
    if (openSwitch.update()){
      digitalWrite (LIGHT, HIGH);
      return;
    }
  }
}

void playFile(const char *filename)
{
  Serial.print("Playing file: ");

  // Start playing the file.  This sketch continues to
  // run while the file plays.
  char fullname[64];
  strcpy(fullname, AUDIO_DIR);
  strcat(fullname, filename);
  Serial.println(fullname);
  playWav1.play(fullname);

  // A brief delay for the library read WAV info
  delay(5);

  // Simply wait for the file to finish playing.
  while (playWav1.isPlaying()) {
    // uncomment these lines if you audio shield
    // has the optional volume pot soldered
    //float vol = analogRead(15);
    //vol = vol / 1024;
    // sgtl5000_1.volume(vol);
  }
  Serial.println("Done playing.");
}
