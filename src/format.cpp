#include <string>
#include "format.h"
#include <iostream>

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) { 
    int hours = seconds / 3600;
    int remaining_seconds = seconds - (hours * 3600);
    int mins = remaining_seconds / 60;
    int secs = seconds - (hours * 3600 + mins * 60);
  
  	char output[20];
    sprintf(output, "%02u:%02u:%02u", hours, mins, secs);
  	return output;
}