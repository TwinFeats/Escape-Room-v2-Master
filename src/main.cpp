#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#define PJON_MAX_PACKETS 2
#define PJON_PACKET_MAX_LENGTH 33
#include <PJONSoftwareBitBang.h>
#include <LiquidCrystal.h>
#include <TM1637TinyDisplay.h>
#include <arduino-timer.h>
#include <Tone.h>
#include "DFRobotDFPlayerMini.h"
#include <ButtonDebounce.h>
#include <tracks.h>

LiquidCrystal lcd(52, 50, 42, 44, 46, 48);

#define PIN_COUNTDOWN_CLK    1
#define PIN_COUNTDOWN_DIO    2
#define PIN_LCD_RS           3
#define PIN_MP3_BUSY         4
#define PIN_MP3_TX           5
#define PIN_MP3_RX           6
#define PIN_LCD_E            7
#define PIN_LCD_D4           8
#define PIN_LCD_D5           9
#define PIN_TONE             10
#define PIN_LCD_D6           11
#define PIN_LCD_D7           12
#define PIN_COMM             13


//analog
#define PIN_MP3_VOLUME      A1
#define PIN_TONE_VOLUME     A2
#define PIN_LED_BRIGHTNESS  A3


// Lock combo is 4219

Timer<1> brightnessTimer;
int lastBrightness = 0;

bool checkBrightness(void *t) {
  int val = analogRead(A1);
  if (val < 64) val = 64;
  if (abs(lastBrightness - val) > 32) {
    lastBrightness = val;
  }
  return true;
}

bool blinkingCountdown = false;

void gameOver() {

}

/*
 * ----------------Countdown timer ------------------------
 */

Timer<1> timer;
TM1637TinyDisplay clock(PIN_COUNTDOWN_CLK, PIN_COUNTDOWN_DIO);

int countdownSecs = 60 * 60;
boolean countdownRunning = false;  // starts when case is closed then opened
                                   // (case starts open when powered on)

boolean isGameOver = false;

int convertSecsToTimeRemaining(int secs) {
  int mins = secs / 60;
  secs = secs % 60;
  mins = mins * 100;  // shift over 2 decimal digits
  return mins + secs;
}

bool updateCountdown(void *t) {
  if (isGameOver) {
    if (blinkingCountdown) {
      clock.showNumberDec(0, 0b01000000, true);
    } else {
      clock.showString("    ");
    }
    blinkingCountdown = !blinkingCountdown;
    return true;
  }
  if (!countdownRunning) return true;
  countdownSecs--;
  if (countdownSecs < 0) {
    countdownRunning = false;
    countdownSecs = 0;
    gameOver();
  }
  int num = convertSecsToTimeRemaining(countdownSecs);
  //  Serial.println(num);
  clock.showNumberDec(num, 0b01000000, true);
  return true;
}

void penalizeSeconds(int secs) {
  countdownSecs -= secs;
}

void setCountdown(int secs) {
  countdownSecs = secs;
}

void initClock() {
  clock.showString("    ");
}

/* --------------------END countdown timer------------------ */

/* -------------------- DFPlayer -----------------------------*/

DFRobotDFPlayerMini mp3Player;

Timer<1> mp3Timer;
int mp3Queue[10];
int mp3Count = 0;
boolean mp3Playing = false;

void checkMp3Queue() {
  if (mp3Count > 0) {
    mp3Playing = true;
    mp3Player.play(mp3Queue[0]);
    for (int i=1;i<mp3Count;i++) {
      mp3Queue[i-1] = mp3Queue[i];
    }
    mp3Count--;
  }
}

void playTrack(int track);
void overridePlay(int track) {
    mp3Count = 0;
    mp3Player.stop();
    mp3Playing = false;
    playTrack(track);
}

void playTrack(int track) {
  if (mp3Count < 10) {
    mp3Queue[mp3Count++] = track;
    if (!mp3Playing) {
      checkMp3Queue();
    }
  } else {
    overridePlay(TRACK_HACK_ATTEMPT_DETECTED);
    setCountdown(countdownSecs >> 1);
  }
}

void playInfoThenTrack(int track) {
  playTrack(TRACK_INCOMING_MSG);
  playTrack(track);
}

