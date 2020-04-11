#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>
#include "linux_parser.h"

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
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
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

// DONE: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  long mem_total = 0;
  long mem_free = 0;
  string line;
  string key;
  string value;
  string unit;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value >> unit) {
        if (key == "MemTotal") {
          mem_total = std::stol(value);
        }
        if (key == "MemFree") {
          mem_free = std::stol(value);
        }
      }
    }
    return static_cast<double>(mem_total - mem_free) / mem_total;
  }
  return 0.0;
}

// DONE: Read and return the system uptime
long LinuxParser::UpTime() {
  string uptime;
  string idle_time;
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()){
    filestream >> uptime >> idle_time;
    return static_cast<long>(std::stold(uptime));
  }
  return 0; 
}

// DONE: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return LinuxParser::ActiveJiffies() + LinuxParser::IdleJiffies(); }

// DONE: Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
    std::ifstream filestream(kProcDirectory + "/" + std::to_string(pid) + kStatFilename);
    string value;
    string line;
    long utime{0};
    long stime{0};
    //long cutime{0};
    //long cstime{0};
    if (filestream.is_open()) {
      if(std::getline(filestream, line)) {
        std::istringstream linestream(line);
        int counter = 0;
        while (linestream >> value) {
          counter++;
          if (counter == 14) utime = std::stol(value);
          else if (counter == 15) stime = std::stol(value);
          //else if (counter == 16) cutime = std::stol(value);
          //else if (counter == 17) cstime = std::stol(value);
        }
        //return (utime + stime + cutime + cstime) / sysconf(_SC_CLK_TCK);
        return (utime + stime) / sysconf(_SC_CLK_TCK);
      }
    }
  return 0; 
}

// DONE: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
    vector<string> cpu_stat = LinuxParser::CpuUtilization();
    long user = std::stol(cpu_stat.at(LinuxParser::CPUStates::kUser_));
    long nice = std::stol(cpu_stat.at(LinuxParser::CPUStates::kNice_));
    long system = std::stol(cpu_stat.at(LinuxParser::CPUStates::kSystem_));
    long irq = std::stol(cpu_stat.at(LinuxParser::CPUStates::kIRQ_));
    long softirq = std::stol(cpu_stat.at(LinuxParser::CPUStates::kSoftIRQ_));
    long steal = std::stol(cpu_stat.at(LinuxParser::CPUStates::kSteal_));
    return user + nice + system + irq + softirq + steal;
}
// DONE: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies(){
  vector<string> cpu_stat = LinuxParser::CpuUtilization();
  long idle = std::stol(cpu_stat.at(LinuxParser::CPUStates::kIdle_));
  long iowait = std::stol(cpu_stat.at(LinuxParser::CPUStates::kIOwait_));
  return idle + iowait;
}

// DONE: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  std::ifstream filestream(kProcDirectory + kStatFilename);
  vector<string> cpu_stat;
  string line;
  string value;
  if (filestream.is_open()){
    while(std::getline(filestream, line)){
      std::istringstream linestream(line);
      while (linestream >> value) {  
        if (value == "cpu") continue;
        else cpu_stat.push_back(value);
      }
      return cpu_stat;
    }    
  }
  return {}; 
}

// DONE: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  std::ifstream filestream(kProcDirectory + kStatFilename);
  string key;
  string value;
  string line;
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processes") return std::stoi(value);
      }
    }
  }
  return 0; 
}

// DONE: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  std::ifstream filestream(kProcDirectory + kStatFilename);
  string key;
  string value;
  string line;
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") return std::stoi(value);
      }
    }
  }
  return 0;
}

// DONE: Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  std::ifstream filestream(kProcDirectory + "/" + std::to_string(pid) + kCmdlineFilename);
  string line;
  if (filestream.is_open()) {
    if (std::getline(filestream, line)) return line;
  }
  return string(); 
}

// DONE: Read and return the memory used by a process
string LinuxParser::Ram(int pid) { 
  std::ifstream filestream(kProcDirectory + "/" + std::to_string(pid) + kStatusFilename);
  string key;
  string value;
  string line;
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "VmSize") return value;
      }
    }
  }
  return string();
}

// Done: Read and return the user ID associated with a process
// Done: REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) { 
  std::ifstream filestream(kProcDirectory + "/" + std::to_string(pid) + kStatusFilename);
  string key;
  string value;
  string line;
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Uid") return value;
      }
    }
  }
  return string();
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  std::ifstream filestream(kPasswordPath);
  string user;
  string tmp;
  string value;
  string line;
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> user >> tmp >> value) {
        if (value == Uid(pid)) return user;
      }
    }
  }
  return string(); 
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  std::ifstream filestream(kProcDirectory + "/" + std::to_string(pid) + kStatFilename);
  string value;
  string line;
  if (filestream.is_open()) {
    if(std::getline(filestream, line)) {
      std::istringstream linestream(line);
      int counter = 0;
      while (linestream >> value) {
        counter++;
        if (counter == 22) return std::stol(value) / sysconf(_SC_CLK_TCK);
      }
    }
  }
  return 0; 
}