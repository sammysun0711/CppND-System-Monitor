#include <string>
#include <iomanip>
#include "format.h"
#include "sstream"

using std::string;
static constexpr long ONE_HOUR = 60 * 60;
static constexpr long ONE_MINUTE = 60;

// DONE: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) { 
  long hour = seconds / ONE_HOUR;
  seconds-= hour * ONE_HOUR;
  long min = seconds / ONE_MINUTE;
  seconds -= min * ONE_MINUTE;
  long sec = seconds;
  std::ostringstream os;
  os << std::setw(2) << std::setfill('0') << hour << ":" 
     << std::setw(2) << std::setfill('0') << min << ":"
     << std::setw(2) << std::setfill('0') << sec;
  return os.str();
}