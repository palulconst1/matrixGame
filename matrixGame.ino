#include "LedControl.h" //  need the library
#include<LiquidCrystal.h>
#include<EEPROM.h>

struct Poz
{
  byte col;
  byte lin;
};
typedef struct Poz poz;

#define PLAY 0
#define OPTIONS 1
#define HIGHSCORE_MENU 2
#define about 3
#define MENU 4 
#define GAME_PREPARATION 5
#define GAME_PLAY 6
#define NEW_HIGHSCORE 7
#define FINAL_SCREEN 8
#define END_GAME 9

#define LEFT 1
#define RIGHT 2
#define UP 3
#define DOWN 4

#define CHANGE_MATRIXBR 0
#define CHANGE_LCDBR 1
#define CHANGE_LCDCONTRAST 2
#define CHANGE_DIF 3 
#define CHANGE_NAME 4
#define BACK 5

#define MIN_DIF 0
#define MAX_DIF 3

#define CHANGE_BRIGHTNESS 0

int dificulty = 0;
String gitLink = String("https://github.com/palulconst1/matrixGame");
long score = 1;

const int xPin = A0; 
const int yPin = A1;
const int swPin = 13;
bool joyMoved = LOW;


const int dinPin = 12;
const int clockPin = 11;
const int loadPin = 10;
const int rows = 8;
const int cols = 8;
LedControl lc = LedControl(dinPin, clockPin, loadPin, 2); //DIN, CLK, LOAD, No. DRIVER
 
// pin 12 is connected to the MAX7219 pin 1
// pin 11 is connected to the CLK pin 13
// pin 10 is connected to LOAD pin 12
// 1 as we are only using 1 MAX7219


const int RS = 9;
const int enable = 8;
const int contrastPin = 6;

const int d4 = 5;
const int d5 = 4;
const int d6 = 3;
const int d7 = 2;

int distance;
int reading;
long duration;;

poz cursorOptionsMenu[4] = { {0, 0}, {7, 0}, {0, 1}, {10, 1 } };
poz cursorOptionsSettings[6] = { {0, 0}, {6, 0}, {11, 0}, {0, 1}, {6, 1}, {13, 1 } };
byte cursorActualPoz = 0;
LiquidCrystal lcd(RS, enable, d4, d5, d6, d7);
byte lcdBrightness = 9;
byte lcdContrast = 9;

int incomingByte;
char incomingChar;
String incomingString;
byte lcdState = MENU;

byte matrixBrightness = 2;
const byte matrixSize = 8;
bool matrixChanged = true;

bool matrix[matrixSize][matrixSize] = {
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0}  
};

byte matrixByte[matrixSize] = {
  B00000000,
  B01000100,
  B00101000,
  B00010000,
  B00010000,
  B00010000,
  B00000000,
  B00000000
};

byte xPos = 0;
byte yPos = 0;
byte xLastPos = 0;
byte yLastPos = 0;
bool valSW = 0;

long lastBounce = 0;
const int bounceDelay = 200;

const int minThreshold = 200;
const int maxThreshold = 700;

const byte moveInterval = 100;
unsigned long long lastMoved = 0;

long aboutStartTime = 0;
long aboutTimer = 0;
int aboutIndex = 0;
byte aboutState = 0;

byte arrow[8] = {
  B10000,
  B01000,
  B00100,
  B00010,
  B00100,
  B01000,
  B10000,
};



bool joyMovedXMenu = false;
bool joyMovedY = false;
String username = "PLR";
String usernameHighScore = "PLR";
const unsigned int noOfUsernamePos = 3;
int currentStringPos = 0;
bool settingName = LOW;
int contrast = 100;
bool contrastChanged = LOW;
 
void setup()
{
  // the zero refers to the MAX7219 number, it is zero for 1 chip
  lc.shutdown(0, false); // turn off power saving, enables display
  lc.setIntensity(0, matrixBrightness); // sets matrixBrightness (0~15 possible values)
  lc.clearDisplay(0);// clear screen

  pinMode(xPin, INPUT);
  pinMode(yPin, INPUT);
  pinMode(swPin, INPUT_PULLUP);
  pinMode(contrastPin, OUTPUT);

  matrix[xPos][yPos] = 1;

  lcd.createChar(0, arrow);
  lcd.begin(16, 2);
  long startingMsgTimer = millis();
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Hello fellow");
  lcd.setCursor(4, 1);
  lcd.print("Gamer!");
   
  Serial.begin(9600);

  while (millis() - startingMsgTimer < 2500);

  lcd.clear();
  
}

