#include "Response.hpp"

Response::Response(Request req, int client_fd, Config serverConfig)
{
    _client_fd = client_fd;
    _headers["url"] = req.getUrl();
    _serverConfig = serverConfig;
    _headers["http-version"] = req.getHttpVersion();
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

void    Response::sendRedirect(int statusCode, const std::string &location)
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
	// std::cout <<  "res: " << msg.str() << std::endl; // debug
    
    sendMessage(_client_fd, msg.str());
}

void    Response::send( int statusCode)
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
}

void    Response::send( int statusCode, std::string filename)
{
    std::string extension;

    extension = util::GetFileExtension(filename);
    // std::cout << filename << std::endl;
    // std::cout << extension << std::endl;
    if (!extension.empty())
    {
        std::cout << "extension: " << extension << std::endl;
        std::string type = MediaTypes::getType(extension.c_str());
        _headers["Content-Type"] =type.empty() ? "text/plain" : type;
    }
    else
        _headers["Content-Type"] = "text/plain"; // todo fix seg

    // craft a response 
    int fileLength = util::getFileLength(filename);
    std::ostringstream msg;
    msg << _headers["http-version"] << " " << statusCode << " " 
        << HttpStatus::reasonPhrase(statusCode) << "\r\n"
        << "Content-Length: " << fileLength << "\r\n"
        << "Content-Type: " << _headers["Content-Type"] << "\r\n"
        << "Connection: " << _headers["Connection"] << "\r\n"
        << "Date: " << _headers["Date"] << "\r\n"
        << "\r\n";

        if (getHeader("Location") != "") {

            msg << "Location" << getHeader("Location") << "\r\n";
        }
        else if (getHeader("Status") != "") {

            msg << "Status" << getHeader("Status") << "\r\n";
        }
    
	// std::cout << "in rsponse"<<  msg.str() << std::endl; // debug
    // send it to the client
    sendMessage(_client_fd, msg.str());
    
    std::cout << "in rsponse"<<  msg.str() << std::endl; // debug
    // open file and read it by chunks
    readRaw(filename, fileLength);
    std::cout << "aa" << std::endl;
}

void    Response::sendContent( int statusCode, std::string content)
{
    _headers["Content-Type"] = "text/html";
    // craft a response 
    int fileLength = content.length();
    std::ostringstream msg;
    msg << _headers["http-version"] << " " << statusCode << " " 
        << HttpStatus::reasonPhrase(statusCode) << "\r\n"
        << "Content-Type: " << _headers["Content-Type"] << "\r\n"
        << "Content-Length: " << fileLength << "\r\n"
        << "Connection: " << _headers["Connection"] << "\r\n"
        << "Date: " << _headers["Date"] << "\r\n"
        //<< "Server: " << _serverConfig.getServerName()[0] << "\r\n"
        << "\r\n"
        << content;
    
	// std::cout <<  msg.str() << std::endl; // debug
    
    // send it to the client
    sendMessage(_client_fd, msg.str());    
}

void    Response::readRaw(std::string filename, int fileLength)
{
    int fd = open(filename.c_str(), O_RDONLY);
    char buf[BUFSIZE];
    int bytes_read, bytes_written;
    while (fileLength > 0) {
            std::cout << "fileLength: " << fileLength << std::endl;

        bytes_read = read(fd, buf, BUFSIZE);//std::min((int)fileLength, BUFSIZE));
        std::cout << "raed" << std::endl;

        if (bytes_read <= 0) break;
        if (sendRaw(_client_fd, buf, bytes_read) == -1) break;
        std::cout << "sen" << std::endl;
        fileLength -= bytes_read;
    }
    std::cout << "++fileLength: " << fileLength << std::endl;
    close(fd);
}

int Response::sendMessage(int fd, const std::string &s)
{
    return sendRaw(fd, s.c_str(), s.length());
}
void a(int a){
    std::cout << "a" << std::endl;
}
int Response::sendRaw(int fd, const void *buf, int buflen)
{
    const char *pbuf = static_cast<const char*>(buf);
    int bytes_written;

    signal(SIGPIPE, a);
    std::cout << fd << std::endl;
    bytes_written = write(fd, pbuf, buflen);
    signal(SIGPIPE, SIG_DFL);
    
    std::cout << "send" << std::endl;
    if (bytes_written <= 0)
    {
        perror("couldnt send the msg"); // todo should exit?
        return -1;
    }
    return 0;
}

std::string Response::getHeader(std::string header_name)
{
    return _headers[header_name];
}

void Response::setHeader(std::string header_name, std::string value)
{
    std::cout << header_name << " : " << value << std::endl;
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