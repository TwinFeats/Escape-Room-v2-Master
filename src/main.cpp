/*
Escape Room in a case: Starfield
Kent L. Smotherman, copyright © 2021
Released under the Creative Commons Non-commerical use liciense, https://creativecommons.org/licenses/by-nc/3.0/legalcode
In summary, if you want to make this project for friends and family to play for free, go for it! If you want to use this project
as a base and modify it for friends and family to play for free, go for it! If you want to use this project or any modification
of it for a paid experience of any kind, you CANNOT go for it! I did this project as a labor of love for free, and want it to
remain that way.
*/
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#define PJON_MAX_PACKETS 4
#define PJON_PACKET_MAX_LENGTH 52
#include <PJONSoftwareBitBang.h>
#include <LiquidCrystal.h>
#include <TM1637TinyDisplay.h>
#include <arduino-timer.h>
#include "DFRobotDFPlayerMini.h"
#include <ButtonDebounce.h>
/*
This next file is my list of track definitions for the mp3 player.
This way every project source can include it in order to send commands
to this master module to play an MP3 track.
*/
#include <tracks.h>

/*
Here is the list of all pins used by this module, and what they are
connected to.
*/
#define PIN_COUNTDOWN_CLK    2  
#define PIN_COUNTDOWN_DIO    A3
#define PIN_LCD_RS           4
#define PIN_LCD_E            5
#define PIN_LCD_D4           6
#define PIN_LCD_D5           7
#define PIN_LCD_D6           8
#define PIN_LCD_D7           9
//TX of nano, RX of MP3
#define PIN_MP3_TX           10
#define PIN_MP3_RX           A1
#define PIN_MP3_BUSY         12

#define PIN_COMM             13
#define PIN_REPEAT           A4

//analog
#define PIN_MP3_VOLUME      A6
#define PIN_LED_BRIGHTNESS  A2


/*
The LCD screen definition.
*/
LiquidCrystal lcd(PIN_LCD_RS, PIN_LCD_E, PIN_LCD_D4, PIN_LCD_D5, PIN_LCD_D6, PIN_LCD_D7);

/*
Each module uses the PJON library for 1-wire communication between them. The address of
this master module is 1.
*/
PJON<SoftwareBitBang> bus(1);

/*
These are the PJON addresses of the other modules.
*/
#define COMM_ID_SWITCHES      10
#define COMM_ID_MODEM         11
#define COMM_ID_FIREWALL      12
#define COMM_ID_CONTROL_ROOM  13
#define COMM_ID_REACTOR       14


// 3-d printed Lock combo is 4219!

/*
Timer for checking the LED brightness pot.
*/
Timer<1> brightnessTimer;

/*
Brightness has not yet been set, so init to -1 to indicate that.
*/
int lastBrightness = -1;

/*
Debounce for the repeat buton.
*/
ButtonDebounce replayButton(PIN_REPEAT, 250);

/*
Just some forward definitions for common methods.
*/
void playTrack(int);
void overridePlay(int);

/*
Wait 1 minute before sending out the first brightness message. This gives
the other panels time to initialize. The 137 here is the wait time in millis
between brightness pot checks.
 */
long brightnessWaitCount = (1000/137)*60;

/*
This method is called by the brightness timer to check the brightness pot for
a change. The brightness ranges from 0-255 so this method maps the pot values
of 0-1023 to that range.
*/
bool checkBrightness(void *t) {
  if (brightnessWaitCount == 0) {
    uint8_t msg[2];
    int val = analogRead(PIN_LED_BRIGHTNESS);
    val = map(val, 0, 1023, 0, 255);
    if (lastBrightness != val) {
      lastBrightness = val;
      /*
      Send the brightness change command to all other panels (address 0).
      */
      msg[0] = 'B';
      msg[1] = val;
      bus.send(0, msg, 2);
      bus.update();
    }
  } else {
    brightnessWaitCount--;
  }
  /*
  The return true in these timer methods causes the timer to keep running
  */
  return true;
}

/*
The countdown blinks when down to 0 or the player wins
*/
bool blinkingCountdown = false;

/*
Called when the player has run out of time
*/
void gameOver() {
  /*
  Overrides any audio that is playing and playes the losing audio
  */
  overridePlay(TRACK_PLAYER_LOSES);
}

