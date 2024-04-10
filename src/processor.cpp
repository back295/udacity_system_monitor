#include "processor.h"
#include "linux_parser.h"
#include <iostream>

long Processor::GetCurrentIdle() { return LinuxParser::IdleJiffies(); }

long Processor::GetCurrentTotal() { return LinuxParser::Jiffies(); }

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
  static long prevTotal{0}, prevIdle{0};
  long deltaTotal, deltaIdle;

  total_ = GetCurrentTotal();
  idle_ = GetCurrentIdle();

  deltaTotal = total_ - prevTotal;
  deltaIdle = idle_ - prevIdle;

  prevTotal = total_;
  prevIdle = idle_;

  return static_cast<float>(deltaTotal - deltaIdle) / deltaTotal;
}