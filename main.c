#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// easy sudoku
static const char *example1 =
    "53  7    6  195    98    6 "
    "8   6   34  8 3  17   2   6"
    " 6    28    419  5    8  79";

// hard sudoku
static const char *example2 =
    "            5 4    7 169 5 "
    " 6     3  81   42   94 85  "
    "9  8 5  1 1     7  5 6 3 4 ";

// hard sudoku for brute-force algorithms
static const char *example3 =
    "              3 85  1 2    "
    "   5 7     4   1   9       "
    "5      73  2 1        4   9";

static const char *blank =
    "                           "
    "                           "
    "                           ";

static const char *puzzle1 =
    "         "
    "         "
    "    7  1 "
    "    8    "
    "         "
    "    6    "
    " 3  5    "
    "         "
    "         ";

static const char *puzzle2 =
    "  7  8   "
    "    4    "
    "   9   6 "
    "    1 4  "
    "         "
    "  1 8    "
    " 2       "
    "    94   "
    "   2  6  ";

static void print_board(const char *board) {
    for (int i = 0; i < 81; ++i) {
        if (i != 0 && i % 9 == 0) printf("\n");
        if (i % 9 != 0 && i % 3 == 0) printf("|");
        if (i % 27 == 0) printf("+-----+-----+-----+\n");
        if (i % 9 == 0) printf("|");
        printf("%c", board[i]);
        if (i % 3 != 2) printf(" ");
        if (i % 9 == 8) printf("|");
    }
    printf("\n+-----+-----+-----+\n");
}

static int test_modulo_addition(const char *board, int idx, int plus_location) {
    if (idx != plus_location
        && idx + 1 != plus_location
        && idx - 1 != plus_location
        && idx + 9 != plus_location
        && idx - 9 != plus_location)
        return 0;

    if (board[plus_location] == ' ') return 0;
    int sum = 0;
    if (plus_location % 9 != 0) {
        if (board[plus_location - 1] == ' ') return 0;
        sum += board[plus_location - 1] - '0';
    }
    if (plus_location % 9 != 8) {
        if (board[plus_location + 1] == ' ') return 0;
        sum += board[plus_location + 1] - '0';
    }
    if (plus_location / 9 != 0) {
        if (board[plus_location - 9] == ' ') return 0;
        sum += board[plus_location - 9] - '0';
    }
    if (plus_location / 9 != 8) {
        if (board[plus_location + 9] == ' ') return 0;
        sum += board[plus_location + 9] - '0';
    }
    if ((board[plus_location] - '0') != (sum % 9)) {
        return 1;
    }
    return 0;
}

