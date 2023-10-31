//
// Created by Mingbo on 2023-10-07.
//

#include "process_handler.h"

// inline to make it defined in a header file and can be included in multiple files.
inline map<pid_t, Process> process_map = map<pid_t, Process>(); // initialize process map for process.

map<pid_t, Process> *get_process_map() {
    return &process_map;
}

Process *get_process(pid_t pid) {

    if (process_map.find(pid) != process_map.end()) { // process_map.find returns the pointer to the pid,
        // or it will return process_map.end() if not found.
        return &process_map.at(pid);
    }

    return nullptr;
}

void new_process(string command, vector<char *> args, char *output_file_name, bool is_background, string line) {

    if (process_map.size() >= MAX_PT_ENTRIES) {
        printf("Maximum Process count exceeded\n");
        return;
    }

    pid_t pid = fork();

    if (pid < 0) {
        printf("Fork failed\n");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) { // child process

        char *argv[args.size() + 1]; // hold the command-line arguments to be executed.
        for (int i = 0; i < args.size(); i++) { // copy.
            argv[i] = args[i];
        }
        argv[args.size()] = nullptr;

        if (output_file_name != nullptr) {
            freopen(output_file_name, "w", stdout); // redirect stuffs in stdout into output_file_name.
        }

        if (execvp(argv[0], argv) < 0) { // argv[0] is the command we want to execute.
            // execvp returns -1 if it fails.
            printf("execvp error\n");
            _exit(EXIT_FAILURE);
        }

    }

    else { // parent process

        Process process = Process(pid, line, Process::Status::running); // setup a new process.
        process_map.emplace(pid, process); // adds it to the process_map by emplace().

        if (!is_background) { // if it does not runs in background

            waitpid(pid, nullptr, 0); // wait for the child.
            process_map.erase(pid);

        }
    }
}
