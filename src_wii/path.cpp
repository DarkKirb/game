#include <paths.hpp>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
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

std::string get_executable_fname() {
    return "";
}

std::string get_executable_dir() {
    return "";
}

std::string get_ro_data_dir() {
    return "/testgame";
}

std::string get_rw_data_dir() {
    _mkdir("/testgame/data");
    return "/testgame/data";
}
