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

inline RuntimeErr MIN_ONE_ARG(const std::string &c) {
    return RuntimeErr("Error: " + c + " requires at least one argument (key)");
}

inline RuntimeErr MIN_TWO_ARG(const std::string &c) {
    return RuntimeErr("Error: " + c + " requires at least two arguments (key value)");
}

inline RuntimeErr INVALID_CMD(const std::string &c) {
    return RuntimeErr("Error: invalid command \'" + c + "\'");
}

inline RuntimeErr UNKNOWN_TOKEN(const std::string &t) {
    return RuntimeErr("Error: unknown token \'" + t + "\'");
}
