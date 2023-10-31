//
// Created by Mingbo on 2023-10-10.
//
#include "command.h"

void print_resource_usege() {

    struct rusage usage{};

    getrusage(RUSAGE_CHILDREN, &usage);

    printf("User time = %7d seconds\n", usage.ru_utime.tv_sec);
    printf("Sys time = %7d seconds\n", usage.ru_stime.tv_sec);
    //return;
}

/**
 * TEST!
 */
int get_resource_usage_for_pid(pid_t pid) { // uses pipe to get the information from ps.
    int process_time;
    char buffer[100];
    FILE *fp;

    sprintf(buffer, "ps -p %d -o times =", pid); // write the output into buffer.
    fp = popen(buffer, "r");
    fscanf(fp, "%d", &process_time); // ptr to a file object, format, place to store.
    fclose(fp);

    return process_time;
}

/**
 * TEST!
 */
void exit_command() {

    for (auto &it: *get_process_map()) {

        pid_t pid = it.second.pid;
        kill(pid, SIGCONT);
        kill(pid, SIGTERM);
        waitpid(pid, nullptr, 0);
        kill(pid, SIGKILL);
    }
    get_process_map()->clear();

    printf("Resources used:\n");
    print_resource_usege();
    fflush(stdout);
    _exit(EXIT_SUCCESS);
}

/**
 * TEST!
 */
void sleep_command(const vector<char *> &tokens) {
    if (tokens.size() < 2) {
        perror("missing argument");
        return;
    }
    sleep(atoi(tokens[1]));
}

/**
 * TEST!
 */
void suspend_command(const vector<char *> &tokens) {
    if (tokens_contain_pid(tokens)) {
        pid_t pid = get_pid_from_tokens(tokens);
        Process *process = get_process(pid);

        if (process != nullptr) {
            kill(pid, SIGSTOP);
            process->status = Process::Status::paused; // update.
        }
        else {
            perror ("process not found");
        }
    }
    else {
        perror("missing argument");
    }
}
/**
 * TEST!
 */
void resume_command(const vector<char *> &tokens) {

    if (tokens_contain_pid(tokens)) {
        pid_t pid = get_pid_from_tokens(tokens);
        Process *process = get_process(pid);
        if (process != nullptr) {
            kill(pid, SIGCONT);
            process->status = Process::Status::running;
        }
        else {
            perror("process not found");
        }
    }
    else {
        perror("missing argument");
    }
}

/**
 * TEST!
 */
void wait_command(const vector<char *> &tokens) {
    if (tokens_contain_pid(tokens)) {
        pid_t pid = get_pid_from_tokens(tokens);
        Process *process = get_process(pid);
        if (process != nullptr) {
            if (waitpid(pid, nullptr, 0) > 0) {
                get_process_map()->erase(pid);
            }
        }
        else {
            perror("process not found");
        }
    }
    else {
        perror("missing argument");
    }
}

void kill_command(const vector<char *> &tokens) {

    if (tokens_contain_pid(tokens)) {
        pid_t pid = get_pid_from_tokens(tokens);
        Process *process = get_process(pid);

        if (process != nullptr) {
            kill(pid, SIGCONT);
            kill(pid, SIGTERM);
            waitpid(pid, nullptr, 0);
            kill(pid, SIGKILL); // kill the command.
            get_process_map()->erase(pid);
        }
        else {
            perror("process not found");
        }
    }
    perror("missing argument");
}

/**
 * TEST!
 */
void jobs_command() {

    printf("\nRunning processes:\n");

    if (!get_process_map()->empty()) {
        printf("#       PID S SEC COMMAND\n");
        int position;
        for (auto &it: *get_process_map()) {
            Process process = it.second;
            int process_time = get_resource_usage_for_pid(process.pid);
            printf(
                    "%2d: %7d %c %3d %s\n",
                    position, process.pid, process.get_status_code(), process_time, process.command.c_str()
                    );
            position++;
        }
    }
    printf("Processes = &7d active\n", (int) get_process_map()->size());
    printf("Completed processes:");
    print_resource_usege();
}

bool check_tokens(const vector<char *> &tokens) {

    if (tokens.size() > MAX_ARGS) {
        perror( "too manny arguments");
        return false;
    }
    for (int i = 0; i < tokens.size(); i++){
        char *token = tokens[i];

        if (strlen(token) > MAX_LENGTH) {
            perror("too long");
            return false;
        }
    }
    return true;
}

void push_input_to_args(vector<char *> &args, char *file_name) {

    ifstream input((string) file_name);

    string line;

    while(getline(input, line)) {
        vector<string> split_input = split(line);
        vector<char *> input_tokens = string_vector_to_char_vector(split_input);

        for (char *token : input_tokens) {
            args.push_back(token);
        }
    }
}

