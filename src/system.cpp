#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>
#include <iostream>

#include "process.h"
#include "processor.h"
#include "system.h"
#include "linux_parser.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

// DONE: Return the system's CPU
Processor& System::Cpu() { return cpu_;}

// DONE: Return a container composed of the system's processes
vector<Process>& System::Processes() {
    processes_.clear();
    processes_.shrink_to_fit();
    for (const int pid : LinuxParser::Pids()){
        Process p = Process(pid, this->UpTime());
        processes_.emplace_back(p);
    }
    std::sort(processes_.begin(), processes_.end());
    return processes_;
}

// DONE: Return the system's kernel identifier (string)
std::string System::Kernel() {
    if (kernel_.empty()) kernel_ = LinuxParser::Kernel();
    return kernel_;
}
// DONE: Return the system's memory utilization
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

// DONE: Return the operating system name
std::string System::OperatingSystem() {
    if (operating_system_.empty()) operating_system_ = LinuxParser::OperatingSystem();
    return operating_system_;
}

// DONE: Return the number of processes actively running on the system
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

// DONE: Return the total number of processes on the system
int System::TotalProcesses() { 
    if (total_process_ == 0){ total_process_ = LinuxParser::TotalProcesses(); }
    return total_process_;
}

// DONE: Return the number of seconds since the system started running
long int System::UpTime() { return LinuxParser::UpTime(); }