#include "CGI.hpp"

extern char** environ;

char        **fill_args(std::string path) {

	char **args = (char **)malloc(sizeof(char *) * 3);

	// args[0] = strdup("./cgi-bin/php-cgi");
	args[0] = strdup("./cgi-bin/ubuntu/php-cgi");
	// args[1] = strdup(path.c_str());
	args[1] = (char *)0;
    // args[3] = strdup("/dev/null");
    // args[4] = 

    return (args);
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
        // unlink("test.txt");
    }
    else if (pid == 0)
    {

        if (write(fd[1], &req.getContentBody()[0], req.getContentBody().size()) == -1)
            throw std::runtime_error("wrtie dzb");

    // dup2(tmp, 0);
        // printf("%s | %d\n", req.getContentBody().c_str(), req.getContentBody().length());
        dup2(nfd[1], 1);
        dup2(fd[0], 0);
        close(fd[1]);
        close(fd[0]);
        close(nfd[1]);

        if (execve(args[0], args, environ) == - 1)
            throw std::runtime_error("execve error");
    }
    // close(tmp)
    int i = -1;
    while (args[++i] != NULL)
        free(args[i]);
    free(args);
    return cgiOutput;
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
        setenv("CONTENT_LENGTH", std::to_string(req.getContentBody().size()).c_str(), 1);
    if (!req.getMethod().empty())
        setenv("REQUEST_METHOD", req.getMethod().c_str(), 1);
    setenv("REDIRECT_STATUS", "true", 1);
    setenv("SCRIPT_FILENAME", path.c_str(), 1);
    setenv("GATEWAY_INTERFACE", "CGI/1.1", 1);

    if (!req.getQuery().empty())
        setenv("QUERY_STRING", req.getQuery().c_str(), 1);
    // req.getQuery();
    //     AUTH_TYPE
    // CONTENT_LENGTH
    // CONTENT_TYPE
    // GATEWAY_INTERFACE
    // HTTP_*
    // PATH_INFO
    // PATH_TRANSLATED
    // QUERY_STRING
    // REMOTE_ADDR
    // REMOTE_HOST
    // REMOTE_IDENT
    // REMOTE_USER
    // REQUEST_METHOD
    // SCRIPT_NAME
    // SERVER_NAME
    // SERVER_PORT
    // SERVER_PROTOCOL
    // SERVER_SOFTWARE

    std::cout << "------------------------------------" << std::endl;
    // std::cout << "Query :: " << &req.getContentBody()[0] << std::endl;
    std::cout << "------------------------------------" << std::endl;


    std::cout << std::to_string(req.getContentBody().size()).c_str() << std::endl;

    
    return exec_cgi( req, args, path);
}