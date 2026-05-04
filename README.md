*This project has been created as part of the 42 curriculum by "jdupuis" - Julien DUPUIS, "norabino" - Noé RABINOVICI.*

# IRC - Internet Relay Chat Server (C++98)

## Description

This project implements a custom IRC server in C++98 as part of the 42 curriculum.

The goal is to understand and implement the core mechanisms of a real-time network service:
- socket creation and configuration
- non-blocking I/O
- multiplexing with `poll`
- client registration and command parsing
- channel and user management

The server executable is named `ircserv` and accepts client connections on a chosen TCP port with a server password.

## Features

- C++98-compliant codebase
- Multiple client handling with `poll`
- Password-protected server access
- Nickname and username registration
- Channel creation and management
- Operator actions and channel moderation
- Private messaging and channel messaging

Implemented IRC-related commands in this project include:
- `PASS`
- `NICK`
- `USER`
- `JOIN`
- `TOPIC`
- `MODE`
- `KICK`
- `INVITE`
- `PRIVMSG`
- utility/help commands implemented in this codebase (`HELP`, `HELP_MODE`, `CHANNEL`, `CHANNEL_ON`)

## Instructions

### Requirements

- A C++ compiler supporting C++98 (for example `c++` or `clang++`)
- `make`
- A Unix-like environment (Linux/macOS)

### Compilation

From the project root:

```bash
make
```

This builds the `ircserv` executable.

Useful Makefile targets:
- `make`: build the project
- `make clean`: remove object files
- `make fclean`: remove object files and executable
- `make re`: full rebuild

### Execution

Run the server with:

```bash
./ircserv <port> <password>
```

Example:

```bash
./ircserv 6667 mypass
```

Argument rules:
- `port` must be between `1024` and `49151`
- `password` must not be empty

### Quick test with an IRC client

You can connect with a standard IRC client (for example HexChat, irssi, WeeChat) using:
- host: `127.0.0.1`
- port: the same port passed to `ircserv`
- server password: the password passed to `ircserv`

After connecting, register with `PASS`, `NICK`, and `USER`, then test channel and messaging commands.

Note: During development we used the HexChat client to test and run the IRC server.

## Project Structure

Main folders:
- `includes/`: headers (`Server`, `Client`, `Channel`, `Parser`)
- `srcs/`: source files by domain (`server`, `client`, `channel`)
- `subject/`: reference material and provided baseline files
- `objects/`: build artifacts

## Technical Choices

- Event loop based on `poll` for handling multiple file descriptors
- Non-blocking sockets to avoid blocking the whole server on one client
- Separation of responsibilities between server logic, parsing, command handling, and send utilities

## Resources

Classic references used for IRC and network programming:
- RFC 2812 (IRC architecture, channel management, and client protocol)
- Beej's Guide to Network Programming
- `man` pages: `socket(2)`, `bind(2)`, `listen(2)`, `accept(2)`, `poll(2)`, `fcntl(2)`

Useful links:
- https://www.rfc-editor.org/rfc/rfc2812
- https://beej.us/guide/bgnet/

### AI Usage Disclosure

AI tools were used in a limited and supervised way during this project, mainly for:
- improving documentation clarity and README structure
- quick explanations of IRC protocol details and socket API behavior
- proofreading small text sections

AI was not used as a replacement for implementation, debugging, or understanding of core server logic. All architectural decisions, coding, integration, and validation were performed by the project authors.

## Notes

This repository is an educational project and is not intended to be a production-ready IRC daemon.

