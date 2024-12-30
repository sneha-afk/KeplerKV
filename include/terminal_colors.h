#pragma once

#define T_RESET  "\033[0m"
#define T_BRED   "\033[1;31m"
#define T_BBLUE  "\033[1;34m"
#define T_BGREEN "\033[1;32m"
#define T_BYLLW  "\033[1;33m"

#define PRINT_RED(s)    T_BRED s T_RESET
#define PRINT_BLUE(s)   T_BBLUE s T_RESET
#define PRINT_GREEN(s)  T_BGREEN s T_RESET
#define PRINT_YELLOW(s) T_BYLLW s T_RESET

#define CLEAR_SCREEN "\033[H\033[2J"
