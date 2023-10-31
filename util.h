//
// Created by Mingbo on 2023-10-10.
//

#ifndef A1_UTIL_H
#define A1_UTIL_H

//
// Created by Mingbo on 2023-10-01.
//

#ifndef SHELL379_UTIL_H
#define SHELL379_UTIL_H

#include <iostream>
#include <string>
#include <vector>
#include <cstring>

using namespace std;

string rtrim(const string &str);

vector<string> split(const string &str, char delim = ' ');

vector<char *> string_vector_to_char_vector(vector<string> original);

bool tokens_contain_pid(const vector<char *>& tokens);

pid_t get_pid_from_tokens(vector<char *> tokens);

char *trim_first_character(char *str);

#endif


#endif //A1_UTIL_H
