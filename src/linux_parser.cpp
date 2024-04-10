#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <iostream>
#include <string>
#include <vector>

#define ACTIVE_TIME_START_INDEX 13
#define ACTIVE_TIME_END_INDEX 16
#define PROC_START_TIME_INDEX 22

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
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
  return string{};
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream filestream(kProcDirectory + kVersionFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;

    return kernel;
  }

  return string{};
}

// BONUS: Update this to use std::filesystem
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
  float mem_total, mem_free{0};
  string key;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  string line;

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::stringstream linestream(line);
      linestream >> key;
      if (key == "MemTotal:") {
        linestream >> mem_total;
      } else if (key == "MemFree:") {
        linestream >> mem_free;
        break;
      }
    }

    return (mem_total - mem_free) / mem_total;
  }

  return -1;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  string line, up_time_str;

  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::stringstream linestream(line);
    linestream >> up_time_str;

    return std::stol(up_time_str);
  }

  return -1;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  return LinuxParser::ActiveJiffies() + LinuxParser::IdleJiffies();
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
  string line, value;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatFilename);
  long total_time = -1;

  if (filestream.is_open()) {
    total_time = 0;
    std::getline(filestream, line);
    std::stringstream linestream(line);
    for (int i = 0; i <= ACTIVE_TIME_END_INDEX; i++) {
      linestream >> value;
      if (i >= ACTIVE_TIME_START_INDEX && i <= ACTIVE_TIME_END_INDEX) {
        total_time += std::stol(value);
      }
    }
  }
  // (utime + stime + cutime + cstime)/ sysconf(_SC_CLK_TCK)
  return total_time / sysconf(_SC_CLK_TCK);
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  vector<string> jiffies = CpuUtilization();

  return std::stol(jiffies.at(CPUStates::kUser_)) +
         std::stol(jiffies.at(CPUStates::kNice_)) +
         std::stol(jiffies.at(CPUStates::kSystem_)) +
         std::stol(jiffies.at(CPUStates::kIRQ_)) +
         std::stol(jiffies.at(CPUStates::kSoftIRQ_)) +
         std::stol(jiffies.at(CPUStates::kSteal_));
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  vector<string> jiffies = CpuUtilization();
  return std::stol(jiffies.at(CPUStates::kIdle_)) +
         std::stol(jiffies.at(CPUStates::kIOwait_));
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  vector<string> cpu_util;
  string line;
  string value, cpu;

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::stringstream linestream(line);
    linestream >> cpu;
    while (linestream >> value) {
      cpu_util.push_back(value);
    }

    return cpu_util;
  }

  return vector<string>{};
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  int total_process;
  string key;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  string line;

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::stringstream lstream(line);
      lstream >> key;
      if (key == "processes") {
        lstream >> total_process;
        return total_process;
      }
    }
  }

  return -1;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  int running_process;
  string key;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  string line;

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::stringstream linestream(line);
      linestream >> key;
      if (key == "procs_running") {
        linestream >> running_process;
        return running_process;
      }
    }
  }

  return -1;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  string line;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kCmdlineFilename);

  if (filestream.is_open()) {
    std::getline(filestream, line);
    return line;
  }
  return string{};
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  string line, key, used_ram;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatusFilename);

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::stringstream linestream(line);
      linestream >> key;
      if (key == "VmSize:") {
        linestream >> used_ram;
        return std::to_string(std::stoi(used_ram) / 1024);
      }
    }
  }

  return string{};
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  string line, key, value;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatusFilename);

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::stringstream linestream(line);
      while (linestream >> key) {
        if (key == "Uid") {
          linestream >> value;
          return value;
        }
      }
    }
  }

  return string{};
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  string line;
  string name{"default"}, temp, uid, key;
  std::ifstream filestream(kPasswordPath);

  uid = LinuxParser::Uid(pid);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::stringstream linestream(line);

      linestream >> name >> temp >> key;
      if (key == uid) {
        return name;
      }
    }
  }

  return string{};
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  string line, value;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatFilename);

  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::stringstream linestream(line);

    // start_time has index 22.
    for (int i = 0; i < PROC_START_TIME_INDEX; i++) {
      linestream >> value;
    }

    return std::stol(value) / sysconf(_SC_CLK_TCK);
  }

  return -1;
}