#include "CGI.hpp"

extern char** environ;

char        **fill_args(std::string path) {

	char **args = (char **)malloc(sizeof(char *) * 3);

	args[0] = strdup("./cgi-bin/php-cgi");
	args[1] = strdup(path.c_str());
	args[2] = (char *)0;
    // args[3] = strdup("/dev/null");
    // args[4] = 

    return (args);
}


std::string             parseOutput( std::string out ) {

    // out.erase(0, out.find("\n") + 1);
    // out.erase(0, out.find("\n") + 1);
   

    // while (out.find("\n") != std::string::npos) {
    
    // }
    // std::string to;
    std::vector<std::string> headers;
    std::stringstream ss(out);
    std::string line;
    std::string outContent;

    outContent = "";
    line = "";
    while (ss.good())
    {

        getline(ss, line, '\n');
        line = util::trim(line);
        if (line.find("<html>") == std::string::npos && line.length() > 0)
        {
            std::cout << "Found this header " << line << std::endl;
            headers.push_back(line);
        }
        else
            break ;
    }
    while (ss.good())
    {
        getline(ss, line, '\n');
        outContent += line;
    }
    return outContent;
    // if (out != NULL)
    // {
    //     while(std::getline(ss,to,'\n')){
    //     cout << to <<endl;
    //     }
    // }
   // return out;
}

std::string            exec_cgi( Request req, char **args , std::string path ) {

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

        if (write(fd[1], req.getContentBody().c_str(), req.getContentBody().length()) == -1)
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
    std::cout << "Status is " << status << std::endl;
    return parseOutput(cgiOutput);
    // return cgiOutput;
}

std::string    CGI::exec_file(std::string path, Request &req) {

    int fd[2];
    char    **args = fill_args(path);

    char    **envp;

    if (req.getMethod() == "POST")
    {
        if (!req.getHeader("Content-Type").empty())
            setenv("CONTENT_TYPE", req.getHeader("Content-Type").c_str(), 1);

    }
    if (!req.getContentBody().empty())
        setenv("CONTENT_LENGTH", std::to_string(req.getContentBody().length()).c_str(), 1);
    if (!req.getMethod().empty())
        setenv("REQUEST_METHOD", req.getMethod().c_str(), 1);
    setenv("REDIRECT_STATUS", "true", 1);
    setenv("SCRIPT_FILENAME", path.c_str(), 1);
    setenv("GATEWAY_INTERFACE", "CGI/1.1", 1);
    setenv("QUERY_STRING", req.getQuery().c_str(), 1);

    // PATH_INFO
    // PATH_TRANSLATED
    // REMOTE_ADDR
    // REMOTE_HOST
    // REMOTE_IDENT
    // REMOTE_USER

    // SCRIPT_NAME
    //setenv("SERVER_NAME", "localhost", 1)
    // SERVER_PORT
    // SERVER_PROTOCOL
    // SERVER_SOFTWARE
    setenv("SERVER_SOFTWARE", "Webserv/1.0", 1);

    //QUERY_STRING
    //if (!req.getQuery().empty())
    // req.getQuery();
    //     AUTH_TYPE
    // HTTP_*
    // QUERY_STRING


    std::cout << "------------------------------------" << std::endl;
    std::cout << "Query :: "<< req.getContentBody() << std::endl;
    std::cout << "------------------------------------" << std::endl;


    std::cout << std::to_string(req.getContentBody().length()).c_str() << std::endl;
    
    return exec_cgi( req, args, path);
}