/* -------------------- GAME STATE ---------------------------*/
/*
The game is basically a big state machine - every component must be completed
in order, so these constants define those states and their order
*/

// case is first supplied power
#define INITIAL 0
// This is not case power, but the power state for the game
#define POWER_OFF 1
#define POWER_ON 2
#define MODEM 3
#define FIREWALL 4
#define CONTROL_ROOM 5
#define REACTOR_CORE 6

/*
The order of the panels and their display names
*/
uint8_t GAMES[5] = {POWER_ON, MODEM, FIREWALL, CONTROL_ROOM, REACTOR_CORE};
const char *gameNames[5] = {"Power", "Modem", "Firewall", "Control room",
                            "Reactor core"};

uint8_t gameState = INITIAL;  //Starting state

/*
Initialize the game state
*/
void initGameState() {
  //Nothing to do, but here for consistency
}

/* -----------------END GAME STATE ----------------------------*/
/*
 * ----------------Countdown timer ------------------------
 */

/*
Timer for counting down 1 sec
*/
Timer<1> timer;
/*
Initialize the clock unit
*/
TM1637TinyDisplay clock(PIN_COUNTDOWN_CLK, PIN_COUNTDOWN_DIO);

/*
Total time is 1 hour
*/
int countdownSecs = 60 * 60 + 1;
boolean countdownRunning = false;  // starts when case is closed then opened
                                   // (case starts open when powered on)

boolean isGameOver = false;

/*
Converts counter of secs remaining to minutes:secs format
*/
int convertSecsToTimeRemaining(int secs) {
  int mins = secs / 60;
  secs = secs % 60;
  mins = mins * 100;  // shift over 2 decimal digits so result is MMSS
  return mins + secs;
}

