#include "tray.hpp"

using namespace radiotray;

int
main(int argc, char *argv[])
{
    RadioTrayLite(argc, argv).run();

    return 0;
}

