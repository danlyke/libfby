#include "fbynet.h"
#include "fbystring.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <termios.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <netdb.h>
#include <ctype.h>
#include <iostream>

using namespace Fby;
using namespace std;

int termsig = 0;
int got_sighup = 0;
int listensocket;
#define KEEPALIVE_TIME 20

static void catchsignal_TERMINT(int sig)
{
	termsig = sig;
}

static void catchsignal_HUP(int)
{
	got_sighup = 1;
}


struct SignalCatch
{
    int sig;
    void (*handler)(int signal);
} signal_handlers[] =
{
    { SIGPIPE, SIG_IGN },
    { SIGCHLD, SIG_IGN },
    { SIGHUP, catchsignal_HUP },
    { SIGTERM, catchsignal_TERMINT },
};

struct {
    struct sigaction new_signal_action;
    struct sigaction old_signal_action;
}  signal_actions[sizeof(signal_handlers)/sizeof(*signal_handlers)];


void catchsignals()
{
    for (size_t i = 0; i < sizeof(signal_handlers) / sizeof(signal_handlers[0]); ++i)
    {
        memset(&signal_actions[i].new_signal_action,0,
               sizeof(signal_actions[i].new_signal_action));
        sigemptyset(&signal_actions[i].new_signal_action.sa_mask);
        signal_actions[i].new_signal_action.sa_handler = 
            signal_handlers[i].handler;
        sigaction(signal_handlers[i].sig, &signal_actions[i].new_signal_action, 
                  &signal_actions[i].old_signal_action);
    }
}

void releasesignals()
{
    for (size_t i = 0; 
         i < sizeof(signal_handlers) / sizeof(signal_handlers[0]); ++i)
    {
        sigaction(signal_handlers[i].sig, &signal_actions[i].old_signal_action, 
                  &signal_actions[i].new_signal_action);
    }
}


long IntervalTimeoutObject::processTimeStepGetNextTime(long elapsed_microseconds)
{
    if (next_time <= elapsed_microseconds)
    {
        triggered_function();
        if (microsecondsRecurring)
        {
            next_time += microsecondsRecurring;
        }
        else
        {
            return 0;
        }
    }
    next_time -= elapsed_microseconds;
    return next_time;
}



ServerPtr Server::listen(int socket_num)
{
    int socket_opt;
    int resultCode;
    struct addrinfo          ask,*res = NULL;
    memset(&ask, 0, sizeof(ask));
    ask.ai_flags = AI_PASSIVE;
    ask.ai_socktype = SOCK_STREAM;
    char socket_ch[16];
    snprintf(socket_ch, sizeof(socket_ch), "%d", socket_num);
    
    if (0 != (resultCode = getaddrinfo(NULL, socket_ch, &ask, &res))) 
    {
        fprintf(stderr,"getaddrinfo (ipv4): %s\n",gai_strerror(resultCode));
        exit(1);
    }

//    fprintf(stderr, "About to create socket for %d %d %d\n",res->ai_family, res->ai_socktype, res->ai_protocol);
//    fprintf(stderr, "Compare to %d %d %d\n",AF_INET, SOCK_STREAM, 0);

    fd = ::socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    socket_opt = 1; 
    setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&socket_opt,sizeof(socket_opt));
    fcntl(fd,F_SETFL,O_NONBLOCK);
    struct sockaddr_storage  ss;
    memcpy(&ss,res->ai_addr,res->ai_addrlen);
    
    if (-1 == bind(fd, (struct sockaddr*)&ss, res->ai_addrlen)) 
    {
        perror("bind");
        exit(1);
    }
    
    if (-1 == ::listen(fd, 8)) 
    {
        perror("listen");
        exit(1);
    }
    
    return ServerPtr(this);
}

Net::Net() :
    BaseObj(BASEOBJINIT(Net)), servers(), sockets(), timers(), debug(false)
{
//    cout << "Constructing Net object" << endl;
}

Net::~Net()
{
//    cout << "Destroying Net object" << endl;
}


ServerPtr Net::createServer(CreateServerFunction f)
{
    ServerPtr server(new Server(this, f));
    servers.push_back(server);
    return server;
};


TimeoutObjectPtr Net::setTimeout(std::function<void ()> callback,int delay_ms)
{
    TimeoutObjectPtr timeout(new TimeoutObject(
                                 callback,
                                 delay_ms));
    if (debug)
        cout << "Added timeout" << timers.size() << endl;
    timers.push_back(timeout);
    if (debug)
        cout << "Timers is now " << timers.size() << endl;
    return timeout;
}