vector<char *> get_args(const vector<char *> &tokens) {
    vector <char *> args = vector<char *>();

    if (tokens.size() > 1) {
        for (int i = 0; i < tokens.size(); i++) {
            char *token = token[i];

            if ((i != tokens.size() - 1) || (strcmp("&", token) != 0)) {
                if (token[0] == '<') {
                    push_input_to_args(args, trim_first_character(token));
                }
                else if (token[0] != '>') {
                    args.push_back(token);
                }
            }
        }
    }
    return args;
}

char *get_output_file_name(const vector<char *> &tokens) {
    for (char *token :tokens) {
        if (token[0] == '>') {
            return trim_first_character(token);
        }
    }
    return nullptr;
}

bool check_if_background(vector <char *> tokens) {
    if (tokens.size() > 1) {
        char *token = tokens[tokens.size() - 1];
        if (token == "&" ) {
            return true;
        }
    }
    return false;
}

void run_tokens(vector<char *> &tokens, string line) {
    if (!tokens.empty()) {
        pid_t pid = get_pid_from_tokens(tokens);
        string command = tokens[0];
        vector <char *> args = get_args(tokens);
        char *output_file_name = get_output_file_name(tokens);
        bool is_background = check_if_background(tokens);

        if (command == "exit") {
            exit_command();
        }
        else if (command == "jobs") {
            jobs_command();
        }
        else if (command == "kill") {
            kill_command(tokens);
        }
        else if (command == "resume") {
            resume_command(tokens);
        }
        else if (command == "sleep") {
            sleep_command(tokens);
        }
        else if (command == "suspend") {
            suspend_command(tokens);
        }
        else if (command == "wait") {
            wait_command(tokens);
        }
        else if (!command.empty()) {
            new_process(command, args, output_file_name, is_background, line);
        }
    }
}

void run_command(string line) {
    if (line.length() > MAX_LENGTH) {
        perror("too long");
        return;
    }
    line = rtrim(line);

    vector<string> split_input = split(line);
    vector<char *> tokens = string_vector_to_char_vector(split_input);

    if (check_tokens(tokens)) {
        run_tokens(tokens, line);
    }
}






















