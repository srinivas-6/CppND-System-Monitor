#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

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

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  
  string key,value;
  float memTotal, memFree;
  string line;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open())
   {
    while(std::getline(stream, line))
    {
    std::replace(line.begin(),line.end(),':',' ');
    std::istringstream linestream(line);
    while(linestream >> key >> value)
      {
      if(key == "MemTotal")
        //cout << value << "\n";
         memTotal = std::stof(value);
      if(key == "MemFree")
        //cout << value << "\n";
         memFree = std::stof(value);
      break;
      }
    }
   
  }
 return ((memTotal - memFree)/memTotal);
 }

// TODO: Read and return the system uptime
long LinuxParser::UpTime() { 

  string line;
  long uptime;
  std::ifstream stream(kProcDirectory+kUptimeFilename);
  if(stream.is_open())
  {
    std::getline(stream,line);
    std::istringstream linestream(line);
    linestream >> uptime;

  }
  return uptime;

 }

// helper function to get process 

vector <string> LinuxParser::Process_Utilization_Data(int pid) { 

  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  string line, pid_stat;
  vector<string> pidStats; 
  if(stream.is_open())
  {
    
    std::getline(stream, line);
    stream.close();
    std::istringstream linestream(line);
    
    while(linestream >> pid_stat)
      pidStats.emplace_back(pid_stat);
    
    return pidStats;
  }
  return {0};
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  return ActiveJiffies() + IdleJiffies();
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) { 
  
  long ActiveJiffies = 0;
  vector<string> pidStats = Process_Utilization_Data(pid);
  ActiveJiffies+=std::stol(pidStats[13]) + std::stol(pidStats[14]) + 
    std::stol(pidStats[15]) + std::stol(pidStats[16]);
  
  return ActiveJiffies; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  long ActiveJiffies = 0;
  vector<CPUStates> active_states {kUser_,kNice_,kSystem_,kIRQ_,kSoftIRQ_,kSteal_};
  vector <string> utilization_data = CpuUtilization();
  for(auto state:active_states)
    ActiveJiffies+=stol(utilization_data[state]);
  return ActiveJiffies; }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  
  long IdleJiffies = 0;
  vector<CPUStates> idle_states {kIdle_,kIOwait_};
  vector <string> utilization_data = CpuUtilization();
  for (auto state:idle_states)
    IdleJiffies+=stol(utilization_data[state]);
  
  return IdleJiffies; }

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {


    std::string line;
    std::string key, user, nice,system,idle, iowait, irq, softirq, steal, guest, guest_nice;

    vector <string> cpuvalues{};
    std::ifstream stream(kProcDirectory + kStatFilename);
    if (stream.is_open()) {
      while(std::getline(stream, line)){
      std::istringstream lineStream(line);
      while(lineStream >> key >> user >> nice >> system >> idle >>
            iowait >> irq >> softirq >> steal >> guest >>guest_nice)
            {
              if(key=="cpu")
                {
                  cpuvalues.emplace_back(user);
                  cpuvalues.emplace_back(nice);
                  cpuvalues.emplace_back(system);
                  cpuvalues.emplace_back(idle);
                  cpuvalues.emplace_back(iowait);
                  cpuvalues.emplace_back(irq);
                  cpuvalues.emplace_back(softirq);
                  cpuvalues.emplace_back(steal);
                  cpuvalues.emplace_back(guest);
                  cpuvalues.emplace_back(guest_nice);
                }
            }

      }
    }
  return cpuvalues;

}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {

    string line;
    string key, value;
    std::ifstream stream(kProcDirectory + kStatFilename);
    if (stream.is_open()){
      while(std::getline(stream,line))
      {
        std::istringstream linestream(line);
        while (linestream >> key >> value)
          {
            if(key=="processes")
          {
            long total_processes = stoi(value);
            return total_processes;
            break;
          }
          }
      }  
    }
    return 0;
 }

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() { 

    string line;
    string key, value;
    std::ifstream stream(kProcDirectory + kStatFilename);
    if (stream.is_open()){
      while(std::getline(stream,line))
      {
        std::istringstream linestream(line);
        while (linestream >> key >> value)
          {
            if(key=="procs_running")
          {
            long total_processes = stoi(value);
            return total_processes;
            break;
          }
          }
      }  
    }
  return 0;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) { 
  string line;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {std::getline(stream, line); }
  return line;
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
    string line;
    string key, value;
    std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
    if(stream.is_open())
    {
      while(std::getline(stream,line))
      {
        
        std::replace(line.begin(), line.end(),':',' ');

        std::istringstream linestream(line);
        while(linestream >> key >> value)
        {
          //cout << key << "\n";
          if(key == "VmSize")
          {
            int value_ = stoi(value)/1024; // convert to MB
            return std::to_string(value_);
            break;
          }
        }
      }
    }
  return 0;
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) { 

  string line;
  string key, value;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if(stream.is_open())
  {
    while(std::getline(stream,line))
    {
      
      std::replace(line.begin(), line.end(),':',' ');

      std::istringstream linestream(line);
      while(linestream >> key >> value)
      {
        //cout << key << "\n";
        if(key == "Uid")
        {
          return value;
          
          break;

        }
      }
    }
  }
  return 0;
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {

    string line;
    string key, username, x;

    string uid = Uid(pid);
    std::ifstream stream(kPasswordPath);
    if(stream.is_open())
    {
      while(std::getline(stream,line))
      {
        
        std::replace(line.begin(), line.end(),':',' ');

        std::istringstream linestream(line);
        while(linestream >> username >> x >> key)
        {
          //cout << key << "\n";
          if(key == uid)
          {
            return username;
            
            break;

          }
        }
      }
  }
  return 0;

}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) { 
  string line;
  string key,ignore;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if(stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    for (int i = 0; i < 21; i++)
		{
		linestream >> ignore;
		}
		linestream >> key;
		long value = std::stol(key)/sysconf(_SC_CLK_TCK);

    
		return value;
	}
	return 0;
}