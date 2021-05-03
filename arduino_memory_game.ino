#include <Bounce2.h>
#include <U8g2lib.h>
#include <EEPROM.h>

//Initialize display.
U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, A5, A4);

// Notes frequencies
int C = 262;
int D = 294;
int E = 330;
int F = 349;
int A = 444;

// Pins Leds & Notes correspondance
int leds[] = {4, 5, 6, 7};
int ledsNotes[] = {A, D, E, F};
int buzzerPin = 2;

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
int minorswingLeds[][4] = {{4, 5, 6, 7}, {4}, {5}, {6}, {7}, {4, 5, 6, 7}};
int minorswingSize = sizeof(minorswing) / sizeof(minorswing[0]);

// Victory tone
int victory[] = {A, E, A, E};
int victoryDurations[] = {250, 100, 100, 500};
int victoryLeds[][4] = {{4, 5, 6, 7}, {4, 5, 6, 7}, {4, 5, 6, 7}, {4, 5, 6, 7}};
int victorySize = sizeof(victory) / sizeof(victory[0]);

// Error tone
int error[] = {C, C, C, C, C, C, C, C};
int errorDurations[] = {50, 100, 50, 100, 50, 100, 50, 100};
int errorLeds[][4] = {{7}, {7}, {7}, {7}, {7}, {7}};
int errorSize = sizeof(error) / sizeof(error[0]);

int maxLevel = 99;
int randomArray[99];
int inputArray[99];

int runIntro = 1;
int level = 1;
int breakLoop = 0;
int difficulty = 1;
int withSound = 1;
int bestScore = 0;

void drawIntro()
{
  u8g2.setFont(u8g_font_helvB12);
  u8g2.drawStr(10,38,"Memory Game");

  u8g2.drawRFrame(0,0, 128, 64, 10);
}

void draw(void)
{
  u8g2.setFont(u8g_font_helvB12);
  u8g2.drawStr(10,22,"Memory Game");
  u8g2.setFont(u8g_font_helvR08);
  u8g2.setCursor(10, 38);
  u8g2.print("Best score : ");
  u8g2.setCursor(90, 38);
  u8g2.print(bestScore);
  u8g2.setFont(u8g_font_helvR10);
  u8g2.setCursor(10, 54);
  u8g2.print("Score : ");
  u8g2.setCursor(90, 54);
  u8g2.print(level-1);

  u8g2.drawRFrame(0,0, 128, 64, 4);
}

void setup(void)
{
  EEPROM.get(0,bestScore);
  u8g2.begin();
  
  // setup leds
  for (int i = 0; i < 4; i++)
  {
    pinMode(leds[i], OUTPUT);
  }

  // setup input Buttons
  redBtn.attach(redButtonPin, INPUT_PULLUP);
  redBtn.interval(5);
  greenBtn.attach(greenButtonPin, INPUT_PULLUP);
  greenBtn.interval(5);
  yellowBtn.attach(yellowButtonPin, INPUT_PULLUP);
  yellowBtn.interval(5);
  blueBtn.attach(blueButtonPin, INPUT_PULLUP);
  blueBtn.interval(5);

  // setup random sequence
  randomSeed(analogRead(0));
  for (int i = 0; i < maxLevel; i++)
  {
    int randNbr = (random() % 4);
    randomArray[i] = randNbr;
  }
}

void oledLoop(int intro) {     
  u8g2.firstPage();  
  do {
    if(intro)
      drawIntro();
    else 
      draw();
  }
  while (u8g2.nextPage());
}

void loop(void)
{   
  
  oledLoop(1);
  
  if (runIntro)
  {
    delay(200);
    playSong(minorswing, minorswingDurations, minorswingLeds, minorswingSize);
    delay(1500);
    runIntro = 0;
  }

  // Questions
  for (int i = 0; i < maxLevel; i++)
  {
    oledLoop(0);
    for (int ii = 0; ii < level; ii++)
    {
      int index = randomArray[ii];
      int ledSeq[4] = {leds[index]};
      mute(300 / difficulty);
      playNote(ledsNotes[index], 600 / difficulty, ledSeq);
      mute(1);
    }

    // Answer
    input();

    if(level > bestScore)
      EEPROM.put(0,level);

    level += 1;
  }


}

void playSong(int song[], int durations[], int ledsSequence[][4], size_t songSize)
{
  for (int i = 0; i < songSize; i++)
  {
    playNote(song[i], durations[i], ledsSequence[i]);
    mute(50);
  }
}

void playNote(int freq, int duration, int ledsSequence[4])
{
  if (withSound)
    tone(buzzerPin, freq);

  for (int i = 0; i < 4; i++)
  {
    if (ledsSequence[i] != 0)
    {
      digitalWrite(ledsSequence[i], HIGH);
    }
  }
  delay(duration);
}

void mute(int duration)
{
  noTone(buzzerPin);
  for (int i = 0; i < 4; i++)
  {
    digitalWrite(leds[i], LOW);
  }
  delay(duration);
}

void playNoteIfKeyDown()
{
  if (digitalRead(redButtonPin) == LOW)
  {
    if (withSound)
      tone(buzzerPin, ledsNotes[3]);
    digitalWrite(leds[3], HIGH);
  }
  else
  {
    digitalWrite(leds[3], LOW);
  }
  if (digitalRead(greenButtonPin) == LOW)
  {
    if (withSound)
      tone(buzzerPin, ledsNotes[2]);
    digitalWrite(leds[2], HIGH);
  }
  else
  {
    digitalWrite(leds[2], LOW);
  }
  if (digitalRead(blueButtonPin) == LOW)
  {
    if (withSound)
      tone(buzzerPin, ledsNotes[0]);
    digitalWrite(leds[0], HIGH);
  }
  else
  {
    digitalWrite(leds[0], LOW);
  }
  if (digitalRead(yellowButtonPin) == LOW)
  {
    if (withSound)
      tone(buzzerPin, ledsNotes[1]);
    digitalWrite(leds[1], HIGH);
  }
  else
  {
    digitalWrite(leds[1], LOW);
  }
}

void input()
{
  int winThisLevel = 0;
  int count = 0;
  while (count < level)
  {

    playNoteIfKeyDown();

    redBtn.update();
    if (redBtn.rose())
    {
      inputArray[count] = 3;
      count++;
      mute(1);
    }
    greenBtn.update();
    if (greenBtn.rose())
    {
      inputArray[count] = 2;
      count++;
      mute(1);
    }
    blueBtn.update();
    if (blueBtn.rose())
    {
      inputArray[count] = 0;
      count++;
      mute(1);
    }
    yellowBtn.update();
    if (yellowBtn.rose())
    {
      inputArray[count] = 1;
      count++;
      mute(1);
    }
  }

  for (int ii = 0; ii < level; ii++)
  {
    if (inputArray[ii] != randomArray[ii])
    {
      delay(150);
      playSong(error, errorDurations, errorLeds, errorSize);
      delay(1200 / difficulty);
      level = 0;
      winThisLevel = 0;
      EEPROM.get(0,bestScore);
    }
    else
    {
      winThisLevel = 1;
    }
  }

  if (winThisLevel)
  {
    delay(350);
    playSong(victory, victoryDurations, victoryLeds, victorySize);
    delay(800 / difficulty);
  }
}
