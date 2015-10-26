#include "fbynet.h"
#include <string.h>

using namespace Fby;


void Socket::onData(OnDataFunction on_data)
{
    this->on_data = on_data;
}

void Socket::onDrain(OnDrainFunction on_drain)
{
    this->on_drain = on_drain;
}
void Socket::onEnd(OnEndFunction on_end)
{
    this->on_end = on_end;
}
bool Socket::write(char const *data, size_t size)
{
//    fprintf(stderr, "Writing Socket %lx %*s to %d\n", (unsigned long)(this), (int)size, data, fd);
    bool wroteEverything(false);
    ssize_t bytes_written = ::write(fd, data, size);

//    fprintf(stderr, "Wrote %d\n", (int)bytes_written);
    if (bytes_written > 0)
    { 
        if (bytes_written < (ssize_t)size)
        {
            queuedWrite = std::string(data+bytes_written, size - bytes_written);
        }
        else
        {
            wroteEverything  = true;
            emitDrain = true;
        }
    }
    else if (bytes_written < 0 || size)
    {
        perror("Error writing");
    }
    return wroteEverything;
}

bool Socket::write(const std::string &s)
{
    return write(s.data(), s.length());
}
bool Socket::write(const char *data)
{
    return write(data, strlen(data));
}

bool Socket::end(const char *data, size_t length)
{
//        std::cout << "Writing end " << (unsigned long)(this) << std::endl;
    doneWithWrites = true;
    return write(data, length);
}
bool Socket::end(const std::string &s)
{
//        std::cout << "Writing end " << (unsigned long)(this) << std::endl;
    doneWithWrites = true;
    return write(s);
}
bool Socket::end(const char *data)
{
//        std::cout << "Writing end " << (unsigned long)(this) << std::endl;
    doneWithWrites = true;
    return write(data);
}


Socket::Socket()
: BaseObj(BASEOBJINIT(Socket)),
              fd(-1), 
              on_data([](const char *, size_t){}), 
              on_drain([](){}),
              on_end([](){}),
              net(),
              queuedWrite(), emitDrain(false),
              doneWithWrites(false)
{
}

void Socket::SetSocketServerAndFile(Net* net, int fd)
{
//    std::cout << "Setting socket server and file " << fd << std::endl;
    this->net = net;
    this->fd = fd;
}