boolean checkMp3Busy(void* t) {
  mp3Playing = (digitalRead(PIN_MP3_BUSY) == LOW);
  if (!mp3Playing) {
    checkMp3Queue();
  }
  return true;
}

SoftwareSerial mp3Serial(PIN_MP3_RX, PIN_MP3_TX); // RX, TX
void initMP3Player() {
  mp3Serial.begin(9600);
  mp3Player.begin(mp3Serial);
  mp3Player.volume(30);
  mp3Player.play(3);
  pinMode(PIN_MP3_BUSY, INPUT);
  mp3Timer.every(1000, checkMp3Busy);
}


/* --------------------END DFPlayer-------------------------*/

/* -------------------- LCD ---------------------------------*/
byte arrowUp[8] = {
    B00000, B00100, B01110, B10101, B00100, B00100, B00000,
};

byte arrowDown[8] = {
    B00000, B00100, B00100, B10101, B01110, B00100, B00000,
};

Timer<1> lcdTimer;
int msgCount = 0;
char msgLine1Queue[10][16];
char msgLine2Queue[10][16];
char msgBuffer[17];

// char *mallocStringLiteral(const char *str, int len) {
//   char *string = (char *)malloc(len+1);
//   strncpy(string, str, len);
//   string[len] = 0;
//   return string;
// }

bool checkMsgQueue(void* t) {
  if (msgCount > 0) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(msgLine1Queue[0]);
    lcd.setCursor(0, 1);
    lcd.print(msgLine2Queue[0]);

    memmove(msgLine1Queue[0], msgLine1Queue[1], 144);
    memmove(msgLine2Queue[0], msgLine2Queue[1], 144);
    // for (int i=1;i<msgCount;i++) {
    //   msgLine1Queue[i-1] = msgLine1Queue[i];
    //   msgLine2Queue[i-1] = msgLine2Queue[i];
    // }
    msgCount--;
    lcdTimer.in(5000, checkMsgQueue); //schedule next msg check
  }
  return false; //one-shot only
}

void queueMsg(char *line1, char *line2) {
  if (msgCount < 10) {
    memcpy(msgLine1Queue[msgCount], line1, 16);
    memcpy(msgLine2Queue[msgCount], line2, 16);
    // msgLine1Queue[msgCount] = line1;
    // msgLine2Queue[msgCount++] = line2;
    if (msgCount == 1) {  //only one msg, display it now
      checkMsgQueue(NULL);
    }
  } else {
    overridePlay(TRACK_HACK_ATTEMPT_DETECTED);
    setCountdown(countdownSecs >> 1);
  }
}

void initLcd() {
//  lcdTimer.every(3000, checkMsgQueue);
}

/* ---------------END LCD -----------------------------------*/

/* -------------------- GAME STATE ---------------------------*/

// case is first supplied power
#define INITIAL 0
// This is not case power, but the power state for the game
#define POWER_OFF 1
#define POWER_ON 2
// aka Tones module
#define MODEM 3
// aka Mastermind Module
#define FIREWALL 4
// aka Keypad module
#define CONTROL_ROOM 5
// aka Blackbox module
#define REACTOR_CORE 6

#define POWER_SWITCHES 0
#define MODEM_SWITCHES 1
#define FIREWALL_SWITCHES 2
#define CONTROL_ROOM_SWITCHES 3
#define REACTOR_CORE_SWITCHES 4

uint8_t GAMES[5] = {POWER_ON, MODEM, FIREWALL, CONTROL_ROOM, REACTOR_CORE};
const char *gameNames[5] = {"Power", "Modem", "Firewall", "Control room",
                            "Reactor core"};

uint8_t gameState = INITIAL;

void initGameState() {
}

/* -----------------END GAME STATE ----------------------------*/


Tone tonePlayer;

int notes[] = {NOTE_C6, NOTE_D6, NOTE_E6, NOTE_F6, NOTE_G6};

#define NOTES_LENGTH 15
uint8_t numNotesPlayed = 0;
int song[NOTES_LENGTH];
int notesPlayed[NOTES_LENGTH];
boolean modemComplete = false;
Timer<1, millis, int> toneTimer;

