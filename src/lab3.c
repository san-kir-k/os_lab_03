#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <dirent.h>
#include "matrixio.h"
// !для удобства будем хранить матрицу, как одномерный массив длины n*m!

// структура для передачи в функцию, которая будет запущена на нескольких потоках
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

// функция для вывода сообщений на экран
void logs(char* str, int fd) {
    int i = 0;
    while (str[i] != '\0') {
        write(fd, &str[i++], 1);
    }
}

// прочитать размер матрицы и проверить на валидность
int read_dimension(int* d) {
    if (read_int(d) != RI_VALID || (*d) <= 0) {
        char* err = "EOF or invalid dimension input!\n";
        logs(err, STDERR);
        return RI_INVALID;
    }
    return RI_VALID;
}

// полностью скопировать матрицу такого же размера
void deep_copy(int s_size, int c_size, float* from, float* to) {
    for (int i = 0; i < s_size; ++i) {
        for (int j = 0; j < c_size; ++j) {
            to[i * c_size + j] = from[i * c_size + j];
        }
    }
}

// считать матрицу float и обработать ошибки
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

// считаем кусок матрицы (по строкам) по алгоритму свертки (эта функция и будет запущена на потоках)
void* cell_algo(void* arg) {
    // парсим аргументы
    container* c = arg;
    int thread_id = c->thread_id, threads = c->threads,
    s_size = c->s_size, c_size = c->c_size;
    float* m = c->m;
    float* w = c->w;
    float* res = c->res;
    // массив для того, чтобы задавать, как нужно сместиться относительно центральной ячейки
    int dir[3] = {-1, 0, 1};
    // сам алгоритм (чтобы не допустить обращения тредов к одному и тому же месту памяти, будем давать им непересекающиеся секторы)
    // например: тред №1 будет обрабатывать 0-ую строку, тред №2 - 1-ую строку и тд
    for (int str_c = thread_id; str_c < s_size; str_c += threads) {
        for (int col_c = 0; col_c < c_size; col_c++) {
            int count = 0;
            float new_val = 0.0;
            // применяем матрицу свертки, обрабатывая невалидные индексы
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
            // записываем новое значение в массив
            res[str_c * c_size + col_c] = new_val;
        }
    }
    // заканчиваем тред
    pthread_exit(NULL);
}

// алгоритм, запускающий треды 
void algo(int s_size, int c_size, float* m, float* w, float* res, int K, int threads) {
    // создаем копию исходной матрицы, чтобы не испортить ее
    float* tmp_m = (float*)malloc(s_size * c_size * sizeof(float));
    deep_copy(s_size, c_size, m, tmp_m);
    // массив в котором будут храниться структуры, описывающие треды
    pthread_t tids[threads];
    // нужно хранить передаваемые аргументы, иначе цикл отработает и переменная уровня цикла удалится
    container c[threads];
    // K - число раз, сколько нужно применить матрицу свертки
    for (int _k = 0; _k < K; ++_k) {
        for (int i = 0; i < threads; ++i) {
            // заполняем структурку
            c[i].s_size = s_size;
            c[i].c_size = c_size;
            c[i].m = tmp_m;
            c[i].w = w;
            c[i].res = res;
            c[i].thread_id = i;
            c[i].threads = threads;
            // создаем тред с дефолтными атрибутами, в котором будет выполнятся функция cell_algo
            if (pthread_create(&tids[i], NULL, cell_algo, &c[i]) != 0) {
                char* err = "Unable to create a thread!\n";
                logs(err, STDERR);
            }
        }
        // считаем число тредов (к сожалению этот кусок кода работает только на линуксе, мак не знает о /proc/)
        /* раскоментируйте этот код, если хотите получить информацию о числе тредов
        if (_k == 0) { 
            DIR* dir;
            struct dirent* entry;
            int pid = getpid();
            char dirname[256];
            sprintf(dirname, "/proc/%d/task", pid);
            int threads_c = -3;
            if ((dir = opendir(dirname)) == NULL) {
                char* err = "Unable to open dir\n";
                logs(err, STDERR);
            } else {
                while ((entry = readdir(dir)) != NULL) {
                    threads_c++;
                }
                closedir(dir);
            }
            printf("Number of threads per one K: %d\n", threads_c);
        } 
        */
        // ждем все треды, тк нам нужна результирующая матрица полностью, чтобы применить еще k-1 раз матрицу свертки
        for (int i = 0; i < threads; ++i) {
            if (pthread_join(tids[i], NULL) != 0) {
                char* err = "Unable to wait a thread!\n";
                logs(err, STDERR);
            }
        }
        // если K > 1, то нужно перезаписать промежуточный результат во временную матрицу, чтобы работать с промежуточным результатом
        if (K > 1) {
            deep_copy(s_size, c_size, res, tmp_m);
        }
    }
    free(tmp_m);
}

int main(int argc, char* argv[]) {
    // обрабатываем ключи
    if (argc <= 2 && argc >= 4) {
        char* err = "Usage: ./executable -count_of_threads [-w].\n";
        logs(err, STDERR);
        return 1;
    } 
    // конвертация ключа в инт, чтобы получить число тредов
    char* e;
    int threads = strtol(argv[1] + 1, &e, 10);
    if (threads <= 0) {
        char* err = "Count of threads must be positive.\n";
        logs(err, STDERR);
        return 1;
    }
    // ввод всех данных + обработка ошибок
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
    float* m = (float*)malloc(m_s_size * m_c_size * sizeof(float));
    if (m == NULL) {
        char* err = "Matrix m: malloc error!\n";
        logs(err, STDERR);
        return 1;
    }
    float* result = (float*)malloc(m_s_size * m_c_size * sizeof(float));
    if (result == NULL) {
        free(m);
        char* err = "Matrix result: malloc error!\n";
        logs(err, STDERR);
        return 1;
    }
    int get_return = get_matrix(m_s_size, m_c_size, m);
    if (get_return != RF_VALID) {
        free(m);
        free(result);
        return 1;
    }
    // ввод окна
    float* w = (float*)malloc(w_s_size * w_c_size * sizeof(float));
    get_return = get_matrix(w_s_size, w_c_size, w);
    if (get_return != RF_VALID) {
        free(m);
        free(result);
        free(w);
        return 1;
    }
    // ввод K;
    get_return = read_int(&K);
    if (get_return == RI_INVALID) {
        char* err = "Invalid int input!\n";
        logs(err, STDERR);
        free(m);
        free(result);
        free(w);
        return 1;
    }
    // запуск алгоритма и бенчмарк
    clock_t begin = clock();
    algo(m_s_size, m_c_size, m, w, result, K, threads);
    clock_t end = clock();
    float time_spent = 1000.0 * (double)(end - begin) / CLOCKS_PER_SEC;
    if (get_return == RI_EOF) {
        char endl = '\n';
        write(STDIN, &endl, 1);
    }
    // вывод
    if (argc != 3) {
        char* output_msg = "Result matrix is:\n";
        logs(output_msg, STDOUT);
        print_matrix(m_s_size, m_c_size, result);
    }
    char* time_msg = "Time spent on algo (ms):\n";
    logs(time_msg, STDOUT);
    write_float(time_spent);
    char endl = '\n';
    write(STDOUT, &endl, 1);
    free(m);
    free(result);
    free(w);
    return 0;
}
