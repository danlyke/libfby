#ifndef FBYNET_H_INCLUDED
#define FBYNET_H_INCLUDED

#include "fby.h"
#include "iostream"

// class Net definition
#include <functional>
#include <map>

namespace Fby {
    
    FBYCLASSPTR(Socket);
    FBYCLASSPTR(Server);
    FBYCLASSPTR(Net);

    FBYCLASSPTR(HTTPRequest);
    FBYCLASSPTR(HTTPResponse);

    typedef std::function<void (const char *data, size_t length)> OnDataFunction;
    typedef std::function<void ()> OnEndFunction;
    typedef std::function<void ()> OnDrainFunction;
    typedef std::function<void (const std::string &, const std::string &)> OnNameValueFunction;
    typedef std::function<void (SocketPtr)> CreateServerFunction;
    typedef std::function<void (HTTPRequestPtr request, HTTPResponsePtr response)> RespondToHTTPRequestFunction;
    typedef std::function<HTTPResponsePtr (std::string host, std::string method, std::string path,
                                           std::vector< std::string, std::string> attributes ) > CreateHTTPResponseFunction;

FBYCLASS(Socket) : public BaseObj
    {
        friend class Net;
    
    private:
        Socket(const Socket &);
        Socket &operator=(const Socket&);
    
        int fd;
        OnDataFunction on_data;
        OnDrainFunction on_drain;
        OnEndFunction on_end;
        Net * net;
        std::string queuedWrite;
        bool emitDrain;
        bool doneWithWrites;
    
        void SetSocketServerAndFile(Net* net, int fd);
    public:
        Socket();
        void onData(OnDataFunction on_data);
        void onDrain(OnDrainFunction on_drain);
        void onEnd(OnEndFunction on_end);
        bool write(const char *data, size_t length);
        bool write(const std::string &s);
        bool write(const char *data);
        bool end(const char *data, size_t length);
        bool end(const std::string &s);
        bool end(const char *data);
    };



FBYCLASS(Server) : public BaseObj
    {
        friend class Net;
    private:
        Server(const Server &);
        Server &operator=(const Server &);

        CreateServerFunction create_func;
        int fd;
        Net *net;

    public:
        ServerPtr listen(int socket);
        Server(Net * net, CreateServerFunction create_func);
    };


    FBYCLASSPTR(IntervalTimeoutObject);
FBYCLASS(IntervalTimeoutObject) : public BaseObj
    {
        friend class Net;
        FBYUNCOPYABLE(IntervalTimeoutObject);
    private:
        std::function<void ()> triggered_function;
        bool stillNeedsEvents;
        long microsecondsRecurring;
        long next_time;
    public:
    IntervalTimeoutObject(std::function<void()> triggered_function,
                          long microsecondsStart, long microsecondsRecurring, const char *name, int size)
        :
        BaseObj(name,size),
            triggered_function(triggered_function),
            stillNeedsEvents(true),
            microsecondsRecurring(microsecondsRecurring),
            next_time(microsecondsStart)
            {}
        void unref() { stillNeedsEvents = false; }
        void ref() { stillNeedsEvents = true; }

        long processTimeStepGetNextTime(long elapsed_microseconds);
    };

    FBYCLASSPTR(TimeoutObject);
FBYCLASS(TimeoutObject) : public IntervalTimeoutObject
    {
        friend class Net;

    public:
    TimeoutObject(std::function<void()> triggered_function,
                  long microseconds) : IntervalTimeoutObject(triggered_function,
                                                             microseconds, 0,
                                                             BASEOBJINIT(TimeoutObject))
        {}
    };

    FBYCLASSPTR(IntervalObject);
FBYCLASS(IntervalObject) : public IntervalTimeoutObject
    {
        friend class Net;
    public:
    IntervalObject(std::function<void()> triggered_function,
                   long microseconds) : IntervalTimeoutObject(triggered_function,
                                                              microseconds, microseconds,
                                                              BASEOBJINIT(IntervalObject))
        {}
    IntervalObject(std::function<void()> triggered_function,
                   long microsecondsStart, long microsecondsRecurring) :
        IntervalTimeoutObject(triggered_function,
                              microsecondsStart,
                              microsecondsRecurring, BASEOBJINIT(IntervalObject))
        {}
    };


    FBYCLASSPTR(ImmediateObject);
FBYCLASS(ImmediateObject) : public IntervalTimeoutObject
    {
        friend class Net;
    public:
    ImmediateObject(std::function<void()> triggered_function)
        : IntervalTimeoutObject(triggered_function,
                                0, 0, BASEOBJINIT(ImmediateObject))
        {}
    };



FBYCLASS(Net) : public BaseObj
    {
    public:
        std::vector<ServerPtr> servers;
        std::vector<SocketPtr> sockets;
        std::vector<IntervalTimeoutObjectPtr> timers;

        bool debug;
    
        Net();
        ~Net();
    public:
        ServerPtr createServer(CreateServerFunction f);
        void loop();

        TimeoutObjectPtr setTimeout(std::function<void ()> callback,int delay_ms);
        template <typename Arg0> TimeoutObjectPtr
            setTimeout(std::function<void (Arg0)> callback, int delay_ms,
                       Arg0 arg0);
        void clearTimeout(TimeoutObjectPtr timeout);
        IntervalObjectPtr setInterval(std::function<void ()> callback,
                                      int delay_ms);
        IntervalObjectPtr setInterval(std::function<void ()> callback,
                                      int delay_ms, int recurring_ms);
        void clearInterval(IntervalObjectPtr interval);
        ImmediateObjectPtr setImmediate(std::function<void ()> callback);
        void clearImmediate(ImmediateObjectPtr immediate);
    };


FBYCLASS(HTTPResponse) : public BaseObj
    {
        FBYUNCOPYABLE(HTTPResponse);
    private:
        SocketPtr socket;
        bool wroteHeader;

    public:
        HTTPResponse(SocketPtr);
        void writeHead( int resultCode,
                        const std::map<std::string,std::string> &);
        void writeHead( int resultCode);
        void writeHead( int resultCode, const char *);
        void redirect(const std::string &path);
        void respondHTML(int code, std::map<std::string, std::string> attrs,
                         const std::string &title, const std::string &body);
        void respondHTML(int code, 
                         const std::string &title, const std::string &body);
        void respondHTML(int code, const std::string &body);
        void respondRaw(int code, std::map<std::string, std::string> attrs,
                        const std::string &body);
        void respondRaw(int code, 
                        const std::string &body);

        bool write(const char *data, size_t length) { return socket->write(data,length);}
        bool write(const std::string &s) { return socket->write(s);}
        bool write(const char *data) { return socket->write(data);}
        bool end(const char *);
        bool end(const char *data, size_t length);
        bool end(const std::string &s);
        void onDrain(OnDrainFunction on_drain);
    };

FBYCLASS(HTTPRoute) : public BaseObj
    {
    public:

        bool wants(const std::string &host, const std::string &method, const std::string &path);
    };


