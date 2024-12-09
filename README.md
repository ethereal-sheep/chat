# Chat

A 1-to-1 chat application written in `c++20`, built with `boost::asio`.

## Build & Run

Install `chat` with cmake. Running the below commands should build the `chat` application into the `build` directory.

NOTE: Make sure `boost` has been installed on your system. I use `boost:1.84.0`.

```bash
cd chat
cmake -S . -B build
cmake --build build
```

Then, simple run the app in server mode, or client mode. Client mode optionally accepts a host address, but defaults to localhost `127.0.0.1`.

```bash
./build/chat server
# or
./build/chat client
```

## Running Tests

To run tests, run the following command. Make sure you are in the `chat` project folder, and have already built the project.

```bash
ctest --test-dir build
```
