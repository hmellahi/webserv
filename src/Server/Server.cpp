#include "Server.hpp"

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
			FD_SET(fd, &readfds);
		// highest file descriptor number, need it for the select function
		if (fd > max_fd)
			max_fd = fd;
	}
}

void Server::waitingForConnections(int &activity, fd_set &readfds)
{
	activity = select(FD_SETSIZE, &readfds, NULL, NULL, NULL);
	Socket::testConnection(activity, "select error");
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
		if (FD_ISSET(socketDescriptor, &readfds))
		{
			new_socket = Socket::acceptConnection(socketDescriptor, address, addrlen);
			// if a position is empty
			if (clients.size() < FD_SETSIZE)
				clients.push_back(new_socket);
		}
	}
}

std::vector<std::string> Server::getServerNames() const
{
	return _config.get_server_name();
}

void	Server::updateLocationConfig(std::string path)
{
	_locConfig = _config;
	std::map<std::string, Config> locations = _locConfig.getLocation();
	std::map<std::string, Config>::iterator location;
	for (location = locations.begin(); location != locations.end(); location++)
	{
		std::string locationPath = location->first;
		std::cout << "location" << locationPath << std::endl;
		if (!strncmp(locationPath.c_str(), path.c_str(), locationPath.size()))
		{
			_locConfig = location->second;
			break;
		}
	}
	std::cout << "not found" << std::endl;
}

Response Server::handleConnection(std::string &requestBody, int &client_fd, std::vector<Server> &servers)
{
	// parse request body and create new request object
	Request req(requestBody);

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
			// std::cout << "host: " << host << std::endl;
			// std::cout << "server_name: " << *server_name << std::endl;
			// std::string = *server_name + ":" + std::to_string(server.get_port());
			// server->updateLocationConfig(req.get_url());
			if (*server_name == host)
				return server->handleRequest(req, client_fd);
		}
	}
	// server->_locConfig = server->_config;
	// server[0].updateLocationConfig(req.get_url());
	return (servers[0].handleRequest(req, client_fd));
}

void Server::closeConnection(std::vector<Socket> &clients, fd_set &readfds, int clientIndex, int sd)
{
	close(sd);
	clients.erase(clients.begin() + clientIndex);
	FD_CLR(sd, &readfds);
}

void Server::RecvAndSend(std::vector<Socket> &clients, fd_set &readfds, std::vector<Server> &servers)
{
	int sd;
	char requestBody[1025];
	int valread;
	Response res;

	for (int i = 0; i < clients.size(); i++)
	{
		// sd : socket descriptor
		sd = clients[i];
		if (FD_ISSET(sd, &readfds))
		{
			// Check if it was for closing
			if ((valread = read(sd, requestBody, 1024)) == 0)
			{
				// Somebody disconnected
				std::cout << "Connection closed" << std::endl;
				// Close the socket and mark as 0 in list for reuse
				closeConnection(clients, readfds, i, sd);
			}
			// otherwise handle the request
			else
			{
				// set the string terminating NULL byte on the end
				requestBody[valread] = '\0';
				// convert requestBody to a string
				std::string requestBodyStr = requestBody;
				// handle connection and store response
				res = handleConnection(requestBodyStr, sd, servers);
				// if (res.getHeader("Connection") == "close")
				closeConnection(clients, readfds, i, sd);
			}
		}
	}
}

std::vector<Socket> Server::getSockets()
{
	return _serverSockets;
}

Socket Server::addPort(int port)
{
	// create new socket on the given port
	Socket new_socket(port);
	// save socker fd
	_serverSockets.push_back(new_socket);
	// debugging
	std::cout << "Server started, go to 127.0.0.1:" << port << std::endl;
	return new_socket;
}

Response Server::handleRequest(Request req, int client_fd)
{
	// Check if the request body is valid
	_locConfig = _config;
	// updateLocationConfig("/" + req.get_url());
	int isRequestValid = req.get_status() == 0; // HttpStatus::OK;

	Response res(req, client_fd, _locConfig);
	if (!isRequestValid)
	{
		res.send(req.get_status());
		return (res);
	}

	// Check if the request body size doesnt exceed
	// the 
	if (_locConfig.get_client_max_body_size() < atoi(req.getHeader("Content-Length").c_str()))
	{
		res.send(HttpStatus::PayloadTooLarge);
		return (res);
	}

	int methodINdex = getMethodIndex(req.get_method());
	std::string location = "/" + req.get_url();

	// Check if there is a redirection
	std::pair<int, std::string> redirection = _locConfig.get_redirectionPath();
	std::cout << redirection.first << std::endl;
	if (redirection.first != 0)
	{
		int status_code = redirection.first;
		std::string location = redirection.second;
		if (!location.empty() && location[0] == '/')
			location = util::getFullUrl(req.get_url(), req.getHeader("Host"));
		std::cout << "redirection happened to " << location << std::endl;
		res.sendRedirect(status_code, location);
		return res;
	}

	/*
		Check if the request with the given method and location is permitted
	*/
	bool isAllowed = true;
	std::vector<std::string> allowedMethods = _locConfig.get_allowedMethods();

	if (allowedMethods.size() > 0)
	{
		/// std::cout << "allowed methods: " << allowedMethods[0] << std::endl;
		std::vector<std::string>::iterator it;
		it = find(allowedMethods.begin(), allowedMethods.end(), req.get_method());
		if (it == allowedMethods.end())
			isAllowed = false;
	}

	if (!isAllowed)
		res.send(HttpStatus::MethodNotAllowed);
	else
		(this->*handleMethod(methodINdex))(req, res);
	return (res);
}

