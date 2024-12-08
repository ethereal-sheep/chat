#include "terminal.h"

#include <cctype>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

class raw_mode_impl {
public:
    raw_mode_impl() {
        enable_raw_mode();
    }
    ~raw_mode_impl() {
        disable_raw_mode();
    }

    /**
    Tutorial and explanation here:
    https://viewsourcecode.org/snaptoken/kilo/02.enteringRawMode.html

    High level idea is to:
    1. disable canonical mode so that we can read bytes directly on input
    2. disable terminate and suspend signals so that we can end gracefully
    3. disable echoing so that we can properly display our chat messages

    We do this so that we have greater control on how text is written to
    the terminal, as well as how we capture user input.
     */
    void enable_raw_mode() {
        if (tcgetattr(STDIN_FILENO, &original_) == -1) {
            throw std::runtime_error("failed on tcgetattr");
        }
        termios raw = original_;
        raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
        raw.c_oflag &= ~(OPOST);
        raw.c_cflag |= (CS8);
        raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
        raw.c_cc[VMIN] = 0;
        raw.c_cc[VTIME] = 1;
        if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) {
            throw std::runtime_error("failed on tcsetattr");
        }
    }

    void disable_raw_mode() {
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_);
    }

private:
    termios original_;
};

// use pimpl + meyer singleton idiom to enable raw mode throught program lifetime
// only when program is complete then we turn off raw mode
raw_mode_impl& get_raw_mode_impl() {
    static raw_mode_impl impl;
    return impl;
}

// can be extended to allow disabling raw mode, but not needed for this program
void terminal::enable_raw_mode() {
    get_raw_mode_impl();
}

terminal::input_event make_printable(char c) {
    assert(std::isprint(c));
    terminal::input_event event;
    event.ctrl = false;
    event.enter = false;
    event.backspace = false;
    event.c = c;
    event.raw = c;
    return event;
}

terminal::input_event make_backspace() {
    terminal::input_event event;
    event.ctrl = false;
    event.enter = false;
    event.backspace = true;
    event.c = 0;
    event.raw = 127;
    return event;
}

terminal::input_event make_ctrl(char c) {
    assert(1 <= c && c <= 26);
    terminal::input_event event;
    event.ctrl = true;
    event.enter = c == 13; // enter shares 13 with ctrl-m
    event.backspace = false;
    event.c = 'a' + (c - 1);
    event.raw = c;
    return event;
}

void terminal::poll_for_input(std::function<void(input_event)> event_handler,
                              std::function<bool()> pred) {
    // we already set VTIME = 1 in enable_raw_mode
    // but ideally we set it in this fn in the future

    while (true) {
        char c = '\0';
        if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN) {
            throw std::runtime_error("failed on read");
        }

        // do not bother continuing
        if (pred()) {
            break;
        }

        // timeout; no input
        if (c == '\0') {
            continue;
        }

        if (std::isprint(c)) {
            // printable character
            event_handler(make_printable(c));
        } else if (1 <= c && c <= 26) {
            // ctrl character
            event_handler(make_ctrl(c));
        } else if (c == 127) {
            // backspace character
            event_handler(make_backspace());
        }
        // ignore everything else

        // try to replicate SIGINT behavior by breaking the poll here
        // since we prevented the terminal from receiving the signal
        // TODO find a way to replicate SIGTSTP
        if (c == '\3') {
            break;
        }
    }
}