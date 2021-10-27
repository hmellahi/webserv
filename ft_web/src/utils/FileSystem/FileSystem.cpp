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
        // std::cout << "[" << path << "]" << std::endl;
        // std::cout << "path" << path + indexFiles[i] << std::endl;
    
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
        return (IS_DIRECTORY); // todo change 

    int fd = open(filename.c_str(), O_RDONLY);
    if (fd < 1)
        return (HttpStatus::Forbidden);
    //     {
    //         status = HttpStatus::Forbidden;
    //         return ("");
    //     }
    // F_OK
    // Tests whether the file exists.
    // R_OK
    // Tests whether the file can be accessed for reading.
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