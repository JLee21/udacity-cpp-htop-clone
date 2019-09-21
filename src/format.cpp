#include "format.h"
#include <iostream>
#include <string>

using std::string;

string Format::ElapsedTime(long seconds) {
  // INPUT: Long int measuring seconds
  // OUTPUT: HH:MM:SS
  int hours = seconds / 3600;
  int remaining_seconds = seconds - (hours * 3600);
  int mins = remaining_seconds / 60;
  int secs = seconds - (hours * 3600 + mins * 60);

  char output[20];
  sprintf(output, "%02u:%02u:%02u", hours, mins, secs);
  return output;
}

/*
Aleternative Implementation

std::string Format::Pad(string s, char c) {
  s.insert(s.begin(), 2 - s.size(), c);
  return s;
}

string Format::Time(long int time) {
  int hours = time / (60 * 60);
  int minutes = (time / 60) % 60;
  long seconds = time % 60;
  return std::string(Format::Pad(to_string(hours), '0') + ":" +
                     Format::Pad(to_string(minutes), '0') + ":" +
                     Format::Pad(to_string(seconds), '0'));
}

*/