#include "check.hpp"
#include "type_packet.h"
#include "common.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <vector>
#include <sys/stat.h>
#include <iostream>
#include <string>
#include <dirent.h>
#include <nlohmann/json.hpp>
#include <libconfig.h>
#include <pwd.h>
#include <cstring>
using json = nlohmann::json;
using namespace std;

const char* SOCKET_PATH;
const char* working_directory;

int read_setting_file()
{
    config_t cfg;
    config_init(&cfg);

    if (!config_read_file(&cfg, "/usr/local/bin/config.cfg")) {
        fprintf(stderr, "%s:%d - %s\n",
                config_error_file(&cfg),
                config_error_line(&cfg),
                config_error_text(&cfg));
        config_destroy(&cfg);
        return EXIT_FAILURE;
    }
    if (config_lookup_string(&cfg, "SOCKET_PATH", &SOCKET_PATH) < 0) {
        printf("Error format setting file!\n");
        exit(-1);
    }
    if (config_lookup_string(&cfg, "working_directory", &working_directory)<0) {
        printf("Error format setting file!\n");
        exit(-1);
    }
    return 0;
}


void send_request(int& fd, request& pkg)
{
    check(send(fd, (void*)&pkg, sizeof(pkg), MSG_WAITALL));
}

void receive_back_result_analist(response& res, int& fd)
{
    check(recv(fd, (void*)&res, sizeof(response), MSG_WAITALL));
}