// 0 on success, 1 on error
static int test_digit(const char *board, int idx) {
    int logging = 0;

    if (board[idx] == ' ') {
        fprintf(stderr, "warning: testing non-existent digit\n");
        return 0;
    }
    if (board[idx] < '0' || board[idx] > '9') {
        fprintf(stderr, "error: invalid character in board\n");
        return 1;
    }

    if (logging) {
        print_board(board);
    }

    // row
    if (logging) printf("testing row:\n");
    int flags = 0;
    for (int i = idx / 9 * 9; i < idx / 9 * 9 + 9; ++i) {
        if (board[i] == ' ') continue;
        if (logging) printf("Testing digit %c\n", board[i]);
        int flag = 1 << (board[i] - '0');
        if (flags & flag) return 1;
        flags |= flag;
        if (logging) printf("%02x\n", flags);
    }

    // col
    if (logging) printf("testing col:\n");
    flags = 0;
    for (int i = idx % 9; i < idx % 9 + 81; i += 9) {
        if (board[i] == ' ') continue;
        if (logging) printf("Testing digit %c\n", board[i]);
        int flag = 1 << (board[i] - '0');
        if (flags & flag) return 1;
        flags |= flag;
        if (logging) printf("%02x\n", flags);
    }

    // box
    if (logging) printf("testing box:\n");
    flags = 0;
    int box_start = idx % 9 / 3 * 3 + idx / 9 / 3 * 3 * 9;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            char ch = board[box_start + i + 9 * j];
            if (ch == ' ') continue;
            if (logging) printf("Testing digit %c\n", ch);
            int flag = 1 << (ch - '0');
            if (flags & flag) return 1;
            flags |= flag;
            if (logging) printf("%02x\n", flags);
        }
    }

    // extra rules

    // diagonals
    flags = 0;
    if (idx % 10 == 0) {
        for (int i = 0; i < 81; i += 10) {
            if (board[i] == ' ') continue;
            int flag = 1 << (board[i] - '0');
            if (flags & flag) return 1;
            flags |= flag;
        }
    }
    flags = 0;
    if (idx % 8 == 0) {
        for (int i = 8; i <= 72; i += 8) {
            if (board[i] == ' ') continue;
            int flag = 1 << (board[i] - '0');
            if (flags & flag) return 1;
            flags |= flag;
        }
    }

    // modulo addition (puzzle 1)
    // if (test_modulo_addition(board, idx, 0)
    //     || test_modulo_addition(board, idx, 8)
    //     || test_modulo_addition(board, idx, 12)
    //     || test_modulo_addition(board, idx, 14)
    //     || test_modulo_addition(board, idx, 20)
    //     || test_modulo_addition(board, idx, 24)
    //     || test_modulo_addition(board, idx, 28)
    //     || test_modulo_addition(board, idx, 30)
    //     || test_modulo_addition(board, idx, 32)
    //     || test_modulo_addition(board, idx, 34)
    //     || test_modulo_addition(board, idx, 38)
    //     || test_modulo_addition(board, idx, 42)
    //     || test_modulo_addition(board, idx, 46)
    //     || test_modulo_addition(board, idx, 48)
    //     || test_modulo_addition(board, idx, 50)
    //     || test_modulo_addition(board, idx, 52)
    //     || test_modulo_addition(board, idx, 56)
    //     || test_modulo_addition(board, idx, 60)
    //     || test_modulo_addition(board, idx, 66)
    //     || test_modulo_addition(board, idx, 68)
    //     || test_modulo_addition(board, idx, 72)
    //     || test_modulo_addition(board, idx, 80)) {
    //     return 1;
    // }


    if (test_modulo_addition(board, idx, 0)
        || test_modulo_addition(board, idx, 6)
        || test_modulo_addition(board, idx, 8)
        || test_modulo_addition(board, idx, 27)
        || test_modulo_addition(board, idx, 30)
        || test_modulo_addition(board, idx, 34)
        || test_modulo_addition(board, idx, 46)
        || test_modulo_addition(board, idx, 50)
        || test_modulo_addition(board, idx, 53)
        || test_modulo_addition(board, idx, 72)
        || test_modulo_addition(board, idx, 74)
        || test_modulo_addition(board, idx, 80)) {
        return 1;
    }

    return 0;
}

// 0: valid (possibly partial) solution, 1: invalid
static int solve(char *board) {
    int idx_to_try = -1;
    for (int i = 0; i < 81; ++i) {
        if (board[i] == ' ') {
            idx_to_try = i;
            break;
        }
    }
    if (idx_to_try == -1) {
        // board is filled in
        for (int i = 0; i < 81; ++i) {
            if (test_digit(board, i)) {
                return 1;
            }
        }
        return 0;
    }
    // try placing a digit at idx_to_try
    for (int digit_to_try = 1; digit_to_try <= 9; ++digit_to_try) {
        board[idx_to_try] = '0' + digit_to_try;
        if (idx_to_try < 10) {
            printf("Trying to place %i at %i\n", digit_to_try, idx_to_try);
            print_board(board);
            printf("%s\n", board);
        }
        if (!test_digit(board, idx_to_try) && !solve(board)) {
            // success!
            //printf("Found a solution:\n");
            printf("%s\n", board);
            //print_board(board);
            //return 0;
        }
        // error
    }
    board[idx_to_try] = ' ';
    return 1;
}

