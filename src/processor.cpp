#include "processor.h"
#include <string>
#include "linux_parser.h"
#include <iostream> // TODO: remove me

using std::string;
using namespace LinuxParser;

float Processor::Utilization() { 
  /* What we're parsing:
	cpu  32057 13633 11500 707503 3672 0 443 0 0 0
   	https://github.com/Leo-G/DevopsWiki/wiki/How-Linux-CPU-Usage-Time-and-Percentage-is-calculated
    
    TODO: instead of using ints for the map's keys, use a list of strings.
  */
    string line, val, cpu_str;
	std::map<int, float> hash;
  	int counter = 0;
  
    std::ifstream stream(kProcDirectory + kStatFilename);
    if (stream.is_open()) {
      std::getline(stream, line);
      std::istringstream linestream(line);
      linestream >> cpu_str;
      while(linestream >> val){
        hash[counter] = std::stof(val);
        counter++;
      }
    }
  	double tot_cpu_time = 0;
  	for (int i=0; i <= 7; i++){
      tot_cpu_time += hash[i];
    }
 	float tot_cpu_idle_time = hash[3] + hash[4];
  	float tot_cpu_usage_time = tot_cpu_time - tot_cpu_idle_time;
  	float tot_cpu_perct = tot_cpu_usage_time / tot_cpu_time;
  
  	// Calculate the CPU usage since the last time interval
  	if (prev_usage != 0){
     	tot_cpu_perct = (tot_cpu_usage_time - prev_usage) / (tot_cpu_time - prev_total);
    }
  
  	// Cache previous results for the next calculdation
  	prev_total = tot_cpu_time;
  	prev_usage = tot_cpu_usage_time;
  
    return tot_cpu_perct;
}