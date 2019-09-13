#include "processor.h"
#include <string>
#include "linux_parser.h"

using std::string;
using namespace LinuxParser;

double Processor::Utilization() {
  /* What we're parsing from /proc/stat
      cpu  32057 13633 11500 707503 3672 0 443 0 0 0

      https://github.com/Leo-G/DevopsWiki/wiki/How-Linux-CPU-Usage-Time-and-Percentage-is-calculated

    clarity.
  */
  // TODO: instead of using ints for the map's keys, use a list of strings for
  string line, val, cpu_str;
  std::map<int, double> hash;
  int counter = 0;

  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> cpu_str;
    while (linestream >> val) {
      hash[counter] = std::stod(val);
      counter++;
    }
  }
  double totCpuTime = 0;
  for (int i = 0; i <= 7; i++) totCpuTime += hash[i];
  double totCpuIdleTime = hash[3] + hash[4];
  double totCpuUsageTime = totCpuTime - totCpuIdleTime;
  double totCpuPerct = totCpuUsageTime / totCpuTime;

  // Calculate the CPU usage since the last time interval
  if (prevUsage != 0)
    totCpuPerct = (totCpuUsageTime - prevUsage) / (totCpuTime - prevTotal);

  // Cache previous results for the next calculation
  prevTotal = totCpuTime;
  prevUsage = totCpuUsageTime;

  return totCpuPerct;
}