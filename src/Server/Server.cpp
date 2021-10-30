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
			std::cout << "Connection started" << std::endl;
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
	for (location = locations.begin(); location != locations.end(); location++)
	{
		std::string locationPath = location->first;
		// std::cout << "location" << locationPath << std::endl;
		if (!strncmp(locationPath.c_str(), path.c_str(), locationPath.size()))
		{
			_locConfig = location->second;
			return locationPath;
		}
	}
	return "";
	// if (location == locations.end()) // means a location with the given path isnt found
		// return ;
	// erase the location path from root
	// _locConfig._root.erase(, path.size());
	// std::cout << "not found" << std::endl;
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
			// server->updateLocationConfig(req.getUrl());
			if (*server_name == host)
				return server->handleRequest(req, client_fd);
		}
	}
	// server->_locConfig = server->_config;
	// server[0].updateLocationConfig(req.getUrl());
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
	static int c;
	Response res;
	std::string requestBodyStr;
	int j;
	for (int i = 0; i < clients.size(); i++)
	{
		// sd : socket descriptor
		sd = clients[i];
		if (FD_ISSET(sd, &readfds))
		{
			valread = read(sd, requestBody, 1024);
			// valread = 1;
			// j = -1;
			// while (valread > 0)
			// {
			// 	valread = read(sd, requestBody, 1024);
			// 	j++;
			// 	DBG("reading");
			// }
			// DBG("done");
			// Check if it was for closing
			c += valread;
				std::cout << "-------------------------------------\n";
			std::cout << "buffer: " << c << std::endl;
				std::cout << "-------------------------------------\n";
			if (valread <= 0)
			{
				// Somebody disconnected
				std::cout << "Connection closed" << std::endl;
				// Close the socket and remove client from client list
				closeConnection(clients, readfds, i, sd);
			}
			// otherwise handle the request
			else
			{
				// std::cout << "-------------------------------------\n"
				// 		<< "client with id " << sd 
				// 		<< "--------------------------------" << std::endl;
				requestBody[valread] = '\0';
				
				// std::cout << "\n\nsize\n\n" << strlen(requestBody) << std::endl;
				requestBodyStr = requestBody;
				// std::cout << "\n\nsize\n\n" << requestBodyStr.size() << std::endl;
				// if (requestBodyStr != requestBody) {
				// 	std::cout << "nooooooooooooo\n";
				// }
				// set the string terminating NULL byte on the end
				// convert requestBody to a string
				// handle connection and store response
				res = handleConnection(requestBodyStr, sd, servers);
				// if (res.getHeader("Connection") == "close")
					// closeConnection(clients, readfds, i, sd);
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

char	*join(char *s1, char *s2, int s1len, int s2len)
{
	unsigned int	i;
	unsigned int	l;
	char			*s3;

	if (!s1 || !s2)
		return (NULL);
	i = s1len + s2len;
	s3 = (char *)malloc((i + 1) * sizeof(char));
	if (!s3)
		return (NULL);
	s3[i] = '\0';
	i = 0;
	while (i <= s1len)
	{
		s3[i] = s1[i];
		i++;
	}
	l = 0;
	while (i <= s2len)
	{
		s3[i++] = (char)s2[l];
		l++;
	}
	return (s3);
}


Response Server::handleRequest(Request req, int client_fd)
{
	int contentLength =  atoi(req.getHeader("Content-Length").c_str());
	// if (contentLength > 0)
	// {
	// 	if (req.getContentBody().size() < contentLength)
	// 	{
	// std::cout << "-------------------------------------\n";
	// std::cout << "ContentLength:" << req.getContentBody().size()  << std::endl;
	// std::cout << "-------------------------------------\n";
	std::map<int, Request>::iterator it = unCompletedRequests.find(client_fd);
	if (it == unCompletedRequests.end() && req.getContentBody().size() < contentLength)
	{
		unCompletedRequests[client_fd] = req;
		return Response();
	}
	else if (it != unCompletedRequests.end())
	{
		(it->second)._content_body += req._buffer;
		req = it->second;
		int contentLength = atoi(req.getHeader("Content-Length").c_str());
		// std::cout << "-------------------------------------\n";
		// std::cout << "recieved:" << req.getContentBody().size() << "| max: " << contentLength << std::endl;
		// std::cout << "-------------------------------------\n";
		unCompletedRequests[client_fd] = req;
		if (req.getContentBody().size() < contentLength)
			return Response();
		unCompletedRequests.erase(it);
	}
	// std::cout << "-------------------------------------\n";
	// std::cout << "Content:" << req.getContentBody() << std::endl;
	// std::cout << "Done Reading : " <<  req.getContentBody().size() << std::endl;
	// std::cout << "-------------------------------------\n";
	std::string locationPath = updateLocationConfig("/" + req.getUrl());
	// std::cout << "old url:" << req.getUrl() << std::endl;
	// req.setUrl((req.getUrl()).erase(0, locationPath.size()));
	// std::cout << "new url:" << req.getUrl() << std::endl;
	// Check if the request body is valid
	int isRequestValid = req.getStatus() == 0;
	Response res(req, client_fd, _locConfig);
	// if (!isRequestValid)
	// {
		// res.send(req.getStatus());
		// return (res);
	// }
	// Check if the request body size doesnt exceed
	// the  max client body size
	// int contentLength =  atoi(req.getHeader("Content-Length").c_str());
	// if (_locConfig.get_client_max_body_size() < contentLength)
	// {
	// 	res.send(HttpStatus::PayloadTooLarge);
	// 	return (res);
	// }
	// std::cout << "Content-Length: " << contentLength << std::endl;
	char buffer[1025];
	int nbytes;
	// std::cout << "\n-------------------------------------\n";
	// std::cout << "BODY SIZE " << req.getContentBody().size() << std::endl;
	// std::cout << "\n-------------------------------------\n";
	int bytes_read = req.getContentBody().size();
	// std::cout << req.getContentBody();
	// while (bytes_read < contentLength)
	// {
	// 	// nbytes = read(client_fd, buffer, 1024);//std::min(1024, bytes_read - nbytes));
	// 	nbytes = read(client_fd, buffer, std::min(1024, contentLength - bytes_read));
	// 	if (nbytes <= 0) break;
	// 	buffer[nbytes] = '\0';
	// 	req._content_body += buffer;
	// 	std::cout << buffer;
	// 	bytes_read += nbytes;
	// 	// std::cout << "read: " << bytes_read << "| bodySize : " << contentLength << std::endl;
	// }
	// std::cout << "\n-------------------------------------\n";
	// std::cout << "done reading" << std::endl;
	// std::cout << "\n-------------------------------------\n";
	// req._content_body = complete_body;
	int methodINdex = getMethodIndex(req.getMethod());
	std::string location = "/" + req.getUrl();

	// Check if there is a redirection
	std::pair<int, std::string> redirection = _locConfig.get_redirectionPath();
	// std::cout << redirection.first << std::endl;
	if (redirection.first != 0)
	{
		int status_code = redirection.first;
		std::string location = redirection.second;
		if (!location.empty() && location[0] == '/')
			location = util::getFullUrl(req.getUrl(), req.getHeader("Host"));
		std::cout << "redirection happened to " << location << std::endl;
		res.sendRedirect(status_code, location);
		return res;
	}

	bool isAllowed = checkPermissions(req.getMethod());

	if (!isAllowed)
		res.send(HttpStatus::MethodNotAllowed);
	else
		(this->*handleMethod(methodINdex))(req, res);
	return (res);
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
		/// std::cout << "allowed methods: " << allowedMethods[0] << std::endl;
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

void Server::getHandler(Request req, Response res)
{
	std::string filename = _locConfig.getRoot() + req.getUrl();
    // std::cout << filename << std::endl;
	int status = FileSystem::getFileStatus(filename.c_str());
	// std::cout << "-------------------------------------\n";
	// std::cout << "Status"<< status << std::endl;
	// std::cout << "-------------------------------------\n";
	// check the file requested is a directory
	// DBG("HELLO");
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
	
	// check if the requested file isnt a static file
	// if so then pass it to CGI
	std::string fileExtension = util::GetFileExtension(filename.c_str());
	std::map<std::string, std::string> cgis = _locConfig.getCGI();
	std::map<std::string, std::string>::iterator cgi;
	std::string cgiOutput;
	for (cgi = cgis.begin(); cgi != cgis.end(); ++cgi)
	{
		std::string cgiType = cgi->first;
		std::string cgiPath = cgi->second;
		// std::cout <<"filename" << cgiType << std::endl;
		if (fileExtension == cgiType)
		{
			try {
				// execute the file using approriate cgi
				std::cout <<"filename : " << filename << std::endl;
				std::cout <<"content" << cgiOutput << std::endl;
				cgiOutput = CGI::exec_file(filename.c_str());
				return res.sendContent(HttpStatus::OK, cgiOutput);
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
	std::vector<std::string> tokens = util::split(req.getUrl(), "/");
	std::string filename = tokens[tokens.size() - 1];
	if (!filename.empty())
	{
		std::string uploadLocation = _locConfig.getUploadPath() + filename;
		try {
			FileSystem::uploadFile(uploadLocation, req.getContentBody());
			return res.send(HttpStatus::Created);
		}
		catch (std::exception& e)
		{
			return res.send(HttpStatus::InternalServerError); //todo compare with nginx?	
		}
	}
	res.send(HttpStatus::Forbidden);
}

void Server::deleteHandler(Request req, Response res)
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

		std::vector<u_int32_t> ports = serverConfig->getPorts();
		// if (ports.size() == 0)
		// {
		// 	if (usedPorts.find(DEFAULT_PORT) == usedPorts.end())
		// 	{
		// 		new_socket = newServer.addPort(DEFAULT_PORT);
		// 		serversSockets.push_back(new_socket);
		// 		usedPorts.insert(DEFAULT_PORT);
		// 	}
		// }
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
