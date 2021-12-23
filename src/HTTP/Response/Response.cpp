#include "Response.hpp"

Response::Response(Request req, int client_fd, Config serverConfig)
{
    _client_fd = client_fd;
    _headers["url"] = req.getUrl();
    _serverConfig = serverConfig;
    _headers["http-version"] = req.getHttpVersion();
    if (_headers["http-version"].empty())
        _headers["http-version"] =  "HTTP/1.1";
    _headers["Connection"] = req.getHeader("Connection");
    _headers["Date"] = util::getCurrentDate(); 
    file_to_send=-1;
    nbytes_left = 0;
}

Response::Response()
{
    nbytes_left = 0;
    file_to_send=0;
    _headers["http-version"]= "HTTP/1.1";
}

Response::Response(const Response& src)
{
    _headers = src.getHeaders();
    _client_fd = src.getClientFd();
    _serverConfig = src.getServerConfig();
    file_to_send = src.file_to_send;
    nbytes_left = src.nbytes_left;
    _msg = src._msg;
}

Response    Response::sendRedirect(int statusCode, const std::string &location)
{
    // todo change
    // std::string redirectPageContent = Server::getErrorPageContent(statusCode, _serverConfig);
    std::string redirectPageContent ="";
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

Response    Response::send(int statusCode)
{
    std::pair<int, std::string> errorPageContent = Server::getErrorPageContent(statusCode, _serverConfig);
    std::ostringstream msg;
    if (HttpStatus::isError(statusCode))
        _headers["Connection"]="close";
    else
        _headers["Connection"]="keep-alive";
    // clean this shityy code
    file_to_send = errorPageContent.first;
    if (file_to_send == -8)
        nbytes_left = 0;
    else
        nbytes_left = util::getFileLength(errorPageContent.first);
    msg << _headers["http-version"] << " " << statusCode << " " 
        << HttpStatus::reasonPhrase(statusCode) << "\r\n"
        << "Connection: " <<_headers["Connection"] << "\r\n"
        << "Date: " << _headers["Date"] << "\r\n"
        << "Content-Type: text/html\r\n"
        << "Server: server dyal lah y7sn l3wan" << "\r\n"
        << "Content-Length: " << (nbytes_left ? nbytes_left: errorPageContent.second.size()) << "\r\n\r\n";
        if (file_to_send == -8)
            msg << errorPageContent.second;
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
    if (file_to_send >= FD_SETSIZE || file_to_send < 0)
        throw std::runtime_error("couldnt do shit abt it");
    nbytes_left = fileLength;
    std::ostringstream msg;
    msg << _headers["http-version"] << " " << statusCode << " " 
        << HttpStatus::reasonPhrase(statusCode) << "\r\n"
        << "Content-Length: " << fileLength << "\r\n"
        << "Content-Type: " << _headers["Content-Type"] << "\r\n"
        << "Connection: " << _headers["Connection"] << "\r\n"
        << "Date: " << _headers["Date"] << "\r\n"
        << "\r\n";
    
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
    char buf[1000];
    bytes_read = read(fd, buf, 100);
    if (bytes_read <= 0)
        throw std::runtime_error("couldnt read");
    nbytes_left -= bytes_read;
    return std::string(buf, bytes_read);
}


int Response::sendMessage(int fd, const std::string &s)
{
    // return sendRaw(fd, s.c_str(), s.length());
    _msg = s;
    return 1;
}

void a(int a){}
int Response::sendRaw(int fd, const void *buf, int buflen)
{
    const char *pbuf = static_cast<const char*>(buf);

    signal(SIGPIPE, a);
    // std::cerr << "written: " << 0 << std::endl;
    std::cout << "-------------" << std::endl;
    std::cout << pbuf << std::endl;
    std::cout << "-------------" << std::endl;
    int bytes_written = write(fd, pbuf, buflen);
    signal(SIGPIPE, SIG_DFL);
    std::cout << "written: " << bytes_written << std::endl;
    if (bytes_written <= 0)
        throw std::runtime_error("Could not write to client");
    // _msg = pbuf;
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