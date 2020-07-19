#include <string>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function

string Format::get_format(int t)
{
    std::string time = std::to_string(t);
    if(time.length() < 2) {
        time = "0" + time;
    }
    return time;
}



string Format::ElapsedTime(long seconds) { 
    long int h = seconds/3600;
    long int m = (seconds - h*3600)/60;
    long int s = seconds - h*3600 - m*60;
    std::string elapsedTime = get_format(h) + ":" + get_format(m) + ":" + get_format(s);
    return elapsedTime; 
    
    }




