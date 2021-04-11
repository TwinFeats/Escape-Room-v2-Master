// DO NOT change these constants, some logic depends on the ordering
// case closed during game
#define TRACK_COUNTDOWN_STILL_RUNNING 1
// case is first opened by plaer
#define TRACK_INTRO 2
// game power up successful
#define TRACK_POWERED_UP 3
// modem powered up
#define TRACK_MODEM_ON 4
// firewall powered up
#define TRACK_FIREWALL_ON 5
// control room powered up
#define TRACK_CONTROL_ROOM_ON 6
// reactor core powered up
#define TRACK_REACTOR_UP 7

#define TRACK_TONE1 8
#define TRACK_TONE2 9
#define TRACK_TONE3 10
#define TRACK_TONE4 11
#define TRACK_TONE5 12
#define TRACK_INCOMING_MSG 13
#define TRACK_MODEM_ACQUIRED 14
#define TRACK_FIREWALL_BREECHED 15
#define TRACK_CONTROL_ROOM_ACCESS_GRANTED 16
#define TRACK_REACTOR_DEACTIVATED 17
#define TRACK_CLOSING_MSG 18
#define TRACK_FUNCTION_INACCESSIBLE 19
// played to indicate wrong answer
#define TRACK_WRONG 20
#define TRACK_HACK_ATTEMPT_DETECTED 21
#define TRACK_PLAYER_WINS	22
#define TRACK_PLAYER_LOSES	23
#define TRACK_TIME_50	24
#define TRACK_TIME_40	25
#define TRACK_TIME_30	26
#define TRACK_TIME_20	27
#define TRACK_TIME_10	28
#define TRACK_TIME_5	29
#define TRACK_TIME_1	30

/*
These are the PJON addresses of the other modules.
*/
#define COMM_ID_SWITCHES      10
#define COMM_ID_MODEM         11
#define COMM_ID_FIREWALL      12
#define COMM_ID_CONTROL_ROOM  13
#define COMM_ID_REACTOR       14
