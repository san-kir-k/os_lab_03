#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include "matrixio.h"

typedef struct container container;
struct container {
    int s_size;
    int c_size;
    float* m;
    float* w;
    float* res;
    int thread_id;
    int threads;
};

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

void deep_copy(int s_size, int c_size, float* from, float* to) {
    for (int i = 0; i < s_size; ++i) {
        for (int j = 0; j < c_size; ++j) {
            to[i * c_size + j] = from[i * c_size + j];
        }
    }
}

int get_matrix(int s_size, int c_size, float* m) {
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
void* cell_algo(void* arg) {
    container* c = arg;
    int thread_id = c->thread_id, threads = c->threads,
    s_size = c->s_size, c_size = c->c_size;
    float* m = c->m;
    float* w = c->w;
    float* res = c->res;
    int dir[3] = {-1, 0, 1};
    for (int t = thread_id; t < s_size * c_size; t += threads) {
        int count = 0;
        float new_val = 0.0;
        int str_c = t / s_size;
        int col_c = t % c_size; 
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                if (((str_c + dir[i]) * c_size + col_c + dir[j] >= 0) &&
                ((str_c + dir[i]) * c_size + col_c + dir[j] < c_size * s_size)) {
                    new_val += w[i * 3 + j] * m[(str_c + dir[i]) * c_size + col_c + dir[j]];
                    count++;
                }
            }
        }
        new_val /= count;
        res[str_c * c_size + col_c] = new_val;
    }
    pthread_exit(NULL);
}

// алгоритм 
void algo(int s_size, int c_size, float* m, float* w, float* res, int K, int threads) {
    int sectors = s_size * c_size / threads;
    sectors += (s_size * c_size % threads == 0) ? 0 : 1;
    float tmp_m[s_size * c_size];
    deep_copy(s_size, c_size, m, tmp_m);
    pthread_t tids[threads];
    // нужно хранить передаваемые аргументы, иначе цикл отработает и переменная уровня цикла удалится
    container c[threads];
    for (int _k = 0; _k < K; ++_k) {
        for (int i = 0; i < threads; ++i) {
            c[i].s_size = s_size;
            c[i].c_size = c_size;
            c[i].m = tmp_m;
            c[i].w = w;
            c[i].res = res;
            c[i].thread_id = i;
            c[i].threads = threads;
            if (pthread_create(&tids[i], NULL, cell_algo, &c[i]) != 0) {
                char* err = "Unable to create a thread!\n";
                logs(err, STDERR);
            }
        }
        for (int i = 0; i < threads; ++i) {
            if (pthread_join(tids[i], NULL) != 0) {
                char* err = "Unable to wait a thread!\n";
                logs(err, STDERR);
            }
        }
        deep_copy(s_size, c_size, res, tmp_m);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        char* err = "Usage: ./executable -count_of_threads.\n";
        logs(err, STDERR);
        return 1;
    } 
    // конвертация строки в инт
    char* e;
    int threads = strtol(argv[1] + 1, &e, 10);
    if (threads <= 0) {
        char* err = "Count of threads must be positive.\n";
        logs(err, STDERR);
        return 1;
    }
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
    float m[m_s_size * m_c_size], result[m_s_size * m_c_size];
    int get_return = get_matrix(m_s_size, m_c_size, m);
    if (get_return != RF_VALID) {
        return 1;
    }
    // ввод окна
    float w[w_s_size * w_c_size];
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
    clock_t begin = clock();
    algo(m_s_size, m_c_size, m, w, result, K, threads);
    clock_t end = clock();
    float time_spent = 1000.0 * (double)(end - begin) / CLOCKS_PER_SEC;
    if (get_return == RI_EOF) {
        char endl = '\n';
        write(STDIN, &endl, 1);
    }
    char* output_msg = "Result matrix is:\n";
    logs(output_msg, STDOUT);
    print_matrix(m_s_size, m_c_size, result);
    char* time_msg = "Time spent on algo (ms):\n";
    logs(time_msg, STDOUT);
    write_float(time_spent);
    char endl = '\n';
    write(STDOUT, &endl, 1);
    return 0;
}
