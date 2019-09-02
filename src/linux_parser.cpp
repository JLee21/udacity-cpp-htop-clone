#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <unistd.h>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;
using std::cout;
using std::endl;

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
  }
  return value;
}

string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
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

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  // In my implementation, I will calculate:
  // Total used memory = MemTotal - MemFree
  // Non cache/buffer memory (green) which equals Total used memory - (Buffers + Cached memory)
  string MEMTOTAL = "MemTotal:";
  string MEMFREE = "MemFree:";
  string BUFFERS = "Buffers:";
  string CACHED = "Cached:";
  string key, value, line;
  std::map<string, float> hash;
  
  std::ifstream filestream(kProcDirectory+kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        hash[key] = std::stof(value);
      }
    }
  }
  double mem_total_used = hash[MEMTOTAL] - hash[MEMFREE];
  return (mem_total_used - (hash[BUFFERS] + hash[CACHED])) / hash[MEMTOTAL];
}

long LinuxParser::UpTime() {
  // System Uptime
  string uptime, idletime, line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime >> idletime;
  }
  // how to convert a string to a number (long)?
  // kernelhttps://www.geeksforgeeks.org/converting-strings-numbers-cc/
  // or use stof from std lib
   return std::stof(uptime);
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }

int LinuxParser::TotalProcesses() { 
  // IMPROVEMENT: abstract this funtion since it is similar to TotalProcesses and RunningProcesses
  // Parse a file's line for this --> processes 1234
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
  }
}

int LinuxParser::RunningProcesses() { 
  // Parse a file's line for this --> procs_running 1234
  string line;
  string key;
  string value;
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
  }
}

string LinuxParser::Command(int pid) { 
  string line, cmd;
  std::ifstream filestream(LinuxParser::kProcDirectory + to_string(pid) + LinuxParser::kCmdlineFilename);  
  if (filestream.is_open()) {
    // Example line: node^@./compute/node_modules/.bin/nodemon^@compute/main.js^@
    // Example line: /bin/bash^@-x^@/usr/local/bin/start_desktop.sh^@
    std::getline(filestream, line);
    // but only returns "node" or "/bin/bash" and not the entire line
    return line;
  }
}

string LinuxParser::Ram(int pid) {
  string line, key, val;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);  
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> val;
      if(key == "VmSize:")
		return val;
    }
  }		
}

string LinuxParser::Uid(int pid) { 
  string line, key, uid_str;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);  
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> uid_str;
      if(key == "Uid:")
		return uid_str;
    }
  }	
}

string LinuxParser::User(string uid) {
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
      std::stringstream ss(line);
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

long LinuxParser::UpTime(int pid) { 
  /* Example line for a process: 
  1 (sh) S 0 1 1 0 -1 4194560 671 325636 9 375 4 0 641 175 20 0 1 0 2238 4612096 174 18446744073709551615 94845009035264 94845009179164 1407
  31133395840 0 0 0 0 0 65538 1 0 0 17 2 0 0 7 0 0 94845011279720 94845011284512 94845044293632 140731133398418 140731133398461 140731133398
  461 140731133399024 0
  */
  
  // The index of the element to get within /proc/<pid>/stat according to http://man7.org/linux/man-pages/man5/proc.5.html
  const int INDEX = 22;
  string line, val;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);  
  if (filestream.is_open()) {
    std::getline(filestream, line); 
    std::istringstream linestream(line);
    for(int i=1; i<=INDEX; i++){
      linestream >> val;
    }
	return std::stof(val) / sysconf(_SC_CLK_TCK);
  }
}