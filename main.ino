#include <Bounce2.h>

// Notes frequencies
int C = 262;
int D = 294;
int E = 330;
int F = 349;
int A = 444;

// Pins Leds & Notes correspondance
int leds[] = { 4,5,6,7 };
int ledsNotes[] = { A, D, E, F };
int buzzerPin = 3;

// input btn
int blueButtonPin = 8;
int yellowButtonPin = 9;
int greenButtonPin = 10;
int redButtonPin = 11;
Bounce redBtn = Bounce();
Bounce yellowBtn = Bounce();
Bounce greenBtn = Bounce();
Bounce blueBtn = Bounce();

// Intro tone
int minorswing[] = {A, C, E, D, F, A};
int minorswingDurations[] = {250, 250, 500, 250, 100, 500};
int minorswingLeds[][4] = {{4,5,6,7}, {4},{5},{6},{7}, {4,5,6,7}};
int minorswingSize = sizeof(minorswing)/sizeof(minorswing[0]);

// Victory tone
int victory[] = {A, E, A, E};
int victoryDurations[] = {250, 100, 100, 500};
int victoryLeds[][4] = {{4,5,6,7}, {4,5,6,7}, {4,5,6,7}, {4,5,6,7}};
int victorySize = sizeof(victory)/sizeof(victory[0]);

// Error tone
int error[] = {C, C, C, C, C, C, C, C};
int errorDurations[] = {50, 100, 50,100, 50, 100, 50, 100};
int errorLeds[][4] = {{7},{7},{7},{7},{7},{7}};
int errorSize = sizeof(error)/sizeof(error[0]);

int maxLevel = 99;
int randomArray[99]; 
int inputArray[99];

int runIntro = 1;
int level = 1;
int breakLoop = 0;
int difficulty = 1;
int withSound = 1;

void setup() {
  // setup leds
  for(int i=0;i<4;i++) {
    pinMode(leds[i], OUTPUT);
  }

  // setup input Buttons
  redBtn.attach(redButtonPin,INPUT_PULLUP);
  redBtn.interval(5); 
  greenBtn.attach(greenButtonPin,INPUT_PULLUP);
  greenBtn.interval(5); 
  yellowBtn.attach(yellowButtonPin,INPUT_PULLUP);
  yellowBtn.interval(5); 
  blueBtn.attach(blueButtonPin,INPUT_PULLUP);
  blueBtn.interval(5); 

  // setup random sequence
  randomSeed(analogRead(0));
  for(int i=0;i<maxLevel;i++){
      int randNbr = (random() % 4);
      randomArray[i] = randNbr;
  } 
}

void loop() {
   
    if(runIntro) {
      delay(200);
      playSong(minorswing, minorswingDurations, minorswingLeds, minorswingSize);
      delay(1500);
      runIntro = 0;
    }

    // Questions
    for(int i=0;i<maxLevel;i++){
      for(int ii=0;ii<level;ii++){
        int index = randomArray[ii];
        int ledSeq[4] = {leds[index]};
        mute(300/difficulty);
        playNote(ledsNotes[index], 600/difficulty, ledSeq);
        mute(1);
      }
     
      // Answer
      input();

      level += 1;
    }
}

void playSong(int song[], int durations[], int ledsSequence[][4], size_t songSize) {
  for(int i = 0; i<songSize; i++)
  {
    playNote(song[i], durations[i], ledsSequence[i]);
    mute(50);
  }
}

void playNote(int freq, int duration, int ledsSequence[4]) {
  if(withSound) tone(buzzerPin,freq);
  for(int i = 0; i<4; i++){
    if(ledsSequence[i]!=0) {
      digitalWrite(ledsSequence[i], HIGH);
    }
  }
  delay(duration);
}

void mute(int duration) {
  noTone(buzzerPin);
  for(int i=0;i<4;i++) {
    digitalWrite(leds[i], LOW);
  }
  delay(duration);
}

void input() {
  int winThisLevel = 0;
  int count = 0;
  while(count < level) {
      
      if(digitalRead(redButtonPin) == HIGH) {
          if(withSound) tone(buzzerPin,ledsNotes[3], 50);
          digitalWrite(leds[3], HIGH);
      }else {
           digitalWrite(leds[3], LOW);
      }
      if(digitalRead(greenButtonPin) == HIGH) {
          if(withSound) tone(buzzerPin,ledsNotes[2], 50);
          digitalWrite(leds[2], HIGH);
      }else {
           digitalWrite(leds[2], LOW);
      }
      if(digitalRead(blueButtonPin) == HIGH) {
          if(withSound) tone(buzzerPin,ledsNotes[0], 50);
          digitalWrite(leds[0], HIGH);
      }else {
           digitalWrite(leds[0], LOW);
      }
      if(digitalRead(yellowButtonPin) == HIGH) {
          if(withSound) tone(buzzerPin,ledsNotes[1], 50);
          digitalWrite(leds[1], HIGH);
      }else {
           digitalWrite(leds[1], LOW);
      }

      redBtn.update();
      if (redBtn.fell()) {
          inputArray[count] = 3;
          count++;
      }
       greenBtn.update();
      if (greenBtn.fell()) {
          inputArray[count] = 2;
          count++;
      }
      blueBtn.update();
      if (blueBtn.fell()) {
          inputArray[count] = 0;
          count++;
      }
      yellowBtn.update();
      if (yellowBtn.fell()) {
          inputArray[count] = 1;
          count++;
      }

  }

 
  
  for(int ii=0;ii<level;ii++){
    if(inputArray[ii] != randomArray[ii]) {
      delay(150);
      playSong(error, errorDurations, errorLeds, errorSize);
      delay(2000/difficulty);
      level = 0;
      winThisLevel = 0;
    }
    else {
      winThisLevel = 1;
    }
  }

  if(winThisLevel) {
      delay(350);
      playSong(victory, victoryDurations, victoryLeds, victorySize);
      delay(1000/difficulty);
  }

}