methodType Server::handleMethod(int methodIndex)
{
	std::map<int, methodType> methodsHandler;

	methodsHandler[GET] = &Server::getHandler;
	methodsHandler[POST] = &Server::postHandler;
	methodsHandler[DELETE] = &Server::deleteHandler;

	return (methodsHandler[methodIndex] ? methodsHandler[methodIndex] : &Server::methodNotFoundHandler); // todo clean
}

void Server::getHandler(Request req, Response res)
{
	std::string filename = _locConfig.getRoot() + req.get_url();
    // std::cout << filename << std::endl;
	int status = FileSystem::getFileStatus(filename.c_str());
	// std::cout << "-------------------------------------\n";
	// std::cout << "Status"<< status << std::endl;
	// std::cout << "-------------------------------------\n";
	// check the file requested is a directory
	if (status == IS_DIRECTORY)
	{
		if (filename[filename.length() - 1] != '/')
		{
			std::cout << "-------------------------------------\n";
			std::cout << "[" << util::getFullUrl(req.get_url() + "/", req.getHeader("Host")) << "]" << std::endl;
			// std::cout << "new url" << req.get_url() << std::endl;
			std::cout << "-------------------------------------\n";
			return res.sendRedirect(301, util::getFullUrl(req.get_url() + "/", req.getHeader("Host")));
		}
		// if so then check if there is any default pages in the current dir (index.html index ect..)
		std::string indexFileName = FileSystem::getIndexFile(filename, _locConfig.get_index());
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
				std::cout << "Url: " << req.get_url() << std::endl;
				std::cout << "File: " << filename << std::endl;
				std::cout << "Root: " << _locConfig.getRoot() << std::endl;
				Indexing indexing(_locConfig.getRoot(), req.get_url());

				return res.sendContent(HttpStatus::OK, indexing.getBody());
			}
			if (false)
			{
				// SOON
				//res.send(HttpStatus::NotImplemented);
			}
			// otherwise show permission denied page
			else
				return res.send(HttpStatus::Forbidden);
		}
	}
	else if (status != HttpStatus::OK)
		return res.send(status);
	
	// check if the requested file isnt a static file
	// if so then pass it to CGI
	std::string fileExtension = util::GetFileExtension(filename.c_str());
	std::map<std::string, std::string> cgis = _locConfig.get_cgi();
	std::map<std::string, std::string>::iterator cgi;
	std::string cgiOutput;
	for (cgi = cgis.begin(); cgi != cgis.end(); ++cgi)
	{
		std::string cgiType = cgi->first;
		std::string cgiPath = cgi->second;
		if (fileExtension == cgiType)
		{
			try {
				// execute the file using approriate cgi
				cgiOutput = CGI::exec_file(filename.c_str());
				return res.send(HttpStatus::OK, cgiOutput);
			}
			catch (const std::exception)
			{
				// some went wrong while executing the file
				return res.send(HttpStatus::InternalServerError);
			}
		}
	}
	// otherwise jst read it
	return res.send(HttpStatus::OK, filename);
}

void Server::postHandler(Request req, Response res)
{
	// todo
	res.send(HttpStatus::NotImplemented);
}

void Server::deleteHandler(Request req, Response res)
{
	// check if the file is founded and is a file 
	// if its a directory so its forbidden to remove 
	std::string path = _locConfig.getRoot() + req.get_url();
	if (FileSystem::getFileStatus(path) == HttpStatus::OK)
	{
		if (remove(path.c_str()) == 0)
			res.send(HttpStatus::NoContent);
		else
			res.send(HttpStatus::Forbidden);
	}
	if (FileSystem::getFileStatus(path) == IS_DIRECTORY)
		res.send(HttpStatus::Forbidden);
}

void Server::methodNotFoundHandler(Request req, Response res)
{
	res.send(HttpStatus::NotImplemented);
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
	std::vector<Socket> clients;
	// Socket				new_socket;
	int max_sd;
	int addrlen, activity;
	struct sockaddr_in address;

	// std::cout << "after " << &serversSockets << std::endl;
	// std::cout << servers.size() << std::endl;
	// std::cout << serversSockets.size() << std::endl;

	addrlen = sizeof(address);
	puts("Waiting for connections ...");
	while (TRUE)
	{
		// clear the sockets set
		FD_ZERO(&readfds);
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

		std::vector<u_int32_t> ports = serverConfig->get_listen();
		if (ports.size() == 0)
		{
			if (usedPorts.find(DEFAULT_PORT) == usedPorts.end())
			{
				new_socket = newServer.addPort(DEFAULT_PORT);
				serversSockets.push_back(new_socket);
				usedPorts.insert(DEFAULT_PORT);
			}
		}
		for (port = ports.begin(); port != ports.end(); port++)
		{
			if (usedPorts.find(*port) == usedPorts.end())
			{
				new_socket = newServer.addPort(*port);
				serversSockets.push_back(new_socket);
				usedPorts.insert(*port);
			}
		}	
		servers.push_back(newServer);
	}
	// this is where magic happens :wink:
	loop(serversSockets, servers);
}
