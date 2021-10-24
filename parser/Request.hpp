#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <string>
#include <algorithm>
#include <map>
#include <vector>
#include <cstring>
#include "../utils/Utils.hpp"
#include "../Config/macros.hpp"
class Request
{
	public:
		Request();
		Request(std::string &buffer);
		Request(Request const &src);
		Request &operator=(Request const &rhs);
		~Request();
		std::string getHeader(std::string header_name);
		void setHeader(std::string header_name, std::string value);
		void parse();
		void ParseFirstLine(std::string line);
		void ParseHeaders(std::vector<std::string> lines);
		void ParseBody(std::string &buffer);
		void ParseChunkBody(std::string &buffer);
		void parse_query(std::string &query);
		std::map<std::string, std::string> GetHeaders(void) const;
		std::string GetMethod(void) const;
		std::string GetUrl(void) const;
		std::string GetHttpVersion(void) const;
		std::string GetContentBody(void) const;
		int GetStatus(void) const;
	private:
		std::string _buffer;
		std::map<std::string, std::string> _headers;
		std::string _method;
		std::string _url;
		std::string _http_version;
		std::string _query;
		std::map<std::string, std::string> _query_map;
		std::string _content_body;
		int _status;
};
#endif
