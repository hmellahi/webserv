#include "Request.hpp"

Request::Request(void)
{
}

Request::Request(std::string buffer, int buffSize):_status(0)
{
	std::cout << buffSize << std::endl;
	// _buffer = std::string(buffer, buffSize);
	_buffer = buffer;
	std::cout << "-------------------------------------\n";
	std::cout << "buffersize before" << buffSize << std::endl;
	std::cout << "-------------------------------------\n";
	// if (buffSize != _buffer.size())
	// {
	// 	std::cout << "-------------------------------------\n";
	// 	std::cout << "diff" << std::endl;
	// 	std::cout << "-------------------------------------\n";
	// 	// exit(0);
	// }
	_buffSize = buffSize;
	parse();
}

Request::Request(Request const &src)
{
	*this = src;
}

Request & Request::operator=(Request const &rhs)
{
	if (this != &rhs)
	{
		_buffer = rhs._buffer;
		_headers = rhs._headers;
		_method = rhs._method;
		_url = rhs._url;
		_http_version = rhs._http_version;
		_query = rhs._query;
		_content_body = rhs._content_body;
		_status = rhs._status;
		_query_map = rhs._query_map;
	}
	return (*this);
}

Request::~Request(void)
{
}

void Request::parse()
{
	// here lets parse the request getted
	std::vector<std::string> lines;
	
	lines = util::split(_buffer, "\n");
	if (lines.size() > 1)
	{
		ParseFirstLine(lines[0]);
		ParseHeaders(lines);
		if (_headers.find("	-Encoding") != _headers.end() && _headers["Transfer-Encoding"] == "chunked")
			ParseChunkBody(_buffer);
		else
			ParseBody(_buffer);
		// std::cout << "|" << _content_body << "|" << std::endl;
	}
	else
		_status = HttpStatus::BadRequest;; // Bad request
}

void Request::ParseFirstLine(std::string line)
{
	std::vector<std::string> tokens;
	std::vector<std::string> url_token;
	tokens = util::split(line, " ");
	if (tokens.size() == 3)
	{
		_method = tokens[0];
		url_token = util::split(tokens[1], "?");
		_url = url_token[0];
		_query = url_token.size() == 2 ? url_token[1] : "";
		_url.erase(0, 1); // erase "/" at the start
		if (!_query.empty())
			parse_query(_query);
		_http_version = util::trim(tokens[2]);
		if (util::is_valid_method(_method) == false)
			_status = HttpStatus::NotImplemented; // server not support this method
		else if (util::is_valid_version(_http_version) == false)
			_status = HttpStatus::HTTPVersionNotSupported; // HTTP VERSION NOT SUPPORTED
	}
	else
		_status = HttpStatus::BadRequest;; // bad request
}

void Request::parse_query(std::string &query)
{
	std::vector<std::string> tokens;
	std::string key;
	std::string value;
	int pos;

	tokens = util::split(query, "&");
	for (int i = 0; i != tokens.size(); i++)
	{
		pos = tokens[i].find("=");
		if (pos != std::string::npos)
		{
			key = tokens[i].substr(0, pos);
			value = tokens[i].substr(pos + 1, tokens[i].length());
			key = util::trim(key);
			value = util::trim(value);
		}
		else
		{
			key = tokens[i];
			value = "";
		}
		_query_map[key] = value;
	}
}

void Request::ParseHeaders(std::vector<std::string> lines)
{
	std::string key;
	std::string value;
	int pos;
	for (int i = 1; i < lines.size() && !lines[i].empty(); i++)
	{
		// std::cout << "[" << lines[i] << "] " << std::endl;
		if (util::trim(lines[i]).empty())
			continue;
		pos = lines[i].find(":");
		// todo: check with nginx if its neccesary
		if (pos == std::string::npos)
		{
			_status = HttpStatus::BadRequest;
			return ;
		}
		key = lines[i].substr(0, pos);
		value = lines[i].substr(pos + 1, lines[i].length());
		key = util::trim(key);
		value = util::trim(value);
		_headers[key] = value;
		// std::cout << lines[i] << std::endl;
	}
	if (_headers.find("Host") == _headers.end())
		_status = HttpStatus::BadRequest;
}

void Request::ParseBody(std::string &buffer)
{
	//  Each heading is followed by a line feed character \r\n.
	int i  = buffer.find("\r\n\r\n");
	int count = 0;
	if (i != _buffSize)
	{
		if (_headers.find("Content-Length") != _headers.end())
		{
			i += 4;
			while (buffer[i] && count < atoi(_headers["Content-Length"].c_str()))
			{
				// _content_body += buffer[i];
				_content_body.push_back(buffer[i]);
				i++;
				count++;
			}
		}
	}
	// std::cout << "-------------------------------------\n";
	// std::cout << "reqsize" << i << std::endl;
	// std::cout << "-------------------------------------\n";
}

void Request::ParseChunkBody(std::string &buffer)
{
	std::string body;
	std::string hex;
	int size;
	int i = 0;

	body = buffer.substr(buffer.find("\r\n\r\n") + 4, buffer.size() - 1);
	hex = body.substr(0, body.size());
	size = util::to_hex(hex);

	while (size)
	{
		i = body.find("\r\n", i) + 2;
		// _content_body += body.substr(i, size);
		i += size + 2;
		hex = body.substr(i, body.size());
		size = util::to_hex(hex);
	}
}

// getters accessors
std::string Request::getHeader(std::string header_name)
{
	return _headers[header_name];
}

void Request::setHeader(std::string header_name, std::string value)
{
	 _headers[header_name] = value;
}

std::map<std::string, std::string> Request::getHeaders(void) const
{
	return (_headers);
}

std::string Request::getMethod(void) const
{
	return (_method);
}

std::string Request::getUrl(void) const
{
	return (_url);
}

std::string Request::getHttpVersion(void) const
{
	return (_http_version); 
}

std::vector<char>  Request::getContentBody(void) const
{
	return (_content_body);
}

void	Request::setUrl(std::string url)
{
	_url = url;
}

int Request::getStatus(void) const
{
	return (_status);
}


