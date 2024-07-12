# keplerKV manual

The full manual for KeplerKV including syntax and command specifics.

Last updated: 2024-07-11

## Contents

1. [General syntax](#general-syntax)
2. [QUIT](#QUIT): quitting the program.
3. [SET](#set): setting a key
4. [GET](#get): retrieving a key
5. [DEL](#del): deleting a key
6. [UPDATE](#update): updating a key
7. [LIST](#list): lists out all values in the store currently

## General syntax

Commands are **case-insentive** and may have alternate syntax, see the specific command for shorthands.

All tokens are assumed space-delimited and whitespace is ignored.

---

Optional comma delimiters can be used for clarity, be sure to separate distinct tokens by **spaces** when required. For example, the following examples show equivalent commands:
```bash
\set apple 1 banana 2
\set apple 1, banana 2
\set apple,1 banana,2
```

```bash
\get apple banana
\get apple, banana
```
---

Multiple commands can be completed at once by separating them with semicolons:
```bash
\set a 1 ; \get a
    OK              # confirmation of \set
    a | int: 1      # output of \get
```

## QUIT

**`{\q, \quit}`**

Quit the KeplerKV program.

## SET

**`{\set, \s} key value [k2 v2 k3 v3 ...]`**

Set a key-value pair to the store.

### Supported types
1. Integers
    ```bash
    \set num 1
    ```
2. Floats
    ```bash
    \set num 1.2
    ```
3. Strings
    ```bash
    \set str "hello world!"
    ```

    **Strings must be denoted with starting and closing quotation single or double quotation marks.**

    To have a quotation mark inside of a string, it must be of the opposite kind.
    ```bash
    \set str 'look its a "string", nice'
    ```
4. Lists
    ```bash
    \set list [1, 2, 3]
    ```

    Multidimensional lists are supported.
    ```bash
    \set matrix [[1, 2, 3], [4, 5, 6], [7, 8, 9]]
    ```

## GET

**`{\get, \g} key [k2 k3 ...]`**

Get the value of a key in the store.

```bash
\set num 1
\get num
    num | int: 1
```

If the key is not present in the store, `NOT FOUND` is returned.

## DEL

**`{\del, \delete \d} key [k2 k3 ...]`**

Delete a key from the store.
```bash
\set num 1
\del num
    OK
```

If the key is not present in the store, `NOT FOUND` is returned and no effect is done to the store.


## UPDATE

**`{\update, \u} key value [k2 v2 k3 v3 ...]`**

Update the value of a key that is **present** in the store.

Keys must be set in the store to some initial value before they can be updated. The new value can be of a different data type than the old, **keys are not held to a specific type during their lifetime!**.

```bash
\set id 123
\update id "an actual name!"
```

## LIST

**`{\list, \ls, \l}`**

List out all items that are currently in the store.

```bash
\set a 1, b "abc", c [1, 2, 3]
\list
    c | list: [int: 1, int: 2, int: 3]
    b | str: "abc"
    a | int: 1
```