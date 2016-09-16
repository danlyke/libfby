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

using namespace std;
using namespace Fby;

void HTTPResponse::onDrain(OnDrainFunction on_drain)
{
    socket->onDrain(on_drain);
}


void HTTPRequest::onData(OnDataFunction on_data)
{
    this->on_data = on_data;
}
void HTTPRequest::onEnd(OnEndFunction on_end)
{
    this->on_end = on_end;
}


HTTPResponse::HTTPResponse(SocketPtr socket) :
    BaseObj(BASEOBJINIT(HTTPResponse)),
    socket(socket),
    wroteHeader(false)
{
}
  
struct HTTPResultCodes {
    int resultCode;
    const char *resultText;
};

const char content_type[] = "Content-Type";
const char crlf[] = "\r\n";
const char colonspace[] = ": ";
const char textslashhtml[] = "text/html";
const char imageslashjpeg[] = "image/jpeg";

struct MIMETypesByExtension {
    const char *extension;
    const char *mimeType;
};


const char defaultMimeType[] = "application/octet-stream";

static MIMETypesByExtension mimeTypesByExtension[] = {
    { ".htm", 	textslashhtml },
    { ".html", 	textslashhtml },
    { ".ico", 	"image/x-icon" },
    { ".jpeg", 	imageslashjpeg },
    { ".jpg", 	imageslashjpeg },
    { ".js", 	"application/x-javascript" },
    { ".png", 	"image/png" },
    { ".txt", 	"text/plain" },
    { ".svg",	"image/svg+xml" },
    // Needs a Content-Encoding header too...
    { ".svgz",	"image/svg+xml" },
    { ".json", "application/json" },
    { ".css", "text/css" },
};

const char *MimeTypeForExtension(const string &path)
{
    const char *mimeType = defaultMimeType;
    
    for (size_t i = 0; i < sizeof(mimeTypesByExtension) / sizeof(*mimeTypesByExtension); ++i)
    {
        if (ends_with(path, mimeTypesByExtension[i].extension))
        {
            mimeType = mimeTypesByExtension[i].mimeType;
            break;
        }
    }
    return mimeType;
}


