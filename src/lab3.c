#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include "matrixio.h"

void logs(char* str, int fd) {
    int i = 0;
    while (str[i] != '\0') {
        write(fd, &str[i++], 1);
    }
}

int read_dimension(int* d) {
    if (read_int(d) != RI_VALID || (*d) <= 0) {
        char* err = "EOF or invalid dimension input!\n";
        logs(err, STDERR);
        return RI_INVALID;
    }
    return RI_VALID;
}

int get_matrix(int str_c, int col_c, float m[str_c][col_c]) {
    int scan_return = scan_matrix(str_c, col_c, m);
    if (scan_return == RF_INVALID || scan_return == RF_INVALID_EOF) {
        char endl = '\n';
        char* err = "Invalid matrix, try to enter again!\n";
        if (scan_return == RF_INVALID_EOF) {
            write(STDERR, &endl, 1);
        }
        logs(err, STDERR);
    }  else if (scan_return == RF_VALID_EOF) {
        char* err = "\nUnexpected EOF, try to enter again!\n";
        logs(err, STDERR);
    }
    return scan_return;
}

int main() {
    char* input_msg = "Enter matrix dimensions, matrix, enter window, enter K.\n";
    logs(input_msg, STDOUT);
    int m_str_c = 0, m_col_c = 0, w_str_c = 3, w_col_c = 3, K = 0;
    // ввод матрицы исходной
    if (read_dimension(&m_str_c) != RI_VALID) {
        return 1;
    }
    if (read_dimension(&m_col_c) != RI_VALID) {
        return 1;
    }
    float m[m_str_c][m_col_c], result[m_str_c][m_col_c];
    int get_return = get_matrix(m_str_c, m_col_c, m);
    if (get_return != RF_VALID) {
        return 1;
    }
    // ввод окна
    float w[w_str_c][w_col_c];
    get_return = get_matrix(w_str_c, w_col_c, w);
    if (get_return != RF_VALID) {
        return 1;
    }
    // ввод K
    get_return = read_int(&K);
    if (get_return == RI_INVALID) {
        char* err = "Invalid int input!\n";
        logs(err, STDERR);
        return 1;
    }
    if (get_return == RI_EOF) {
        char endl = '\n';
        write(STDIN, &endl, 1);
    }
    // algo
    char* output_msg = "Result matrix is:\n";
    logs(output_msg, STDOUT);
    print_matrix(m_str_c, m_col_c, m);
    print_matrix(w_str_c, w_col_c, w);
    return 0;
}