void checkNotes() {
  if (numNotesPlayed != NOTES_LENGTH) return;
  for (int i = 0; i < NOTES_LENGTH; i++) {
    if (notesPlayed[i] != song[i]) return;
  }
  playInfoThenTrack(TRACK_MODEM_ACQUIRED);
  modemComplete = true;
  gameState++;
}

bool playNextNote(int note) {
  tonePlayer.play(notes[note++]);
  if (note < 5) {
    toneTimer.in(1000, playNextNote, note);
  } else {
    Serial.println("stop");
    numNotesPlayed = 0;
    tonePlayer.stop();
  }
  return false;
}

void initTone() {
  tonePlayer.begin(PIND);
  for (int i = 0; i < NOTES_LENGTH; i++) {
    song[i] = random(5);
  }
}
/* --------------END TONES -------------------------*/
#define COMM_ID_SWITCHES      10
#define COMM_ID_MODEM         11
#define COMM_ID_FIREWALL      12
#define COMM_ID_CONTROL_ROOM  13
#define COMM_ID_REACTOR       14

int commIds[] = {COMM_ID_MODEM, COMM_ID_FIREWALL, COMM_ID_CONTROL_ROOM, COMM_ID_REACTOR};
PJON<SoftwareBitBang> bus(1);

void error_handler(uint8_t code, uint16_t data, void *custom_pointer) {
  if(code == PJON_CONNECTION_LOST) {
    Serial.print("Connection lost with device id ");
    Serial.println(bus.packets[data].content[0], DEC);
  }
}

void commReceive(uint8_t *data, uint16_t len, const PJON_Packet_Info &info) {
  char line1[17], line2[17];
  switch(data[0]) {
    case 'L':  //LCD msg
      queueMsg((char *)&data[1],(char *)&data[17]);
      break;
    case 'T':  //Play tone
      tonePlayer.play(notes[data[1]], 1000);
      break;
    case 'P': //Play tone song
      playNextNote(0);
      break;
    case 'M':  //Play MP3
      playTrack(data[1]);
      break;
    case 'C': //Adjust countdown clock
      countdownSecs -= data[1]*60;
      break;
    case 'D': //Panel done
      // ??????
      if (gameState == REACTOR_CORE) {
        //player wins!
      } else {
        bus.send(COMM_ID_SWITCHES, "C", 1); //Tell switches it's clear to go to next state
      }
      break;
    case 'G':  //Progress game state
      gameState++;  //game state is advanced by Switches panel
      if (gameState == POWER_OFF) { //Case has been opened
        delay(1000);
        playTrack(TRACK_INTRO);
        countdownRunning = true;  //start clock
      } else if (gameState == POWER_ON) {
        //switches panel will have done everything needed, nothing to do here
      }
      else if (gameState > POWER_ON) {
        bus.send(gameState+8,"A", 1);
      }

      break;
    case 'O': //Panel lid opened
      sprintf(line1, "%-16s", "Hack attempt");
      sprintf(line2, "%-16s", "Fire initiated");
      queueMsg(line1, line2);
      overridePlay(TRACK_HACK_ATTEMPT_DETECTED);
      countdownSecs = 10;
      break;
    default:  //Unknown
      //???
      break;

  }
}

void initCommsBus() {
  bus.strategy.set_pin(PIN_COMM);
  bus.include_sender_info(false);
  bus.set_error(error_handler);
  bus.set_receiver(commReceive);
  bus.begin();
}

void setup() {
  Serial.begin(9600);
   while (!Serial)
     ;  // wait for serial attach
  Serial.println("Starting");
  initCommsBus();
  lcd.begin(16, 2);
  lcd.clear();
  lcd.createChar(1, arrowUp);
  lcd.createChar(2, arrowDown);
  randomSeed(analogRead(0));
  initGameState();
  clock.setBrightness(7);
  countdownRunning = true;
  timer.every(1000, updateCountdown);
  brightnessTimer.every(100, checkBrightness);
  initTone();
  initMP3Player();

  gameState = REACTOR_CORE;
  //  reportSwitches();
  // reportKeycode();
}

void loop() {
  toneTimer.tick();
  timer.tick();
  brightnessTimer.tick();
  lcdTimer.tick();
  mp3Timer.tick();
  bus.update();
  bus.receive(750);  //try to receive for .75 ms
  if (isGameOver) {
  }
}