template <typename Arg0> TimeoutObjectPtr
Net::setTimeout(std::function<void (Arg0)> callback,
                    int delay_ms,
                    Arg0 arg0)
{
    return setTimeout([=]() { callback(arg0); }, delay_ms);
}
void Net::clearTimeout(TimeoutObjectPtr timeout)
{
    auto timer(std::find(timers.begin(), timers.end(),timeout));
    if (timer != timers.end())
    {
        if (debug)
            cout << "Removing timeout" << (timer - timers.begin()) << endl;
        timers.erase(timer);
    }
}
IntervalObjectPtr Net::setInterval(std::function<void ()> callback,int delay_ms)
{
    IntervalObjectPtr timeout(new IntervalObject(
                                  callback,
                                  delay_ms,
                                  delay_ms));
    if (debug)
        cout << "Added interval A " << timers.size() << endl;
    timers.push_back(timeout);
    if (debug)
        cout << "Timers is now " << timers.size() << endl;
    return timeout;
}

IntervalObjectPtr Net::setInterval(std::function<void ()> callback,int delay_ms, int recurring_ms)
{
    IntervalObjectPtr timeout(new IntervalObject(
                                  callback,
                                  delay_ms,
                                  recurring_ms));
    if (debug)
        cout << "Added interval B " << timers.size() << endl;
    timers.push_back(timeout);
    if (debug)
        cout << "Timers is now " << timers.size() << endl;
    return timeout;
}
void Net::clearInterval(IntervalObjectPtr interval)
{
    auto timer(std::find(timers.begin(), timers.end(),interval));
    if (timer != timers.end())
    {
        if (debug)
            cout << "Removing interval" << (timer - timers.begin()) << endl;
        timers.erase(timer);
    }
}

ImmediateObjectPtr Net::setImmediate(std::function<void ()> callback)
{
    ImmediateObjectPtr timeout(new ImmediateObject(callback));
    if (debug)
        cout << "Added immediate " << timers.size() << endl;
    timers.push_back(timeout);
    if (debug)
        cout << "Timers is now " << timers.size() << endl;
    return timeout;
}
void Net::clearImmediate(ImmediateObjectPtr immediate)
{
    auto timer(std::find(timers.begin(), timers.end(), immediate));
    if (timer != timers.end())
    {
        if (debug)
            cout << "Removing timeout" << (timer - timers.begin()) << endl;
        timers.erase(timer);
    }
}



