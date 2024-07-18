#pragma once
#include <stdexcept>

using RuntimeErr = std::runtime_error;

static const RuntimeErr WRONG_FMT = RuntimeErr("Error: incorrect command format");
static const RuntimeErr NOT_IDENT = RuntimeErr("Error: expected identifier");
static const RuntimeErr NOT_FOUND = RuntimeErr("Error: not found in store");
static const RuntimeErr VAL_AFTER_IDENT
    = RuntimeErr("Error: expected value after identifier");
static const RuntimeErr CIRCULAR_REF = RuntimeErr("Error: circular reference detected");

inline RuntimeErr MIN_ONE_ARG(const std::string &c) {
    return RuntimeErr("Error: " + c + " requires at least one argument (key)");
}

inline RuntimeErr MIN_TWO_ARG(const std::string &c) {
    return RuntimeErr("Error: " + c + " requires at least two arguments (key value)");
}