/*
The clock timer calls this function to count down 1 sec
*/
bool updateCountdown(void *t) {
  if (isGameOver) {
    if (blinkingCountdown) {
        int num = convertSecsToTimeRemaining(countdownSecs);
      /*
      Blinks the time
      */
      clock.showNumberDec(num, 0b01000000, true);
    } else {
      clock.showString("    ");
    }
    blinkingCountdown = !blinkingCountdown;
    return true;
  }
  if (!countdownRunning) return true;
  countdownSecs--;
  /*
  This if laadder plays time remaining warnings at the various intervals
  */
  if (countdownSecs == 50*60) {
    playTrack(TRACK_TIME_50);
  }
  else if (countdownSecs == 40*60) {
    playTrack(TRACK_TIME_40);
  }
  else if (countdownSecs == 30*60) {
    playTrack(TRACK_TIME_30);
  }
  else if (countdownSecs == 20*60) {
    playTrack(TRACK_TIME_20);
  }
  else if (countdownSecs == 10*60) {
    playTrack(TRACK_TIME_10);
  }
  else if (countdownSecs == 5*60) {
    playTrack(TRACK_TIME_5);
  }
  else if (countdownSecs == 60) {
    playTrack(TRACK_TIME_1);
  }
  if (countdownSecs < 0) {
    countdownRunning = false;
    countdownSecs = 0;
    gameOver();
  }
  int num = convertSecsToTimeRemaining(countdownSecs);
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

/*
The mp3 player var
*/
DFRobotDFPlayerMini mp3Player;

Timer<1> mp3Timer;  //timer for checking if there is audio to play
Timer<1> mp3Volume; //timer for checking the mp3 volume pot
Timer<1> mp3Wait;   //timer for checking if the mp3 player is already playing

/*
This is a track queue for the mp3 player. If a panel want a track played, that
track is added to the queue and played after the current track (if any) finishes playing.
*/
int mp3Queue[10];
int mp3Count = 0;
/*
This is true if a track is playing
*/
boolean mp3Playing = false;

/*
This keeps track of the last track played so that the repeat button can replay it.
Only "important" tracks are remembered, so tracks like "40 minutes remaining" don't count.
*/
int lastTrack = 0;

/*
The mp3Wait timer calls this to check if there is a track playing.
*/
bool waitForMp3(void *t) {
  if (!mp3Playing) {
    /*
    This is a bit of a hack - when the case is opened by the player intro intro track is played,
    but the clock has not yet started running. So when the track is finished playing, we enable
    the clock to run. This is harmless when the clock is already running and a track finishes
    playing.
    */
    countdownRunning = true;
  }
  return mp3Playing;
}

/*
The mp3Volume timer calls this to check the mp3 volume pot.
*/
boolean checkMp3Volume(void* t) {
  int val = analogRead(PIN_MP3_VOLUME);
  /*
  The mp3 player volume goes from 0-30.
  */
  val = map(val, 0, 1023, 0, 30);
  mp3Player.volume(val);
  return true;
}

/*
This funciton is used to check if there is a track in the queue to play.
*/
void checkMp3Queue() {
  if (mp3Count > 0) { //there is at least 1 track waiting to play
    int track = mp3Queue[0];  //get the first track in the play queue
    /*
    The non-trivial tracks are < 24 or > 30
    */
    if ((track < 8 && track > 12 && track < 24) || track > 30) {
      lastTrack = track;  //remember this as the last track requested
    }
    mp3Playing = true;
    mp3Player.playMp3Folder(track); //play the first track in the queue
    /*
    move all the rest of the tracks down. So track 1 in the queue becomes track 0, etc.
    */
    for (int i=1;i<mp3Count;i++) {
      mp3Queue[i-1] = mp3Queue[i];
    }
    mp3Count--; //1 less track in the queue since we just started playing one.
  }
}


void playTrack(int track);  //just a forward declaration

/*
This function is used to play important audio tracks. It will clear the entire queue
and then play the specified track.
*/
void overridePlay(int track) {
    mp3Count = 0;
    mp3Player.stop();
    mp3Playing = false;
    playTrack(track);
}

/*
Requests that a track be played by adding it to the queue.
*/
void playTrack(int track) {
  if (mp3Count < 10) {  //queue is not full
    mp3Queue[mp3Count++] = track; //add the track to the queue
    if (!mp3Playing) {  //if not currently playing a track
      checkMp3Queue();  //check the queue, which will play this track
    }
  } else {
    /*
    If the queue is full, the player has been doing crazy stuff and
    causing lots of track to be queued. This is treated as an attempt
    to hack the game
    */
    overridePlay(TRACK_HACK_ATTEMPT_DETECTED);
    setCountdown(countdownSecs >> 1); //half the remaining time!
  }
}

/*
This function is called by the mp3Timer to check the busy pin of the player
*/
boolean checkMp3Busy(void* t) {
  mp3Playing = (digitalRead(PIN_MP3_BUSY) == LOW);
  if (!mp3Playing) {  //if not playing, check the queue for waiting tracks
    checkMp3Queue();
  }
  return true;
}

/*
Initialize the software serial communication for the mp3 player
*/
SoftwareSerial mp3Serial(PIN_MP3_RX, PIN_MP3_TX); // RX, TX

/*
Used for debugging, activates all panels for testing.
Triggered by pressing the replay button before closing the
case after connecting the batteries.
*/
void activateAll() {
  for (int i=10;i<15;i++) {
    bus.send(i,"A", 1); //Send the activate message to panel
    while(bus.update());
  }
  playTrack(TRACK_TONE1);
}

/*
Called by the replay button debouncer
*/
void replayLastTrack(int state) {
  /*
  If button was pressed and there is a last track to play
  */
  if (state == LOW) {
    if (lastTrack != 0) {
      playTrack(lastTrack);
    } else if (!countdownRunning && gameState == 0) {
      activateAll();
    }
  }
}

/*
Initialize the mp3 player vars
*/
void initMP3Player() {
  mp3Serial.begin(9600);
  mp3Player.begin(mp3Serial);
  mp3Player.volume(15);
  pinMode(PIN_MP3_BUSY, INPUT_PULLUP);
  mp3Timer.every(237, checkMp3Busy); //check if the mp3 player is playing about 4 times a sec
  mp3Volume.every(337, checkMp3Volume); //check the mp3 player volume pot about 3 times a sec
  replayButton.setCallback(replayLastTrack);  //debounce the replay button and tie it to its callback function
}


/* --------------------END DFPlayer-------------------------*/

/* -------------------- LCD ---------------------------------*/
/*
Custom up arrow character
*/
byte arrowUp[8] = {
    B00000, B00100, B01110, B10101, B00100, B00100, B00000,
};

/*
Custom down arrow char
*/
byte arrowDown[8] = {
    B00000, B00100, B00100, B10101, B01110, B00100, B00000,
};

/*
Timer for displaying messages. This is used by the queue to allow
a message to be visible for 5 seconds before the next message waiting
is shown.
*/
Timer<1> lcdTimer;

int msgCount = 0; //count of messages in queue
char msgLine1Queue[10][17]; //queue for line 1 of the LCD
char msgLine2Queue[10][17]; //queue for line 2 of the LCD

boolean displayingMessage = false;  //used to ensure msg is visible for minimum time
/*
Called by the timer to check if there is a waiting message to display.
*/
bool checkMsgQueue(void* t) {
  if (msgCount > 0) { // any waiting message in queue?
  /*
  These next 3 lines were put here after many hours of debugging. I had an issue
  where messages were showing up on the LCD as random characters. To complicate
  matters, the switch position messages on startup would always display fine,
  but after that every message either was displayed fine or every message would
  be random garbage. My best guess after lots of debugging was that something was
  wrong in the LCD driver library itself, so these lines of code effectively
  reset the LCD driver library. Weird.
  */
    lcd.begin(16, 2);
    lcd.createChar(1, arrowUp);
    lcd.createChar(2, arrowDown);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(msgLine1Queue[0]);
    lcd.setCursor(0, 1);
    lcd.print(msgLine2Queue[0]);
    Serial.println(msgLine1Queue[0]);
    Serial.println(msgLine2Queue[0]);

/*
Move all the following messages in queue down one slot, so that
message 1 becomes mesage 0, etc. This basically removes the message
just displayed (message 0) from the queue.
*/
    memmove(msgLine1Queue[0], msgLine1Queue[1], 17*9);
    memmove(msgLine2Queue[0], msgLine2Queue[1], 17*9);
    msgCount--;
    /*
    If there are more messages waiting to be display, return true so the timer
    will keep running to get them displayed.
    */
    return msgCount > 0;
  }
  displayingMessage = false;
  return false; //No more messages to display, so kill the timer by returning false.
}


/*
Add a new message to the queue
*/
void queueMsg(const char *line1, const char *line2) {
  // Make sure the queue isn't full
  if (msgCount < 10) {
    memcpy(msgLine1Queue[msgCount], line1, 17);
    memcpy(msgLine2Queue[msgCount++], line2, 17);
    if (!displayingMessage) {  //ok to display this new msg
      displayingMessage = true;
      lcdTimer.every(8000, checkMsgQueue); //schedule next msg check
      checkMsgQueue(NULL);
    } else {
      if (lcdTimer.empty()) {
        lcdTimer.every(8000, checkMsgQueue); //schedule next msg check
      } // no else, when the timer pops it will pick up the next msg in queue in checkMsgQueue
    }
  } else {
    /*
    Just like with the MP3 player, if the queue is every full the player must just
    be doing fast, random things - so call them a hacker and penalize their time!
    */
    overridePlay(TRACK_HACK_ATTEMPT_DETECTED);
    setCountdown(countdownSecs >> 1);
  }
 }

void overrideMsg(const char *line1, const char *line2) {
  msgCount = 0;
  lcdTimer.cancel();
  displayingMessage = false;
  queueMsg(line1, line2);
}

void initLcd() {
  /*
  Clear out the entire message queue memory with 0s.
  */
  memset(msgLine1Queue, 0, 170);
  memset(msgLine2Queue, 0, 170);
}

/* ---------------END LCD -----------------------------------*/

/*
15 notes in the random "song"
*/
#define NOTES_LENGTH 15
/*
The song the player has to reproduce
*/
uint8_t song[NOTES_LENGTH];
/*
The notes the player has played so far
*/
int notesPlayed[NOTES_LENGTH];

/*
Timer for playing each note in the song when the player presses the play button.
*/
Timer<1> toneTimer;
int nextNote = 0; //Next note index in the song to play

/*
Called by the timer to play the next note in the song.
*/
bool playNextNote(void* t) {
  overridePlay(song[nextNote]+TRACK_TONE1);
  nextNote++;
  if (nextNote < NOTES_LENGTH) {  // more notes in song? keep playing
    return true;
  } else {
    nextNote = 0; //done, reset to first note
  }
  return false;
}

/*
Initialize anything for playing the tones
*/
void initTone() {
}

/* --------------END TONES -------------------------*/
/*
The PJON communication system - these are the addresses of each of the other panels
*/
int commIds[] = {COMM_ID_MODEM, COMM_ID_FIREWALL, COMM_ID_CONTROL_ROOM, COMM_ID_REACTOR};

/*
Handle any PJON errors
*/
void error_handler(uint8_t code, uint16_t data, void *custom_pointer) {
  if(code == PJON_CONNECTION_LOST) {
    Serial.print("Connection lost with device id ");
    Serial.println(bus.packets[data].content[0], DEC);
  }
}

/*
Handle an incoming message
*/
void commReceive(uint8_t *data, uint16_t len, const PJON_Packet_Info &info) {
  Serial.print("commReceive ");
  Serial.println((char *)data);
  if (isGameOver) return; //ignore all messages if game is over
  char line2[17];
  switch(data[0]) {
    case 'L':  //LCD msg
      queueMsg((char *)&data[1],(char *)&data[18]);
      break;
    case 'Z':  //Immediate LCD msg
      overrideMsg((char *)&data[1],(char *)&data[18]);
      break;
    case 'T':  //Play tone
      toneTimer.cancel(); //cancel song if it is playing
      overridePlay(data[1]+TRACK_TONE1);
      break;
    case 'P': //Play tone song
      toneTimer.cancel();
      nextNote = 0;
      toneTimer.every(370, playNextNote);
      break;
    case 'S': //Store song - when the Modem panel is activated it sends us the random song
      memcpy(song, &data[1], NOTES_LENGTH);
      line2[0] = 0;
      for (int i=0;i<NOTES_LENGTH;i++) {
        sprintf(&line2[i], "%i", song[i]);
      }
      queueMsg("Song",line2);
      break;
    case 'M':  //Play MP3
      playTrack(data[1]);
      break;
    case 'C': //Adjust countdown clock
      countdownSecs -= data[1]*60;
      break;
    case 'D': //The current panel has been completed by the player
      if (gameState == REACTOR_CORE) {
        //player wins!
      } else {
        bus.send(COMM_ID_SWITCHES, "C", 1); //Tell switches it's clear to go to next state
      }
      break;
    case 'G':  //Progress game state
      gameState++;  //game state is advanced by Switches panel
      if (gameState == POWER_OFF) { //Case has been opened by the player to start the game
        delay(3000);
        playTrack(TRACK_INTRO);
        mp3Wait.every(1000, waitForMp3);  //start the mp3 busy timer
      } else if (gameState == POWER_ON) {
        //switches panel will have done everything needed, nothing to do here
      }
      else if (gameState > POWER_ON) {
        bus.send(gameState+8,"A", 1); //Send the activate message to the current panel
      }

      break;
    case 'O': //Panel lid opened
    //Nothing to do here
      break;
    default:  //Unknown
      //???
      break;
  }
}

/*
Initialize the PJON communication bus
*/
void initCommsBus() {
  bus.strategy.set_pin(PIN_COMM);
  bus.set_error(error_handler);
  bus.set_receiver(commReceive);
  bus.begin();
}

void setup() {
  Serial.begin(9600);
  Serial.println("Starting");
  initCommsBus();
  lcd.begin(16, 2);
  lcd.clear();
  lcd.createChar(1, arrowUp);
  lcd.createChar(2, arrowDown);
  randomSeed(analogRead(0));  //initialie random number generated by reading disconnected analog pin
  initGameState();
  clock.setBrightness(7);
  countdownRunning = false;
  timer.every(1000, updateCountdown); //update countdown clock every second
  brightnessTimer.every(137, checkBrightness);  //check brightness knob about 8 times a second
  initTone();
  initMP3Player();
}


void loop() {
  /*
  All these tick methods just advance the various timers
  */
  toneTimer.tick();
  timer.tick();
  brightnessTimer.tick();
  lcdTimer.tick();
  mp3Timer.tick();
  mp3Volume.tick();
  mp3Wait.tick();
  replayButton.update();

  bus.update(); //send any pending PJON messages
  bus.receive(750);  //try to receive a message for .75 ms
}