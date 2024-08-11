#pragma once
#include <stdexcept>

using RuntimeErr = std::runtime_error;

#define WRONG_FMT       "Error: incorrect command format"
#define NOT_IDENT       "Error: expected identifier"
#define NOT_FOUND       "Error: not found in store"
#define VAL_AFTER_IDENT "Error: expected value after identifier"
#define CIRCULAR_REF    "Error: circular reference detected"
#define SV_WRONG_TYPE   "Error: wrong type getter used with this value"
#define NESTED_CMD      "Error: nested commands not supported (yet?)"
#define CMD_IN_LIST     "Error: commands not supported within lists"
#define FAIL_OPEN_WRITE "Error: failed to open file to write"
#define FAIL_OPEN_READ  "Error: failed to open file to read (check if it exists!)"
#define NOT_VALID_SAVE  "Error: not a valid KEPLER-SAVE file"
#define UNK_SAVE_ITEM   "Error: unknown item type found in save file"
#define INVALID_FNAME   "Error: invalid filename, must be valid string or identifier"

inline RuntimeErr MIN_ONE_ARG_K(const std::string &c) {
    return RuntimeErr("Error: " + c + " requires at least one argument (key)");
}

inline RuntimeErr MIN_TWO_ARG_KV(const std::string &c) {
    return RuntimeErr("Error: " + c + " requires at least two arguments (key value)");
}

inline RuntimeErr MIN_TWO_ARG_KK(const std::string &c) {
    return RuntimeErr("Error: " + c + " requires at least two arguments (key key)");
}

inline RuntimeErr INVALID_CMD(const std::string &c) {
    return RuntimeErr("Error: invalid command \'" + c + "\' (did you forget an '\'?)");
}

inline RuntimeErr UNKNOWN_TOKEN(const std::string &t) {
    return RuntimeErr("Error: unknown token \'" + t + "\'");
}
