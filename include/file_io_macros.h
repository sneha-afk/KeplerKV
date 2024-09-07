#pragma once

#include <string>

static const std::string FILE_HEADER = "KEPLERKV-SAVE|";
static const int FILE_HEADER_SIZE = (int) FILE_HEADER.size();
static std::string DEFAULT_SAVE_FILE = "default_kep_save";

static const char DELIMITER = '|';
#define WRITE_DELIM    write(&DELIMITER, sizeof(char))
#define WRITE_CHAR(c)  write(&c, sizeof(char))
#define MV_FP_FORWARD  seekg(1, std::ios::cur)
#define MV_FP_BACKWARD seekg(-1, std::ios::cur)
