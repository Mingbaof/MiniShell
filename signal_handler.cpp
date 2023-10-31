//
// Created by Mingbo on 2023-10-07.
//

#include "signal_handler.h"

void handle_signal() {

    signal (SIGINT, [](int signum) { // signal for Ctrl+C
        pid_t pid = waitpid(-1, nullptr, WNOHANG); // waitpid returns a number > 0 if it process terminates.

        if (pid > 0) {
            get_process_map()->erase(pid);
        }
        puts(""); // print extract line in standard output stream.
    });

    signal(SIGTSTP, SIG_IGN); // ignore Ctrl+Z.

    signal(SIGCHLD, [](int signum) { // signal when a child process's status changes.
        pid_t pid = waitpid(-1, nullptr, WNOHANG); // WNOHANG to make it non-blocking for multiple child process case.
        get_process_map()->erase(pid);

    });

}
