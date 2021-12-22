#pragma once

#include "macros.hpp"
#include "Config.hpp"
#include "Request.hpp"
#include "CGI.hpp"
#include "Response.hpp"
#include "../utils/FileSystem/FileSystem.hpp"
#include "Socket.hpp"
#include "ParseConfig.hpp"
#include "Indexing.hpp"

class Server;
class Response;

typedef Response (Server::*methodType)(Request, Response);

class Server
{
private:
    int _serverFd;
    std::vector<Socket> _serverSockets;
    struct sockaddr_in _address;
    int addrlen;
    int _port;
    Config _locConfig;
public:
    Config _config;
    std::map<int, Request> unCompletedRequests;
    // ------------ Constructors --------------------------- //
    Server();
    Server(Config config);
    ~Server();
    // ----------------------------------------------------- //

    // ----------------------------------------------------- //
    static void setup(ParseConfig GlobalConfig);
    static void	loop(std::vector<Socket> &serversSockets, std::vector<Server> &servers);
    static void	closeConnection(std::vector<Socket> &clients, fd_set &readfds, int clientIndex, int sd);
    static void	addClients(std::vector<Socket> clients, int &max_fd, fd_set &readfds);
    static void	waitingForConnections(int &activity, fd_set &readfds);
    static void	addServers(std::vector<Socket> &sockets, int &max_sd, fd_set &readfds);
    static void	acceptNewConnection(std::vector<Socket> &clients, std::vector<Socket> &serversSockets, struct sockaddr_in &address, int &addrlen, fd_set &readfds);
    static void     RecvAndSend(std::vector<Socket> &clients, fd_set &readfds,  std::vector<Server> &servers);
    static Response     handleConnection(std::string &requestBody,  int &requestSize, int &client_fd, std::vector<Server> &servers);

    // ----------------------------------------------------- //

    Socket  addPort(int port, std::string host = "127.0.0.1");
    Response    handleRequest(Request req, int client_fd);

    // --------- Methods Handlers ------------------------- //
    methodType    handleMethod(int methodIndex);
    Response    getHandler(Request req, Response res);
    Response    postHandler(Request req, Response res);
    Response    deleteHandler(Request req, Response res);
    Response    methodNotFoundHandler(Request req, Response res);
    int     getMethodIndex(std::string method_name);
    // ----------------------------------------------------- //

    static std::pair<int, std::string>     getErrorPageContent(int status_code, Config _serverConfig);

    // ----------------- Getters & Setters ------------------ //
    std::vector<Socket> getSockets();
    std::vector<std::string>  getServerNames() const;
    bool	checkPermissions(std::string method);
    std::string	    updateLocationConfig(std::string location);
    // ----------------------------------------------------- //
};

// std::map<int, Response> Server::unCompletedResponses;