static HTTPResultCodes resultCodes[] =
{
    { 100, "Continue" },
    { 101, "Switching Protocols" },
    { 102, "Processing" }, //  (WebDAV; RFC 2518)
    { 200, "OK" },
    { 201, "Created" },
    { 202, "Accepted" },
    { 203, "Non-Authoritative Information" }, //  (since HTTP/1.1)
    { 204, "No Content" },
    { 205, "Reset Content" },
    { 206, "Partial Content" },
    { 207, "Multi-Status" }, //  (WebDAV; RFC 4918)
    { 208, "Already Reported" }, //  (WebDAV; RFC 5842)
    { 226, "IM Used" }, //  (RFC 3229)
    { 300, "Multiple Choices" },
    { 301, "Moved Permanently" },
    { 302, "Found" },
    { 303, "See Other" }, //  (since HTTP/1.1)
    { 304, "Not Modified" },
    { 305, "Use Proxy" }, //  (since HTTP/1.1)
    { 306, "Switch Proxy" },
    { 307, "Temporary Redirect" }, //  (since HTTP/1.1)
    { 308, "Permanent Redirect" }, //  (Experimental RFC; RFC 7238)
    { 400, "Bad Request" },
    { 401, "Unauthorized" },
    { 402, "Payment Required" },
    { 403, "Forbidden" },
    { 404, "Not Found" },
    { 405, "Method Not Allowed" },
    { 406, "Not Acceptable" },
    { 407, "Proxy Authentication Required" },
    { 408, "Request Timeout" },
    { 409, "Conflict" },
    { 410, "Gone" },
    { 411, "Length Required" },
    { 412, "Precondition Failed" },
    { 413, "Request Entity Too Large" },
    { 414, "Request-URI Too Long" },
    { 415, "Unsupported Media Type" },
    { 416, "Requested Range Not Satisfiable" },
    { 417, "Expectation Failed" },
    { 418, "I'm a teapot" }, //  (RFC 2324)
    { 419, "Authentication Timeout" }, //  (not in RFC 2616)
    { 420, "Method Failure" }, //  (Spring Framework)
    { 420, "Enhance Your Calm" }, //  (Twitter)
    { 422, "Unprocessable Entity" }, //  (WebDAV; RFC 4918)
    { 423, "Locked" }, //  (WebDAV; RFC 4918)
    { 424, "Failed Dependency" }, //  (WebDAV; RFC 4918)
    { 426, "Upgrade Required" },
    { 428, "Precondition Required" }, //  (RFC 6585)
    { 429, "Too Many Requests" }, //  (RFC 6585)
    { 431, "Request Header Fields Too Large" }, //  (RFC 6585)
    { 440, "Login Timeout" }, //  (Microsoft)
    { 444, "No Response" }, //  (Nginx)
    { 449, "Retry With" }, //  (Microsoft)
    { 450, "Blocked by Windows Parental Controls" }, //  (Microsoft)
    { 451, "Unavailable For Legal Reasons" }, //  (Internet draft)
    { 452, "Conference Not Found" },
    { 453, "Not Enough Bandwidth" },
    { 454, "Session Not Found" },
    { 455, "Method Not Valid in This State" },
    { 456, "Header Field Not Valid for Resource" },
    { 457, "Invalid Range" },
    { 458, "Parameter Is Read-Only" },
    { 459, "Aggregate operation not allowed" },
    { 460, "Only aggregate operation allowed" },
    { 461, "Unsupported transport" },
    { 462, "Destination unreachable" },
    { 463, "Key management Failure" },
    { 494, "Request Header Too Large" }, //  (Nginx)
    { 495, "Cert Error" }, //  (Nginx)
    { 496, "No Cert" }, //  (Nginx)
    { 497, "HTTP to HTTPS" }, //  (Nginx)
    { 498, "Token expired/invalid" }, //  (Esri)
    { 499, "Client Closed Request" }, //  (Nginx)
    { 499, "Token required" }, //  (Esri)
    { 500, "Internal Server Error" },
    { 501, "Not Implemented" },
    { 502, "Bad Gateway" },
    { 503, "Service Unavailable" },
    { 504, "Gateway Timeout" },
    { 505, "HTTP Version Not Supported" },
    { 506, "Variant Also Negotiates" }, //  (RFC 2295)
    { 507, "Insufficient Storage" }, //  (WebDAV; RFC 4918)
    { 508, "Loop Detected" }, //  (WebDAV; RFC 5842)
    { 509, "Bandwidth Limit Exceeded[29]" }, //  (Apache bw/limited extension)
    { 510, "Not Extended" }, //  (RFC 2774)
    { 511, "Network Authentication Required" }, //  (RFC 6585)
    { 551, "Option not supported" },
    { 598, "Network read timeout error" }, //  (Unknown)
    { 599, "Network connect timeout error" }, //  (Unknown)
};

static void WriteResultCode(SocketPtr socket, int resultCode)
{
    char achResultCode[32] = "";
    int len = snprintf(achResultCode, sizeof(achResultCode), "HTTP/1.1 %d", resultCode);

    socket->write(achResultCode, len);
    size_t i;
    for (i = 0; i < (sizeof(resultCodes) / sizeof(*resultCodes)); ++i)
    {
        if (resultCodes[i].resultCode == resultCode)
            break;
    }
    if (i < (sizeof(resultCodes) / sizeof(*resultCodes)))
    {
        socket->write(" ");
        socket->write(resultCodes[i].resultText);
    }
    else
    {
        socket->write(" Unknown Status Code");
    }
}

void WriteContentTypeTextHTML(SocketPtr socket)
{
    socket->write(crlf);
    socket->write(content_type);
    socket->write(colonspace);
    socket->write(textslashhtml);
}

void WriteTwoNewLines(SocketPtr socket)
{
    socket->write(crlf);
    socket->write(crlf);
}

void HTTPResponse::writeHead( int resultCode,
           const std::map<std::string,std::string> &headers)
{
    WriteResultCode(socket, resultCode);
    bool wroteContentType(false);
    for (auto header = headers.begin(); header != headers.end(); ++header)
    {
        if (header->first == content_type)
            wroteContentType = true;
        socket->write(crlf);
      
        socket->write(header->first);
        socket->write(": ");
        socket->write(header->second);
    }
    if (!wroteContentType)
    {
        WriteContentTypeTextHTML(socket);
    }
    WriteTwoNewLines(socket);
}

void HTTPResponse::writeHead( int resultCode)
{
    writeHead(resultCode, "text/html");
}

void HTTPResponse::writeHead( int resultCode, const char *)
{
#if 0
    socket->write("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n");
#else
    WriteResultCode(socket, resultCode);
    WriteContentTypeTextHTML(socket);
    WriteTwoNewLines(socket);
#endif
}


