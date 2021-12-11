#pragma once

# include "macros.hpp"
# include "Request.hpp"

class CGI
{
   public:
      // static std::string     exec_file(std::string path);
      static std::pair<std::string, std::map<std::string , std::string> >     exec_file(std::string path, Request &req, std::string cgiPath);
      // static void exec_file_py(std::string path, Request &req)
};