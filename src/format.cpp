#include "format.h"

#include <string>

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
  string hours = (std::to_string(seconds / 3600).length() > 1)
                     ? std::to_string(seconds / 3600)
                     : "0" + std::to_string(seconds / 3600);
  string minitues = (std::to_string((seconds % 3600) / 60).length() > 1)
                        ? std::to_string((seconds % 3600) / 60)
                        : "0" + std::to_string((seconds % 3600) / 60);
  string secs = (std::to_string(seconds % 60).length() > 1)
                    ? std::to_string(seconds % 60)
                    : "0" + std::to_string(seconds % 60);

  return hours + ":" + minitues + ":" + secs;
}