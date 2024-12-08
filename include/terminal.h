#ifndef TERMINAL_H
#define TERMINAL_H

#include <functional>
#include <iostream>
#include <optional>

// simple concept for streamable objects
template <typename T>
concept Streamable = requires(std::ostream os, T value) {
                         { os << value };
                     };

namespace terminal {
// currently only supporting printable chars, ctrl a-z, enter, and backspace
// multi-bytes wide keys like arrow keys won't be handled,
// but it's something to consider for the future
struct input_event {
    bool ctrl;
    bool enter;
    bool backspace;
    char c;
    char raw;
};

// enables raw mode (do not echo, capture input immediately, etc.)
void enable_raw_mode();

// polls for input;
// calls event_handler on input event;
// stops polling if predicate returns true
void poll_for_input(std::function<void(input_event)> event_handler, std::function<bool(void)> pred);

// helper fns to write to terminal; use stderr since it always flush;
// TODO enable write with ANSI color or text styling like bold, italic, etc.
inline void write(Streamable auto obj) {
    std::cerr << obj;
}
inline void writeln(Streamable auto obj) {
    std::cerr << obj << "\r\n";
}
inline void clearln() {
    std::cerr << "\r\e[K";
}
inline void backspace() {
    std::cerr << "\b \b";
}

} // namespace terminal

#endif // TERMINAL_H
