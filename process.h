//
// Created by Mingbo on 2023-10-09.
//

#ifndef A1_PROCESS_H
#define A1_PROCESS_H

#include <unistd.h>
#include <cstring>
#include <map>
#include <sys/resource.h>
#include <string>

using namespace std;

class Process {
public:
    enum class Status {running, paused};

    pid_t pid;
    string command;
    Status status;

    Process (pid_t pid, string command, Status status);

    char get_status_code();

};

#endif //A1_PROCESS_H