void
Net::loop()
{
	time_t now;
    struct timeval      previous_time;
	int                 max_fd;
	fd_set              read_fds,write_fds;

//    fprintf(stderr, "Beginning of loop\n");
//    fprintf(stderr, "Server size is %d sockets size is %d\n",
//            (int)(servers.size()), (int)(sockets.size()));
    termsig = 0;
    catchsignals();

    gettimeofday(&previous_time, NULL);

	for (;(servers.size() || sockets.size()) && !termsig;) 
	{
		FD_ZERO(&read_fds);
		FD_ZERO(&write_fds);

		max_fd = 0;
		/* add listening socket */
        for (auto server = servers.begin(); server != servers.end(); ++server)
        {
            FD_SET((*server)->fd,&read_fds);
            if ((*server)->fd > max_fd)
                max_fd = (*server)->fd;
		}
        for (auto socket = sockets.begin(); socket != sockets.end(); ++socket)
        {
            FD_SET((*socket)->fd, &read_fds);
            if ((*socket)->fd > max_fd)
                max_fd = (*socket)->fd;
            if ((*socket)->queuedWrite.size())
                FD_SET((*socket)->fd, &write_fds);
		}

        /* CHECK FOR WRITES HERE! */

        struct timeval current_time;
        gettimeofday(&current_time, NULL);
        time_t elapsed_seconds = current_time.tv_sec - previous_time.tv_sec;
        long elapsed_microseconds =
            (long)(current_time.tv_usec / 1000) - (long)(previous_time.tv_usec / 1000);
        elapsed_microseconds += int(elapsed_seconds) * 1000;
        previous_time = current_time;

        long next_timer_microseconds(LONG_MAX);

        if (debug)
            cout << "at line " << __LINE__ << " Timers is now " << timers.size() << endl;
        for (size_t i = 0; i < timers.size(); ++i)
        {
            if (debug)
                cout << "Looking at time for timer " << i << " of " << timers.size() << endl;
            auto timer(timers[i]);
            if (debug)
                cout << "About to process time" << endl;
            long next_time(timer->processTimeStepGetNextTime(elapsed_microseconds));

            if (debug)
                cout << "Got " << next_time << endl;
            if (next_time == 0)
            {
                timers.erase(timers.begin() + i);
                --i;
            }
            else if (next_time < next_timer_microseconds)
            {
                next_timer_microseconds = next_time;
            }
        }
        if (debug)
            cout << "at line " << __LINE__ << " Timers is now " << timers.size() << endl;

        struct timeval *ptv(NULL), tv;

        if (next_timer_microseconds != LONG_MAX)
        {
            ptv = &tv;
            tv.tv_sec  = next_timer_microseconds / 1000 ;
            tv.tv_usec = (next_timer_microseconds % 1000) * 1000;
        }

		if (-1 == select(max_fd+1,
				 &read_fds,
				 &write_fds,
				 NULL,ptv))
		{
			continue;
		}
		now = time(NULL);

        for (auto server = servers.begin(); server != servers.end(); ++server)
        {
            if (FD_ISSET((*server)->fd,&read_fds)) 
            {
                int fd = accept((*server)->fd, NULL, NULL);
                if (-1 == fd)
                {
                    if (EAGAIN != errno)
                        perror("accept");
                }	
                else
                {
                    SocketPtr socket(new Socket);
                    socket->SetSocketServerAndFile(this, fd);
					fcntl(fd,F_SETFL,O_NONBLOCK);
                    sockets.push_back(socket);
                    if (debug)
                        fprintf(stderr,"%03d: %ld new request (%d)\n",
                                (*server)->fd, now, fd);
                    (*server)->create_func(socket);
				}
			}
		}
        if (debug)
            cout << "at line " << __LINE__ << " Timers is now " << timers.size() << endl;
		
        for (auto socket = sockets.begin(); socket != sockets.end(); ++socket)
        {
            if (FD_ISSET((*socket)->fd, &read_fds))
            {
                char buffer[65536];
                size_t len = read((*socket)->fd, buffer, sizeof(buffer));
                if (len > 0)
                {
                    buffer[len]  = 0;
                    if (debug)
                        fprintf(stderr, "Socket %lx Read %d bytes from %d '%*s'\n", (unsigned long)(&**socket), (int)len, (*socket)->fd, (int)len, buffer);
                    (*socket)->on_data(buffer, len);
                    if (debug)
                        std::cerr << "Sent bytes to " << (*socket)->fd << " done " << (*socket)->doneWithWrites << std::endl;
                    if (debug)
                        cout << "After read Timers is now " << timers.size() << endl;
                }
                else
                {
                    (*socket)->on_end();
                    if (debug)
                        cout << "After onend Timers is now " << timers.size() << endl;
                    (*socket)->fd = -1;
                    if (debug)
                    {
                        fprintf(stderr,"%03d: read of %d bytes\n", (*socket)->fd, (int)len);
                        perror("Short read");
                    }
                }
            }
            if (debug)
                cout << "at line " << __LINE__ << " Timers is now " << timers.size() << endl;
            if (FD_ISSET((*socket)->fd, &write_fds))
            {
                if (debug)
                    cout << "at line " << __LINE__ << " Timers is now " << timers.size() << endl;
                size_t len = write((*socket)->fd,
                                   (*socket)->queuedWrite.data(), (*socket)->queuedWrite.size());
                if (len > 0)
                {
                    if (len < (*socket)->queuedWrite.size())
                    {
                        (*socket)->queuedWrite.erase(0, len);
                    }
                    else
                    {
                        (*socket)->emitDrain = true;
                        (*socket)->queuedWrite.clear();
                    }
                }
                if (debug)
                    cout << "at line " << __LINE__ << " Timers is now " << timers.size() << endl;
            }
            if ((*socket)->emitDrain)
            {
                if ((*socket)->on_drain)
                {
                    if (debug)
                        cout << "at line " << __LINE__ << " Timers is now " << timers.size() << endl;
                    (*socket)->on_drain();
                    if (debug)
                        cout << "at line " << __LINE__ << " Timers is now " << timers.size() << endl;
                }
                (*socket)->emitDrain = false;
            }

        }
        if (debug)
            cout << "Before drain check Timers is now " << timers.size() << endl;

        for (ssize_t i = 0; i < (ssize_t)(sockets.size()); ++i)
        {
            SocketPtr socket = sockets[i];
            if (debug)
                fprintf(stderr, "Checking socket %lx status %d\n", (unsigned long)(&*socket), socket->doneWithWrites);
            if (sockets[i]->doneWithWrites
                && !sockets[i]->emitDrain)
            {
                close(sockets[i]->fd);
                sockets[i]->fd = -1;
            }
            if (sockets[i]->fd < 0)
            {
                sockets.erase(sockets.begin() + i);
                --i;
            }
        }
        if (debug)
            cout << "After drain check Timers is now " << timers.size() << endl;
        

        for (size_t i = 0; i < servers.size(); ++i)
        {
            if (servers[i]->fd < 0)
            {
                servers.erase(servers.begin() + i);
                --i;
            }
        }
        if (debug)
            cout << "At end of loop, Timers is now " << timers.size() << endl;
    /* DO THE WRITE, AND THE CLOSE */
	}

    releasesignals();
}

