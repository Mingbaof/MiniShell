//
// Created by Mingbo on 2023-10-09.
//

#include "process.h"

Process::Process(pid_t pid, string command, Status status) {
    this->pid = pid;
    this->command = command;
    this->status = status;
}

char Process::get_status_code() { // since it is a member of the Process class.

    if (status == Status::running) {
        return 'R';
    }
    else if (status == Status::paused) {
        return 'P';
    }
    else {
        return ' ';
    }
}

