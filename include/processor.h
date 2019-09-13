#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  double Utilization();  // TODO: See src/processor.cpp

  // TODO: Declare any necessary private members
 private:
  float prevTotal = 0.0;
  float prevUsage = 0.0;
};

#endif