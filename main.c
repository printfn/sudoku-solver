#include <stdlib.h>
#include <stdio.h>
#include <strings.h>

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

void print_board(const char *board) {
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
        if (idx_to_try < 0) {
            printf("Trying to place %i at %i\n", digit_to_try, idx_to_try);
            print_board(board);
            printf("%s\n", board);
        }
        if (!test_digit(board, idx_to_try) && !solve(board)) {
            // success!
            return 0;
        }
        // error
    }
    board[idx_to_try] = ' ';
    return 1;
}

static int test(void) {
    for (int i = 0; i < 81; ++i) {
        if (example1[i] != ' ') {
            test_digit(example1, i);
        }
    }
    char temp_board[82];
    strcpy(temp_board, example1);
    temp_board[2] = '1';
    if (test_digit(temp_board, 2))
        return 1;
    temp_board[2] = '2';
    if (test_digit(temp_board, 2))
        return 1;
    temp_board[2] = '4';
    if (test_digit(temp_board, 2))
        return 1;
    temp_board[2] = '3';
    if (!test_digit(temp_board, 2))
        return 1;
    temp_board[2] = '5';
    if (!test_digit(temp_board, 2))
        return 1;
    temp_board[2] = '6';
    if (!test_digit(temp_board, 2))
        return 1;
    temp_board[2] = '7';
    if (!test_digit(temp_board, 2))
        return 1;
    temp_board[2] = '8';
    if (!test_digit(temp_board, 2))
        return 1;
    temp_board[2] = '9';
    if (!test_digit(temp_board, 2))
        return 1;
    temp_board[2] = ' ';
    temp_board[5] = '3';
    // invalid board test
    if (!test_digit(temp_board, 5))
        return 1;
    // (partial) valid board test
    if (test_digit(
        "53467891262 195    98    6 "
        "8   6   34  8 3  17   2   6"
        " 6    28    419  5    8  79", 10)) {
        return 1;
    }
    return 0;
}

int main(int argc, char *argv[]) {
    if (test()) {
        fprintf(stderr, "Tests failed\n");
        return 1;
    }
    char board[82];
    strcpy(board, example3);
    if (solve(board)) {
        printf("Unable to solve board:\n");
        print_board(board);
        return 1;
    }
    printf("Found a solution:\n");
    print_board(board);
    return 0;
}
