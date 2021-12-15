#include "CGI.hpp"

extern char** environ;

char        **fill_args(std::string cgiPath, std::string path) {

	char **args = (char **)malloc(sizeof(char *) * 3);

	// args[0] = strdup("/Users/hmellahi/.brew/bin/php-cgi");
    // std::cout << "executing " <<  cgiPath.c_str() << std::endl;
	args[0] = strdup(cgiPath.c_str());
	args[1] = strdup(path.c_str());
	args[2] = (char *)0;
    return (args);
}


std::pair<std::string, std::map<std::string , std::string> >parseOutput( std::string out  ) {


    std::pair<std::string, std::map<std::string , std::string> >  ret;
    std::map<std::string, std::string> headers;
    std::stringstream ss(out);
    std::string line;
    std::string outContent;
    std::vector<std::string> tab;

    outContent = "";
    line = "";
    while (ss.good())
    {
        getline(ss, line, '\n');
        std::cout << line << std::endl;
        line = util::trim(line);
        // std::cerr << "line :" << line << "|" << std::endl;
        // if (line.find("<html>") == std::string::npos && line.length() > 0)
        if (line.length() > 0)
        {
            // headers.push_back(line);
            tab = util::split(line, ": ");
            if (tab[0] == "Set-Cookie") {
    
                headers[tab[0]] = util::trim(tab[1]);
                setenv("HTTP_COOKIE", headers[tab[0]].c_str(), 1);
            }
            else if (tab[0] == "Status") {
            
                setenv("REDIRECT_STATUS", tab[1].c_str(), 0);
                std::cerr << "Status is " << tab[1].c_str() << std::endl;
                headers[tab[0]] = util::trim(tab[1]);
                // setenv("HTTP_COOKIE", headers[tab[0]].c_str(), 1);
            }
            else if (tab[0] == "Location") {
                std::cerr << "testing location " << std::endl;
                headers[tab[0]] = util::trim(tab[1]);
                
            }
            // else if (tab[0] != "X-Powered-By" && tab[0] != "")
           // std::cout << "Found this header "<< "|" << tab[0] << "|" << headers[tab[0]] << "|" << std::endl;

        }
        else
            break ;
    }
    while (ss.good())
    {
        getline(ss, line, '\n');
        outContent += line;
    }
    ret.first = outContent;
    ret.second = headers;
    return ret;

}

std::pair<std::string, std::map<std::string , std::string> > exec_cgi( Request req, char **args , std::string path ) {

    int         fd[2];
    int         nfd[2];
    std::string cgiOutput = "";

    if (pipe(fd) == -1)
        throw std::runtime_error("pipe error");
    if (pipe(nfd) == -1)
        throw std::runtime_error("pipe error");

    pid_t pid = fork();

    if (pid == -1)
        throw std::runtime_error("fork error");
    int status;

    if (pid > 0) {
        close(fd[1]);
        close(fd[0]);
        close(nfd[1]);

        FILE *result;

        result = fdopen(nfd[0], "r");
        char c;
        while((c = fgetc(result)) != EOF)
            cgiOutput += c;
        fclose(result);
        close(nfd[0]);
    }
    else if (pid == 0)
    {
        // todo check 
        if (write(fd[1], req.getContentBody().data(), req.getContentBody().size()) < 0)
            throw std::runtime_error("write error");

        dup2(nfd[1], 1);
        dup2(fd[0], 0);
        close(fd[1]);
        close(fd[0]);
        close(nfd[1]);
        close(nfd[0]);

        if ((status = execve(args[0], args, environ)) == - 1)
            throw std::runtime_error("execve error");

    }
    int i = -1;
    while (args[++i] != NULL)
        free(args[i]);
    free(args);

    // remove cgi useless headers
    return parseOutput(cgiOutput);
}

bool exists(const std::string& name) {
    if (FILE *file = fopen(name.c_str(), "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    }   
}



std::pair<std::string, std::map<std::string , std::string> >  CGI::exec_file(std::string path, Request &req, std::string cgiPath) {

    int fd[2];
    if (!exists(path)) {
       throw std::runtime_error("File Not Found");
    }
    std::pair<std::string, std::map<std::string , std::string> >  ret;
    
    char    **args = fill_args(cgiPath, path);

    char    **envp;
    if (req.getMethod() == "POST")
    {
        if (!req.getHeader("Content-Type").empty())
            setenv("CONTENT_TYPE", req.getHeader("Content-Type").c_str(), 1);
    }
    if (!req.getContentBody().empty())
        setenv("CONTENT_LENGTH", std::to_string(req.getContentBody().size()).c_str(), 1);
    if (!req.getMethod().empty())
        setenv("REQUEST_METHOD", req.getMethod().c_str(), 1);
    setenv("REDIRECT_STATUS", "true", 1);
    setenv("SCRIPT_FILENAME", path.c_str(), 1);
    setenv("GATEWAY_INTERFACE", "CGI/1.1", 1);
    setenv("QUERY_STRING", req.getQuery().c_str(), 1);

    setenv("HTTP_ACCEPT_LANGU© ÏAGE", "en-US,en;q=0.9", 0);
    setenv("HTTP_ACCEPT_ENCODING", "", 0);
    setenv("HTTP_ACCEPT", "" , 0);
    if (!req.getHeader("cookie").empty())
        setenv("cookie", req.getHeader("cookie").c_str(), 1);
    // setenv("HTTP_USER_AGENT", "", 0);
    // setenv("HTTP_UPGRADE_INSECURE_REQUESTS", "", 0);
    // setenv("HTTP_CACHE_CONTROL", "", 0);
    // setenv("HTTP_CONNECTION", "", 0);
    // setenv("HTTP_HOST", "", 0);
    // setenv("SERVER_NAME", "", 0);
    // setenv("SERVER_PORT", "",1);
    // setenv("SERVER_ADDR", "", 0);
    // setenv("REMOTE_PORT", "", 0);
    // setenv("REMOTE_ADDR", "", 0);
    setenv("PATH_INFO", "./cgi-bin", 0);
    setenv("SERVER_SOFTWARE", "server", 0);
    setenv("REQUEST_SCHEME", "http", 0);
    setenv("SERVER_PROTOCOL", "HTTP/1.1", 0);
    // setenv("DOCUMENT_ROOT", "", 0);
    // setenv("DOCUMENT_URI", path.c_str(), 0);
    // setenv("REQUEST_URI", path.c_str(), 0);
    // setenv("SCRIPT_NAME", path.c_str(), 0);
    // setenv("PHP_SELF", path.c_str(), 0);
    // setenv("REQUEST_TIME_FLOAT", "", 0);
    // setenv("REQUEST_TIME", "", 0);
    // setenv("FCGI_ROLE", "RESPONDER", 0);


    // std::cerr << "------------------------------------" << std::endl;
    // //std::cerr << "Query :: "<< req.getContentBody().data() << std::endl;
    // std::cerr << "------------------------------------" << std::endl;


    // std::cerr << std::to_string(req.getContentBody().size()).c_str() << std::endl;
    
    return exec_cgi( req, args, path);
}
