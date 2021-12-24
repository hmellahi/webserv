#include "Server.hpp"
fd_set writefds;
extern const char* configFilePath;
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
			// if (clients[i].type == READ_SOCKET)
				FD_SET(fd, &readfds);
			// else
				FD_SET(fd, &writefds);
		}
		// highest file descriptor number, need it for the select function
		if (fd > max_fd)
			max_fd = fd;
	}
	// std::cerr << "maxfd: " << max_fd << std::endl;
}

void Server::waitingForConnections(int &activity, fd_set &readfds)//, fd_set &writefds)
{
	activity = select(FD_SETSIZE, &readfds, &writefds, NULL, NULL);
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
		// new connection
		if (FD_ISSET(socketDescriptor, &readfds))
		{
			std::cerr << "Connection started" << std::endl;
			try{
				new_socket = Socket::acceptConnection(socketDescriptor, address, addrlen);
				// if a position is empty
				if (new_socket < FD_SETSIZE)
				{
					new_socket._lastuse = util::get_time();
					clients.push_back(new_socket);
					// std::cerr << "new client:" << new_socket << std::endl;
				}
			}
			catch(std::exception& e)
			{
				// std::cerr << "wtf bro" << std::endl;
			}
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
	std::map<std::string, Config>::reverse_iterator location;
	std::cerr << "path " << path << std::endl;
	for (location = locations.rbegin(); location != locations.rend(); location++)
	{
		std::string locationPath = location->first;
		std::cerr << "location " << locationPath << std::endl;
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
	std::cerr << "****************************************" << std::endl;
	for (int i = 0;i < req.getContentBody().size();i++)
		std::cerr << req.getContentBody()[i];	
	std::cerr << std::endl<< "****************************************" << std::endl;
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
	// if (clients[clientIndex].type == READ)
		// FD_CLR(sd, &readfds);
	// else
		// FD_CLR(sd, &writefds);
	clients.erase(clients.begin() + clientIndex);
	close(sd);
	std::cerr << "Connection closed: " << sd << std::endl;
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
	// bool isDone;
	for (int i = 0; i < clients.size(); i++)
	{
		sd = clients[i];
		// check if socket is expired
		// std::cerr << sd << "| now : " << util::get_time()<<"last time: "<< clients[i]._lastuse << ", pass: "<<  util::get_time() - clients[i]._lastuse<< std::endl;
		if ((util::get_time() - clients[i]._lastuse) > 70)
		{
			closeConnection(clients, readfds, i, sd);
			std::cerr << "new clients size " << clients.size() << std::endl;
			continue;
		}
		// std::cerr << "wsup" << std::endl;
		// isDone = true;
		// check if sd is ready to read
		if (sd > 0 && FD_ISSET(sd, &readfds))
		{
			
			std::cout << "started reading" << std::endl;
			valread = read(sd, requestBody, BUFSIZE);
			std::cout << "end read" << std::endl;
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
				//std::cerr << requestBody<<std::endl;
				// handle connection and store response
				try 
				{
					res = handleConnection(requestBodyStr, valread, sd, servers);
    			}
				catch(std::exception &e)
				{
					std::cerr << "Error: " << e.what() << std::endl;
					closeConnection(clients, readfds, i, sd);
					continue;
				}
				// std::cerr << "nbytes: " <<  res.nbytes_left << std::endl;
				// if (res.nbytes_left > 0)	
				// {
				std::cerr << "new request hehe" << std::endl;
				if (res._msg != "")
				{
					unCompletedResponses[res._client_fd] = res;
					clients[i].type = WRITE_SOCKET; // todo useless
				}
				// }
				// if (!res.nbytes_left && res.getHeader("Connection") == "close")
				// 	closeConnection(clients, readfds, i, sd);
				clients[i]._lastuse = util::get_time();
			}
		}
		else if (FD_ISSET(sd, &writefds))
		{
			// check if doesnt have uncompleted response with this client
			// std::cerr << "ready for writing" << std::endl;
			if (unCompletedResponses.find(sd) == unCompletedResponses.end())
			{
				// isDone = false; // useless for now
				continue;
			}
			std::cout << " writing.." << sd << std::endl;
			res = unCompletedResponses[sd];
			if (res._msg != "")
			{
				std::cout << "a writing to client: " << sd << std::endl;
				try {
					res.sendRaw(sd, res._msg.c_str(), res._msg.size());
				}
				catch(std::exception &e)
				{
					closeConnection(clients, readfds, i, sd);
					unCompletedResponses.erase(sd); 
					std::cout << "Error: " << e.what() << std::endl;
					continue;
				}	
				res._msg = "";
				std::cout << "was here " << res.nbytes_left<< std::endl;
				if (!res.nbytes_left)  
				{
					std::cout << "file closed:" << res.file_to_send << std::endl;
					close(res.file_to_send);
					unCompletedResponses.erase(sd);
					std::cerr << "done " << std::endl;
				}
				else
				{
					unCompletedResponses[sd] = res;
					std::cerr << res.nbytes_left << std::endl;
				}
				if (!res.nbytes_left && res.getHeader("Connection") != "keep-alive")
					closeConnection(clients, readfds, i, sd);
			}
			else if (res.nbytes_left > 0)
			{
				std::cout << "writing to client: " << sd << std::endl;
				std::string to_send;
				// this will set response to Internal server
				if (!FileSystem::isReadyFD(res.file_to_send, READ)) 
				{
					std::cerr << "couldnt read: "<< std::endl;
					closeConnection(clients, readfds, i, sd);
					unCompletedResponses.erase(sd);
					close(res.file_to_send);
					continue;
				}
				else
				{
					try {
						to_send = res.readRaw(res.file_to_send, res.nbytes_left, nbytes);
						std::cout << "done" << std::endl;
					}
					catch(std::exception &e)
					{
						std::cerr << "💣 Error: " << e.what() << "|" << nbytes << std::endl;
						closeConnection(clients, readfds, i, sd);
						unCompletedResponses.erase(sd);
						close(res.file_to_send);
						continue;
					}
				}
				try {
					std::cerr << "sending: "<< std::endl;
					res.sendRaw(sd, to_send.c_str(), nbytes);
					clients[i]._lastuse = util::get_time();
				}
				catch(std::exception &e)
				{
					closeConnection(clients, readfds, i, sd);
					unCompletedResponses.erase(sd);
					close(res.file_to_send);
					std::cerr << "Error: " << e.what() << std::endl;
					continue;
				}
				std::cerr << "left: " << res.nbytes_left << std::endl;
				unCompletedResponses[sd] = res;
				if (res.nbytes_left == 0)
				{
					std::cerr << "file closed:" << res.file_to_send << std::endl;
					close(res.file_to_send); // check where else should be closed
					if (unCompletedResponses.erase(sd))
						std::cerr << "sdina: " << res.nbytes_left << std::endl;

					if (res.getHeader("Connection") != "keey-alive")
						closeConnection(clients, readfds, i, sd);
					else
						clients[i].type = READ_SOCKET;
				}
			}
		}
		// std::cerr << sd << "| now : " << util::get_time()<<"last time: "<< clients[i]._lastuse << ", pass: "<<  util::get_time() - clients[i]._lastuse<< std::endl;
		// if ((util::get_time() - clients[i]._lastuse) > 10)
		// {
		// 	closeConnection(clients, readfds, i, sd);
		// 	std::cerr << "new clients size " << clients.size() << std::endl;
		// }
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
	std::cerr << "Server started, go to " << host << ":" << port << ", started " << new_socket._lastuse << std::endl;
	return new_socket;
}

Response Server::handleRequest(Request req, int client_fd)
{
	std::string locationPath = updateLocationConfig("/" + req.getUrl());
	if (locationPath=="/")locationPath ="";
	std::cerr << "match" << locationPath << std::endl;
	int contentLength = atoi(req.getHeader("Content-Length").c_str());
	std::map<int, Request>::iterator it = unCompletedRequests.find(client_fd);
	// Request unCompletedReq = it->second;
	// Check if the request body is valid
	Response res(req, client_fd, _locConfig);
	// std::cerr << "size"<< unCompletedRequests.size() << std::endl;
	bool isNotCompletedYet = ((req.getContentBody().size() < contentLength && !req.isChunkedBody) || (req.isChunkedBody && !req.isChunkedBodyEnd));
	std::cerr << "completed: "<< !isNotCompletedYet << std::endl;
	if (it == unCompletedRequests.end() && isNotCompletedYet)
	{
		/********************** CHEKS ************************/
		std::cerr << "-------------------------------------\n";
		if (req.getStatus() != HttpStatus::OK)
			return res.send(req.getStatus());
		// Check if the request body size doesnt exceed
		// the max client body size
		std::cerr << "max :" << _locConfig.get_client_max_body_size() << "got " << contentLength << std::endl;
		if (_locConfig.get_client_max_body_size() < req._bodySize) // toddo check for chunked
			return res.send(HttpStatus::PayloadTooLarge);
		/// check if method is allowed
		bool isAllowed = checkPermissions(req.getMethod());
		if (!isAllowed)
			return res.send(HttpStatus::MethodNotAllowed);
		/******************************************************/

		req.isChunked = true;
		// do checks ( permission..)
		static int i;
		req._fileLocation = "/tmp/cgi" + util::ft_itos(i++);
		if (req.getMethod() == "POST" && !_locConfig.getUploadPath().empty())
		{
			req.isUpload = true;
			std::vector<std::string> tokens = util::split(req.getUrl(), "/");
			std::string filename = tokens[tokens.size() - 1];
			if (filename.empty())
				return res.send(HttpStatus::BadRequest);
			req._fileLocation = _locConfig.getUploadPath() + filename;
		}
		req.fd = open(req._fileLocation.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0644);
		if (!FileSystem::isReadyFD(req.fd, WRITE))
		{
			close(req.fd);
			remove(req._fileLocation.c_str());
			return res.send(HttpStatus::InternalServerError);
		}
		std::string _buff;
		int nbytes_wrote = write(req.fd, &(req.getContentBody()[0]), req.getContentBody().size());
		if (nbytes_wrote <= 0)
		{
			close(req.fd);
			remove(req._fileLocation.c_str());
			return res.send(HttpStatus::InternalServerError);
		}
		if (!req.isChunkedBody)
			req.nbytes_left = contentLength - nbytes_wrote;
		std::cerr << "-------------------------------------\n";
		std::cerr << "recieved:" <<  req.nbytes_left << "| max: " << req.getHeader("Content-Length") << std::endl;
		std::cerr << "-------------------------------------\n";
		unCompletedRequests[client_fd] = req;
		return res;
	}
	else if (it != unCompletedRequests.end())
	{
		Request oldRequest = req;
		req = it->second;
		int nbytes_wrote;
		if (!FileSystem::isReadyFD(req.fd, WRITE))
			return res.send(HttpStatus::InternalServerError);
		if (!req.isChunkedBody)
			nbytes_wrote = write(req.fd, oldRequest._buffer.c_str(), oldRequest._buffSize);
		else
		{
			std::string _buff = util::ParseChunkBody(req.unchunked, oldRequest._buffer, req.isChunkedBodyEnd);
			nbytes_wrote = write(req.fd, _buff.c_str(), _buff.size());
		}
		if (nbytes_wrote <= 0)
		{
			remove(req._fileLocation.c_str());
			unCompletedRequests.erase(it);
			close(req.fd);
			std::cerr << "was here \n";
			return res.send(HttpStatus::InternalServerError);
		}
		if (!req.isChunkedBody)
			req.nbytes_left -= nbytes_wrote;
		std::cerr << "-------------------------------------\n";
		std::cerr << "recieved:" <<  req.nbytes_left << "| max: " << req.getHeader("Content-Length") << std::endl;
		std::cerr << "-------------------------------------\n";
		unCompletedRequests[client_fd] = req;
		if ((req.nbytes_left > 0 && !req.isChunkedBody) || (req.isChunkedBody && !req.isChunkedBodyEnd))
			return res;
		unCompletedRequests.erase(it);
		if (req.isUpload)
		{
			std::cerr << "uploaded" << std::endl;
			close(req.fd);
			return Response(req, client_fd, _locConfig).send(HttpStatus::Created);
		}
	}
	std::cerr << "-------------------------------------\n";

	// check the file requested is a directory
	std::string oldUrl = req.getUrl();
	std::string newUrl = req.getUrl().erase(0, locationPath.size());
	req.setUrl(newUrl);
	std::string filename = _locConfig.getRoot() + req.getUrl();
	if (locationPath != "" && FileSystem::getFileStatus(filename) == IS_DIRECTORY && oldUrl[oldUrl.size()-1] != '/')
	{
		std::string location = util::getFullUrl(oldUrl + "/", req.getHeader("Host"));
		return res.sendRedirect(HttpStatus::MovedPermanently, location);
	}

	std::cerr << "-------------------------------------\n";
	if (req.getStatus() != HttpStatus::OK)
		return res.send(req.getStatus());
	
	// Check if the request body size doesnt exceed
	// the max client body size
	if (_locConfig.get_client_max_body_size() < req._bodySize) // CHECK
		return res.send(HttpStatus::PayloadTooLarge);

	int methodIndex = getMethodIndex(req.getMethod());
	std::string location = "/" + req.getUrl();

	// Check if there is a redirection
	std::pair<int, std::string> redirection = _locConfig.get_redirectionPath();
	if (redirection.first != 0)
	{
		int status_code = redirection.first;
		std::string location = redirection.second;
		std::cerr << "before " << location << std::endl;
		if (!location.empty() && location[0] == '/')
			location = util::getFullUrl(location.erase(0, 1), req.getHeader("Host"));
		std::cerr << "my location " << location << std::endl;
		return res.sendRedirect(status_code, location);
	}

	/// check if method is allowed
	bool isAllowed = checkPermissions(req.getMethod());
	
	if (!isAllowed)
		return res.send(HttpStatus::MethodNotAllowed);

	// check if the requested file isnt a static file
	// if so then pass it to CGI
	std::string fileExtension = util::GetFileExtension(filename.c_str());
	std::map<std::string, std::string> cgis = _locConfig.getCGI();
	std::map<std::string, std::string>::iterator cgi;
	std::string cgiOutput;
	std::map<std::string, std::string> headers;
	std::cerr << "am..." << std::endl;

	for (cgi = cgis.begin(); cgi != cgis.end(); cgi++)
	{
		std::string cgiType = cgi->first;
		std::string cgiPath = cgi->second;
		// execute the file using approriate cgi
		if (fileExtension != cgiType) continue;
		try {
			std::cerr << "Was here" << std::endl;
			std::pair<std::string, std::map<std::string, std::string> > cgiRes = CGI::exec_file(filename.c_str(), req, cgiPath);
			cgiOutput = cgiRes.first;
			std::cerr <<"content" << cgiOutput << std::endl;
			headers = cgiRes.second;
			std::map<std::string, std::string>::iterator it;

			it = headers.begin();
			while (it != headers.end())
			{
				if (it->first == "Status") {
					res.setHeader(it->first, it->second);
					std::cerr << "status header is set" << it->second << std::endl;
				}
				else if (it->first == "Location")
					res.setHeader(it->first, it->second);
				it++;

			}
			int statusCode;
			std::istringstream(headers["Status"]) >> statusCode;
			
			res.nbytes_left = 0;
			return res.sendContent( (headers.find("Status") != headers.end() && statusCode) ? statusCode : HttpStatus::OK, cgiOutput);
		}
		catch (const std::exception& e)
		{
			std::cout << "Exception " << e.what() << std::endl;
			if (!strncmp(e.what(), "File Not Found", 14))
				return res.send(HttpStatus::NotFound);
			std::cout << "yoo " << std::endl;
			// smtg went wrong while executing the file
			return res.send(HttpStatus::InternalServerError);
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

	if (allowedMethods.size() == 0)
		return true;
	std::vector<std::string>::iterator it;
	it = find(allowedMethods.begin(), allowedMethods.end(), method);
	return (it != allowedMethods.end());
}

methodType Server::handleMethod(int methodIndex)
{
	std::map<int, methodType> methodsHandler;

	methodsHandler[GET] = &Server::getHandler;
	methodsHandler[POST] = &Server::postHandler;
	methodsHandler[DELETE] = &Server::deleteHandler;
	methodsHandler[4] = &Server::methodNotFoundHandler;
	return (methodsHandler[methodIndex]);
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
			return res.sendRedirect(HttpStatus::MovedPermanently, util::getFullUrl(req.getUrl() + "/", req.getHeader("Host")));
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
	std::vector<std::string> tokens = util::split(req.getUrl(), "/");
	std::string filename = tokens[tokens.size() - 1];

	if (_locConfig.getUploadPath().empty())
	{
		if (FileSystem::getFileStatus(_locConfig.getRoot() + filename) == IS_DIRECTORY)
			return res.send(HttpStatus::Forbidden);
		return getHandler(req, res);
	}
	// std::cerr << filename << "|\n";

	if (!filename.empty())
	{
		std::string uploadLocation = _locConfig.getUploadPath() + filename;
		try {
			// if (atoi(req.getHeaders()["Content-Length"].c_str()) < req.getBufferSize())
				FileSystem::uploadFile(uploadLocation, req.getContentBody());
			std::cerr << "heey" << std::endl;
			return res.send(HttpStatus::Created);
		}
		catch (std::exception& e)
		{
			return res.send(HttpStatus::InternalServerError);
		}
	}
	std::cerr << "couldnt" << std::endl;
	return res.send(HttpStatus::Forbidden);
}

Response Server::deleteHandler(Request req, Response res)
{
	// check if the file is founded and is a file 
	// if its a directory so its forbidden to remove 
	std::string path = _locConfig.getRoot() + req.getUrl();
	std::cerr << "couldnt delete" << std::endl;
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

std::pair<int, std::string> Server::getErrorPageContent(int status_code, Config _serverConfig)
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
			std::string configPath = std::string(configFilePath,  configFilePath + strlen(configFilePath));
			size_t index = configPath.rfind('/');
			std::string errorPagePath = filename;
			if (std::string::npos != index)
				errorPagePath = configPath.substr(0, index) + "/" + errorPagePath;
			return std::make_pair(FileSystem::readFile(errorPagePath, status), "");
		}
		catch (const std::exception &e)
		{
			std::cerr << "exeception" << e.what() << std::endl;
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
	return std::make_pair(-8, html.str());
}

int Server::getMethodIndex(std::string method_name)
{
	if (method_name == "GET")
		return GET;
	else if (method_name == "POST")
		return POST;
	else if (method_name == "DELETE")
		return DELETE;
	return 4;
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
	int a=0;
	while (TRUE)
	{
		// clear the sockets set
		// std::cerr << "done" << ++a << std::endl;
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
	std::map<u_int32_t, std::string>::iterator address;
	std::vector<Server> servers;
	std::map<u_int32_t, std::string> IP_HOSTS;

	for (serverConfig = serversConfigs.begin(); serverConfig != serversConfigs.end(); serverConfig++)
	{
		Server newServer(*serverConfig);

		std::map<u_int32_t, std::string> addresses = serverConfig->getHostPort();
		std::cerr << "server configs size: " << addresses.size() << std::endl;
		// for (port = ports.begin(); port != ports.end(); port++)
		for (address = addresses.begin(); address!= addresses.end(); address++)
		{
			u_int32_t port = address->first;
			std::string host = address->second;
			// if (usedPorts.find(port) == usedPorts.end())
			// {
			std::cerr << "port" << port << std::endl;
			// try {
			if (IP_HOSTS[port] != host)
				serversSockets.push_back(newServer.addPort(port, host));
			IP_HOSTS[port] = host;
			// }
			// catch (std::exception &e)
			// {

			// }
				// 	usedPorts.insert(port);
			// }
		}
		// IP_HOSTS.insert(addresses.begin(), addresses.end());
		servers.push_back(newServer);
	}
	// this is where magic happens :wink:
	loop(serversSockets, servers);

}