void loop() {
  if(!contrastChanged) {
    analogWrite(contrastPin, contrast);
  }
//  displayNewHighscore
  switch (lcdState)
  {
  case MENU:
    displayMenu();
    break;

  case PLAY:
    play();
    break;

  case END_GAME:
    displayEndGame();
    break;

  case OPTIONS:
    displayOptions();
    break;

  case HIGHSCORE_MENU:
    displayHighScore();
    break;

  case GAME_PREPARATION:
    displayGamePreparation();
    break;

  case GAME_PLAY:
    displayGamePlay();
    break;

  case NEW_HIGHSCORE:
    displayNewHighscore();
    break;

  case FINAL_SCREEN:
    displayFinalScreen();
    break;
  case about:
    displayAbout();
    break;
  }
}

void updateMatrix() {
  for (int row = 0; row < matrixSize; row++) {
    for (int col = 0; col < matrixSize; col++) {
      lc.setLed(0, row, col, matrix[row][col]);
    }
  }
}

void updateByteMatrix() {
  for (int row = 0; row < matrixSize; row++) {
    lc.setRow(0, row, matrixByte[row]);
  }
}

byte updatePositions() {
  int xValue = analogRead(xPin);
  int yValue = analogRead(yPin);

  xLastPos = xPos;
  yLastPos = yPos;
  
  if (xValue < minThreshold) {
    if (xPos < matrixSize - 1) {
      xPos++;
      return RIGHT;
    } 
    else {
      xPos = 0;
    }
  }

  if (xValue > maxThreshold) {
    if (xPos > 0) {
      xPos--;
      return LEFT;
    }
    else {
      xPos = matrixSize - 1;
    }
  }

  if (yValue > maxThreshold) {
    if (yPos < matrixSize - 1) {
      yPos++;
      return UP;
    } 
    else {
      yPos = 0;
    }
  }

  if (yValue < minThreshold) {
    if (yPos > 0) {
      yPos--;
      return DOWN;
    }
    else {
      yPos = matrixSize - 1;
    }
  }

  if(xPos != xLastPos || yPos != yLastPos) {
    matrixChanged = true;
    matrix[xLastPos][yLastPos] = 0;
    matrix[xPos][yPos] = 1;
  }
  return 0;
}

byte updatePositionsLcd() {
  int xValue = analogRead(xPin);
  int yValue = analogRead(yPin);

  if(xValue > minThreshold && xValue < maxThreshold && yValue > minThreshold && yValue < maxThreshold) {
    joyMoved = LOW;
  }

  if(joyMoved == LOW) {
    if (xValue < minThreshold) {
      joyMoved = HIGH;
      return RIGHT;
    }
  
    if (xValue > maxThreshold) {
      joyMoved = HIGH;
      return LEFT;
    }
  
    if (yValue > maxThreshold) {
      joyMoved = HIGH;
      return UP;
    }
  
    if (yValue < minThreshold) {
        joyMoved = HIGH;
        return DOWN;
    }
  }
  return 0;
}

byte buttonPressed(){
  if(millis() - lastBounce > bounceDelay) {
    valSW = digitalRead(swPin);
    if(valSW == 0) {
      lastBounce = millis();
    return 1;
    }
  }
  return 0;
}

void setupName() {
  while(settingName) {
  lcd.setCursor(7, 1);//print dificulty
  lcd.print("N:");
  lcd.print(username); 
  
  lcd.setCursor(currentStringPos + 9, 0);//print dificult
  lcd.write(byte(0));
  
  if(buttonPressed()) {
    settingName = LOW;
  }
   byte joyMove = updatePositionsLcd();
  //modify the letter
  if (joyMove == 0) {
      joyMovedXMenu = false;
      joyMovedY = false;
  }

  if (joyMove == LEFT && joyMovedXMenu == false) {
    if(currentStringPos > 0)
      currentStringPos --;
    else currentStringPos = noOfUsernamePos - 1;
    joyMovedXMenu = true;
    lcd.clear();
  }

  if (joyMove == RIGHT && joyMovedXMenu == false) {
    if(currentStringPos < noOfUsernamePos - 1)
      currentStringPos ++;
    else currentStringPos = 0;
    joyMovedXMenu = true;
    lcd.clear();
  }

  if (joyMove == DOWN && joyMovedY == false) {
    if(username[currentStringPos] >= 'A' && username[currentStringPos] < 'Z')
      username[currentStringPos] ++;
    else if(username[currentStringPos] == 'Z')username[currentStringPos] = ' '; 
    else if(username[currentStringPos] == ' ')username[currentStringPos] = 'A';
    lcd.clear();
    joyMovedY = true;
  }

  if (joyMove == UP && joyMovedY == false) {
    if(username[currentStringPos]  > 'A' && username[currentStringPos] <= 'Z' )
      username[currentStringPos] --;
    else if(username[currentStringPos] == 'A')username[currentStringPos] = ' '; 
    else if(username[currentStringPos] == ' ')username[currentStringPos] = 'Z';
    lcd.clear();
    joyMovedY = true;
  }
 }
 lcd.clear();
}

