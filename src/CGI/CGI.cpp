#include "CGI.hpp"
extern char** environ;

# include "FileSystem/FileSystem.hpp"
char        **fill_args(std::string cgiPath, std::string path) {

	char **args = (char **)malloc(sizeof(char *) * 3);

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
    std::vector<std::string> cgiHeaders;
    cgiHeaders.push_back("Set-Cookie");
    cgiHeaders.push_back("Status");
    cgiHeaders.push_back("Content-type");
    cgiHeaders.push_back("charset");
    cgiHeaders.push_back("Location");
    cgiHeaders.push_back("X-Powered-By");

    outContent = "";
    line = "";
    while (ss.good())
    {
        getline(ss, line, '\n');
        if (find(cgiHeaders.begin(), cgiHeaders.end(), line.substr(0, line.find(":"))) == cgiHeaders.end())
            break ;
        // std::cerr << line << std::endl;
        line = util::trim(line);
        // std::cerr << "line :" << line << "|" << std::endl;
        // if (line.find("<html>") == std::string::npos && line.length() > 0)
            // headers.push_back(line);
        tab = util::split(line, ": ");
        if (tab[0] == "Set-Cookie") {

            headers[tab[0]] = util::trim(tab[1]);
            setenv("HTTP_COOKIE", headers[tab[0]].c_str(), 1);
        }
        else if (tab[0] == "Status") {
        
            setenv("REDIRECT_STATUS", tab[1].c_str(), 0);
            // std::cerr << "Status is " << tab[1].c_str() << std::endl;
            headers[tab[0]] = util::trim(tab[1]);
            // setenv("HTTP_COOKIE", headers[tab[0]].c_str(), 1);
        }
        else  {
            // std::cerr << "testing location " << std::endl;
            headers[tab[0]] = util::trim(tab[1]);
            
        }
        // else if (tab[0] != "X-Powered-By" && tab[0] != "")
        // std::cerr << "Found this header "<< "|" << tab[0] << "|" << headers[tab[0]] << "|" << std::endl;
    }
    while (ss.good())
    {
        
        getline(ss, line, '\n');
        std::cerr <<  "adding this line " << line << "|\n"; 
        outContent += line;
    }
    ret.first = outContent;
    ret.second = headers;
    return ret;

}


std::pair<std::string, std::map<std::string , std::string> > exec_cgi( Request req, char **args)
{

    int         fd[2];
    int         nfd[2];
    int         f_err[2];

    std::string cgiOutput = "";

    if (pipe(fd) == -1)
        throw std::runtime_error("pipe error");
    if (pipe(nfd) == -1)
        throw std::runtime_error("pipe error");
    if (pipe(f_err) == -1)
        throw std::runtime_error("pipe error");

    pid_t pid = fork();

    if (pid == -1)
        throw std::runtime_error("fork error");
    int status;

    if (pid > 0) {

        close(f_err[1]);
        close(fd[1]);
        close(fd[0]);
        close(nfd[1]);
        std::cout << "wsup status : "  << std::endl;
        std::cout << "no more waiting" << std::endl;
       
        FILE *result;
        std::cout << "Reading 2 : \n"; 
        char c;
        
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
        {
            if (!FileSystem::isReadyFD(f_err[0], READ))
                throw std::runtime_error("fd not ready read");
            std::cout << "sd \n"; 
            result = fdopen(f_err[0], "r");
            while((c = fgetc(result)) != EOF)
                cgiOutput += c;
            close(f_err[0]);
            fclose(result);
            std::cerr << cgiOutput;
            std::cout << "done \n"; 
        }

        if (!FileSystem::isReadyFD(nfd[0], READ))
            throw std::runtime_error("fd not ready read");
        result = fdopen(nfd[0], "r");
        // char c;
        while((c = fgetc(result)) != EOF)
            cgiOutput += c;
        fclose(result);
        close(nfd[0]);
        std::cout << "dd \n"; 
		// remove(req._fileLocation.c_str());
    }
    else if (pid == 0)
    {
       
        if (req.isChunked)
        {
            std::cerr << "file size" << util::getFileLength(req.fd) << std::endl;
            if (dup2(req.fd, 0) == -1)
                throw std::runtime_error("dup error");
        } 
        else
        {
            if (!FileSystem::isReadyFD(fd[1], WRITE))
                throw std::runtime_error("fd not ready write");
            if (req.getContentBody().size() > 0 && write(fd[1], req.getContentBody().data(), req.getContentBody().size()) <= 0)
                throw std::runtime_error("write error");
            if (dup2(fd[0], 0) == -1)
                throw std::runtime_error("dup error");
        }
        if (dup2(f_err[1], 2) == -1)
            throw std::runtime_error("dup error");
        if (dup2(nfd[1], STDOUT_FILENO) == -1)
            throw std::runtime_error("dup error");
        close(fd[1]); 
        close(fd[0]);
        // close(req.fd);
        // std::cout << "req" << req.fd << std::endl;
        close(nfd[1]);
        close(nfd[0]);
        close(f_err[1]);
        close(f_err[0]);
        if ((status = execve(args[0], args, environ)) != 0)
            throw std::runtime_error("execve error");
    }
    wait(NULL);

    int i = -1;
    while (args[++i] != NULL)
        free(args[i]);
    free(args);
    std::cout << "done" << std::endl;
    // remove cgi useless headers
    return parseOutput(cgiOutput);
}



std::pair<std::string, std::map<std::string , std::string> >  CGI::exec_file(std::string path, Request &req, std::string cgiPath) {

    std::pair<std::string, std::map<std::string , std::string> >  ret;
    
    char    **args = fill_args(cgiPath, path);

    // std::cerr << " wnk " << std::string(req.getContentBody().data(),req.getContentBody().data()+req.getContentBody().size())<< std::endl;
    if (req.getMethod() == "POST")
    {
        if (!req.getHeader("Content-Type").empty())
            setenv("CONTENT_TYPE", req.getHeader("Content-Type").c_str(), 1);
    }
    if (req.isChunked)
    {
        std::cerr << "a" << util::ft_itos(util::getFileLength(req.fd)).c_str()<< std::endl;
        setenv("CONTENT_LENGTH", util::ft_itos(util::getFileLength(req.fd)).c_str(), 1);
    }
    else {

        std::cout << "b" << req.getContentBody().size() << std::endl;
        setenv("CONTENT_LENGTH", util::ft_itos(req.getContentBody().size()).c_str(), 1);
    } 
    if (!req.getMethod().empty())
        setenv("REQUEST_METHOD", req.getMethod().c_str(), 1);
    setenv("REDIRECT_STATUS", "true", 1);

    setenv("SCRIPT_FILENAME", path.c_str(), 1);
    setenv("GATEWAY_INTERFACE", "CGI/1.1", 1);
    setenv("QUERY_STRING", req.getQuery().c_str(), 1);
    setenv("HTTP_ACCEPT_LANGUAGE", "en-US,en;q=0.9", 0);
    setenv("HTTP_ACCEPT_ENCODING", "", 0);
    setenv("HTTP_ACCEPT", "" , 0);
    if (!req.getHeader("cookie").empty())
        setenv("cookie", req.getHeader("cookie").c_str(), 1);
    //  tab = util::split(line, ": ");
    std::cout << req.getUrl()<<std::endl;   
    setenv("PATH_INFO", req.getUrl().c_str(), 0);
    setenv("SERVER_SOFTWARE", "server", 0);
    setenv("REQUEST_SCHEME", "http", 0);
    setenv("SERVER_PROTOCOL", "HTTP/1.1", 0);
    return exec_cgi( req, args);
}
