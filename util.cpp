//
// Created by Mingbo on 2023-10-10.
//

#include "util.h"

string rtrim(const string &str) {
    // find the last whitespace and trims it.
    // return empty string if no trailing whitespace characters.
    size_t end = str.find_last_not_of(" \n\r\t\f\v");
    return (end == string::npos) ? "" : str.substr(0, end + 1);
}

vector<string> split(const string &str, char delim) {
    // spilts the input string into parts
    // returns the vector containing the spilt substrings.
    vector<string> result;
    string temp;

    for (char i : str) {
        if (i == delim) {
            result.push_back(temp);
            temp = "";
        } else {
            temp += i;
        }
    }

    if (!temp.empty()) {
        result.push_back(temp);
    }

    return result;
}

vector<char *> string_vector_to_char_vector(vector<string> original) {
    // converts each string in the string vector to array char* and stores it to return.
    vector<char *> conversion = vector<char *>();

    for (const auto &str : original) {
        char *charStr = new char[str.size() + 1];
        strcpy(charStr, str.c_str());
        conversion.push_back(charStr);
    }

    return conversion;
}


bool tokens_contain_pid(const vector<char *>& tokens) {
    // size < 2 means no pid in the token.
    if (tokens.size() < 2) {
        return false;
    }

    return true;
}

pid_t get_pid_from_tokens(vector<char *> tokens) {
    char *pid_token = tokens[1];
    pid_t pid = atoi(pid_token);

    return pid;
}

char *trim_first_character(char *str) {
    // removes the first character.
    return str + 1;
}
