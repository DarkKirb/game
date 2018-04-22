#include <paths.hpp>
#include <os.hpp>
#include <init.hpp>
#include <stdlib.h>
#include <sstream>
#include <trim.hpp>
#include <errno.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>

#ifdef OS_WIN
#include <windows.h>
#else
#include <unistd.h>
#define HAS_PROC
#ifdef OS_LINUX
#define PROCPATH "/proc/self/exe"
#elif defined(OS_FREEBSD)
#define PROCPATH "/proc/curproc/file"
#elif defined(OS_SOLARIS)
#define PROCPATH "/proc/self/path/a.out"
#else
#undef HAS_PROC
#endif
#endif

std::string get_executable_fname() {
#ifdef OS_WIN
    char buf[1024];
    GetModuleFileName(nullptr, buf, 1023);
    return buf;
#else
#ifdef HAS_PROC
    char buf[1024];
    if(readlink(PROCPATH, buf, 1023))
        return buf;
#endif
    //Get executable path from argv
    auto args = getArgs();
    auto exec = args[0];
    if(exec[0] == '/')
        return exec;
    if(exec.find("/") != std::string::npos) {
        auto rp = realpath(exec.c_str(), nullptr);
        std::string path = rp;
        free(rp);
        return path;
    }
    std::istringstream f(getenv("PATH"));
    std::vector<std::string> entries;
    std::string s;
    while(std::getline(f, s, ':')) {
        entries.push_back(s);
    }
    for(auto &path : entries) {
        auto dir = path + '/' + exec;
        if(exists(dir))
            return dir;
    }
    throw "Could not find the executable's directory!";
#endif
}

std::string get_executable_dir() {
    return rtrim(get_executable_fname(), PATH_SEP);
}

std::string get_ro_data_dir() {
    auto exec_dir = get_executable_dir();
    std::vector<std::string> paths = {
        exec_dir + PATH_SEP + "assets",
        exec_dir + PATH_SEP + ".." + PATH_SEP + "assets",
        "/usr/share/testgame",
        "/usr/local/share/testgame",
        std::string(getenv("HOME")) + "/.local/share/testgame"
    };
    for(auto &dir : paths) {
        if(is_dir(dir))
            return dir;
    }
    throw "Data folder not found";
}

static void _mkdir(const char *dir);
std::string get_rw_data_dir() {
#ifdef OS_WINDOWS
    std::string dir = getenv("APPDATA");
    dir += "\\testgame";
#else
    std::string dir = getenv("HOME");
    dir += ".config/testgame";
#endif
    _mkdir(dir.c_str());
    return dir;
}


bool exists(std::string &path) {
    return access(path.c_str(), F_OK) != -1;
}

bool is_dir(std::string &path) {
    if(!exists(path))
        return false;
    auto *dir = opendir(path.c_str());
    if(dir) {
        closedir(dir);
        return true;
    } else if (errno == ENOTDIR) {
        return false;
    }
    return true;
}
static void _mkdir(const char *dir) {
        char tmp[1024];
        char *p = NULL;
        size_t len;

        snprintf(tmp, sizeof(tmp),"%s",dir);
        len = strlen(tmp);
        if(tmp[len - 1] == '/')
                tmp[len - 1] = 0;
        for(p = tmp + 1; *p; p++)
                if(*p == '/') {
                        *p = 0;
                        mkdir(tmp, S_IRWXU);
                        *p = '/';
                }
        mkdir(tmp, S_IRWXU);
}
