#include "LedControl.h" //  need the library
#include<LiquidCrystal.h>
#include<EEPROM.h>
#include "pitches.h"

struct Poz
{
  byte col;
  byte lin;
};
typedef struct Poz poz;

#define PLAY 0
#define OPTIONS 1
#define HIGHSCORE_MENU 2
#define ABOUT 3
#define MENU 4 
#define SELECT_SONG 5
#define NEW_HIGHSCORE 7
#define GAME_OVER 8

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

#define MIN_DIF 1
#define MAX_DIF 4

#define CHANGE_BRIGHTNESS 0

const int buzzerPin = 7;

byte dificulty;
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

poz cursorOptionsMenu[4] = { {0, 0}, {7, 0}, {0, 1}, {10, 1 } };
poz cursorOptionsSettings[6] = { {0, 0}, {6, 0}, {11, 0}, {0, 1}, {6, 1}, {13, 1 } };
byte cursorActualPoz = 0;
LiquidCrystal lcd(RS, enable, d4, d5, d6, d7);
byte lcdBrightness;
byte lcdContrast;

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

byte heartShape[] = {
  B00000,
  B01010,
  B11111,
  B11111,
  B11111,
  B01110,
  B00100,
  B00000
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
String username;

String usernameHighScore1;
unsigned int highScore1 = 0;
String usernameHighScore2;
unsigned int highScore2 = 0;
String usernameHighScore3;
unsigned int highScore3 = 0;

const unsigned int noOfUsernamePos = 3;
unsigned int currentStringPos = 0;
bool settingName = LOW;
unsigned int contrast = 100;
bool contrastChanged = LOW;


// memorii
#define currentUsername 0
#define matrixBr 0
#define lcdBr 3
#define lcdCt 6
#define hs1 10
#define hs2 20
#define hs3 30
#define hsn1 40
#define hsn2 50
#define hsn3 60
#define dificultyMem 70

poz playerPoz = {4, 0};

byte const nrOfTiles = 3;
poz tilesPoz[nrOfTiles] = { {1, 8}, {4, 8}, {7, 8} };
int activeTiles[nrOfTiles] = {0, 0, 0};
int tilesDelay[nrOfTiles] = {0, 0, 0};

int tileSpeed = 1500;
int startingTileSpeed = 1500;
unsigned long lastSpeedCheck[nrOfTiles] = {0, 0, 0};

unsigned long lastDelayCheck[nrOfTiles] = {0, 0, 0};

bool doublePress[nrOfTiles] = {0, 0, 0};
long lastDoubleBounce = 0;
const int doubleBounceDelay = 300;

int blinkRate = 100;
unsigned long lastBlinked = 0;
unsigned long lastBlinkedTile[nrOfTiles] = {0, 0, 0};

int tilesColected = 0;
byte lifes = 3;
long gameplayStarted;

long lastScoreAdd;
int scoreDelay = 300;

bool stopMusic = 0;



// muzica
int currentNote = 0;




 
void setup()
{
//  deleteHighScore();
  
  lc.shutdown(0, false);
  lc.clearDisplay(0);

  pinMode(xPin, INPUT);
  pinMode(yPin, INPUT);
  pinMode(swPin, INPUT_PULLUP);
  pinMode(contrastPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  matrix[xPos][yPos] = 1;


  //read mem
  matrixBrightness = readIntFromEEPROM(matrixBr);
  if(matrixBrightness > 15)
    matrixBrightness = matrixBrightness / 10;
  lc.setIntensity(0, matrixBrightness);

  lcdBrightness = readIntFromEEPROM(lcdBr);
  if(lcdBrightness > 10)
    lcdBrightness = lcdBrightness / 10;

  lcdContrast = readIntFromEEPROM(lcdCt);
  if(lcdContrast > 10)
    lcdContrast = lcdContrast / 10;
  contrast = (lcdContrast + 1) * 20;
  analogWrite(contrastPin, contrast);

  dificulty = readIntFromEEPROM(dificultyMem);
  if(dificulty > 10)
    dificulty = dificulty / 10;
    
  username = readStringFromEEPROM(currentUsername);
  if(username == "")
    username = "PLR";

  highScore1 = readIntFromEEPROM(hs1);
  highScore2 = readIntFromEEPROM(hs2);
  highScore3 = readIntFromEEPROM(hs3);

  usernameHighScore1 = readStringFromEEPROM(hsn1);
  usernameHighScore2 = readStringFromEEPROM(hsn2);
  usernameHighScore3 = readStringFromEEPROM(hsn3);

    

  lcd.createChar(0, arrow);
  lcd.createChar(1, heartShape);
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
  if(!stopMusic)
    music();
  switch (lcdState)
  {
  case MENU:
    displayMenu();
    break;

  case PLAY:
    play();
    break;

  case OPTIONS:
    displayOptions();
    break;

  case HIGHSCORE_MENU:
    displayHighScore();
    break;

  case SELECT_SONG:
    selectSong();
    break;

  case NEW_HIGHSCORE:
    displayNewHighscore();
    break;

  case GAME_OVER:
    displayGameOver();
    break;
  case ABOUT:
    displayAbout();
    break;
  }
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

byte playerMove() {
  int xValue = analogRead(xPin);
  int yValue = analogRead(yPin);

  if(xValue > minThreshold && xValue < maxThreshold && yValue > minThreshold && yValue < maxThreshold) {
    joyMoved = LOW;
  }

  if(joyMoved == LOW) {
    if (xValue < minThreshold) {
      if(playerPoz.col == matrixSize - 2) {
        return 0;
      }
      joyMoved = HIGH;
      return RIGHT;
    }
  
    if (xValue > maxThreshold) {
      if(playerPoz.col == matrixSize - 7) {
        return 0;
      }
      joyMoved = HIGH;
      return LEFT;
    }
  
    if (yValue > maxThreshold) {
      if(playerPoz.lin == matrixSize - 3) {
        return 0;
      }
      joyMoved = HIGH;
      return UP;
    }
  
    if (yValue < minThreshold) {
      if(playerPoz.lin == matrixSize - 8) {
        return 0;
      }
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
  lcd.setCursor(7, 1);
  lcd.print("N:");
  lcd.print(username); 
  
  lcd.setCursor(currentStringPos + 9, 0);
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

  if (buttonPressed() == 1)
  {
    lcd.clear();
    lcdState = cursorActualPoz;
    cursorActualPoz = 0;

    
    lastDelayCheck[0] = millis();
    lastDelayCheck[1] = millis();
    lastDelayCheck[2] = millis();
    score = 0;
    gameplayStarted = millis();
    lifes = 3;
    tileSpeed = startingTileSpeed - dificulty*100;
    tilesColected = 0;
    lastScoreAdd = millis();
    playerPoz = {4, 0};
    tilesPoz[0] = {1, 8};
    tilesPoz[1] = {4, 8};
    tilesPoz[2] = {7, 8};
    stopMusic = 0;
    currentNote = 0;
    
      
    return;
  }
}

void play() {

  playMenu();
  if(millis() - lastScoreAdd > scoreDelay) {
  score = score + 1 * (2001 - tileSpeed)/500;
  lastScoreAdd = millis();
  }

  tileSpeed = startingTileSpeed - tilesColected * 20;

  if(tilesColected > 0 && tilesColected % 10 == 0){
    score = score + dificulty * 15;
  }
  
  if(lifes <= 0) {
    stopMusic = 1;
    lc.clearDisplay(0);
    lcd.clear();
    if(score > highScore3)
      lcdState = NEW_HIGHSCORE;
    else
      lcdState = GAME_OVER;
    return;
  }
  byte moved = playerMove();
  switch (moved)
  {
  case LEFT:
    ClearDisplayPlayer();
    playerPoz.col --;
    break;

  case RIGHT:
    ClearDisplayPlayer();
    playerPoz.col ++;
    break;

  case UP:
    ClearDisplayPlayer();
    playerPoz.lin ++;
    break;

  case DOWN:
    ClearDisplayPlayer();
    playerPoz.lin --;
    break;
  }

  displayTiles();
  DisplayPlayer();

  

  if(buttonPressed()) {
    lastDoubleBounce = millis();
    byte col = playerPoz.col;
    byte lin = playerPoz.lin + 1;
    for(byte i = 0; i < nrOfTiles; i++) {
      byte tileCol1 = tilesPoz[i].col;
      byte tileCol2 = tilesPoz[i].col - 1;
      
      byte tileLine1 = tilesPoz[i].lin;
      byte tileLine2 = tilesPoz[i].lin + 1;
      byte tileLine3 = tilesPoz[i].lin + 2;
      if(col == tileCol1 || col == tileCol2){
        if(lin == tileLine1 || lin == tileLine2 || lin == tileLine3) {
          if(!doublePress[i]) {
        tilesColected ++;
        score = score + (2000 - tileSpeed)/20;
        activeTiles[i] = 0;
        ClearDisplayTile(tilesPoz[i]);
        tilesPoz[i].lin = 7;
        break;
          }
          else {
            while(millis() - lastDoubleBounce < doubleBounceDelay) {
              if(buttonPressed()) {
            tilesColected ++;
            score = score + (2000 - tileSpeed)/20 + 25;
            activeTiles[i] = 0;
            ClearDisplayTile(tilesPoz[i]);
            tilesPoz[i].lin = 7;
            doublePress[i] = 0;
            break;
              }
          }
          }
      }
      }
    }
  }
  
}

void displayOptions()
{ 
  lcd.setCursor(1, 0);
  lcd.print("BM:");
  lcd.print(matrixBrightness);

  lcd.setCursor(7, 0);
  lcd.print("BL:");
  lcd.print(lcdBrightness);

  lcd.setCursor(12, 0);
  lcd.print("CL:");
  lcd.print(lcdContrast);
  
  lcd.setCursor(1, 1);
  lcd.print("DIF:");
  lcd.print(dificulty);

  lcd.setCursor(7, 1);
  lcd.print("N:");
  lcd.print(username);

  lcd.setCursor(14, 1);
  lcd.print("EX");

  turnOnMatrix();

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
      turnOffMatrix();
      lcdState = MENU;
      lcd.clear();
      cursorActualPoz = 0;

    writeIntIntoEEPROM(lcdCt, lcdContrast);
    writeIntIntoEEPROM(matrixBr, matrixBrightness);
    writeIntIntoEEPROM(lcdBr, lcdBrightness);
    writeIntIntoEEPROM(dificultyMem, dificulty);
    writeStringToEEPROM(currentUsername, username);
      
      return;
    }


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
      lc.setIntensity(0, matrixBrightness);
    }
    else if (cursorMove == DOWN)
    {
      if (matrixBrightness == 0)
        matrixBrightness = 15;
      else
        matrixBrightness = matrixBrightness - 1;
      lcd.clear();
      lc.setIntensity(0, matrixBrightness);
    }
  }

  if (cursorActualPoz == CHANGE_LCDBR) {
    if (cursorMove == UP)
    {
      if (lcdBrightness == 9)
        lcdBrightness = 0;
      else
        lcdBrightness = lcdBrightness + 1;
      lcd.clear();
    }
    else if (cursorMove == DOWN)
    {
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
      contrastChanged = HIGH;
      if (lcdContrast == 9)
        lcdContrast = 0;
      else
        lcdContrast = lcdContrast + 1;
      lcd.clear();
    }
    else if (cursorMove == DOWN)
    {
      contrastChanged = HIGH;
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
  contrastChanged = LOW;
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
    lcd.print("Tiles");
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

  if(score > highScore1){
    highScore3 = highScore2;
    usernameHighScore3 = usernameHighScore2;
    highScore2 = highScore1;
    usernameHighScore2 = usernameHighScore1;
    highScore1 = score;
    usernameHighScore1 = username;
  }
  else if(score > highScore2){
    highScore3 = highScore2;
    usernameHighScore3 = usernameHighScore2;
    highScore2 = score;
    usernameHighScore2 = username;
  }
  else{
    highScore3 = score;
    usernameHighScore3 = username;
  }
  lcd.setCursor(6, 1);
  lcd.print(username);
  lcd.print(":");
  lcd.setCursor(10, 1);
  lcd.print(score);

  lcdState = HIGHSCORE_MENU;

  writeIntIntoEEPROM(hs1, highScore1);
  writeIntIntoEEPROM(hs2, highScore2);
  writeIntIntoEEPROM(hs3, highScore3);

  writeStringToEEPROM(hsn1, usernameHighScore1);
  writeStringToEEPROM(hsn2, usernameHighScore2);
  writeStringToEEPROM(hsn3, usernameHighScore3);
  
  while (millis() - msgStartTime < 5000);
  lcd.clear();
}

void displayHighScore() {
  highScore1 = readIntFromEEPROM(hs1);
  usernameHighScore1 = readStringFromEEPROM(hsn1);
  highScore2 = readIntFromEEPROM(hs2);
  usernameHighScore2 = readStringFromEEPROM(hsn2);
  highScore3 = readIntFromEEPROM(hs3);
  usernameHighScore3 = readStringFromEEPROM(hsn3);
  
  lcd.setCursor(0, 1);
  lcd.print(usernameHighScore2);
  lcd.print(":");
  lcd.setCursor(4, 1);
  lcd.print(highScore2);

  lcd.setCursor(9, 1);
  lcd.print(usernameHighScore3);
  lcd.print(":");
  lcd.setCursor(13, 1);
  lcd.print(highScore3);

  lcd.setCursor(3, 0);
  lcd.print(usernameHighScore1);
  lcd.print(":");
  lcd.setCursor(7, 0);
  lcd.print(highScore1);

  if (buttonPressed()){
    lcdState = MENU;//go back to main menu
    lcd.clear();
    return;
  }

  if(updatePositionsLcd() == UP){
    deleteHighScore();
    lcd.clear();
  }
  
}

 void selectSong() {
  lcd.setCursor(1, 0);
  lcd.print("Preparing");
}

 void displayGameOver() {
  lcd.setCursor(0, 0);
  lcd.print("GAME OVER");
  lcd.setCursor(0, 1);
  lcd.print(username);
  lcd.print(":");
  lcd.setCursor(4, 1);
  lcd.print(score);
  
  if (buttonPressed()){
    lcdState = MENU;//go back to main menu
    lcd.clear();
    return;
  }
}


void showTile(poz tile) {
  lc.setLed(0, tile.col - 1, tile.lin, true);
  lc.setLed(0, tile.col, tile.lin, true);
  lc.setLed(0, tile.col, tile.lin + 1, true);
  lc.setLed(0, tile.col - 1, tile.lin + 1, true);
  lc.setLed(0, tile.col, tile.lin + 2, true);
  lc.setLed(0, tile.col - 1, tile.lin + 2, true);
}

void ClearDisplayTile(poz tile) {
  lc.setLed(0, tile.col - 1, tile.lin, false);
  lc.setLed(0, tile.col, tile.lin, false);
  lc.setLed(0, tile.col, tile.lin + 1, false);
  lc.setLed(0, tile.col - 1, tile.lin + 1, false);
  lc.setLed(0, tile.col, tile.lin + 2, false);
  lc.setLed(0, tile.col - 1, tile.lin + 2, false);
}

void writeIntIntoEEPROM(int address, int number)
{ 
  byte byte1 = number >> 8;
  byte byte2 = number & 0xFF;
  EEPROM.update(address, byte1);
  EEPROM.update(address + 1, byte2);
}

void writeStringToEEPROM(int addrOffset, const String &strToWrite)
{
  byte len = strToWrite.length();
  EEPROM.update(addrOffset, len);
  for (int i = 0; i < len; i++)
  {
    EEPROM.update(addrOffset + 1 + i, strToWrite[i]);
  }
}

int readIntFromEEPROM(int address)
{
  byte byte1 = EEPROM.read(address);
  byte byte2 = EEPROM.read(address + 1);
  return (byte1 << 8) + byte2;
}

String readStringFromEEPROM(int addrOffset)
{
  int newStrLen = EEPROM.read(addrOffset);
  char data[newStrLen + 1];
  for (int i = 0; i < newStrLen; i++)
  {
    data[i] = EEPROM.read(addrOffset + 1 + i);
  }
  data[newStrLen] = '\0';
  return String(data);
}

void turnOnMatrix() {
  for (int row = 0; row < matrixSize; row++) {
    for (int col = 0; col < matrixSize; col++) {
      lc.setLed(0, row, col, 1);
    }
  }
}

void turnOffMatrix() {
  for (int row = 0; row < matrixSize; row++) {
    for (int col = 0; col < matrixSize; col++) {
      lc.setLed(0, row, col, 0);
    }
  }
}

void DisplayPlayer() {
  lc.setLed(0, playerPoz.col, playerPoz.lin, true);
  lc.setLed(0, playerPoz.col, playerPoz.lin + 1, true);
  lc.setLed(0, playerPoz.col + 1, playerPoz.lin + 1, true);
  lc.setLed(0, playerPoz.col - 1, playerPoz.lin + 1, true);
  lc.setLed(0, playerPoz.col, playerPoz.lin + 2, true);

  if(millis() - lastBlinked > blinkRate) {
    ClearDisplayPlayer();
    lastBlinked = millis();
  }
}

void ClearDisplayPlayer() {
  lc.setLed(0, playerPoz.col, playerPoz.lin, false);
  lc.setLed(0, playerPoz.col, playerPoz.lin + 1, false);
  lc.setLed(0, playerPoz.col + 1, playerPoz.lin + 1, false);
  lc.setLed(0, playerPoz.col - 1, playerPoz.lin + 1, false);
  lc.setLed(0, playerPoz.col, playerPoz.lin + 2, false);
}

void displayTiles(){
  for(byte i = 0; i < nrOfTiles; i++) {
    if(activeTiles[i] == 0) {
      int tileDelay = random(0, 250);
      tileDelay = tileDelay*10;
      tilesDelay[i] = tileDelay;
      activeTiles[i] = 2;
      if(dificulty >= 2) {
        int doublePressed = random(3, 11 - dificulty);
        if(doublePressed == 5)
          doublePress[i] = 1;
      }
    }
  }

  if(activeTiles[0] == 2) {
    if(millis() - lastDelayCheck[0] > tilesDelay[0]) {
    activeTiles[0] = 1;
    }
  }
  
  if(activeTiles[1] == 2) {
    if(millis() - lastDelayCheck[1] > tilesDelay[1]) {
    activeTiles[1] = 1;
    }
  }
  
  if(activeTiles[2] == 2) {
    if(millis() - lastDelayCheck[2] > tilesDelay[2]) {
    activeTiles[2] = 1;
    }
  }

  if(activeTiles[0] == 1)
    displayTile(0);
  if(activeTiles[1] == 1)
    displayTile(1);
  if(activeTiles[2] == 1)
    displayTile(2);
  
}

void displayTile(int tileNr) {
  showTile(tilesPoz[tileNr]);
 
  if(millis() - lastSpeedCheck[tileNr] > tileSpeed) {
    lastSpeedCheck[tileNr] = millis();
    ClearDisplayTile(tilesPoz[tileNr]);
    
    if(tilesPoz[tileNr].lin == 0) {
        tilesPoz[tileNr].lin = 7;
        activeTiles[tileNr] = 0;
        lastDelayCheck[tileNr] = millis();
        lifes --;
        lcd.clear();
      } else {
          tilesPoz[tileNr].lin --;
        }
      if(millis() - lastBlinkedTile[tileNr] > blinkRate) {
        ClearDisplayPlayer();
        lastBlinkedTile[tileNr] = millis();
      }
    }
    if(doublePress[tileNr]) {
      if(millis() - lastBlinkedTile[tileNr] > blinkRate) {
        ClearDisplayTile(tilesPoz[tileNr]);
        lastBlinkedTile[tileNr] = millis();
      }
    }
}

void playMenu(){
  for(byte i = 0; i < lifes; i++) {
    lcd.setCursor(i, 0);
    lcd.write(byte(1));
  }
  lcd.setCursor(5, 0);
  lcd.print("DIF:");
  lcd.setCursor(9, 0);
  lcd.print(dificulty);
  lcd.setCursor(0, 1);
  lcd.print("Score:");
  lcd.setCursor(7, 1);
  lcd.print(score);
}

int pause = 0;
long lastPause = 0;
int songLength = 0;
int duration = 0;

void music(){
  if(stopMusic)
    return;
  

  int melody1[] = {
    NOTE_FS5, NOTE_FS5, NOTE_D5, NOTE_B4, NOTE_B4, NOTE_E5, 
    NOTE_E5, NOTE_E5, NOTE_GS5, NOTE_GS5, NOTE_A5, NOTE_B5, 
    NOTE_A5, NOTE_A5, NOTE_A5, NOTE_E5, NOTE_D5, NOTE_FS5, 
    NOTE_FS5, NOTE_FS5, NOTE_E5, NOTE_E5, NOTE_FS5, NOTE_E5
  };
  
  int durations1[] = {
    8, 8, 8, 4, 4, 4, 
    4, 5, 8, 8, 8, 8, 
    8, 8, 8, 4, 4, 4, 
    4, 5, 8, 8, 8, 8
  };
  
  int melody2[] = {
    REST, NOTE_D5, NOTE_B4, NOTE_D5, //1
    NOTE_CS5, NOTE_D5, NOTE_CS5, NOTE_A4, 
    REST, NOTE_A4, NOTE_FS5, NOTE_E5, NOTE_D5,
    NOTE_CS5, NOTE_D5, NOTE_CS5, NOTE_A4, 
    REST, NOTE_D5, NOTE_B4, NOTE_D5,
    NOTE_CS5, NOTE_D5, NOTE_CS5, NOTE_A4, 
  
    REST, NOTE_B4, NOTE_B4, NOTE_G4, NOTE_B4, //7
    NOTE_A4, NOTE_B4, NOTE_A4, NOTE_D4,
    REST, NOTE_D5, NOTE_B4, NOTE_D5,
    NOTE_CS5, NOTE_D5, NOTE_CS5, NOTE_A4, 
    REST, NOTE_A4, NOTE_FS5, NOTE_E5, NOTE_D5,
    NOTE_CS5, NOTE_D5, NOTE_CS5, NOTE_A4
  };
  
  int durations2[] = {
    2, 8, 4, 8,
    4, 8, 4, 2,
    8, 8, 8, 4, 8,
    4, 8, 4, 2,
    4, 8, 4, 8,
    4, 8, 4, 2,
    4, 8, 4, 8,
    4, 8, 4, 2,
    8, 8, 4, 8,
    8, 8, 8, 4, 8,
    4, 8, 4, 8, 8,
    8, 8, 8, 4, 8
  };

  int melody3[] = {
  NOTE_E5, NOTE_D5, NOTE_FS4, NOTE_GS4, 
  NOTE_CS5, NOTE_B4, NOTE_D4, NOTE_E4, 
  NOTE_B4, NOTE_A4, NOTE_CS4, NOTE_E4,
  NOTE_A4 
  };

  int durations3[] = {
    8, 8, 4, 4,
    8, 8, 4, 4,
    8, 8, 4, 4,
    2
  };

  int melody4[] = {
  NOTE_E2, NOTE_E2, NOTE_E3, NOTE_E2, NOTE_E2, NOTE_D3, NOTE_E2, NOTE_E2,
  NOTE_C3, NOTE_E2, NOTE_E2, NOTE_AS2, NOTE_E2, NOTE_E2, NOTE_B2, NOTE_C3,
  NOTE_E2, NOTE_E2, NOTE_E3, NOTE_E2, NOTE_E2, NOTE_D3, NOTE_E2, NOTE_E2,
  NOTE_C3, NOTE_E2, NOTE_E2, NOTE_AS2
  };

  int durations4[] = {
    8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 1
  };


  switch (dificulty) {
    case 1:
      songLength = sizeof(melody1)/sizeof(melody1[0]);
      duration = 1000/ durations1[currentNote];
      tone(buzzerPin, melody1[currentNote], duration);
      pause = duration * 1.3;
      if(millis() - lastPause > pause){
      noTone(buzzerPin);
      currentNote++;
      lastPause = millis();
      break;
      }

      if(currentNote > songLength)
        currentNote = 0;
    
      break;
      
    case 2:
      songLength = sizeof(melody2)/sizeof(melody2[0]);
      duration = 1000/ durations2[currentNote];
      tone(buzzerPin, melody2[currentNote], duration);
      pause = duration * 1.3;
      if(millis() - lastPause > pause){
      noTone(buzzerPin);
      currentNote++;
      lastPause = millis();
      }
  
      if(currentNote > songLength)
        currentNote = 0;
      
      break;

    case 3:
      songLength = sizeof(melody3)/sizeof(melody3[0]);
      duration = 1000/ durations3[currentNote];
      tone(buzzerPin, melody3[currentNote], duration);
      pause = duration * 1.3;
      if(millis() - lastPause > pause){
      noTone(buzzerPin);
      currentNote++;
      lastPause = millis();
      }
  
      if(currentNote > songLength)
        currentNote = 0;
      
      break;

     case 4:
      songLength = sizeof(melody4)/sizeof(melody4[0]);
      duration = 1000/ durations4[currentNote];
      tone(buzzerPin, melody4[currentNote], duration);
      pause = duration * 1.3;
      if(millis() - lastPause > pause){
      noTone(buzzerPin);
      currentNote++;
      lastPause = millis();
      }
  
      if(currentNote > songLength)
        currentNote = 0;
      
      break;
      
  }

}



void deleteHighScore(){
  writeIntIntoEEPROM(hs1, 0);
  writeIntIntoEEPROM(hs2, 0);
  writeIntIntoEEPROM(hs3, 0);
}
