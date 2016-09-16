#include "fbynet.h"
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>

using namespace std;
using namespace Fby;



int main(int /* argc */, char ** /* argv */)
{
    Net *net = new Net();

    net->createServer([](SocketPtr socket)
                     {
                         HTTPRequestBuilderPtr requestBuilder
                             (new HTTPRequestBuilder
                              (socket,
                                  [](HTTPRequestPtr request, HTTPResponsePtr response)
                                  {
                                      cout << "Headers" << endl;
                                      for (auto v = request->headers.begin();
                                           v != request->headers.end();
                                           ++v)
                                      {
                                          cout << "   " << v->first << " : " << v->second << endl;
                                      }
                                      if (!ServeFile("../t/html", request,response))
                                      {
                                      }
                                  }
                                  ));
                         
                         socket->onData(
                             [socket, requestBuilder](const char *data, int length)
                             {
                                 requestBuilder->ReadData(data, length);
                             });
                     })->listen(5000);
    net->loop();
}
