#pragma once
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#define RF_VALID 0
#define RF_INVALID 2
#define RF_VALID_ENDL 3
#define RF_INVALID_ENDL 4
#define RF_VALID_EOF 5
#define RF_INVALID_EOF 6

#define STDIN 0
#define STDOUT 1
#define STDERR 2

void skip_tail();
bool check_tail();
bool is_valid(char* buf);
int read_float(float* n);
int scan_matrix(int str_c, int col_c, float m[str_c][col_c]);
void write_float(float n);
void print_matrix(int str_c, int col_c, float m[str_c][col_c]);
