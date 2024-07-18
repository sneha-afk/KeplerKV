#pragma once
#include <stdexcept>

static const std::runtime_error WRONG_FMT
    = std::runtime_error("Error: incorrect command format");
static const std::runtime_error NOT_IDENT
    = std::runtime_error("Error: expected identifier");
static const std::runtime_error NOT_FOUND
    = std::runtime_error("Error: not found in store");
static const std::runtime_error VAL_AFTER_IDENT = std::runtime_error(
                            "Error: expected value after identifier");

static const std::runtime_error SET_NUM_ARGS = std::runtime_error(
                        "Error: SET requires at least two arguments (key value)");
static const std::runtime_error GET_NUM_ARGS = std::runtime_error(
                        "Error: GET requires at least one argument (key)");
static const std::runtime_error DEL_NUM_ARGS = std::runtime_error(
                        "Error: DELETE requires at least one argument (key)");
static const std::runtime_error UPD_NUM_ARGS = std::runtime_error(
                        "Error: UPDATE requires at least two arguments (key value)");
