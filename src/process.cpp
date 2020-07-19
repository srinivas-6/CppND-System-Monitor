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

Process::Process(int pid ):pid_(pid), cpuusage_(CpuUtilization()){}
// TODO: Return this process's ID
int Process::Pid() { return pid_; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() { 
  unsigned long int ActiveJiffies = LinuxParser::ActiveJiffies(Pid());
  
  long int pTime = LinuxParser::UpTime(Pid());
  long int sTime = LinuxParser::UpTime();
  
  if(sTime - pTime <= 0)
    return 0.0;
  else
     return (float) (1.0*(ActiveJiffies/sysconf(_SC_CLK_TCK)) / (sTime - pTime));
  }

// TODO: Return the command that generated this process
string Process::Command() { return LinuxParser::Command(Pid()); }

// TODO: Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(Pid()); }

// TODO: Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(Pid()); }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(Pid()); }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const { 
  return (cpuusage_ > a.cpuusage_); 
}