void displayMenu()
{

  lcd.setCursor(1, 0);
  lcd.print("PLAY");
  lcd.setCursor(8, 0);
  lcd.print("SETTINGS");
  lcd.setCursor(1, 1);
  lcd.print("HIGHSCORE");
  lcd.setCursor(11, 1);
  lcd.print("ABOUT");

  lcd.setCursor(cursorOptionsMenu[cursorActualPoz].col, cursorOptionsMenu[cursorActualPoz].lin);
  lcd.write(byte(0));



  //change menu selection
  byte cursorMove = updatePositionsLcd();
  if (cursorMove == LEFT)
  {
    if (cursorActualPoz == 0)
      cursorActualPoz = 3;
    else
      cursorActualPoz = cursorActualPoz - 1;
    lcd.clear();
  }
  else if (cursorMove == RIGHT)
  {
    if (cursorActualPoz == 3)
      cursorActualPoz = 0;
    else
      cursorActualPoz = cursorActualPoz + 1;
    lcd.clear();
  }

  if (buttonPressed() == 1)//if joystick is pressed
  {
    lcd.clear();
    lcdState = cursorActualPoz;
    cursorActualPoz = 0;
    return;
  }
}

void play() {
  lcd.setCursor(1, 0);
  lcd.print("Level 1");
}

void displayEndGame() {
  lcd.setCursor(1, 0);
  lcd.print("game ended");
}


void displayGamePlay() {
  lcd.setCursor(1, 0);
  lcd.print("game ended");
}

void displayOptions()
{
  lcd.setCursor(1, 0);//print dificulty
  lcd.print("BM:");
  lcd.print(matrixBrightness);

  lcd.setCursor(7, 0);//print dificulty
  lcd.print("BL:");
  lcd.print(lcdBrightness);

  lcd.setCursor(12, 0);//print dificulty
  lcd.print("CL:");
  lcd.print(lcdContrast);
  
  lcd.setCursor(1, 1);//print dificulty
  lcd.print("DIF:");
  lcd.print(dificulty);

  lcd.setCursor(7, 1);//print dificulty
  lcd.print("N:");
  lcd.print(username);

  lcd.setCursor(14, 1);//print exit option
  lcd.print("EX");

  if (settingName == LOW) {
  //print cursor
  lcd.setCursor(cursorOptionsSettings[cursorActualPoz].col, cursorOptionsSettings[cursorActualPoz].lin);
  lcd.write(byte(0));

  //change selected field
  int cursorMove = updatePositionsLcd();
  if (cursorMove == LEFT)
  {
    if (cursorActualPoz == 0)
      cursorActualPoz = 5;
    else 
      cursorActualPoz -= 1;
    lcd.clear();
  }
  if (cursorMove == RIGHT)
  {
    if (cursorActualPoz == 5)
      cursorActualPoz = 0;
    else
      cursorActualPoz += 1;
    lcd.clear();
  }

    if (cursorActualPoz == BACK && buttonPressed())
    {
      lcdState = MENU;
      lcd.clear();
      cursorActualPoz = 0;
      return;
    }

  //change dificulty if selected
  if (cursorActualPoz == CHANGE_DIF)
  {
    if (cursorMove == UP)
    {
      if (dificulty == MAX_DIF)
        dificulty = MIN_DIF;
      else
        dificulty = dificulty + 1;
      lcd.clear();
    }
    else if (cursorMove == DOWN)
    {
      if (dificulty == MIN_DIF)
        dificulty = MAX_DIF;
      else
        dificulty = dificulty - 1;
      lcd.clear();
    }
  }

  //change matrixBrightness if selected
  if (cursorActualPoz == CHANGE_MATRIXBR)
  {
    if (cursorMove == UP) {
      if (matrixBrightness == 15)
        matrixBrightness = 0;
      else
        matrixBrightness = matrixBrightness + 1;
      lcd.clear();
    }
    else if (cursorMove == DOWN)
    {
      if (matrixBrightness == 0)
        matrixBrightness = 15;
      else
        matrixBrightness = matrixBrightness - 1;
      lcd.clear();
    }
  }

  if (cursorActualPoz == CHANGE_LCDBR) {
    if (cursorMove == UP)
    {
      contrastChanged = HIGH;
      if (lcdBrightness == 9)
        lcdBrightness = 0;
      else
        lcdBrightness = lcdBrightness + 1;
      lcd.clear();
    }
    else if (cursorMove == DOWN)
    {
      contrastChanged = HIGH;
      if (lcdBrightness == 0)
        lcdBrightness = 9;
      else
        lcdBrightness = lcdBrightness - 1;
      lcd.clear();
    }
  }

  if (cursorActualPoz == CHANGE_LCDCONTRAST) {
    if (cursorMove == UP)
    {
      if (lcdContrast == 9)
        lcdContrast = 0;
      else
        lcdContrast = lcdContrast + 1;
      lcd.clear();
    }
    else if (cursorMove == DOWN)
    {
      if (lcdContrast == 0)
        lcdContrast = 9;
      else
        lcdContrast = lcdContrast - 1;
      lcd.clear();
    }
  }

  if(contrastChanged) {
  contrast = (lcdContrast + 1) * 20;
  analogWrite(contrastPin, contrast);
  }

  if (cursorActualPoz == CHANGE_NAME && buttonPressed()) {
    settingName = HIGH;
    lcd.clear();
    setupName();
  }
  }
}

