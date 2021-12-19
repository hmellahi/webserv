#include "FileSystem.hpp"
#include "Config.hpp"

std::string     FileSystem::readFile(std::string filename, int &status)
{
    if (getFileStatus(filename) != HttpStatus::OK)
        throw std::runtime_error("invalid file");
    std::ifstream  file(filename.c_str());
    int fd = open(filename.c_str(), O_RDONLY);
    char buffer[20000] = {0};
    int ret = read(fd, buffer, 3000);
    // todo check ret
    close(fd);
    status = HttpStatus::OK;
    return (buffer);
}

std::string  FileSystem::getIndexFile(const std::string & path, const std::vector<std::string> &indexFiles)
{
    std::string filename;
    int status = !HttpStatus::OK;

    for (int i = 0; i < indexFiles.size() && status != HttpStatus::OK; i++)
    {
        // std::cerr << "[" << path << "]" << std::endl;
        // std::cerr << "path" << path + indexFiles[i] << std::endl;
    
        status = FileSystem::getFileStatus(path + indexFiles[i]);
        if (status == HttpStatus::OK)
            filename = indexFiles[i];
    }
    return filename;
}

// int     FileSystem::openFile(std::string filename, int &status, struct stat *state)
// {
//     struct stat info;
//     if ((stat(filename.c_str(), &info)) == -1)
//     {
//         status = HttpStatus::NotFound;
//         return ("");
//     }
//     off_t fileLength = info.st_size;
//     if ((int)((info.st_mode & S_IFDIR) == S_IFDIR))
//     {
//         status = IS_DIRECTORY;
//         return (-1);
//     }
//     int fd = open(filename.c_str(), O_RDONLY);
//     if (fd < 1)
//     {
//         status = HttpStatus::Forbidden;
//         return ("");
//     }
// }

int     FileSystem::getFileStatus(std::string filename)
{
    struct stat info;
    
    if ((stat(filename.c_str(), &info)) == -1)
        return (HttpStatus::NotFound);
    
    if (info.st_mode & S_IFDIR)
        return (IS_DIRECTORY);

    int fd = open(filename.c_str(), O_RDONLY);
    if (fd < 1)
        return (HttpStatus::Forbidden);
    //     {
    //         status = HttpStatus::Forbidden;
    //         return ("");
    //     }
    // F_OK
    // Tests whether the file exists.
    // Tests whether the file can be accessed for reading.
    // R_OK
    // X_OK — for a directory, test for search permission. Otherwise, test for execute permission. 
    // const char file[] = filename.c_;   
    // if (access(filename.c_str(), F_OK) == -1)
    //     return (HttpStatus::NotFound);
    // if (access(filename.c_str(), X_OK) == -1)
    //     return (IS_DIRECTORY);
    // if (access(filename, R_OK) == -1)
    //     return (HttpStatus::Forbidden);
    
    return HttpStatus::OK;
}

void    FileSystem::uploadChunkedFile(std::string uploadLocation, Request req)
{
    int i  = req._buffer.find("\r\n\r\n");
	req._buffSize -= i;
    std::map<std::string, std::string> headers;
    headers = req.getHeaders();
	for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); ++it) {
	
		std::cout << it->first << "|" << it->second << std::endl;
	}

    int bufferSize = req.getBufferSize();
    std::cout << "size " << bufferSize << std::endl;
	int count = 0;
    std::fstream new_file;
    new_file.open(uploadLocation.c_str(), std::ofstream::out | std::ofstream::trunc);
    if (!new_file.is_open())
        throw std::runtime_error("couldnt open the file for writing");
	if (i != bufferSize)
	{
		if (headers.find("Content-Length") != headers.end())
		{
			i += 4;
			while (i != bufferSize && count < atoi(headers["Content-Length"].c_str()))
			{
                std::cout << req._buffer[i];
                new_file.write(&req._buffer[i], 1);
				i++;
				count++;
			}

		}
	}
    std::cout << std::endl;
    new_file.close();
}

void    FileSystem::uploadFile(std::string uploadLocation, std::vector<char> content)
{
    std::fstream new_file;

    // std::cerr << "location: " << uploadLocation << std::endl;
    new_file.open(uploadLocation.c_str(), std::ofstream::out | std::ofstream::trunc);
    if (!new_file.is_open())
        throw std::runtime_error("couldnt open the file for writing");
    // todo check
    new_file.write(content.data(), content.size());
    new_file.close();
}

bool    FileSystem::isReadyFD(int fd, int mode)
{
    if (mode == READ)
    {
        fd_set readfds;
        FD_SET(fd, &readfds);
        select(fd+1, &readfds, NULL,NULL,0);
        if (!Socket::testConnection(fd))
            return false;
        std::cout << "is ready " << FD_ISSET(fd, &readfds) << std::endl;
        return (FD_ISSET(fd, &readfds));
    }
    else
    {
        fd_set writefds;
        FD_SET(fd, &writefds);
        select(fd+1, NULL, &writefds,NULL,0);
         if (!Socket::testConnection(fd))
            return false;
        return (FD_ISSET(fd, &writefds));
    }
}