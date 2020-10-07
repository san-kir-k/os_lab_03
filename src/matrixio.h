#pragma once
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#define RF_VALID 0
#define RF_INVALID 1
#define RF_VALID_ENDL 2
#define RF_INVALID_ENDL 3
#define RF_VALID_EOF 4
#define RF_INVALID_EOF 5

#define RI_VALID 0
#define RI_INVALID 1
#define RI_EOF 2

#define STDIN 0
#define STDOUT 1
#define STDERR 2

void skip_tail();
bool check_tail();
bool is_valid(char* buf);
int read_float(float* n);
int read_int(int* n);
int scan_matrix(int str_c, int col_c, float* m);
void write_float(float n);
void print_matrix(int str_c, int col_c, float* m);
