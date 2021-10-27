#include "Request.hpp"

Request::Request(void)
{
}

Request::Request(std::string &buffer): _buffer(buffer), _status(0)
{
	this->parse();
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
		ParseBody(_buffer);
		// std::cout << "|" << _content_body << "|" << std::endl;
	}
	else
		_status = 400; // Bad request
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
		_http_version = tokens[2];
		if (util::is_valid_method(_method) == false)
			_status = 501; // server not support this method
		else if (util::is_valid_version(_http_version) == false)
			_status = 505; // HTTP VERSION NOT SUPPORTED
	}
	else
		_status = 400; // bad request
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
		pos = lines[i].find(":");
		if (pos == std::string::npos)
		{
			_status = 400;
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
		_status = 400;
}

void Request::ParseBody(std::string &buffer)
{
	//  Each heading is followed by a line feed character \r\n.
	int i  = buffer.find("\r\n\r\n");
	int count = 0;
	if (i != std::string::npos)
	{
		if (_headers.find("Content-Length") != _headers.end())
		{
			i += 4;
			while (buffer[i] && count < atoi(_headers["Content-Length"].c_str()))
			{
				_content_body += buffer[i];
				i++;
				count++;
			}
		}
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

std::map<std::string, std::string> Request::get_headers(void) const
{
	return (_headers);
}

std::string Request::get_method(void) const
{
	return (_method);
}

std::string Request::get_url(void) const
{
	return (_url);
}

std::string Request::get_http_version(void) const
{
	return (_http_version); 
}

std::string Request::get_content_body(void) const
{
	return (_content_body);
}

int Request::get_status(void) const
{
	return (_status);
}


