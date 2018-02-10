#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define BIT 32

bool parse_argv(int, char **, char *, int *, int *, int *);
void set_start_end(char *, int *, int *);
bool convert_to_binary(char *, char *);
bool binary_to_binary(char *, int, char *);
bool hexadecimal_to_binary(char *, int, char *);
bool string_to_binary(char *, int, char *);
bool unsigned_decimal_to_binary(unsigned long long, char *);
bool to_negative(char *);
void set_output_bits(char *, char *, int, int, int);
void print_all(char *, int);
void print_binary(char *, int);
void print_hexadecimal_and_unsigned(char *);
void print_hexadecimal(unsigned int);
void print_signed(char *);
unsigned int calculate_decimal(char *);

int main(int argc, char **argv) {
    
    int bit_count, range_start, range_end;
    char bits[BIT];
    bool check;
    
    check = parse_argv(argc, argv, bits, &bit_count, &range_start, &range_end);

    if (!check) {
        printf("Error: %s cannot fit into %d bits.\n", argv[0], BIT);
    }
    else {
        char output_bits[bit_count + 1];
        set_output_bits(output_bits, bits, bit_count, range_start, range_end);
        printf("%s\n", output_bits);
        //print_all(bits, bit_count);
    }

    return 0;
}

bool parse_argv(int argc, char **argv, char *bits, int *bit_count, int *range_start, int *range_end) {
    int i;
    bool check = false;

    // default setting
    *bit_count = BIT;
    *range_start = 0;
    *range_end = 31;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-b") == 0) {
            *bit_count = atoi(argv[i + 1]);
            i++;
        }
        else if (strcmp(argv[i], "-r") == 0) {
            set_start_end(argv[i + 1], range_start, range_end);
            i++;
        }
        else {
            check = convert_to_binary(argv[i], bits);
            strcpy(argv[0], argv[i]);
        }
    }
    
    return check;
}

void set_start_end(char *arg, int *range_start, int *range_end) {
    int i, j, len;
    char temp[3];
    len = strlen(arg);

    for (i = 0, j = 0; i < len; i++) {
        if (arg[i] == ',') {
            temp[j] = '\0';
            *range_start = atoi(temp);
            j = 0;
        }
        else {
            temp[j] = arg[i];
            j++;
        }
    }

    temp[j] = '\0';
    *range_end = atoi(temp);
}

bool convert_to_binary(char *arg, char *bits) {
    int len = strlen(arg);
    bool check;
    
    if (len > 2 && arg[1] == 'b') {
        check = binary_to_binary(arg, len, bits);
    }
    else if (len > 2 && arg[1] == 'x') {
        check = hexadecimal_to_binary(arg, len, bits);
    }
    else {
        check = string_to_binary(arg, len, bits);
    }

    return check;
}

bool binary_to_binary(char *arg, int len, char *bits) {
    int i, j;
    bool check = true;

    for (i = len - 1, j = 31; j >= 0 || i >= 2; i--, j--) {
        if (i < 2) {
            bits[j] = '0';
        }
        else if (j < 0 && arg[i] == '1') {
            check = false; // overflow
        }
        else if (j >= 0) {
            bits[j] = arg[i];
        }
    }

    return check;
}

bool hexadecimal_to_binary(char *arg, int len, char *bits) {
    int i;
    char c;
    unsigned long unsigned_decimal;

    for (i = 2, unsigned_decimal = 0; i < len; i++) {
        c = arg[i];
        unsigned_decimal *= 16;
        unsigned_decimal += (c >= 'A' && c <= 'F') ? (10 + c - 'A') : (0 + c - '0');
    }
    
    return unsigned_decimal_to_binary(unsigned_decimal, bits);
}

bool string_to_binary(char *arg, int len, char *bits) {
    int i;
    unsigned int temp;
    char c;
    unsigned long long unsigned_decimal;
    bool check, negative = false;

    if (arg[0] == '-') {
        negative = true;
        i = 1;
    }
    else {
        i = 0;
    }
    
    for (unsigned_decimal = 0; i < len; i++) {
        c = arg[i];
        unsigned_decimal = unsigned_decimal * 10 + (0 + c - '0');
    }

    check = unsigned_decimal_to_binary(unsigned_decimal, bits);  

    if (negative) {
        check = check && to_negative(bits);
    }

    return check;
}

bool unsigned_decimal_to_binary(unsigned long long unsigned_decimal, char *bits) {
    int i;

    for (i = BIT - 1; i >= 0; i--) {
        bits[i] = (unsigned_decimal & 0b1) == 0 ? '0' : '1';
        unsigned_decimal >>= 1;
    }

    if (unsigned_decimal > 0) {
        return false;
    }

    return true;
}

bool to_negative(char *bits) {
    int i;
    char carry = '1';

    for (i = BIT - 1; i >= 0; i--) {
        bits[i] = bits[i] == '0' ? '1' : '0';
    }

    for (i = BIT - 1; i >= 0; i--) {
        if (carry == '1' && bits[i] == '1') {
            bits[i] = '0';
        }
        else if (carry == '1' && bits[i] == '0') {
            bits[i] = '1';
            carry = '0';
        }
        else {
            break;
        }
    }

    if (carry == '1') {
        return false;
    }

    return true;
}

void set_output_bits(char *output_bits, char *bits, int bit_count, int range_start, int range_end) {
    int i, j, stop;
    stop = BIT - 1 - range_end;
    
    for (i = bit_count - 1, j = BIT - 1 - range_start; i >= 0; i--, j--) {
        if (j < stop) {
            output_bits[i] = '0';
        }
        else {
            output_bits[i] = bits[j];
        }
    }
    output_bits[bit_count] = '\0';
}

void print_all(char *output_bits, int bit_count) {
    print_binary(output_bits, bit_count);
    print_hexadecimal_and_unsigned(output_bits);
    print_signed(output_bits);
}

void print_binary(char *output_bits, int bit_count) {
    int i, counter;

    // 4 chunks binary
    for (i = 0, counter = 1; i < bit_count; i++, counter++) {
        printf("%c", output_bits[i]);
        if (counter % 4 == 0) {
            printf(" ");
            counter = 0;
        }
    }
    printf("(base 2)\n");

    // normal binary
    printf("0b%s (base 2)\n", output_bits);
}

void print_hexadecimal_and_unsigned(char *bits) {
    unsigned int unsigned_decimal;
    unsigned_decimal = calculate_decimal(bits);

    print_hexadecimal(unsigned_decimal);
    printf("%u (base 10 unsigned)\n", unsigned_decimal);
}

void print_hexadecimal(unsigned int unsigned_decimal) {
    int i, temp, len = BIT / 4;
    char hex[len + 1];

    for (i = len - 1; i >= 0; i--) {
        temp = unsigned_decimal % 16;
        hex[i] = (temp >= 10) ? ('A' + temp - 10) : ('0' + temp);
        unsigned_decimal /= 16;
    }
    hex[len] = '\0';
    
    printf("0x");
    printf("%s (base 16)\n", hex);
}

void print_signed(char *bits) {
    int decimal;
    bool negative = false;

    if (bits[0] == '1') {
        to_negative(bits);
        negative = true;
    }

    decimal = (int) calculate_decimal(bits) * (negative ? -1 : 1);

    printf("%d (base 10 signed)\n", decimal);
}

unsigned int calculate_decimal(char *bits) {
    unsigned int temp = 0;
    int i;
    
    for (i = 0; i < BIT; i++) {
        temp = temp * 2 + (bits[i] == '1' ? 1 : 0);
    }

    return temp;
}
