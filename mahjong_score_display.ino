#include "ae7seg.h"

#define PIN_SCK 13
#define PIN_SDI 12
#define PIN_LATCH 11

struct ScorePin {
  int digit;
  int sign;
};

struct WindScorePins {
  ScorePin east;
  ScorePin south;
  ScorePin west;
  ScorePin north;
};

struct Winds {
  int east;
  int south;
  int west;
  int north;
};

WindScorePins scorePins = { {5, 4}, {7, 6}, {9, 8}, {15, 14} };
volatile Winds scores = { 250, 250, 250, 250 };

AE7SEGGPIO ae7seg(PIN_LATCH, PIN_SDI, PIN_SCK);

void setup() {
  // 割り込みピン2
  attachInterrupt(0, setEastSouthScore, FALLING);
  displayAllScores();
}

void loop() {
}

void displayScore(int score) {
  ae7seg.writeNumber(score / 100 % 10);
  ae7seg.writeNumber(score / 10 % 10);
  ae7seg.writeNumber(score % 10);
}

void displayAllScores() {
  ae7seg.beginWrite();
  displayScore(scores.north);
  displayScore(scores.west);
  displayScore(scores.south);
  displayScore(scores.east);
  ae7seg.endWrite();
}

int calcScoreDiff(ScorePin scorePinState) {
  // LOW: 1000  HIGH: 100
  bool digit = scorePinState.digit;
  // LOW: plus  HIGH: minus
  bool sign = scorePinState.sign;

  if (digit && sign) return -1;
  if (digit && !sign) return 1;
  if (!digit && sign) return -10;
  if (!digit && !sign) return 10;
}

void setEastSouthScore() {
  ScorePin eastPinState = { digitalRead(scorePins.east.digit), digitalRead(scorePins.east.sign) };
  // ScorePin southPinState = { digitalRead(scorePins.south.digit), digitalRead(scorePins.south.sign) };
  scores.east += calcScoreDiff(eastPinState);
  displayAllScores();
}
