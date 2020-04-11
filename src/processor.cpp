#include "processor.h"
#include "linux_parser.h"
#include "iostream"

// DONE: Return the aggregate CPU utilization
float Processor::Utilization() {
  long current_total = LinuxParser::Jiffies();
  long current_idle = LinuxParser::IdleJiffies();
  long total_diff = current_total - prev_total_;
  long idle_diff = current_idle - prev_idle_;
  prev_total_ = current_total;
  prev_idle_ = current_idle;
  return static_cast<float>(total_diff - idle_diff) / total_diff;
}