#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <string>
#include <algorithm>
#include <map>
#include <vector>
#include <cstring>
#include "../utils/Utils.hpp"

class Request
{
	public:
		Request();
		Request(std::string buffer, int buffSize);
		Request(Request const &src);
		Request &operator=(Request const &rhs);
		~Request();
		std::string getHeader(std::string header_name);
		void setHeader(std::string header_name, std::string value);
		void parse();
		void ParseFirstLine(std::string line);
		void ParseHeaders(std::vector<std::string> lines);
		void ParseBody(std::string &buffer);
		void parse_query(std::string &query);
		std::map<std::string, std::string> getHeaders(void) const;
		std::string getMethod(void) const;
		std::string getUrl(void) const;
		void	setUrl(std::string url);
		std::string getHttpVersion(void) const;
		std::vector<char> getContentBody(void) const;
		int getStatus(void) const;
		std::string getQuery() const;
		void ParseChunkBody(std::string &buffer);
		std::vector<char> _content_body;
		std::string _buffer;
		int fd;
		int			_buffSize;
		std::string unchunked;
		bool isChunkedBodyEnd;
		bool isChunkedBody;
		int nbytes_left;
		int getBufferSize(void) const;
		bool isUpload;
		std::string _fileLocation;
		bool isChunked;
	private:
		std::map<std::string, std::string> _headers;
		std::string _method;
		std::string _url;
		std::string _http_version;
		std::string _query;
		std::map<std::string, std::string> _query_map;
		int _status;
};
#endif