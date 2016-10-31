#ifndef __KEYBOARD_IO_HPP_INCLUDED__
#define __KEYBOARD_IO_HPP_INCLUDED__

#include <stdlib.h>
#include <termios.h>

#include <gstreamermm.h>

namespace radiotray {

class KeyboardInputThread
{
public:
    KeyboardInputThread(Glib::RefPtr<Gst::PlayBin2> pb)
        : playbin(pb)
    {
    }

    KeyboardInputThread() = delete;
    KeyboardInputThread(const KeyboardInputThread&) = delete;

    void operator()()
    {
        std::cout << "Press <space> to stop/resume playing" << std::endl;

        while (true) {
            int c = getch();
            if (c == kPauseCommand) {
                if (paused) {
                    playbin->set_state(Gst::STATE_PLAYING);
                } else {
                    playbin->set_state(Gst::STATE_NULL);
                }
                paused = not paused;
            }
        }
    }

private:
    Glib::RefPtr<Gst::PlayBin2> playbin;

    static const int kPauseCommand = ' ';
    bool paused = false;

    int getch()
    {
        static struct termios oldt, newt;
        tcgetattr(STDIN_FILENO, &oldt); // save old settings
        newt = oldt;
        newt.c_lflag &= ~(ICANON);               // disable buffering
        newt.c_lflag &= ~(ECHO);                 // disable echo
        tcsetattr(STDIN_FILENO, TCSANOW, &newt); // apply new settings

        int c = getchar();                       // read character
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // restore old settings

        return c;
    }
};

} // namespace radiotray

#endif
