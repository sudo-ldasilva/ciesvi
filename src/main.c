#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define DEFER(n) do { ret = n; goto exit; } while (0)

#include "csv.h"

bool is_a_selected_column(int e, const Column *array, const int array_length) {
    for (int i=0; i<array_length; i++) {
        if (e == array[i]) return true;
    }
    return false;
}

void read_header(FILE *input, FILE *output, const Column *columns, unsigned int columns_length) {
    char c;
    Column col = COLUMN_TIME;
    unsigned int printed_columns = 0;

    while ( (c = fgetc(input)) != DELIM_ROW && c != EOF) {
        if (c == DELIM_COL) {
            if (is_a_selected_column(col, columns, columns_length) && printed_columns < columns_length) {
                fprintf(output, ", ");
                printed_columns++;
            }

            col++;
        }

        else if (isprint(c)) {
            if (is_a_selected_column(col, columns, columns_length)) {
                putc(c, output);
            }
        }
    }

    putc('\n', output);
}

bool read_row(FILE *input, FILE *output, const Column *columns, unsigned int columns_length) {
    char c;
    unsigned int col = 0;
    unsigned int date_index = 0;
    unsigned int printed_columns = 0;

    while ( (c = fgetc(input)) != EOF) {
        if (c == DELIM_ROW) {
            fprintf(output, "\n");
            col = 0;
            date_index = 0;
            return true;
        }

        else if (c == DELIM_COL) {
            // If it printed a columns and it isn't the last one to print
            if (is_a_selected_column(col, columns, columns_length) && printed_columns < columns_length) {
                fprintf(output, ", ");
            }

            col++;
        }

        // Print value
        else if (isprint(c)) {
            if (is_a_selected_column(col, columns, columns_length)) {
                switch (col) {
                    case COLUMN_TIME:
                        if (date_index < 4) putc(c, output);
                        date_index++;
                        break;

                    default:
                        putc(c, output);
                        break;
                }
            }
        }
    };

    return false;
}

int main(int argc, char *argv[]) {
    int ret = 0;
    FILE *input = NULL;

    const Column columns[] = { /* COLUMN_TIME, */ COLUMN_GLAC_MEAN, COLUMN_GLOBAL_AVERAGE_SEA_LEVEL_CHANGE };
    const unsigned int columns_length = sizeof(columns) / sizeof(*columns);

    if (argc != 1) {
        fprintf(stderr, "[ERROR] Expected no parameters\n");
        DEFER(1);
    }

    input = fopen(CSV_PATH, "r");
    if (!input) DEFER(2);

    read_header(input, stdout, columns, columns_length);
    while (read_row(input, stdout, columns, columns_length));

exit:
    if (input) { fclose(input); input = NULL; }
    return ret;
}
