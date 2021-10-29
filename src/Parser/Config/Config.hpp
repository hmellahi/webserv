#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <iostream>
#include <algorithm>
#include <map>
#include <vector>
#include "../utils/Utils.hpp"
#include <string>

class Config
{
	public:
		Config();
		Config(Config const &src);
		Config &operator=(Config const &rhs);
		~Config();
		void init_map_attr();
		void init_map_loc();
		void check_server(std::vector<std::string>::iterator &it);
		// functions to save to the attributes
		void root(std::vector<std::string>::iterator &it);
		void page_error(std::vector<std::string>::iterator &it);
		void client_max_body_size(std::vector<std::string>::iterator &it);
		void isAutoIndexOn(std::vector<std::string>::iterator &it);
		void redirectionPath(std::vector<std::string>::iterator &it);
		void allow_methods(std::vector<std::string>::iterator &it);
		void upload_path(std::vector<std::string>::iterator &it);
		void cgi(std::vector<std::string>::iterator &it);		
		void server_name(std::vector<std::string>::iterator &it);
		void listen(std::vector<std::string>::iterator &it);
		void locations(std::vector<std::string>::iterator &it);
		void index(std::vector<std::string>::iterator &it);
		// getters 
		std::string getRoot(void) const;
		std::map<int, std::string> get_error_pages(void) const;
		int get_client_max_body_size(void) const;
		std::vector<std::string> getIndex(void) const;
		std::pair<int, std::string> get_redirectionPath(void) const;
		std::vector<std::string> get_allowedMethods(void) const;
		std::string getUploadPath(void) const;
		std::map<std::string, std::string> getCGI(void) const;
		std::vector<std::string> getServerName(void) const;
		std::vector<u_int32_t> getPorts(void) const;
		bool get_isAutoIndexOn(void) const;
		std::map<std::string, Config> getLocation(void) const;	

		Config		getConfig(std::string path) const;
		std::string getRoot(std::string path) const;
		std::string getUploadPath(std::string path) const;
		std::vector<std::string> getAllowedMethods(std::string path) const;
		std::pair<int, std::string> getRedirectionPath(std::string path) const;
		std::vector<std::string> getIndex(std::string path) const;
		bool isAutoIndexOn(std::string path) const;
		int getClientMaxBodySize(std::string path) const;

		void	setIndex(std::vector<std::string> indexFiles);
		void	setRoot(std::string root);
		void	setAllowedMethods(std::vector<std::string> allowedMethods);
	
//	private:
	   	std::string	_root;
		std::map<int, std::string>	_error_pages;
		int	_client_max_body_size;
		bool _isAutoIndexOn;
		std::vector<std::string> _index;
		std::pair<int, std::string> _redirectionPath;
		std::vector<std::string> _allowedMethods;
		std::string			_uploadPath;
		std::map<std::string, std::string>	_cgi;
		std::vector<std::string>	_server_name;
		std::vector<u_int32_t>	_listen;
		std::map<std::string, Config>	_locations;
};
#endif
