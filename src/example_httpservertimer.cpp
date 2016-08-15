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

    auto timer = net->setInterval([]() { cout << "Timer!" << endl; }, 10 * 1000);

    net->createServer([net](SocketPtr socket)
                     {
                         HTTPRequestBuilderPtr requestBuilder
                             (new HTTPRequestBuilder
                              (socket,
                                  [net](HTTPRequestPtr request, HTTPResponsePtr response)
                                  {
                                      for (auto v = request->headers.begin();
                                           v != request->headers.end();
                                           ++v)
                                      {
                                          cout << "   " << v->first << " : " << v->second << endl;
                                      }
                                      if (!ServeFile("../t/html", request,response))
                                      {
                                          response->writeHead(404);
                                          response->end("<html><head><title>Nope!</title></head><body><h1>Ain't there, dude!</h1></body></html>\n");
                                      }
                                      auto timer = net->setInterval([]() { cout << "Other Timer!" << endl; }, 5 * 1000);
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
