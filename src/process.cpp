#include <stdio.h>
#include <unistd.h>
#include <cctype>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"

using std::string;
using std::to_string;
using std::vector;
using namespace std;

int Process::Pid() { return id_; }

float Process::CpuUtilization() const { return LinuxParser::CPUUsage(id_); }

string Process::Command() {
  // constrain returned string to be less than 50 charaters
  string cmd = LinuxParser::Command(id_);
  return cmd.size() > 50 ? cmd.substr(0, 50) + "..." : cmd;
}

string Process::Ram() {
  char mb_str[10];
  string ram_str = LinuxParser::Ram(id_);
  double ram_kb = ram_str.size() > 0 ? std::stof(ram_str) : 0;
  sprintf(mb_str, "%6.0f", ram_kb / 1024);  // assuming ram_str is always in KB
  return mb_str;
}

string Process::User() {
  string uid = LinuxParser::Uid(id_);
  return LinuxParser::User(uid);
}

long int Process::UpTime() { return LinuxParser::UpTime(id_); }

// Less than operator
bool Process::operator<(const Process& a) const {
  return CpuUtilization() < a.CpuUtilization();
}

// Greater than operator
bool Process::operator>(const Process& a) const {
  return CpuUtilization() > a.CpuUtilization();
}