#include <unistd.h>
#include <stdio.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;
using namespace std;

int Process::Pid() { return id_; }

float Process::CpuUtilization() const { 
  return LinuxParser::CPUUsage(id_);
}

string Process::Command() { 
  // constrain returned string to be less than 50 charaters 
  string cmd = LinuxParser::Command(id_);
  return cmd.size() > 50 ? cmd.substr(0, 50) + "..." : cmd;
}

string Process::Ram() {
  char mb_str [10];
  string ram_str = LinuxParser::Ram(id_);
  sprintf(mb_str, "%6.0f", std::stof(ram_str) / 1024); // assuming ram_str is always in KB
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