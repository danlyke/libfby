#include "fbynet.h"
#include <string>
#include <iostream>
using namespace std;
using namespace Fby;

int main(int /* argc */, char ** /* argv */)
{
    Net *net = new Net();

    net->createServer([](SocketPtr socket)
                     {
                         socket->onData(
                             [socket](const char *data, int length)
                             {
                                 string s("Got ");
                                 s += string(data, length);
                                 s += "\n";
                                 cout << s;
                                 socket->write(s);
                             });
                     })->listen(5000);
    net->loop();
}
