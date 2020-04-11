#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

// Constructor
Process::Process(int pid, long system_uptime){
    pid_ = pid;
    user_ = LinuxParser::User(pid);
    command_ = LinuxParser::Command(pid);
    ram_ =  std::stol(LinuxParser::Ram(pid));
    process_uptime_ = LinuxParser::UpTime(pid);
    active_jiffies_ = LinuxParser::ActiveJiffies(pid);
    system_uptime_ = system_uptime;
    process_uptime_ = process_uptime_;
    cpu_utilization_ = CpuUtilization();
}
// DONE: Return this process's ID
int Process::Pid() { return pid_; }

// DONE: Return this process's CPU utilization
float Process::CpuUtilization() {
    return static_cast<float>(active_jiffies_) / this->UpTime();
}

// DONE: Return the command that generated this process
string Process::Command() { return command_; }

// DONE: Return this process's memory utilization
string Process::Ram() { return std::to_string(ram_/1000); }

// DONE: Return the user (name) that generated this process
string Process::User() { return user_; }

// DONE: Return the age of this process (in seconds)
long int Process::UpTime() { return system_uptime_ - process_uptime_; }

// DONE: Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const { 
    return this->cpu_utilization_ > a.cpu_utilization_;
}