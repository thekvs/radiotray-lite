#ifndef __OPTIONS_HPP_INCLUDED__
#define __OPTIONS_HPP_INCLUDED__

#include <iostream>
#include <memory>

#include <getopt.h>

namespace radiotray
{
class CmdLineOptions
{
public:
    CmdLineOptions() = default;

    bool parse(int argc, char** argv);
    void show_help();

    bool resume = false;
    bool help = false;
};

} // namespace

#endif
