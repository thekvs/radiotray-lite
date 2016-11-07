#ifndef __KEYBOARD_CONTROL_HPP_INCLUDED__
#define __KEYBOARD_CONTROL_HPP_INCLUDED__

#include <memory>
#include <vector>
#include <string>

#include <stdlib.h>
#include <termios.h>

#include <gstreamermm.h>

namespace radiotray
{

class Player;

class KeyboardControl
{
public:
    KeyboardControl(std::shared_ptr<Player> player, std::vector<std::string> stations)
        : player(player)
        , stations(stations)
    {
    }

    KeyboardControl() = delete;
    KeyboardControl(const KeyboardControl&) = delete;

    void operator()()
    {
        std::cout << "Press <space> to stop/resume playing." << std::endl;
        std::cout << "Press n/p to play next/previous station." << std::endl;

        size_t index = 0;
        auto stations_count = stations.size();

        player->play(stations[index]);

        while (true) {
            auto c = getch();

            if (c == kPauseCommand) {
                if (paused) {
                    player->start();
                } else {
                    player->stop();
                }
                paused = not paused;
            } else if (c == kNextStationCommand) {
                auto new_index = (stations_count > 1 ? (index + 1) % stations_count : index);
                if (new_index != index) {
                    player->play(stations[new_index]);
                    index = new_index;
                }
            } else if (c == kPreviousStationCommand) {
                if (stations_count > 1) {
                    auto new_index = (index == 0 ? stations_count - 1 : (index - 1) % stations_count);
                    if (new_index != index) {
                        player->play(stations[new_index]);
                        index = new_index;
                    }
                }
            }
        }
    }

private:
    std::shared_ptr<Player> player;
    std::vector<std::string> stations;

    static const int kPauseCommand = ' ';
    static const int kNextStationCommand = 'n';
    static const int kPreviousStationCommand = 'p';

    bool paused = false;

    int
    getch()
    {
        static struct termios oldt, newt;
        tcgetattr(STDIN_FILENO, &oldt); // save old settings
        newt = oldt;
        newt.c_lflag &= ~(ICANON); // disable buffering
        newt.c_lflag &= ~(ECHO); // disable echo
        tcsetattr(STDIN_FILENO, TCSANOW, &newt); // apply new settings

        int c = getchar(); // read character
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // restore old settings

        return c;
    }
};

} // namespace radiotray

#endif