    FBYCLASSPTR(HTTPRequestBuilder);
    FBYCLASSPTR(HTTPRequest);
FBYCLASS(HTTPRequest) : public BaseObj
    {
        friend class HTTPRequestBuilder;
    private:
        OnDataFunction on_data;
        OnEndFunction on_end;
    public:
        std::string method;
        std::string path;
        std::string protocol;
        std::map<std::string, std::string> headers;
        void onData(OnDataFunction on_data);
        void onEnd(OnEndFunction on_end);
        HTTPRequest();
    };


FBYCLASS(HTTPRequestBuilder) : public BaseObj
    {
        friend class HTTPServer;
    private: 
        void ConsumeLeadingWhitespace(const char **data, size_t &length);
        void ReadHTTPMethod(const char **data, size_t &length);
        void ConsumeHTTPMethodWhitespace(const char **data, size_t &length);
        void ReadHTTPPath(const char **data, size_t &length);
        void ConsumeHTTPPathWhitespace(const char **data, size_t &length);
        void ReadHTTPProtocol(const char **data, size_t &length);
        void ConsumeHTTPProtocolNewline(const char **data, size_t &length);
        void ReadHTTPHeaderName(const char **data, size_t &length);
        void ReadHTTPHeaderNameContinue(const char **data, size_t &length);
        void ConsumeHTTPHeaderNameWhitespace(const char **data, size_t &length);
        void ReadHTTPHeaderValue(const char **data, size_t &length);
        void ReadHTTPRequestBuilderData(const char **data, size_t &length);
        void ReadResetReadState(const char **data, size_t &length);

        void ResetReadState();

        void GenerateHTTPResponder();
    protected:

        virtual void EmitNameValue(std::string name, const std::string &value);

        RespondToHTTPRequestFunction on_request;
        size_t content_length;
        SocketPtr socket;
    public:
        void (HTTPRequestBuilder::*readState)(const char **data, size_t &length);

        HTTPRequestPtr request;

        std::string headerName;
        std::string headerValue;
        void ReadData(const char *data, size_t length);
        HTTPRequestBuilder(SocketPtr socket, RespondToHTTPRequestFunction);
    };

    FBYCLASSPTR(HTTPServer);

FBYCLASS(HTTPServer) : public Server
    {
    public:
        HTTPServer(Net *net, RespondToHTTPRequestFunction f);
    
    };


//inline HTTPServer::HTTPServer(Net *net, RespondToHTTPRequestFunction)
//                  : Server(net, []() {})
//{
//}




    inline Server::Server(Net * net, CreateServerFunction create_func)
        : BaseObj(BASEOBJINIT(Server)),
        create_func(create_func), fd(-1), net(net)
    {
    }

    FBYCLASSPTR(BodyParserURLEncoded);
FBYCLASS(BodyParserURLEncoded) : public BaseObj
    {
    private:
        void ResetReadState();
        void ReadName(const char **data, size_t &length);
        void ReadNamePlusSpace(const char **data, size_t &length);
        void ReadNameEntity1(const char **data, size_t &length);
        void ReadNameEntity2(const char **data, size_t &length);
        void ReadValue(const char **data, size_t &length);
        void ReadValuePlusSpace(const char **data, size_t &length);
        void ReadValueEntity1(const char **data, size_t &length);
        void ReadValueEntity2(const char **data, size_t &length);
        void EmitNameValue(const std::string &name, const std::string &value);


        int ReadDataAsHexDigit(const char **data, size_t &length);
        void AppendUntil( std::string &which, const char toggleOn,
                          const char **data, size_t &length);
        void (BodyParserURLEncoded::*readState)(const char **data, size_t &length);
        std::string name;
        std::string value;
        int entity;
   
    public:
        OnNameValueFunction on_name_value;
        void on_data(const char *data, size_t length);
        void on_end();
        void onNameValue(OnNameValueFunction on_name_value);
        BodyParserURLEncoded();
    };


    FBYCLASSPTR(NameValuePair);
FBYCLASS(NameValuePair) : public BaseObj
    {
    public:
        std::map<std::string, std::string> nvp;
        NameValuePair();
    };

    inline NameValuePair::NameValuePair() :
        BaseObj(BASEOBJINIT(NameValuePair)),
        nvp()
        {
        }
    extern bool ServeFile(const char * fileRoot, HTTPRequestPtr request, HTTPResponsePtr response);

} // end of namspace Fby

#endif /* #ifndef FBYNET_H_INCLUDED */
