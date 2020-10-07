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

void deep_copy(int s_size, int c_size,
float to_copy[s_size][c_size], float copy[s_size][c_size]) {
    for (int i = 0; i < s_size; ++i) {
        for (int j = 0; j < c_size; ++j) {
            copy[i][j] = to_copy[i][j];
        }
    }
}

int get_matrix(int s_size, int c_size, float m[s_size][c_size]) {
    int scan_return = scan_matrix(s_size, c_size, m);
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

// считаем одну ячейку
void cell_algo(int str_c, int col_c, int s_size, int c_size,
float m[s_size][c_size], float w[3][3], float res[s_size][c_size]) {
    float new_val = 0.0;
    int dir[3] = {-1, 0, 1};
    int count = 0;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (str_c + dir[i] >= 0 && col_c + dir[j] >= 0 && str_c + dir[i] < s_size && col_c + dir[j] < c_size) {
                new_val += w[i][j] * m[str_c + dir[i]][col_c + dir[j]];
                count++;
            }
        }
    }
    new_val /= count;
    res[str_c][col_c] = new_val;
}

// алгоритм (пока однопоточный)
void algo(int s_size, int c_size, float m[s_size][c_size],
float w[3][3], float res[s_size][c_size], int K) {
    float tmp_m[s_size][c_size];
    deep_copy(s_size, c_size, m, tmp_m);
    for (int _k = 0; _k < K; ++_k) {
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                cell_algo(i, j, 3, 3, tmp_m, w, res);
            }
        }
        deep_copy(s_size, c_size, res, tmp_m);
    }
}

int main() {
    char* input_msg = "Enter matrix dimensions, matrix, enter window, enter K.\n";
    logs(input_msg, STDOUT);
    int m_s_size = 0, m_c_size = 0, w_s_size = 3, w_c_size = 3, K = 0;
    // ввод матрицы исходной
    if (read_dimension(&m_s_size) != RI_VALID) {
        return 1;
    }
    if (read_dimension(&m_c_size) != RI_VALID) {
        return 1;
    }
    float m[m_s_size][m_c_size], result[m_s_size][m_c_size];
    int get_return = get_matrix(m_s_size, m_c_size, m);
    if (get_return != RF_VALID) {
        return 1;
    }
    // ввод окна
    float w[w_s_size][w_c_size];
    get_return = get_matrix(w_s_size, w_c_size, w);
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
    algo(m_s_size, m_c_size, m, w, result, K);
    if (get_return == RI_EOF) {
        char endl = '\n';
        write(STDIN, &endl, 1);
    }
    char* output_msg = "Result matrix is:\n";
    logs(output_msg, STDOUT);
    print_matrix(m_s_size, m_c_size, result);
    return 0;
}