void HTTPResponse::respondHTML(int resultCode, std::map<std::string, std::string> attrs,
                               const std::string &title, const std::string &body)
{
    attrs["Content-Type"] = "text/html; charset=utf-8";
    
    std::string html("<html><head><title>");
    html += title;
    html += "</title></head>\n<body><h1>";
    html += title;
    html += "</h1><p>" + body + "</p></body></html>\n";
    
    attrs["Content-Length"] = std::to_string(html.length());
    writeHead(resultCode, attrs);
    end(html);
}

void HTTPResponse::respondHTML(int resultCode, 
                               const std::string &title, const std::string &body)
{
    std::map<std::string, std::string> attrs;
    respondHTML(resultCode,attrs,title,body);
}

void HTTPResponse::respondRaw(int resultCode, std::map<std::string, std::string> attrs,
                              const std::string &body)
{
//    attrs["Content-Type"] = "text/html; charset=utf-8";
    attrs["Content-Length"] = std::to_string(body.length());
    writeHead(resultCode, attrs);
    end(body);
}

void HTTPResponse::respondRaw(int resultCode, 
                              const std::string &body)
{
    std::map<std::string, std::string> attrs;
    respondRaw(resultCode,attrs,body);
}

void HTTPResponse::respondHTML(int resultCode, const std::string &body)
{
    std::map<std::string, std::string> attrs;

    std::string title;
    size_t i;
    for (i = 0; i < (sizeof(resultCodes) / sizeof(*resultCodes)); ++i)
    {
        if (resultCodes[i].resultCode == resultCode)
            break;
    }
    if (i < (sizeof(resultCodes) / sizeof(*resultCodes)))
    {
        title = resultCodes[i].resultText;
    }
    else
    {
        title = to_string(resultCode);
    }

    respondHTML(resultCode,attrs,title,body);
}

void HTTPResponse::redirect(const std::string &path)
{
    std::map<std::string,std::string> attrs;
    attrs["Location"] = path;
    string redirect("The document has moved <a href=\"");
    redirect += path;
    redirect += "\">here</a>";
    respondHTML(302, attrs, "302 Found", redirect);
        
               
}


bool HTTPResponse::end(const char *data, size_t length)
{
    return socket->end(data,length);
}

bool HTTPResponse::end(const std::string &s)
{
    return socket->end(s);
}

bool HTTPResponse::end(const char *s)
{
    return socket->end(s);
}

static bool AddStringUntilWhitespace(std::string &str, const char **data, size_t &length )
{
    bool nextState = false;
    size_t i;
    for (i = 0; i < length && !isspace((*data)[i]); ++i)
    {}

    str.append((*data), i);
    if (i < length) nextState = true;
    
    (*data) += i;
    length -= i;

    return nextState;
}

void HTTPRequestBuilder::ConsumeLeadingWhitespace(const char **data, size_t &length)
{
    while (length && isspace(**data))
    {
        length--;
        (*data)++;
    }
    if (!isspace(**data))
    {
        readState = &HTTPRequestBuilder::ReadHTTPMethod;
    }
}


void HTTPRequestBuilder::ReadHTTPMethod(const char **data, size_t &length)
{
    if (AddStringUntilWhitespace(request->method, data, length))
        readState = &HTTPRequestBuilder::ConsumeHTTPMethodWhitespace;
}

void HTTPRequestBuilder::ConsumeHTTPMethodWhitespace(const char **data, size_t &length)
{
    request->path.clear();
    while (length && isspace(**data))
    {
        if (**data == '\r'
            || **data == '\n')
            THROWEXCEPTION("Unexpected newline after HTTP Method");
        length--;
        (*data)++;
    }
    if (length)
        readState = &HTTPRequestBuilder::ReadHTTPPath;
}

void HTTPRequestBuilder::ReadHTTPPath(const char **data, size_t &length)
{
    if (AddStringUntilWhitespace(request->path,data,length))
        readState = &HTTPRequestBuilder::ConsumeHTTPPathWhitespace;
}

void HTTPRequestBuilder::ConsumeHTTPPathWhitespace(const char **data, size_t &length)
{
    while (length && isspace(**data))
    {
        if (**data == '\r'
            || **data == '\n')
            THROWEXCEPTION("Unexpected newline after HTTP Path");

        length--;
        (*data)++;
    }
    if (length)
        readState = &HTTPRequestBuilder::ReadHTTPProtocol;
}

void HTTPRequestBuilder::ReadHTTPProtocol(const char **data, size_t &length)
{
   if (AddStringUntilWhitespace(request->protocol,data,length))
        readState = &HTTPRequestBuilder::ConsumeHTTPProtocolNewline;
}

