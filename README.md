# Theater App - Winsock

Console application developed for Distributed Systems class, for selling and buying theater tickets.

Project source code was written in its entirety in C++, and makes extensive use of the JSON library [JSON for Modern C++](https://github.com/nlohmann/json), by Niels Lohmann.

Communication between client and server was achieved using the Windows implementation of sockets, __Winsock__.

All functions are properly documented using the _Doxygen_ syntax, in hopes no confusion for their purpose is arisen.

## Message Structure

Messages sent by client and server are serialized in JSON and comprised of two fields:
- The code, corresponding to an _enum_ defined in [message.h](src/messages/message.h)
- The content, which can be anything from an integer, to a string, or even a serialized object / list of objects

### Example of a message:

```json
{
  "code": 0,
  "message": "HELLO"
}
```

__Note:__ 0 corresponds to a "HELLO" message.
