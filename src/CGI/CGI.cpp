#include "CGI.hpp"

std::string     CGI::exec_file(std::string path)
{
    int filedes[2];
	char **a = (char **)malloc(sizeof(char *) * 6);

	a[0] = strdup("./cgi-bin/php-cgi");
	a[1] = strdup("-q");
	a[2] = strdup(path.c_str());
	a[3] = strdup("2>");
    a[4] = strdup("/dev/null");
    a[5] = (char *)0;
	std::string f = "";

    if (pipe(filedes) == -1) {
        perror("pipe");
        exit(1);
    }

    pid_t pid = fork();
        
    if (pid == -1) {
        perror("fork");
        exit(1);
    }
    if (pid > 0) {
        close(filedes[1]);
        FILE *result;

        result = fdopen(filedes[0], "r");
        char c;
        while((c = fgetc(result)) != EOF)
            f += c;
        close(filedes[0]);
        fclose(result);

    }
    else if (pid == 0)
    {
        dup2(filedes[1], 1);
        close(filedes[1]);
        close(filedes[0]);

        execve(a[0], a, NULL);
        perror("execve");
        exit(1);
    }
    int i = -1;
    while (a[++i] != NULL)
        free(a[i]);
    free(a);
    return f;
}