static int test(void) {
    if (!test_modulo_addition("123495678457618239689273415214387596365129784798564123831756942542931867976842351", 0, 0)) {
        return 1;
    }
    if (test_modulo_addition("953495678457618239689273415214387596365129784798564123831756942542931867976842351", 0, 0)) {
        return 1;
    }
    if (test_modulo_addition("953495678457618239689273415214387596365129784798564123831756942542931867976842353", 80, 80)) {
        return 1;
    }
    // for (int i = 0; i < 81; ++i) {
    //     if (example1[i] != ' ') {
    //         test_digit(example1, i);
    //     }
    // }
    // char temp_board[82];
    // strcpy(temp_board, example1);
    // temp_board[2] = '1';
    // if (test_digit(temp_board, 2))
    //     return 1;
    // temp_board[2] = '2';
    // if (test_digit(temp_board, 2))
    //     return 1;
    // temp_board[2] = '4';
    // if (test_digit(temp_board, 2))
    //     return 1;
    // temp_board[2] = '3';
    // if (!test_digit(temp_board, 2))
    //     return 1;
    // temp_board[2] = '5';
    // if (!test_digit(temp_board, 2))
    //     return 1;
    // temp_board[2] = '6';
    // if (!test_digit(temp_board, 2))
    //     return 1;
    // temp_board[2] = '7';
    // if (!test_digit(temp_board, 2))
    //     return 1;
    // temp_board[2] = '8';
    // if (!test_digit(temp_board, 2))
    //     return 1;
    // temp_board[2] = '9';
    // if (!test_digit(temp_board, 2))
    //     return 1;
    // temp_board[2] = ' ';
    // temp_board[5] = '3';
    // // invalid board test
    // if (!test_digit(temp_board, 5))
    //     return 1;
    // // (partial) valid board test
    // if (test_digit(
    //     "53467891262 195    98    6 "
    //     "8   6   34  8 3  17   2   6"
    //     " 6    28    419  5    8  79", 10)) {
    //     return 1;
    // }
    return 0;
}

// int main(int argc, char *argv[]) {
//     // if (test()) {
//     //     fprintf(stderr, "Tests failed\n");
//     //     return 1;
//     // }
//     char board[82];
//     strcpy(board, puzzle2);
//     if (solve(board)) {
//         printf("Unable to solve board:\n");
//         print_board(board);
//         return 1;
//     }
//     printf("Found a solution:\n");
//     printf("%s\n", board);
//     print_board(board);
//     return 0;
// }

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "No files!\n");
        return 1;
    }
    int sum[82] = {0,};
    printf("Reading %i files\n", argc - 1);
    for (int i = 1; i < argc; ++i) {
        printf("Reading %s\n", argv[i]);
        FILE *f = fopen(argv[i], "r");
        char board[82];
        int board_idx = 0;
        while (1) {
            int ci = fgetc(f);
            if (ci == -1) {
                board[board_idx++] = '\0';
                if (board_idx < 81) {
                    fprintf(stderr, "Too short: %s\n", argv[i]);
                    return 1;
                }
                break;
            }
            char c = (char)ci;
            if (c >= '0' && c <= '9') {
                board[board_idx++] = c;
            } else if (c == ' ' || c == '\n') {
            } else {
                fprintf(stderr, "Found invalid char in file %s: %c\n", argv[i], c);
            }
        }
        fclose(f);
        printf("Checking board...\n");
        print_board(board);
        for (int j = 0; j < 81; ++j) {
            if (test_digit(board, j)) {
                fprintf(stderr, "Error in sudoku (digit %i)\n", j);
            }
            sum[j] += board[j] - '0';
        }
    }
    printf("Normal modulo (0 => 9):\n");
    for (int i = 0; i < 81; ++i) {
        int x = (sum[i] + 8) % 9 + 1;
        printf("%i", x);
    }
    printf("\n");
    printf("Adds 1:\n");
    for (int i = 0; i < 81; ++i) {
        int x = sum[i] % 9 + 1;
        printf("%i", x);
    }
    printf("\n");
}