void HTTPRequestBuilder::ConsumeHTTPProtocolNewline(const char **data, size_t &length)
{
    while (**data == '\r')
    {
        length--;
        (*data)++;
    }
    if (**data == '\n')
    { 
        length--;
        (*data)++;
        headerName.clear();
        readState = &HTTPRequestBuilder::ReadHTTPHeaderName;
    }
    else if (**data != '\r')
        THROWEXCEPTION("Expected newline after HTTP Protocol");
}




void HTTPRequestBuilder::GenerateHTTPResponder()
{
    HTTPResponsePtr response(new HTTPResponse(socket));
    on_request(request, response);
    
#if 0
    for (auto route = routes.begin();
         route != routes.end();
         ++route)
    {
        if ((*route)->wants(host, method, path))
        {
            response = (*route)->create_response(host, method, path, headers);
        }
    }
#endif
}

void HTTPRequestBuilder::ReadHTTPHeaderName(const char **data, size_t &length)
{
    if (**data == '\r')
    {
        length--;
        (*data)++;
    }
    else if (**data == '\n')
    {
        length--;
        (*data)++;
        if (!headerName.empty())
            EmitNameValue(headerName, headerValue);
        GenerateHTTPResponder();
        readState = &HTTPRequestBuilder::ReadHTTPRequestBuilderData;
    }
    else if (isspace(**data))
    {
        if (headerName.empty())
            THROWEXCEPTION("Continuation of HTTP header with no header name");
        readState = &HTTPRequestBuilder::ReadHTTPHeaderValue;
    }
    else
    {
        if (!headerName.empty())
            EmitNameValue(headerName, headerValue);
        headerName.clear();
        headerValue.clear();
        readState = &HTTPRequestBuilder::ReadHTTPHeaderNameContinue;
    }
}
void HTTPRequestBuilder::ReadHTTPHeaderNameContinue(const char **data, size_t &length)
{
    size_t i = 0;
    for (i = 0; i < length && (*data)[i] != ':'; ++i)
    {
        if (isspace((*data)[i]))
            THROWEXCEPTION("Unexpected whitespace in HTTP header name");
    }
    if ((*data)[i] == ':')
    {
        headerName.append(*data, i);
        headerValue.clear();
        ++i;
        readState = &HTTPRequestBuilder::ConsumeHTTPHeaderNameWhitespace;
    }
    else
    {
        headerName.append(*data, i);
    }
    length -= i;
    *data += i;
}
void HTTPRequestBuilder::ConsumeHTTPHeaderNameWhitespace(const char **data, size_t &length)
{
    while (length && isspace(**data))
    {
        if (**data == '\r'
            || **data == '\n')
            THROWEXCEPTION("Unexpected newline after HTTP Path");

        length--;
        (*data)++;
    }
    if (length)
        readState = &HTTPRequestBuilder::ReadHTTPHeaderValue;
}

void HTTPRequestBuilder::ReadHTTPHeaderValue(const char **data, size_t &length)
{
    size_t i = 0;
    for (i = 0; i < length && (*data)[i] != '\r' && (*data)[i] != '\n'; ++i)
    {
    }
    if (i)
    {
        headerValue.append(*data, i);
        *data += i;
        length -= i;
    }
    if (**data == '\r')
    {
        (*data)++;
        length--;
    }
    if (**data == '\n')
    {
        (*data)++;
        length--;
        readState = &HTTPRequestBuilder::ReadHTTPHeaderName;
    }

}

void HTTPRequestBuilder::ReadResetReadState(const char ** /* data */, size_t & /* length */)
{
    ResetReadState();
}

void HTTPRequestBuilder::ReadHTTPRequestBuilderData(const char **data, size_t &length)
{
    string s(*data, length);
    if (content_length > length)
    {
        request->on_data(*data, length);
        *data += length;
        content_length -= length;
        length = 0;
    }
    else
    {
        request->on_data(*data, content_length);
        request->on_end();
        *data += content_length;
        length -= content_length;
        content_length = 0;
        readState = &HTTPRequestBuilder::ReadResetReadState;
     }
}



void HTTPRequestBuilder::ReadData(const char *data, size_t length)
{
    while (length > 0)
    { 
        size_t oldLength(length);
        void (HTTPRequestBuilder::*oldReadState)(const char **data, size_t &length) (readState);
        const char *oldData(data);
    
        ((this)->*(readState))(&data, length);
        if (oldLength - length != (size_t)(data - oldData))
            THROWEXCEPTION("Read state error: data & length out of sync");
        if (length == oldLength && (readState == oldReadState))
            THROWEXCEPTION("Read state infinite loop detected");
    }
}