/*
#include "command.h"

void print_resource_usege() {

    struct rusage usage{};
    getrusage(RUSAGE_CHILDREN, &usage); // get the usage of child and stores it into usage.

    printf("User time = %7d seconds\n", usage.ru_utime.tv_sec);
    printf("Sys time = %7d seconds\n\n", usage.ru_stime.tv_sec);

}

int get_resource_usage_for_pid(pid_t pid) {

    int process_time;
    char buffer[100];
    FILE *fp;

    sprintf(buffer, "ps -p %d -o times", pid); // create command to run ps command and filter output.
    // -p %d -o times specifies the accumulated CPU time.

    fp = popen(buffer, "r"); // setup the pipe to read from buffer.

    fscanf(fp, "%d", &process_time); // read the process time from fp and stores it into process_time.

    pclose(fp);

    return process_time;

}

// all commands

void jobs_command() {

    printf("\nRunning Processes:\n");

    if (!get_process_map()->empty()) { // check if list is empty.

        printf("#        PID S SEC COMMAND\n");

        int position = 0;

        for (auto &it: *get_process_map()) {

            Process process = it.second;
            int process_time = get_resource_usage_for_pid(process.pid); // get the process time.

            printf(
                    "%2d: %7d %c %3d %s\n",
                    position, process.get_status_code(), process_time, process.command.c_str()
                    ); // process.get_status_code since it is a function of class process.

            position++;
        }
    }
    printf("Processes = %7d active\n", (int) get_process_map()->size()); // size of the map is the # of processes active.
    printf("Completed process:\n");

    print_resource_usege();
}

void exit_command() {

    for (auto &it: *get_process_map()) {

        pid_t pid = it.second.pid;
        kill(pid, SIGCONT);
        kill(pid, SIGTERM);
        waitpid(pid, nullptr, 0);
        kill(pid, SIGKILL);
    }

    get_process_map()->clear();

    printf("Resources used:\n");
    print_resource_usege();

    fflush(stdout);
    _exit(EXIT_SUCCESS);

}

void kill_command(const vector <char *> &tokens) {

    if (tokens_contain_pid(tokens)) {
        // get pid and process.
        pid_t pid = get_pid_from_tokens(tokens);
        Process *process = get_process(pid);

        if (process != nullptr) {
            kill(pid, SIGCONT);
            kill(pid, SIGTERM);
            waitpid(pid, nullptr, 0);
            kill(pid, SIGKILL);

            get_process_map()->erase(pid); // remove it from the map.
        }
        else {
            printf("couldn't find process with pid");
        }
    }
    else {
        printf("input pid missing");
    }
}

void resume_command(const vector<char *> &tokens) {

    if (tokens_contain_pid(tokens)) {
        // get pid and process.
        pid_t pid = get_pid_from_tokens(tokens);
        Process *process = get_process(pid); // process needs to be pointer since next step.

        if (process != nullptr) {

            kill(pid, SIGCONT); // resume.
            process->status = Process::Status::running; // update status.
        }
        else {
            printf("process not found with pid");
        }

    }
    else {
        printf("input pid missing");
    }
}

void suspend_command(const vector<char *> &tokens) {

    if (tokens.size() > 2) { // if tokens contains pid.
        pid_t pid = get_pid_from_tokens(tokens);
        Process *process = get_process(pid);

        if (process != nullptr) {
            kill(pid, SIGSTOP);
            process->status = Process::Status::paused; // update the status.
        }
        else {
            printf("process not found.");
        }
    }
    else {
        printf("input pid missing");
    }
}

void wait_command(const vector<char *> &tokens) {

    if (tokens.size() > 2) {
        pid_t pid = get_pid_from_tokens(tokens);
        Process *process = get_process(pid);

        if (process != nullptr) {
            if (waitpid(pid, nullptr, 0) > 0) { // waitpid returns value > 0 it process with 'pid' exited successfully.
                // the if statement will wait until the process exit.
                get_process_map()->erase(pid); // once the process exited, remove it.
            }
        }
        else {
            printf("process not found");
        }
    }
    else {
        printf("input pid missing");
    }
}

void sleep_command(const vector<char *> &tokens) {

    if (tokens.size() < 2) {
        printf("time missing for sleep");
        return;
    }
    else {
        sleep(atoi(tokens[1]));
    }
}

bool check_tokens(const vector<char *> &tokens) {

    if (tokens.size() > MAX_ARGS) { // check for arguments.
        return false;
    }

    for (int i = 0; i < tokens.size(); i++) {

        char *token = tokens[i];
        if (strlen(token) > MAX_LENGTH) { // check for lengths.
            return false;
        }
    }
    return true;
}

void push_input_to_args(vector<char *> &args, char *file_name) {

    ifstream input((string) file_name); // open the file.

    string line;
    while (getline(input, line)) {

        vector <string> split_input = split(line);
        vector<char *> input_tokens = string_vector_to_char_vector(split_input); // convert string into char

        for (char *input_token : input_tokens) {  // loop through input_tokens and pushes into input_token.
            args.push_back(input_token);
        }
    }
}

vector<char *> get_args(const vector<char *> &tokens) {

    vector<char *> args = vector<char *>(); // setup a vector to store the extracted arguments.

    if (tokens.size() > 1) {
        for (int i = 0; i < tokens.size(); i++) {

            char *token = tokens[i];
            if (i != tokens.size() - 1 || (strcmp(token, "&") != 0)) { // check for &.

                if (token[0] == '<') { // check for input
                    push_input_to_args(args, trim_first_character(token)); // get rid of <.
                }
                else if (token[0] == '>') {
                    args.push_back(token); // do not append tokens for file output to args.
                }
            }
        }
    }
    return args;
}

char *get_output_file_name(const vector<char *> &tokens) {
    for (char *token : tokens) {
        if (token[0] == '>') {
            return trim_first_character(token); // remove > from command,
        }
    }
    return nullptr;
}

bool check_is_background(vector<char *> tokens) {
    if (tokens.size() > 1) {
        char *last_token = tokens[tokens.size() - 1];
        return strcmp(last_token, "&") == 0; // it will return true if the last token is &.
    }
    return false;
}

void run_tokens(vector< char *> tokens, string line) {

    if (!tokens.empty()) {
        string command = tokens[0]; // get command;
        vector<char *> args = get_args(tokens); // get args.
        bool is_background = check_is_background(tokens); // get &.
        char *output_file_name = get_output_file_name(tokens); // get filename.

        if (command == "exit") {
            exit_command();
        }
        else if (command == "jobs") {
            jobs_command();
        }
        else if (command == "kill") {
            kill_command(tokens);
        }
        else if (command == "resume") {
            resume_command(tokens);
        }
        else if (command == "sleep") {
            sleep_command(tokens);
        }
        else if (command == "suspend") {
            suspend_command(tokens);
        }
        else if (command == "wait") {
            wait_command(tokens);
        }
        else if (!command.empty()) {
            new_process(command, args, output_file_name, is_background, line);
        }
    }
}

void run_command(string line) {

    if (line.length() > MAX_LENGTH) {
        printf("Maximum line length reached\n");
        return;
    }
    line = rtrim(line);

    vector<string> split_input = split(line);
    vector <char *> tokens = string_vector_to_char_vector(split_input);

    if (check_tokens(tokens)) {
        run_tokens(tokens, line);
    }
}
*/




