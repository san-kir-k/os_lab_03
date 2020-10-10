import sys
import random
import string

def main():
    if len(sys.argv) != 2:
        print(f"Usage: {sys.argv[0]} <test directory>")
        sys.exit(1)

    test_dir = sys.argv[1]
    line_matrix_count = [10, 1000]
    for test_count in range(2):
        val_list = [round(random.uniform(-10.0, 10.0), random.randint(0, 3)) for _ in range(line_matrix_count[test_count] ** 2)]
        s = line_matrix_count[test_count]
        c = line_matrix_count[test_count]
        k = [1, 2]
        window = [round(random.uniform(0.0, 2.0), 1) for _ in range(9)]
        for ki in k:
            test_name = "{}/test_{}_{}".format(test_dir, test_count + 1, ki)
            with open(f'{test_name}.txt', 'w') as ftest:
                ftest.write(f'{s} {c}\n')
                for t in val_list:
                    ftest.write(f'{t} ')
                ftest.write(f'\n')
                for t in window:
                    ftest.write(f'{t} ')
                ftest.write(f'\n')
                ftest.write(f'{ki}\n')

main()