void HTTPRequestBuilder::EmitNameValue(std::string name, const std::string &value)
{
    if (name == "Content-Length")
        content_length = stoi(value);
    std::transform(name.begin(), name.end(), name.begin(), ::tolower);
    request->headers[name] = value;
}


void HTTPRequestBuilder::ResetReadState()
{
    readState = &HTTPRequestBuilder::ConsumeLeadingWhitespace;
    request = HTTPRequestPtr(new HTTPRequest); 
   
}

HTTPRequestBuilder::HTTPRequestBuilder(SocketPtr socket,
                                       RespondToHTTPRequestFunction on_request) 
    :
    BaseObj(BASEOBJINIT(HTTPRequestBuilder)),
    on_request(on_request),
    content_length(-1),
    socket(socket),
    readState(),
    request(),
    headerName(),
    headerValue()
{
    ResetReadState();
}

HTTPRequest::HTTPRequest() :
    BaseObj(BASEOBJINIT(HTTPRequest)),
    on_data([](const char *, size_t){}),
    on_end(),
    method(),
    path(),
    protocol(),
    headers()
{
}




bool ::Fby::ServeFile(const char * fileRoot, HTTPRequestPtr request, HTTPResponsePtr response)
{
    int fd;
    string path(fileRoot);
    string filename(request->path);
    if (filename.length() == 0)
    {
        response->respondHTML(404,
                              "Not Found",
                              "Zero length filename, this ain't right");
        return true;
    }
    if (filename[0] != '/')
    {
        filename = "/" + filename;
    }
    if (string::npos != filename.find("/."))
    { 
        response->respondHTML(403,
                              "Wrist Slap",
                              "Gotta /. in your filename, not cool");
        return true;
    }

    path += filename;

    struct stat statbuf;

    if (stat(path.c_str(), &statbuf))
    { 
        response->respondHTML(404,
                              "Not Found",
                              "Unable to find <i>" + path + "</i>");
    }

    if (S_ISDIR(statbuf.st_mode))
    {
        if (path[path.length() - 1] != '/')
        {
            response->redirect(filename + '/');
            return true;
        }
        path += "index.html";
    }
    cout << "Reading " << path << endl;

    if (0 < (fd = open(path.c_str(), O_RDONLY)))
    {
        // Need to do file type stuff here!
        response->writeHead(200, MimeTypeForExtension(path));
        int buflen = 8192;
        char *buffer = new char[buflen];
        int len = read(fd, buffer, buflen);
        if (len < buflen)
        {
            response->end(buffer, len);
            close(fd);
            delete[] buffer;
        }
        else
        {
            response->write(buffer, len);
            response->onDrain([buflen, buffer, fd, response]()
                              {
                                  int len = read(fd, buffer, buflen);
                                  if (len < buflen)
                                  {
                                      response->end(buffer, len);
                                      close(fd);
                                      delete[] buffer;
                                  }
                                  else
                                  {
                                      response->write(buffer, len);
                                  }
                              });
        }

        return true;
    }
    return false;
}
              

BodyParserURLEncoded::BodyParserURLEncoded()
    :
    BaseObj(BASEOBJINIT(BodyParserURLEncoded)),
    readState(&BodyParserURLEncoded::ReadName),
    name(),
    value(),
    entity(-1),
    on_name_value([](const std::string &, const std::string &){})

{
}


void BodyParserURLEncoded::ResetReadState()
{
    name.clear();
    value.clear();
    readState = &BodyParserURLEncoded::ReadName;
}


void BodyParserURLEncoded::AppendUntil( string &which, const char toggleOn,
                            const char **data, size_t &length)
{
    size_t i;
    for (i = 0;
         i < length && (toggleOn != (*data)[i])
             && ('%' != (*data)[i])
             && ('+' != (*data)[i]);
         ++i)
    {
    }
    which.append(*data, i);
    *data += i;
    length -= i;
}

void BodyParserURLEncoded::ReadName(const char **data, size_t &length)
{
    AppendUntil(name, '=', data, length);
    if (length)
    {
        switch(**data)
        {
        case '=' :
            readState = &BodyParserURLEncoded::ReadValue;
            break;
        case '%' :
            readState = &BodyParserURLEncoded::ReadNameEntity1;
            break;
        case '+' :
            readState = &BodyParserURLEncoded::ReadNamePlusSpace;
            break;
        default:
            assert(0);
            break;
        }
        ++(*data);
        --length;
    }
}

