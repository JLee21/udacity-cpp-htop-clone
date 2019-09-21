#include <dirent.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <exception>

#include "linux_parser.h"
#define printVariableNameAndValue(x) \
  cout << "name**" << (#x) << "** value => " << x << "\n"

using std::cout;
using std::endl;
using std::string;
using std::to_string;
using std::vector;

string LinuxParser::OperatingSystem() {
  // Parse a file's line for this --> PRETTY_NAME="Ubuntu 16.04.5 LTS"
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
    filestream.close();
  }
  return value;
}

string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream filestream(kProcDirectory + kVersionFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
    filestream.close();
  }
  return kernel;
}

// return a vector of ints that denote all of the PIDs
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

float LinuxParser::MemoryUtilization() {
  // In my implementation, I will calculate:
  // Total used memory = MemTotal - MemFree
  // Non cache/buffer memory (green) which equals Total used memory - (Buffers +
  // Cached memory)
  string key, value, line;
  std::map<string, float> hash;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        hash[key] = std::stof(value);
      }
    }
    filestream.close();
  }
  double memTotalUsed = hash["MemTotal:"] - hash["MemFree:"];
  return (memTotalUsed - (hash["Buffers:"] + hash["Cached:"])) /
         hash["MemTotal:"];
}

long LinuxParser::UpTime() {
  // System Uptime (as opposed to Process Time)
  string uptime, idletime, line;
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> uptime >> idletime;
    filestream.close();
  }
  return std::stol(uptime);
}

int LinuxParser::TotalProcesses() {
  // TODO: abstract this funtion since it is similar to TotalProcesses and
  // RunningProcesses Parse a file's line for this --> processes 1234
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processes") {
          return std::stoi(value);
        }
      }
    }
    filestream.close();
  }
  return 0;
}

int LinuxParser::RunningProcesses() {
  // Parse a file's line for this --> procs_running 1234
  string line, key, value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") {
          return std::stoi(value);
        }
      }
    }
    filestream.close();
  }
  return 0;
}

string LinuxParser::Command(int pid) {
  string line, line2, cmd = string();
  std::ifstream filestream(LinuxParser::kProcDirectory + to_string(pid) +
                           LinuxParser::kCmdlineFilename);
  if (filestream.is_open()) {
    // Example line:
    // node^@./compute/node_modules/.bin/nodemon^@compute/main.js^@
    std::getline(filestream, line);
  }
  return line + line2;
}

float LinuxParser::CPUUsage(int pid){
  // Help from here https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
  const int MIN_INDEX = 14, MAX_INDEX = 17;
  string line, val;
  std::map<int, float> vals;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);  
  if (filestream.is_open()) {
    std::getline(filestream, line); 
    std::istringstream linestream(line);
    for(int i=1; i<MIN_INDEX; i++) linestream >> val; // skip the first MIN_INDEX elements
    for(int i=MIN_INDEX; i<=MAX_INDEX; i++){
      linestream >> val;
      vals[i] = std::stof(val);
    }
  }
  float totTimeSpent = vals[14] + vals[15] + vals[16] + vals[17];
  float seconds = UpTime() - UpTime(pid);
  return ((totTimeSpent / sysconf(_SC_CLK_TCK)) / seconds);
}

string LinuxParser::Ram(int pid) {
  string line, key, val, result = string();
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> val;
      if (key == "VmSize:") result = val;
    }
  }
  return result;
}

string LinuxParser::Uid(int pid) {
  string line, key, uid_str, result = string();
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> uid_str;
      if (key == "Uid:") result = uid_str;
    }
  }
  return result;
}

string LinuxParser::User(string uid) {
  // Search within /etc/passwd for a line like this --> david:x:1000:1000:David
  // Silver Help from
  // http://www.cplusplus.com/reference/cstdio/sscanf/?kw=sscanf Help on string
  // C-string conversion
  // https://www.geeksforgeeks.org/how-to-convert-c-style-strings-to-stdstring-and-vice-versa/
  string line, s, result = string();
  const char delim = ':';
  vector<string> vals = {"", "", ""};
  std::ifstream filestream_pswd(LinuxParser::kPasswordPath);
  if (filestream_pswd.is_open()) {
    while (std::getline(filestream_pswd, line)) {
      // Split the line into parts deliminated by ":"
      // https://www.techiedelight.com/split-string-cpp-using-delimiter/
      // Example line: redis:x:112:118::/var/lib/redis:/bin/false
      std::stringstream ss(line);
      for (int i = 0; i <= 2; i++) {
        getline(ss, s, delim);
        vals[i] = s;
      }
      if (vals[2] == uid) result = vals[0];
    }
  }
  return result;
}

long LinuxParser::UpTime(int pid) {
  // The index of the element to get within /proc/<pid>/stat according to
  // http://man7.org/linux/man-pages/man5/proc.5.html
  /* Example line for a process:
  1 (sh) S 0 1 1 0 -1 4194560 671 325636 9 375 4 0 641 175 20 0 1 0 2238 4612096
  174 18446744073709551615 ... and so on
  */
  const int MIN_INDEX = 14, MAX_INDEX = 17;
  string line, val;
  std::map<int, double> hash;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    // Is there an ideal way to iterate through linestream? Maybe using a
    // linstream "seek"?
    for (int i = 1; i < MIN_INDEX; i++) linestream >> val;
    for (int i = MIN_INDEX; i <= MAX_INDEX; i++) {
      linestream >> val;
      hash[i] = std::stof(val);
    }
    /*
    NOTE: I believe the instructions in the lessons are not accurate:
    instructions were to only grab the starttime of the process, which means
    how much time elapsed since boottime.
    That does not equate to how much time a process has been running/waiting.
        So, what I have done is add up all the scheduled run time of the process
    and divide it by system clock ticks to get the desried process uptime.
    (utime + stime + cutime + cstime) / sysconf(_SC_CLK_TCK)
    */
    return (hash[14] + hash[15] + hash[16] + hash[17]) / sysconf(_SC_CLK_TCK);
  }
  return 0;
}