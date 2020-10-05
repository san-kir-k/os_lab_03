#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include "matrixio.h"

void logs(char* err) {
    char endl = '\n';
    int i = 0;
    while (err[i] != '\n') {
        write(STDERR, &err[i++], 1);
    }
    write(STDERR, &endl, 1);
}

int get_matrix(int str_c, int col_c, float m[str_c][col_c]) {
    int scan_return = scan_matrix(str_c, col_c, m);
    if (scan_return == RF_INVALID || scan_return == RF_INVALID_EOF) {
        char endl = '\n';
        char* err = "Invalid matrix, try to enter again!\n";
        if (scan_return == RF_INVALID_EOF) {
            write(STDERR, &endl, 1);
        }
        logs(err);
    } 
    return scan_return;
}

int main() {
    int str_c = 0, col_c = 0;
    if (read_int(&str_c) != RI_VALID || str_c <= 0) {
        char* err = "str_c: EOF or invalid int input!\n";
        logs(err);
        return 1;
    }
    if (read_int(&col_c) != RI_VALID || col_c <= 0) {
        char* err = "col_c: EOF or invalid int input!\n";
        logs(err);
        return 1;
    }
    float m[str_c][col_c];
    int get_return = get_matrix(str_c, col_c, m);
    if (get_return == RF_INVALID || get_return == RF_INVALID_EOF) {
        return 1;
    } else {
        if (get_return == RF_VALID_EOF) {
            char endl = '\n';
            write(STDIN, &endl, 1);
        }
        // algo
        print_matrix(str_c, col_c, m);
    } 
    return 0;
}
