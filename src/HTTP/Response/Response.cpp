#include "Response.hpp"

Response::Response(Request req, int client_fd, Config serverConfig)
{
    _client_fd = client_fd;
    _headers["url"] = req.get_url();
    _serverConfig = serverConfig;
    _headers["http-version"] = req.get_http_version();
    // _headers = req.get_headers();
    _headers["Connection"] = req.getHeader("Connection");
    _headers["Date"] = util::getCurrentDate(); 
}

Response::Response()
{

}

Response::Response(const Response& src)
{
    _headers = src.getHeaders();
    _client_fd = src.getClientFd();
    _serverConfig = src.getServerConfig();
}

void    Response::send( int statusCode)
{
    std::string errorPageContent = Server::getErrorPageContent(statusCode, _serverConfig);
    std::ostringstream msg;

    _headers["Content-Type"] = "text/html";
    msg << _headers["http-version"] << " " << statusCode << " " 
        << HttpStatus::reasonPhrase(statusCode) << "\r\n"
        << "Content-Type: text/html\r\n"
        << "Content-Length: " << errorPageContent.size() << "\r\n"
        << "Connection: close" << "\r\n"
        << "Date: " << _headers["Date"] << "\r\n"
        << "\r\n"
        << errorPageContent;
    sendMessage(_client_fd, msg.str());
}

void    Response::send( int statusCode, std::string filename)
{
    std::string extension;

    extension = util::GetFileExtension(filename);
    if (!extension.empty())
        _headers["Content-Type"] = MediaTypes::getType(extension.c_str());
    else
        _headers["Content-Type"] = "text/plain"; // todo fix seg
    // todo
    // add all mandatory response headers
    // craft a response 
    int fileLength = util::getFileLength(filename);
    std::ostringstream msg;
    msg << _headers["http-version"] << " " << statusCode << " " 
        << HttpStatus::reasonPhrase(statusCode) << "\r\n"
        << "Content-Type: " << _headers["Content-Type"] << "\r\n"
        << "Content-Length: " << fileLength << "\r\n"
        << "Connection: " << _headers["Connection"] << "\r\n"
        << "Date: " << _headers["Date"] << "\r\n"
        // << "Server: " << _serverConfig.get_server_name()[0] << "\r\n"
        << "\r\n";
    
	// std::cout <<  msg.str() << std::endl; // debug
    
    // send it to the client
    sendMessage(_client_fd, msg.str());
    
    // open file and read it by chunks
    readRaw(filename, fileLength);
}

void    Response::readRaw(std::string filename, int fileLength)
{
    int fd = open(filename.c_str(), O_RDONLY);
    // std::string filename;
    char buf[BUFSIZE];
    int bytes_read, bytes_written;
    while (fileLength > 0) {
        bytes_read = read(fd, buf, BUFSIZE);//std::min((int)fileLength, BUFSIZE));
        if (bytes_read <= 0) break;
        if (sendRaw(_client_fd, buf, bytes_read) == -1) break;
        fileLength -= bytes_read;
    }
    close(fd);
}

int Response::sendMessage(int fd, const std::string &s)
{
    return sendRaw(fd, s.c_str(), s.length());
}

int Response::sendRaw(int fd, const void *buf, int buflen)
{
    const char *pbuf = static_cast<const char*>(buf);
    int bytes_written;

    while (buflen > 0) {
        bytes_written = write(fd, pbuf, buflen);
        // std::cout << pbuf << std::endl;
        if (bytes_written == -1) return -1;
        pbuf += bytes_written; 
        buflen -= bytes_written;
    }

    return 0;
}

std::string Response::getHeader(std::string header_name)
{
    return _headers[header_name];
}

void Response::setHeader(std::string header_name, std::string value)
{
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