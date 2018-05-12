//@drkrarduknight
#include <Tinyfont.h>
#include <Arduboy2.h>
#include "Sprites.h"
#include "AlienQuestions.h"

#define INTROLEN 1

Arduboy2 arduboy;
Tinyfont tinyfont = Tinyfont(arduboy.sBuffer, Arduboy2::width(), Arduboy2::height());

int scroll = 0;
byte frames = 0;
byte titleframes = 0;

const char introTxt[INTROLEN][MAXSTRINGLENGTH] PROGMEM = {"HELLO TRAVELLER, YOU HAVE \nCRASHED YOUR SHIP ON A \nSTRANGE PLANET AND ARE IN \nSEARCH OF MISSING PARTS. \nYOU WILL COME ACROSS \nSTARVED AND CRAZY SMART \nALIENS. SURVIVE BY \nANSWERING THEIR QUESTIONS \nAND SECURING YOUR PARTS. \nIF YOU ANSWER WRONG, YOU \nWILL BE EATEN! REMEMBER -\nA MEANS 'YES' AND B MEANS \n'NO'. HAPPY FORAGING!"};

enum state {TITLE, INTRO, QUESTION, CORRECT, INCORRECT, DRAW, VICTORY};

int questionNo;
int gameState = TITLE;

void intro() {
  tinyfont.setCursor(0, 0);
  int counter = 0;
  
  for(int i = 0; i < INTROLEN; i++) {
    while((char)pgm_read_byte(introTxt[i] + counter) != '\0') {
      tinyfont.print((char)pgm_read_byte(introTxt[i] + counter));
      arduboy.display();
      delay(20);
      counter++;
    }
    
    while(!(arduboy.justPressed(A_BUTTON) || arduboy.justPressed(B_BUTTON))) {
      arduboy.pollButtons();
    }
  }
  
  gameState = DRAW;
}

void titlePage() {
  if(titleframes == 8) titleframes = 0;
  Sprites::drawSelfMasked(-2, 0, ForagerAnim, titleframes);
  if(arduboy.everyXFrames(7)) titleframes++; 
  arduboy.setCursor(42, 48);
  arduboy.print("PRESS A");
  if(arduboy.justPressed(A_BUTTON)){
    gameState = INTRO;
    questionNo = 0;
  }
  else {
    return;
  }

}

void askQuestion(struct alienQ* alien) {
  tinyfont.setCursor(0, 0);
  int counter = 0;
  Sprites::drawOverwrite(102, 32, DeathAlien, 0);

  while((char)pgm_read_byte(alien->question + counter) != '\0') {
    tinyfont.print((char)pgm_read_byte(alien->question + counter));
    arduboy.display();
    delay(20);
    counter++;
  }

  int answer = -1;

  while(1) {
    if(!arduboy.nextFrame()) continue;
    arduboy.pollButtons();
    if(arduboy.justPressed(A_BUTTON)) {
      answer = 1;
      break;
    }
    else if(arduboy.justPressed(B_BUTTON)) {
      answer = 0;
      break;
    }
  }
  
  if(answer == -1) {
    return;
  }
  
  if(answer == pgm_read_byte(alien->answer)) {
    if(questionNo != NUMALIENS - 1) {
      gameState = CORRECT;  
      questionNo++;
    }
    else {
      gameState = VICTORY;
    }
  }
  else {
    gameState = INCORRECT;
  }
}

void correct() {
  tinyfont.setCursor(0, 0);
  tinyfont.print("WOW! YOU'RE NOT HOPELESS,\nFORAGER");
  
  if(arduboy.justPressed(A_BUTTON) || arduboy.justPressed(B_BUTTON)) {
    gameState = DRAW;
  }
}

void victory() {
  if(frames == 3) frames = 0;
  Sprites::drawSelfMasked(0, 0, liftoff, frames);
  if(arduboy.everyXFrames(3)) frames++; 
  arduboy.setCursor(54,45);
  arduboy.print("LIFTOFF!");
  tinyfont.setCursor(19,60);
  tinyfont.print("PRESS A TO RESTART");
  
  if(arduboy.justPressed(A_BUTTON)) {
    gameState = TITLE;
  }
  
}

void incorrect() {
  arduboy.setCursor(12, 10);
  arduboy.setTextSize(2);
  arduboy.print("GAME OVER");
  tinyfont.setCursor(10, 30);
  tinyfont.print("YOU ARE LOST FOREVER");
  tinyfont.setCursor(8, 48);
  tinyfont.print("PRESS A OR B TO RESTART");
  
  
  if(arduboy.justPressed(A_BUTTON) || arduboy.justPressed(B_BUTTON)) {
    gameState = TITLE;
  }
}

void draw() {
  byte* sprite;
  switch(questionNo) {
    case 0: sprite = NoHead; break;
    case 2: sprite = NoHull; break;
    case 4: sprite = NoThrusters; break;
    case 6: sprite = LeftThruster; break;
    case 8: sprite = CompleteShip; break;
    default: gameState = QUESTION; return;
  }
  
  for(int i = 0; i < 4; i++) {
    Sprites::drawOverwrite(i*32, (scroll % 64) - 64, Stars, 0);
    Sprites::drawOverwrite(i*32, (scroll % 64), Stars, 0);
  }
  scroll += 23;
  
  // Prevent Overflows
  if(scroll == 6400) {
    scroll = 0;
  }

  Sprites::drawSelfMasked(40, 0, sprite, 0);

  if(arduboy.justPressed(A_BUTTON) || arduboy.justPressed(B_BUTTON)) {
    gameState = QUESTION;
  }
}

void setup() {
  arduboy.begin();
  arduboy.clear();
  arduboy.setFrameRate(28);
}

void loop() {
  if(!arduboy.nextFrame()) return;
  arduboy.clear();
  arduboy.pollButtons();
    switch(gameState) {
    case TITLE:
      titlePage();
      break;  
    case INTRO:
      intro();
      break;
    case QUESTION:
      askQuestion(myAliens + questionNo);
      break;
    case CORRECT:
      correct();
      break;
    case INCORRECT:
      incorrect();
      break;
    case VICTORY:
      victory();
      break;
    case DRAW:
      draw();
      break;
  }
  
  
  arduboy.display();
}
  
