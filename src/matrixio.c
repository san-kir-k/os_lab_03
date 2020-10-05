#include "matrixio.h"

void skip_tail() {
    char ch;
    while (read(STDIN, &ch, 1) > 0) {
        if (ch == '\n') {
            break;
        }
    }
}

bool check_tail() {
    char ch;
    bool to_return = true;
    while (read(STDIN, &ch, 1) > 0) {
        if (ch == ' ') {
            continue;
        } else if (ch == '\n') {
            break;
        } else {
            to_return = false;
            break;
        }
    }
    return to_return;
}

bool is_valid(char* buf) {
    int i = 0;
    int count_of_dots = 0;
    while (buf[i] != '\0') {
        if (isdigit(buf[i]) || buf[i] == '.') {
            if (buf[i] == '.') {
                count_of_dots++;
            }
            i++;
        } else {
            return false;
        }
    }
    return (count_of_dots <= 1 && i > 0) ? true : false;
}

int read_float(float* n) {
    char ch;
    char buf[30];
    int count = 0; 
    int read_return = 1; // проверка на еоф (read возвращает 0, если еоф)
    float sign = 1.0;
    bool is_start = true, is_endl = false;
    while ((read_return = read(STDIN, &ch, 1)) > 0) {
        if (ch == ' ' && is_start) {
            continue;
        }
        is_start = false;
        if (ch == '-') {
            sign *= -1;
        } else if (ch == ' ') {
            break;
        } else if (ch == '\n') {
            is_endl = true;
            break;
        } else {
            buf[count++] = ch;
        }
    }
    buf[count] = '\0';
    if (!is_valid(buf)) {
        if (read_return == 0) {
            return RF_INVALID_EOF;
        }
        return (is_endl) ? RF_INVALID_ENDL : RF_INVALID;
    }
    *n = atof(buf) * sign;
    if (read_return == 0) {
        return RF_VALID_EOF;
    }
    return (is_endl) ? RF_VALID_ENDL : RF_VALID;
}

int scan_matrix(int str_c, int col_c, float m[str_c][col_c]) {
    int read_return = 0;
    for (int i = 0; i < str_c; ++i) {
        for (int j = 0; j < col_c; ++j) {
            read_return = read_float(&m[i][j]);
            if (read_return == RF_INVALID || read_return == RF_INVALID_ENDL) {
                if (read_return == RF_INVALID) {
                    skip_tail();
                }
                return RF_INVALID;
            } else if (read_return == RF_VALID_EOF || read_return == RF_INVALID_EOF) {
                if (i == str_c - 1 && j == col_c - 1 && read_return == RF_VALID_EOF) {
                    return RF_VALID_EOF;
                }
                return RF_INVALID_EOF;
            }
        }
        if (read_return != RF_VALID_ENDL) {
            if (!check_tail()) {
                skip_tail();
                return RF_INVALID;
            }
        }
    }
    return RF_VALID;
}

void write_float(float n) {
    char buf[30];
    sprintf(buf, "%7.3f", n);
    int i = 0;
    while(buf[i] != '\0') {
        write(STDOUT, &buf[i++], 1);
    }
}

void print_matrix(int str_c, int col_c, float m[str_c][col_c]) {
    char space = ' ';
    char endl = '\n';
    for (int i = 0; i < str_c; ++i) {
        for (int j = 0; j < col_c; ++j) {
            write_float(m[i][j]);
            write(STDOUT, &space, 1);
        }
        write(STDOUT, &endl, 1);
    }
}
