# slnp - IPC over named pipes
SLNP is a simple IPC library without other external dependencies, but, it can only work with Windows Named Pipes.

## Building and Installation
This project can be built using CMake. You can configure to build Logger and Documentation optionally.

Build produces two static libraries and example applications which demonstrate API usage.

Libraries and applications are installed under /lib and /bin directories respectively.

## Components
To have a class available for IPC, you just derive from IPCTarget (see /example/Sample.h).

Instead of using another library for serialization, Value, Request and Response objects are used for transferring primitive types via strings over pipes for simplicity.

Registry class is used for looking up objects and managing them. The server uses Registry to match classes and instances.

## Communication
The server creates a new thread and a pipe to handle requests for each client. Reads and writes to this pipe is sync.

The client uses a pipe to send Requests and receive responses. Access to this pipe is controlled via a mutex. However, clients can make async calls via API using futures.

## TODO
Add more tests