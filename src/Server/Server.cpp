#include "Server.hpp"
fd_set writefds;
std::map<int, Response> unCompletedResponses;

Server::Server(){

};

Server::Server(Config config)
{
	_config = config;
}

Server::~Server()
{
	// for (int i = 0; i < _serverSockets.size(); i++)
	// 	close(_serverSockets[i]);
}

void Server::addClients(std::vector<Socket> clients, int &max_fd, fd_set &readfds)
{
	int fd;
	// add child sockets to set
	for (int i = 0; i < clients.size(); i++)
	{
		// socket descriptor
		fd = clients[i];
		// if valid socket descriptor then add to read list
		if (fd > 0)
		{
			if (clients[i].type == READ_SOCKET)
				FD_SET(fd, &readfds);
			else
				FD_SET(fd, &writefds);
		}
		// highest file descriptor number, need it for the select function
		if (fd > max_fd)
			max_fd = fd;
	}
}

void Server::waitingForConnections(int &activity, fd_set &readfds)//, fd_set &writefds)
{
	activity = select(FD_SETSIZE, &readfds, &writefds, NULL, NULL);
	Socket::testConnection(activity, "select error");
}

bool exists(const std::string& name) {
    if (FILE *file = fopen(name.c_str(), "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    }   
}

void Server::addServers(std::vector<Socket> &sockets, int &max_sd, fd_set &readfds)
{
	std::vector<Socket>::iterator it = sockets.begin();
	for (it = sockets.begin(); it != sockets.end(); it++)
	{
		FD_SET(*it, &readfds);
		if (*it > max_sd)
			max_sd = *it;
	}
}

void Server::acceptNewConnection(std::vector<Socket> &clients, std::vector<Socket> &serversSockets,
								 struct sockaddr_in &address, int &addrlen, fd_set &readfds)
{
	int socketDescriptor;
	Socket new_socket;
	std::vector<Socket>::iterator it = serversSockets.begin();

	for (it = serversSockets.begin(); it != serversSockets.end(); it++)
	{
		socketDescriptor = *it;
		// new connection
		if (FD_ISSET(socketDescriptor, &readfds))
		{
			std::cerr << "Connection started" << std::endl;
			new_socket = Socket::acceptConnection(socketDescriptor, address, addrlen);
			// if a position is empty
			if (clients.size() < FD_SETSIZE)
				clients.push_back(new_socket);
		}
	}
}

std::vector<std::string> Server::getServerNames() const
{
	return _config.getServerName();
}

std::string	Server::updateLocationConfig(std::string path)
{
	_locConfig = _config;
	std::map<std::string, Config> locations = _locConfig.getLocation();
	std::map<std::string, Config>::iterator location;
	std::cout << "path " << path << std::endl;
	for (location = locations.begin(); location != locations.end(); location++)
	{
		std::string locationPath = location->first;
		std::cout << "location " << locationPath << std::endl;
		if (!strncmp(locationPath.c_str(), path.c_str(), locationPath.size()))
		{
			_locConfig = location->second;
			return locationPath;
		}
	}
	return "";
}
Response Server::handleConnection(std::string &requestBody, int &requestSize, int &client_fd, std::vector<Server> &servers)
{
	// parse request body and create new request object
	Request req(requestBody, requestSize);	
	// Loop throught all serversname of all running servers
	// then look for the servername who matches the request host header
	std::vector<Server>::iterator server;
	for (server = servers.begin(); server != servers.end(); server++)
	{
		std::vector<std::string> server_names = server->getServerNames();
		std::vector<std::string>::iterator server_name;
		for (server_name = server_names.begin(); server_name != server_names.end(); server_name++)
		{
			std::string host = util::split(req.getHeader("Host"), ":")[0];
			// std::cerr << "host: " << host << std::endl;
			// std::cerr << "server_name: " << *server_name << s	td::endl;
			// std::string = *server_name + ":" + std::to_string(server.get_port());
			if (*server_name == host)
				return server->handleRequest(req, client_fd);
		}
	}
	return (servers[0].handleRequest(req, client_fd));
}

void Server::closeConnection(std::vector<Socket> &clients, fd_set &readfds, int clientIndex, int sd)
{
	close(sd);
	clients.erase(clients.begin() + clientIndex);
	FD_CLR(sd, &readfds);
	std::cout << "Connection closed" << std::endl;
}

void Server::RecvAndSend(std::vector<Socket> &clients, fd_set &readfds, std::vector<Server> &servers)
{
	int sd;
	char requestBody[BUFSIZE+1];
	int valread;
	int requestSize;
	Response res;
	std::string requestBodyStr;
	int j;
	int nbytes;
	for (int i = 0; i < clients.size(); i++)
	{
		sd = clients[i];
		// check if sd is ready to read
		if (sd > 0 && FD_ISSET(sd, &readfds))
		{
			valread = read(sd, requestBody, BUFSIZE);
			// 	std::cerr << "-------------------------------------\n";
			// std::cerr << "buffer: " << c << std::endl;
			// 	std::cerr << "-------------------------------------\n";
			if (valread <= 0)
			{
				// Somebody disconnected
				// std::cerr << "Connection closed" << std::endl;
				// Close the socket and remove client from client list
				closeConnection(clients, readfds, i, sd);
			}
			// otherwise handle the request
			else
			{
				// std::cerr << "-------------------------------------\n"
				// 		<< "client with id " << sd 
				// 		<< "--------------------------------" << std::endl;
				// set the string terminating NULL byte on the end
				requestBody[valread] = '\0';
				// convert requestBody to a string
				requestBodyStr = std::string(requestBody, valread);
				// handle connection and store response
				requestSize = valread;
				try 
				{
					res = handleConnection(requestBodyStr, requestSize, sd, servers);
    			}
				catch(std::exception &e)
				{
					std::cerr << "Error: " << e.what() << std::endl;
					closeConnection(clients, readfds, i, sd);
				}
				// std::cout << "nbytes: " <<  res.nbytes_left << std::endl;
				if (res.nbytes_left > 0)
				{
					unCompletedResponses[res._client_fd] = res;
					clients[i].type= WRITE_SOCKET;
				}
				else if (res.getHeader("Connection") == "close")
					closeConnection(clients, readfds, i, sd);
			}
		}
		else if (FD_ISSET(sd, &writefds))
		{
			if (unCompletedResponses.find(sd) != unCompletedResponses.end())
			{
				res = unCompletedResponses[sd];
				if (res.nbytes_left > 0)
				{
					std::string to_send = res.readRaw(res.file_to_send,  BUFSIZE, nbytes);
					res.sendRaw(sd, to_send.c_str(), nbytes);
					unCompletedResponses[sd] = res;
					if (res.nbytes_left == 0)
					{
						unCompletedResponses.erase(sd);
						clients[i].type = READ_SOCKET;
						if (res.getHeader("Connection") == "close")
							closeConnection(clients, readfds, i, sd);
					}
				}
			}
		}
	}
}

std::vector<Socket> Server::getSockets()
{
	return _serverSockets;
}

Socket Server::addPort(int port, std::string host)
{
	// create new socket on the given port
	Socket new_socket(port, host);
	// save socker fd
	_serverSockets.push_back(new_socket);
	// debugging
	std::cout << "Server started, go to " << host << ":" << port << std::endl;
	return new_socket;
}


Response Server::handleRequest(Request req, int client_fd)
{
	int contentLength =  atoi(req.getHeader("Content-Length").c_str());
	std::map<int, Request>::iterator it = unCompletedRequests.find(client_fd);
	if (it == unCompletedRequests.end() && req.getContentBody().size() < contentLength)
	{
		// Check if the request body size doesnt exceed
		// the max client body size
		if (_locConfig.get_client_max_body_size() < (contentLength / 1e6))
		{
			std::cerr << "given" << _locConfig.get_client_max_body_size() << "limit :" << (contentLength / 1e6) << std::endl;
			Response res(req, client_fd, _locConfig);
			res.send(HttpStatus::PayloadTooLarge);
			res.setHeader("Connection", "close");
			return (res);
		}
		unCompletedRequests[client_fd] = req;
		return Response();
	}
	else if (it != unCompletedRequests.end())
	{
		for (int i = 0; i < req._buffSize; i++)
			(it->second)._content_body.push_back(req._buffer[i]);
		req = it->second;
		int contentLength = atoi(req.getHeader("Content-Length").c_str());
		std::cerr << "-------------------------------------\n";
		std::cout << "recieved:" << req.getContentBody().size() << "| max: " << contentLength << std::endl;
		std::cerr << "-------------------------------------\n";
		unCompletedRequests[client_fd] = req;
		if (req.getContentBody().size() < contentLength)
			return Response();
		unCompletedRequests.erase(it);
	}
	std::string locationPath = updateLocationConfig("/" + req.getUrl());
	std::cout << "-------------------------------------\n";
	// std::cout << "before |" << req.getUrl() << "|" << std::endl;
	// int x = req.getUrl()[(req.getUrl()).length() - 1] == '/' ? 0 : 1;
	std::cout << locationPath <<std::endl;
	std::string newUrl = req.getUrl().erase(0, locationPath.size());
	req.setUrl(newUrl);
	std::string filename = _locConfig.getRoot() + req.getUrl();
	// if (locationPath[locationPath.size() - 1] == '/')
	// {
		// loc
	// }
	// check the file requested is a directory
	std::cout << "after |" << req.getUrl() << "|" << std::endl;
	std::cout << "-------------------------------------\n";
	// Check if the request body is valid
	Response res(req, client_fd, _locConfig);
	// if (req.getStatus() != HttpStatus::OK) // TODO FIX
	// {
	// 	// todo close connection
	// 	res.send(req.getStatus());
	// 	return (res);
	// }
	
	// Check if the request body size doesnt exceed
	// the max client body size
	if (_locConfig.get_client_max_body_size() < (contentLength / 1e6))
	{
		// todo close connection
		res.send(HttpStatus::PayloadTooLarge);
		return (res);
	}

	int methodIndex = getMethodIndex(req.getMethod());
	std::string location = "/" + req.getUrl();

	// Check if there is a redirection
	std::pair<int, std::string> redirection = _locConfig.get_redirectionPath();
	if (redirection.first != 0)
	{
		int status_code = redirection.first;
		std::string location = redirection.second;
		if (!location.empty() && location[0] == '/')
			location = util::getFullUrl(req.getUrl(), req.getHeader("Host"));
		std::cerr << "redirection happened to " << location << std::endl;
		res.sendRedirect(status_code, location);
		return res;
	}

	/// check if method is allowed
	bool isAllowed = checkPermissions(req.getMethod());
	
	if (!isAllowed)
	{
		// todo close connection
		res.send(HttpStatus::MethodNotAllowed);
		return (res);
	}

	// std::string filename = _locConfig.getRoot() + req.getUrl();
	std::cerr << "-------------------------------------\n";
	std::cerr << _locConfig.getIndex().size() << "|" << filename << "|" << std::endl;
	std::cerr << "-------------------------------------\n";
	// todo
    // if (FileSystem::getFileStatus(filename) == HttpStatus::NotFound)
	// {
	// 	res.send(HttpStatus::NotFound);
	// 	// todo close connection
	// 	return res;
	// }
	// check if the requested file isnt a static file
	// if so then pass it to CGI
	std::string fileExtension = util::GetFileExtension(filename.c_str());
	std::map<std::string, std::string> cgis = _locConfig.getCGI();
	std::map<std::string, std::string>::iterator cgi;
	std::string cgiOutput;
	std::map<std::string, std::string> headers;

	for (cgi = cgis.begin(); cgi != cgis.end(); cgi++)
	{
		std::string cgiType = cgi->first;
		std::string cgiPath = cgi->second;
		// execute the file using approriate cgi
		if (fileExtension != cgiType) continue;
		try {

			if (!exists(filename)) {
				res.send(HttpStatus::NotFound);
				return res;
			}
			std::cerr <<"filename : " << filename << std::endl;
			std::cerr <<"content" << cgiOutput << std::endl;
			std::pair<std::string, std::map<std::string, std::string> > cgiRes = CGI::exec_file(filename.c_str(), req, cgiPath);
			cgiOutput = cgiRes.first;
			headers = cgiRes.second;
			std::map<std::string, std::string>::iterator it;

			it = headers.begin();
			while (it != headers.end())
			{
				if (it->first == "Status")
					res.setHeader(it->first, it->second);
				else if (it->first == "Location")
					res.setHeader(it->first, it->second);
				it++;
			}
			res.sendContent( headers.find("Status") != headers.end() ? std::stoi(headers["Status"]) : HttpStatus::OK, cgiOutput);
			return res;
		}
		catch (const std::exception& e)
		{
			std::cout << "Exception " << e.what() << std::endl;
			// some went wrong while executing the file
			res.send(HttpStatus::InternalServerError);
			return (res);
		}
	}
	return (this->*handleMethod(methodIndex))(req, res);
}

/*
	Check if the request with the given method and location is permitted
*/
bool	Server::checkPermissions(std::string method)
{
	bool isAllowed = true;
	std::vector<std::string> allowedMethods = _locConfig.get_allowedMethods();

	if (allowedMethods.size() > 0)
	{
		std::vector<std::string>::iterator it;
		it = find(allowedMethods.begin(), allowedMethods.end(), method);
		if (it == allowedMethods.end())
			isAllowed = false;
	}
	return isAllowed;
}

methodType Server::handleMethod(int methodIndex)
{
	std::map<int, methodType> methodsHandler;

	methodsHandler[GET] = &Server::getHandler;
	methodsHandler[POST] = &Server::postHandler;
	methodsHandler[DELETE] = &Server::deleteHandler;

	return (methodsHandler[methodIndex] ? methodsHandler[methodIndex] : &Server::methodNotFoundHandler); // todo clean
}

Response Server::getHandler(Request req, Response res)
{
	std::string filename  = "";
	filename = _locConfig.getRoot() + req.getUrl();
	int status = FileSystem::getFileStatus(filename.c_str());
	
	// check the file requested is a directory
	if (status == IS_DIRECTORY)
	{
		if (filename[filename.length() - 1] != '/')
			return res.sendRedirect(301, util::getFullUrl(req.getUrl() + "/", req.getHeader("Host")));
		// if so then check if there is any default pages in the current dir (index.html index ect..)
		std::string indexFileName = FileSystem::getIndexFile(filename, _locConfig.getIndex());
		// if indexfile is found then
		// Concatenate filename and directory path
		if (!indexFileName.empty())
			filename += indexFileName;
		// otherwise
		// check if autoindex is on
		else
		{
			// if so then list all files in the current directory [soon]
			if (_locConfig.get_isAutoIndexOn())
			{
				Indexing indexing(_locConfig.getRoot(), req.getUrl());
				return res.sendContent(HttpStatus::OK, indexing.getBody());
			}
			// otherwise show permission denied page
			else
				return res.send(HttpStatus::Forbidden);
		}
	}
	else if (status != HttpStatus::OK)
		return res.send(status);
	return res.send(HttpStatus::OK, filename);
}

Response Server::postHandler(Request req, Response res)
{
	// todo how to handle static files in post req
	if (_locConfig.getUploadPath().empty())
		return res.send(HttpStatus::Forbidden);
	std::vector<std::string> tokens = util::split(req.getUrl(), "/");
	std::string filename = tokens[tokens.size() - 1];
	std::cout << "Posthandler check : " << req.getBufferSize() << "\n";
	if (!filename.empty()) 
	{
		std::string uploadLocation = _locConfig.getUploadPath() + filename;
		try {

			if (atoi(req.getHeaders()["Content-Length"].c_str()) < req.getBufferSize())
				FileSystem::uploadFile(uploadLocation, req.getContentBody());
			else {
			
				FileSystem::uploadChunkedFile(uploadLocation, req);
			}
			return res.send(HttpStatus::Created);
		}
		catch (std::exception& e)
		{
			return res.send(HttpStatus::InternalServerError); //todo compare with nginx?	
		}
	}
	return res.send(HttpStatus::Forbidden);
}

Response Server::deleteHandler(Request req, Response res)
{
	// check if the file is founded and is a file 
	// if its a directory so its forbidden to remove 
	std::string path = _locConfig.getRoot() + req.getUrl();
	int status = FileSystem::getFileStatus(path);
	if (status == HttpStatus::OK)
	{
		if (remove(path.c_str()) == 0)
			res.send(HttpStatus::NoContent);
		else
			res.send(HttpStatus::Forbidden);
	}
	else if (status == IS_DIRECTORY)
		res.send(HttpStatus::Forbidden);
	else
		res.send(status);
	return res;
}

Response Server::methodNotFoundHandler(Request req, Response res)
{
	return res.send(HttpStatus::NotImplemented);
}

std::string Server::getErrorPageContent(int status_code, Config _serverConfig)
{
	std::map<int, std::string> errorPages = _serverConfig.get_error_pages();
	std::map<int, std::string>::iterator it;
	it = errorPages.find(status_code);
	int status;
	// check if there is a custom error page
	if (it != errorPages.end())
	{
		try
		{
			std::string filename = it->second;
			// todo
			// will be changed to the server root path
			return (FileSystem::readFile("src/Conf/" + filename, status));
		}
		catch (const std::exception)
		{
		}	
	}
	// otherwise make one
	std::ostringstream html;
	html<< "<html><head><title>" << status_code 
		<< " " << HttpStatus::reasonPhrase(status_code)
		<<  "</title></head><body><center><h1>"
		<< status_code 
		<< " " << HttpStatus::reasonPhrase(status_code)
		<< "</h1></center><hr><center>Web server"
		<< "dyal lay7sn l3wan/1.18.0 (Ubuntu)</center></body></html>";
	return (html.str());
}

int Server::getMethodIndex(std::string method_name)
{
	if (method_name == "GET")
		return GET;
	else if (method_name == "POST")
		return POST;
	else if (method_name == "DELETE")
		return DELETE;
	return GET;
}

void Server::loop(std::vector<Socket> &serversSockets, std::vector<Server> &servers)
{
	// set of socket descriptors
	fd_set readfds;
	// Socket				new_socket;
	int max_sd;
	int addrlen, activity;
	struct sockaddr_in address;

	// std::cerr << "after " << &serversSockets << std::endl;
	// std::cerr << servers.size() << std::endl;
	// std::cerr << serversSockets.size() << std::endl;

	addrlen = sizeof(address);
	puts("Waiting for connections ...");
	while (TRUE)
	{
		// clear the sockets set
		std::cerr << "test 0" << std::endl;
		FD_ZERO(&readfds);
		FD_ZERO(&writefds);
		// add all servers sockets to the sockets set  [readfds]

		Server::addServers(serversSockets, max_sd, readfds);

		// add child sockets to the sockets set
		Server::addClients(clients, max_sd, readfds);

		// wait for an activity on one of the client sockets , timeout is NULL ,
		// so wait indefinitely	
		Server::waitingForConnections(activity, readfds);

		// If something happened on the servers sockets ,
		// then its an incoming connection
		Server::acceptNewConnection(clients, serversSockets, address, addrlen, readfds);

		// otherwise its some IO operation on some other socket
		// recieve Client Request And Send Back A Response();
		Server::RecvAndSend(clients, readfds, servers);

	}
}
std::vector<Socket> clients;
	std::vector<Socket> serversSockets;

void Server::setup(ParseConfig GlobalConfig)
{
	std::vector<Config> serversConfigs = GlobalConfig.getServers();
	std::vector<Config>::iterator serverConfig;
	std::vector<u_int32_t>::iterator port;
	std::vector<Server> servers;
	std::set<u_int32_t> usedPorts;
	std::vector<u_int32_t> socets;
	Socket new_socket;

	for (serverConfig = serversConfigs.begin(); serverConfig != serversConfigs.end(); serverConfig++)
	{
		Server newServer(*serverConfig);

		std::vector<u_int32_t> ports = serverConfig->getPorts();
		for (port = ports.begin(); port != ports.end(); port++)
		{
			if (usedPorts.find(*port) == usedPorts.end())
			{
				new_socket = newServer.addPort(*port, "0.0.0.0");// serverConfig->host);
				serversSockets.push_back(new_socket);
				usedPorts.insert(*port);
			}
		}
		servers.push_back(newServer);
	}
	// this is where magic happens :wink:
	loop(serversSockets, servers);

}