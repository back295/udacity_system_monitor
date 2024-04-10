#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();  // TODO: See src/processor.cpp
  long GetCurrentIdle();
  long GetCurrentTotal();

  // TODO: Declare any necessary private members
 private:
  long total_;
  long idle_;
};

#endif