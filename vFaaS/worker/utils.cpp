#include "utils.hpp"

namespace util{

void readFileToBytes(const std::string& path, std::vector<uint8_t>& codeBytes){
    int fd = open(path.c_str(), O_RDONLY);
    if (fd < 0) throw std::runtime_error("Couldn't open file " + path);
    struct stat statbuf;
    int staterr = fstat(fd, &statbuf);
    if (staterr < 0) throw std::runtime_error("Couldn't stat file " + path);
    size_t fsize = statbuf.st_size;
    posix_fadvise(fd, 0, 0, POSIX_FADV_SEQUENTIAL);
    codeBytes.resize(fsize);
    int rc = read(fd, codeBytes.data(), fsize);
    if (rc < 0 || rc != fsize) {
        perror("Couldn't read file");
        throw std::runtime_error("Couldn't read file " + path);
    }else std::cout << "Read " << rc << " Bytes." << std::endl;
    close(fd);
    return;
}

bool startWorker(int *pid, int *infd, int *outfd){
    char* command[] = {"./execfunc", NULL};
    int p1[2], p2[2];
    if (!pid || !infd || !outfd) return false;
    if (pipe(p1) == -1) goto err_pipe1;
    if (pipe(p2) == -1) goto err_pipe2;
    if ((*pid = fork()) == -1) goto err_fork;
    if (*pid){
        /* Parent process. */
        *infd = p1[1];  // writeFd
        *outfd = p2[0]; // readFd
        close(p1[0]);
        close(p2[1]);
        return true;
    } else {
        /* Child process. */
        dup2(p1[0], 0); // redirect stdin
        dup2(p2[1], 1); // redirect stdout
        close(p1[0]);
        close(p1[1]);
        close(p2[0]);
        close(p2[1]);
        execv(command[0], command);
        /* Error occured. */
        fprintf(stderr, "error running %s: %s", command[0], strerror(errno));
        abort();
    }
err_fork:
    close(p2[1]);
    close(p2[0]);
err_pipe2:
    close(p1[1]);
    close(p1[0]);
err_pipe1:
    perror("[Host] Couldn't fork new process.");
    return false;
}

void readBytes(int fd, unsigned char* buffer, int bufferLength){
    int cpos = 0;
    while (cpos < bufferLength) {
        int rc = read(fd, buffer + cpos, bufferLength - cpos);
        if (rc < 0) {
            perror("[Host Worker Read] Couldn't Read from worker.");
            throw "[Host Worker Read] Couldn't Read from worker.";
        } else {
            cpos += rc;
        }
    }
}
}