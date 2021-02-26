// DO NOT change these constants, some logic depends on the ordering
// case closed during game
#define TRACK_COUNTDOWN_STILL_RUNNING 1
// case is first opened by player
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

#define TRACK_INCOMING_MSG 8
#define TRACK_MODEM_ACQUIRED 9
#define TRACK_FIREWALL_BREECHED 10
#define TRACK_CONTROL_ROOM_ACCESS_GRANTED 11
#define TRACK_REACTOR_DEACTIVATED 12
#define TRACK_CLOSING_MSG 13
#define TRACK_FUNCTION_INACCESSIBLE 14
// played to indicate wrong answer
#define TRACK_WRONG 15
#define TRACK_HACK_ATTEMPT_DETECTED 16
