#include "Request.hpp"

Request::Request(void)
{
}

Request::Request(std::string buffer, int buffSize):_buffer(buffer),_status(HttpStatus::OK)
{
	_buffSize = buffSize;
	nbytes_left= 0;
	isChunked = false;
	parse();
	// std::cerr << buffer << std::endl;
}

int	Request::getBufferSize( void )  const {
	return _buffSize;
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
		// _buffer = std::string(rhs._buffer, rhs._buffSize);
		_headers = rhs._headers;
		isUpload = rhs.isUpload;
		isChunked = rhs.isChunked;
		_method = rhs._method;
		_url = rhs._url;
		_http_version = rhs._http_version;
		_query = rhs._query;
		_content_body = rhs._content_body;
		_status = rhs._status;
		_bodySize = rhs._bodySize;
		_query_map = rhs._query_map;
		_buffSize = rhs._buffSize;
		fd = rhs.fd;
		isChunkedBody = rhs.isChunkedBody;
		isChunkedBodyEnd = rhs.isChunkedBodyEnd;
		nbytes_left= rhs.nbytes_left;
		_fileLocation = rhs._fileLocation;
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
	// for (int i = 0; i < lines.size(); i++) {
	// 	std::cerr << lines[i] << std::endl;
	// }
	isChunkedBody = false;
	if (lines.size() > 1)
	{
		std::cerr << lines[0] << std::endl;
		ParseFirstLine(lines[0]);
		ParseHeaders(lines);
		if (_headers["Transfer-Encoding"] == "chunked")
		{
			_bodySize = _buffSize - (_buffer.find("\r\n\r\n") + 4);
			isChunkedBody = true;
			std::string body = _buffer.substr(_buffer.find("\r\n\r\n") +4, _buffer.size());
			std::string out = util::ParseChunkBody(unchunked, body, isChunkedBodyEnd);
			_content_body.assign(out.begin(), out.end()); // optimize
		}
		else
		{
			_bodySize = atoi(getHeader("Content-Length").c_str());
			ParseBody(_buffer);
		}
		// std::cerr << "|" << _content_body << "|" << std::endl;
	}
	else
		_status = HttpStatus::BadRequest; // Bad request
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
		else if (util::is_valid_version(_http_version) == false) // todo support only http1.1?
			_status = HttpStatus::HTTPVersionNotSupported; // HTTP VERSION NOT SUPPORTED
	}
	else
		_status = HttpStatus::BadRequest; // bad request
}

void Request::parse_query(std::string &query)
{
	std::vector<std::string> tokens;
	std::string key;
	std::string value;
	size_t pos;

	tokens = util::split(query, "&");
	for (size_t i = 0; i != tokens.size(); i++)
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
	size_t pos;
	for (size_t i = 1; i < lines.size(); i++)
	{
		std::cerr << lines[i] << std::endl;
		if (lines[i] == "\r")
            break;
		if (util::trim(lines[i]).empty())
			continue;
		pos = lines[i].find(":");
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
		// std::cerr << lines[i] << std::endl;
	}
	if (_headers.find("Host") == _headers.end())
		_status = HttpStatus::BadRequest;
}

void Request::ParseBody(std::string &buffer)
{
	//  Each heading is followed by a line feed character \r\n.
	int i  = buffer.find("\r\n\r\n");
	// _buffSize -= i;
	int count = 0;
	if (i != _buffSize)
	{
		if (_headers.find("Content-Length") != _headers.end())
		{
			i += 4;
			while (i != _buffSize && count < atoi(_headers["Content-Length"].c_str()))
			{
				// _content_body += buffer[i];
				_content_body.push_back(buffer[i]);
				i++;
				count++;
			}
		}
	}
	// std::cerr << "-------------------------------------\n";
	// std::cerr << "reqsize" << i << std::endl;
	// std::cerr << "-------------------------------------\n";
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
		std::cerr << "assigning " << body.substr(i, size) << std::endl;
		_content_body.assign( body.substr(i, size).c_str(),  body.substr(i, size).c_str() + size);
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

std::string Request::getQuery() const
{
	return _query;
}


