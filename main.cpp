#include <iostream>
#include <string>
#include "command.h"
#include "signal_handler.h"

using namespace std;

int main() {

    string line; // store input in line.
    handle_signal(); // handle the signal from command running.

    for (line; cout << "shell379 >" && getline(cin, line); ) {

        if (!line.empty()) {
            run_command(line);
        }
    }

    return 0;
}