int BodyParserURLEncoded::ReadDataAsHexDigit(const char **data, size_t &length)
{
    int result(-1);
    if (**data >= '0' && **data <= '9')
    {
        result = **data - '0';
        ++(*data);
        --length;
    }
    else if (**data >= 'A' && **data <= 'F')
    {
        result = **data - 'A' + 0xa;
        ++(*data);
        --length;
    } 
    else if (**data >= 'a' && **data <= 'f')
    {
        result = **data - 'a' + 0xa;
        ++(*data);
        --length;
    } 
    return result;
}

void BodyParserURLEncoded::ReadNamePlusSpace(const char ** /* data */, size_t & /* length */)
{
    name.append(" ");
    readState = &BodyParserURLEncoded::ReadName;
}

void BodyParserURLEncoded::ReadNameEntity1(const char **data, size_t &length)
{
    int result = ReadDataAsHexDigit(data, length);
    if (result >= 0)
    {
        entity = result << 4;
        readState = &BodyParserURLEncoded::ReadNameEntity2;
    }
    else
    {
        readState = &BodyParserURLEncoded::ReadName;
    }
}

void BodyParserURLEncoded::ReadNameEntity2(const char **data, size_t &length)
{
    int result = ReadDataAsHexDigit(data, length);
    if (result >= 0)
    {
        entity |= result;
        char ch[2];
        ch[0] = (char)(entity);
        ch[1] = '\0';
        name.append(ch);
    }
    readState = &BodyParserURLEncoded::ReadName;
}

void BodyParserURLEncoded::ReadValue(const char **data, size_t &length)
{
    AppendUntil(value, '&', data, length);
    if (length)
    {
        switch(**data)
        {
        case '&' :
            EmitNameValue(name,value);
            name.clear();
            value.clear();
            readState = &BodyParserURLEncoded::ReadName;
            break;
        case '%' :
            readState = &BodyParserURLEncoded::ReadValueEntity1;
            break;
        case '+' :
            readState = &BodyParserURLEncoded::ReadValuePlusSpace;
            break;
        default:
            assert(0);
            break;
        }
        ++(*data);
        --length;
    }
}

void BodyParserURLEncoded::ReadValuePlusSpace(const char ** /* data */, size_t & /* length */)
{
    value.append(" ");
    readState = &BodyParserURLEncoded::ReadValue;
}


void BodyParserURLEncoded::ReadValueEntity1(const char **data, size_t &length)
{
    int result = ReadDataAsHexDigit(data, length);
    if (result >= 0)
    {
        entity = result << 4;
        readState = &BodyParserURLEncoded::ReadValueEntity2;
    }
    else
    {
        readState = &BodyParserURLEncoded::ReadValue;
    }
}

void BodyParserURLEncoded::ReadValueEntity2(const char **data, size_t &length)
{
    int result = ReadDataAsHexDigit(data, length);
    if (result >= 0)
    {
        entity |= result;
        char ch[2];
        ch[0] = (char)(entity);
        ch[1] = '\0';
        value.append(ch);
    }
    readState = &BodyParserURLEncoded::ReadValue;
}

void BodyParserURLEncoded::EmitNameValue(const std::string &name, const std::string &value)
{
    on_name_value(name, value);
}

void BodyParserURLEncoded::on_data( const char *data, size_t length)
{
    while (length > 0)
    {
        ((this)->*(readState))(&data, length);
    }
}

void BodyParserURLEncoded::on_end()
{
    EmitNameValue(name,value);
    ResetReadState();
}

void BodyParserURLEncoded::onNameValue(OnNameValueFunction on_name_value)
{
    this->on_name_value = on_name_value;
}



#if 0
NODEJS drain example
// Write the data to the supplied writable stream 1MM times.
// Be attentive to back-pressure.
function writeOneMillionTimes(writer, data, encoding, callback) {
    var i = 1000000;
    write();
    function write() {
        var ok = true;
        do {
            i -= 1;
            if (i === 0) {
                // last time!
                writer.write(data, encoding, callback);
            } else {
                // see if we should continue, or wait
                // don't pass the callback, because we're not done yet.
                ok = writer.write(data, encoding);
            }
        } while (i > 0 && ok);
        if (i > 0) {
            // had to stop early!
            // write some more once it drains
            writer.once('drain', write);
        }
    }

#endif
