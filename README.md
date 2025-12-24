# C++ IRC Server

This project is an implementation of an Internet Relay Chat (IRC) server in C++98. 

The server is designed to handle multiple clients simultaneously using non-blocking I/O with `poll()`. It supports basic IRC features such as user registration, joining channels, and sending private messages.

## How to use


- **Compile:** `make`
- **Run:** `./ircserv <port> <password>`
- **Connect:** `nc -C localhost <port>`

### Registration

To register with the server, send the following commands in order:
1.  `PASS <password>`
2.  `NICK <your_nickname>`
3.  `USER <username> 0 * :<realname>`

### Joining a Channel
- `JOIN #<channelname>`

### Sending Messages

Once in a channel, you can send messages.

- **To a channel:** `PRIVMSG #<channelname> :Your message here`
  - Example: `PRIVMSG #chat :Hello everyone!`
- **To a user directly:** `PRIVMSG <nickname> :Your private message here`
  - Example: `PRIVMSG bob :Hi Bob!`

### Channel Operator Commands

If you are the operator of a channel (i.e., you were the first to join), you can use the following commands:

- **KICK:** Eject a user from the channel.
  - `KICK #<channelname> <nickname> :<reason>`
- **INVITE:** Invite a user to a channel (especially useful for invite-only channels).
  - `INVITE <nickname> #<channelname>`
- **TOPIC:** Change or view the channel topic.
  - `TOPIC #<channelname> :New topic` (to set)
  - `TOPIC #<channelname>` (to view)
- **MODE:** Change channel modes. The syntax is `MODE #<channel> <+/-><mode> [parameter]`.

  - **i: Invite-only**
    - `MODE #chat +i` (Makes the channel invite-only)
    - `MODE #chat -i` (Removes the invite-only restriction)
  - **t: Topic protection**
    - `MODE #chat +t` (Only operators can change the topic)
    - `MODE #chat -t` (Any user in the channel can change the topic)
  - **k: Channel key (password)**
    - `MODE #chat +k <password>` (Sets a password required to join)
    - `MODE #chat -k` (Removes the password)
  - **o: Operator status**
    - `MODE #chat +o <nickname>` (Gives operator status to a user)
    - `MODE #chat -o <nickname>` (Removes operator status from a user)
  - **l: User limit**
    - `MODE #chat +l <number>` (Sets a maximum number of users for the channel)
    - `MODE #chat -l` (Removes the user limit)
  - **Combining modes:** You can combine modes in one command. The parameters must follow in the same order.
    - `MODE #chat +o-i bob` (Gives bob operator status and removes invite-only)
    - `MODE #chat +lk 10 secret` (Sets user limit to 10 and sets a password)


## Test Commands :

### Terminal 1
./ircserv 6667 test

### Terminal 2
nc -C localhost 6667
PASS test
NICK will
USER will0 0 * :Wilfried
JOIN #chan
PRIVMSG #chan :This is a test
TOPIC #chan :TESTOPIC
TOPIC #chan
INVITE tom #chan
KICK #chan tom :test
MODE #chan +i
MODE #chan t
MODE #chan k
MODE #chan o
MODE #chan l

### Terminal 3
nc -C localhost 6667
PASS test
NICK tom
USER tom0 0 * :Thomas
JOIN #chan
PRIVMSG #chan :This is a msg testtttt
PRIVMSG will :This is a priv msg test

### Terminal 4
nc -C localhost 6667
PASS test
NICK mehdi
USER meh0 0 * :di
JOIN #chan
