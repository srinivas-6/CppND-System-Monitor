#ifndef FORMAT_H
#define FORMAT_H

#include <string>

namespace Format {
std::string ElapsedTime(long times);
std::string get_format(int t);   // TODO: See src/format.cpp
};                                    // namespace Format

#endif