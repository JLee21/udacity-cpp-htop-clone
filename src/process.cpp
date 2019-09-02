#include <unistd.h>
#include <stdio.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;
using namespace std;

int Process::Pid() { return id_; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() { 
  return 0;
}

string Process::Command() { return LinuxParser::Command(id_); }

string Process::Ram() {
  char mb_str [10];
  string ram_str = LinuxParser::Ram(id_);
  // assuming ram_str is always in KB
  sprintf(mb_str, "%6.1f", std::stof(ram_str) / 1e3);
  return mb_str;
}

string getUserFromUid(string uid){
  // Help from http://www.cplusplus.com/reference/cstdio/sscanf/?kw=sscanf
  // Help on string C-string conversion https://www.geeksforgeeks.org/how-to-convert-c-style-strings-to-stdstring-and-vice-versa/
  // Search within /etc/passwd for a line like this --> david:x:1000:1000:David Silver
  string line, s;
  const char delim = ':';
  vector<string> vals = {"", "", ""};
  std::ifstream filestream_pswd(LinuxParser::kPasswordPath);
  if (filestream_pswd.is_open()) {
    while (std::getline(filestream_pswd, line)) {
      // Split the line into parts deliminated by ":"
      // https://www.techiedelight.com/split-string-cpp-using-delimiter/
      // Example line: redis:x:112:118::/var/lib/redis:/bin/false
      stringstream ss(line);
      for(int i = 0; i<=2; i++){
          getline(ss, s, delim);
          vals[i] = s;
      }
      if(vals[2] == uid){
		return vals[0];
      } else{
       return "unknown"; 
      }
    }
  }
}

string Process::User() {
  string uid = LinuxParser::Uid(id_);
  return LinuxParser::User(uid);
}

long int Process::UpTime() { return LinuxParser::UpTime(id_); }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a[[maybe_unused]]) const { return true; }