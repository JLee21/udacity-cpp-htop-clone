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

float Process::CpuUtilization() { 
  return LinuxParser::CPUUsage(id_);
}

string Process::Command() { return LinuxParser::Command(id_); }

string Process::Ram() {
  char mb_str [10];
  string ram_str = LinuxParser::Ram(id_);
  sprintf(mb_str, "%6.0f", std::stof(ram_str) / 1e3); // assuming ram_str is always in KB
  return mb_str;
}

string Process::User() {
  string uid = LinuxParser::Uid(id_);
  return LinuxParser::User(uid);
}

long int Process::UpTime() { return LinuxParser::UpTime(id_); }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a[[maybe_unused]]) const { return true; }