void displayAbout() {
  if (aboutStartTime == 0)//when opening about screen
  {
    aboutStartTime = millis();
    aboutTimer = aboutStartTime;
    aboutIndex = 0;
    lcd.setCursor(0, 1);
    for (int i = 0; i < 16; i++)
      lcd.print(gitLink[i]);
  }
  switch(aboutState)
  {
  case 0:
    lcd.setCursor(0, 0);
    lcd.print("Nume Joc");
    break;
  case 1:
    lcd.setCursor(0, 0);
    lcd.print("Constantinescu");
    break;
  case 2:
    lcd.setCursor(0, 0);
    lcd.print("Paul");
    break;
  }

  if (millis() - aboutTimer > 1500)//change what is printed on the upper row
  {
    if (aboutState == 2)
      aboutState = 0;
    else
      aboutState += 1;
    
    aboutTimer = millis();
    lcd.clear();
  }

  if (millis() - aboutStartTime > 500) {

    aboutIndex += 1;
    aboutStartTime = millis();
    lcd.setCursor(0, 1);
    for (int i = aboutIndex; i < aboutIndex + 16; i++)
      lcd.print(gitLink[i]);
  }
  if (aboutIndex == strlen("https://github.com/palulconst1/matrixGame") - 16)
  {
    aboutIndex = 0;
    lcd.setCursor(0, 1);
    lcd.print(gitLink);
  }

  if (buttonPressed())//check if joystick is pressed
  {
    lcd.clear();
    lcdState = MENU;//go back to menu
    aboutIndex = 0;
    return;
  }
}

void displayNewHighscore() {
  long msgStartTime = millis();
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("NEW HIGHSCORE");
  lcd.setCursor(6, 1);
  lcd.print(score);
  writeEEPROM(score);
  usernameHighScore = username;
  lcd.setCursor(9, 1);
  lcd.print(usernameHighScore);
  writeStringEEPROM(usernameHighScore);

  lcdState = FINAL_SCREEN;
  while (millis() - msgStartTime < 5000);
}

void displayHighScore() {
  long highscore;
  readEEPROM(highscore);
  readStringEEPROM(usernameHighScore);
  
  lcd.setCursor(0, 0);
  lcd.print("HIGHSCORE:");
  lcd.print(highscore);

  lcd.setCursor(0, 1);
  lcd.print("NAME:");
  lcd.print(usernameHighScore);

  if (buttonPressed()){
    lcdState = MENU;//go back to main menu
    lcd.clear();
    return;
  }
}

 void displayGamePreparation() {
  lcd.setCursor(1, 0);
  lcd.print("Preparing");
}

 void displayFinalScreen() {
  lcd.setCursor(1, 0);
  lcd.print("Final Screen");
}

void writeEEPROM(long &x) {
  byte *v = (byte*)(void*)&x;
  int addr = 0;
  for (int i = 0; i < sizeof(x); i++)
    EEPROM.update(addr++, *v++);

}

void writeStringEEPROM(String &x) {
  byte *v = (byte*)(void*)&x;
  int addr = 0;
  for (int i = 0; i < sizeof(x); i++)
    EEPROM.update(addr++, *v++);

}

//read value from eeprom into x
void readEEPROM(long &x) {
  byte *v = (byte*)(void*)&x;
  int addr = 0;
  for (int i = 0; i < sizeof(x); i++)
    *v++ = EEPROM.read(addr++);
}

void readStringEEPROM(String &x) {
  byte *v = (byte*)(void*)&x;
  int addr = 0;
  for (int i = 0; i < sizeof(x); i++)
    *v++ = EEPROM.read(addr++);
}
