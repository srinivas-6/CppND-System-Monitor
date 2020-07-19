#include "processor.h"
#include "linux_parser.h"
#include <vector>
#include <algorithm>
#include <string>

// This function Returns the aggregate CPU utilization

float Processor::Utilization() { 
    
  std::vector<std::string> cpu_string_data = LinuxParser::CpuUtilization();

  // initialize float vector with same size as returned by our CPU_Utilization function
  std::vector<float> cpu_elmts_float(cpu_string_data.size());

  // transform str to float

  std::transform(cpu_string_data.begin(), cpu_string_data.end(), cpu_elmts_float.begin(), [](const std::string &str){
    return std::stof(str);});
  
  //extract idle and non idle data w.r.t to each state
  const float idle_time = cpu_elmts_float[LinuxParser::CPUStates::kIdle_] + cpu_elmts_float[LinuxParser::CPUStates::kIOwait_];
  const float non_idle_time = cpu_elmts_float[LinuxParser::CPUStates::kUser_] + cpu_elmts_float[LinuxParser::CPUStates::kNice_] 
    		+ cpu_elmts_float[LinuxParser::CPUStates::kSystem_] + cpu_elmts_float[LinuxParser::CPUStates::kIRQ_]
    		+ cpu_elmts_float[LinuxParser::CPUStates::kSoftIRQ_] + cpu_elmts_float[LinuxParser::CPUStates::kSteal_];
  
  //calculate total util and return util
  const float total_time = idle_time + non_idle_time;
  return non_idle_time / total_time;
 }