#include "Response.hpp"

Response::Response(Request req, int client_fd, Config serverConfig)
{
    _client_fd = client_fd;
    _headers["url"] = req.getUrl();
    _serverConfig = serverConfig;
    _headers["http-version"] = req.getHttpVersion();
    _headers["Connection"] = req.getHeader("Connection");
    _headers["Date"] = util::getCurrentDate(); 
    file_to_send=-1;
    nbytes_left = 0;
}

Response::Response()
{
    nbytes_left = 0;
    file_to_send=0;
}

Response::Response(const Response& src)
{
    _headers = src.getHeaders();
    _client_fd = src.getClientFd();
    _serverConfig = src.getServerConfig();
    file_to_send = src.file_to_send;
    nbytes_left = src.nbytes_left;

}

std::string     Response::CraftRedirectionPage(int statusCode)
{
    std::ostringstream html;

    // html << "<html><head><title>Moved</title></head>
    //         <body>
    //         =Moved=
    //         <p>This page has moved to <a href="http://www.example.org/">http://www.example.org/</a>.</p>\"
    //         </body>
    //         </html>
    //         ";
    return html.str();
}

Response    Response::sendRedirect(int statusCode, const std::string &location)
{
    std::string redirectPageContent = Server::getErrorPageContent(statusCode, _serverConfig);
    std::ostringstream msg;

    msg << "HTTP/1.1" << " " << statusCode << " " 
        << HttpStatus::reasonPhrase(statusCode) << "\r\n"
        << "Connection: close" << "\r\n"
        << "Server: server dyal lah y7sn l3wan" << "\r\n"
        << "Location: " << location << "\r\n"
        << "Date: " << _headers["Date"] << "\r\n"
        << "Content-Type: text/html\r\n"
        << "Content-Length: " << redirectPageContent.size() << "\r\n"
        << "\r\n"
        << redirectPageContent;
    
    sendMessage(_client_fd, msg.str());
    return *this;
}

Response    Response::send( int statusCode)
{
    std::string errorPageContent = Server::getErrorPageContent(statusCode, _serverConfig);
    std::ostringstream msg;

    // clean this shityy code
    msg << _headers["http-version"] << " " << statusCode << " " 
        << HttpStatus::reasonPhrase(statusCode) << "\r\n"
        << "Connection: close" << "\r\n"
        << "Date: " << _headers["Date"] << "\r\n"
        << "Content-Type: text/html\r\n"
        << "Server: server dyal lah y7sn l3wan" << "\r\n"
        << "Content-Length: " << errorPageContent.size() << "\r\n"
        << "\r\n"
        << errorPageContent;
    sendMessage(_client_fd, msg.str());
    return *this;
}

Response    Response::send( int statusCode, std::string filename)
{
    std::string extension;

    extension = util::GetFileExtension(filename);
    if (!extension.empty())
    {
        std::cerr << "extension: " << extension << std::endl;
        std::string type = MediaTypes::getType(extension.c_str());
        _headers["Content-Type"] =type.empty() ? "text/plain" : type;
    }
    else
        _headers["Content-Type"] = "text/plain";

    // craft a response 
    int fileLength = util::getFileLength(filename);
    file_to_send = open(filename.c_str(), O_RDONLY);
    int nbytes;
    nbytes_left = fileLength;
    std::ostringstream msg;
    msg << _headers["http-version"] << " " << statusCode << " " 
        << HttpStatus::reasonPhrase(statusCode) << "\r\n"
        << "Content-Length: " << fileLength << "\r\n"
        << "Content-Type: " << _headers["Content-Type"] << "\r\n"
        << "Connection: " << _headers["Connection"] << "\r\n"
        << "Date: " << _headers["Date"] << "\r\n"
        << "\r\n";
        // << readRaw(file_to_send, BUFSIZE, nbytes)
        // << readRaw(file_to_send, fileLength);

    
    // send it to the client
    sendMessage(_client_fd, msg.str());
    
    // open file
    return *this;
}

Response    Response::sendContent( int statusCode, std::string content)
{
    _headers["Content-Type"] = "text/html";
    // craft a response 
    int fileLength = content.length();
    std::ostringstream msg;
    msg //<< _headers["http-version"] 
    << "HTTP/1.1 " << statusCode << " " 
        << HttpStatus::reasonPhrase(statusCode) << "\r\n"
        << "Content-Type: " << _headers["Content-Type"] << "\r\n"
        << "Content-Length: " << fileLength << "\r\n"
        << "Connection: " << _headers["Connection"] << "\r\n"
        << "Date: " << _headers["Date"] << "\r\n";
        if (getHeader("Location") != "")
            msg << "Location: " << getHeader("Location") << "\r\n";
        else if (getHeader("Status") != "")
            msg << "Status: " << getHeader("Status") << "\r\n";
        msg<< "\r\n"
        << content;
    
    
    // send it to the client
    sendMessage(_client_fd, msg.str()); 

    return *this;
}

std::string Response::readRaw(int fd, int fileLength, int &bytes_read)
{
    char buf[BUFSIZE];
    bytes_read = read(fd, buf, std::min(BUFSIZE, fileLength));
    nbytes_left -= bytes_read;
    // std::cout << "nbytes: " <<  nbytes_left << std::endl;
    // return std::string(buf, bytes_read);
    return std::string(buf, bytes_read);
}


int Response::sendMessage(int fd, const std::string &s)
{
    return sendRaw(fd, s.c_str(), s.length());
}

void a(int a){}
int Response::sendRaw(int fd, const void *buf, int buflen)
{
    const char *pbuf = static_cast<const char*>(buf);

    signal(SIGPIPE, a);
    std::cout << "written: " << 0 << std::endl;
    std::cout << pbuf << std::endl;
    int bytes_written = write(fd, pbuf, buflen);
    signal(SIGPIPE, SIG_DFL);
    std::cout << "written: " << bytes_written << std::endl;
    //todo right?
    if (bytes_written <= 0)
        throw std::runtime_error("Could not write to client");
    return 0;
}

std::string Response::getHeader(std::string header_name)
{
    return _headers[header_name];
}

void Response::setHeader(std::string header_name, std::string value)
{
    //std::cerr << header_name << " : " << value << std::endl;
    _headers[header_name] = value;
}

std::map<std::string, std::string> Response::getHeaders() const
{
    return _headers;
}

int  Response::getClientFd() const
{
    return _client_fd;
}

Config  Response::getServerConfig() const
{
    return _serverConfig;
}