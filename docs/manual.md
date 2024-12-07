# keplerKV manual

The full manual for KeplerKV including syntax and command specifics.

Last updated: 2024-09-06

## Contents

- [General syntax](#general-syntax)
    - [Tokenization](#tokenization)
    - [Command syntax](#command-syntax)
    - [Identifiers](#identifiers)
- Commands: [System](#commands-system)
    - [QUIT](#quit): quitting the program.
    - [CLEAR](#clear): clearing the screen
    - [SAVE](#save): save the store into file
        - [Valid filenames](#valid-filenames)
    - [LOAD](#load): load a store from file
    - [STATS](#stats): gives basic statistics
- Commands: [Data](#commands-data)
    - [SET](#set): setting a key
        - [Supported datatypes](#supported-types)
    - [GET](#get): retrieving a key
    - [DEL](#del): deleting a key
    - [UPDATE](#update): updating a key
    - [LIST](#list): lists out all values in the store currently
    - [RESOLVE](#resolve): resolve nested/recursive references
        - [Explanation of lazy evaluation](#lazy-evaluation)
    - [RENAME](#rename): rename a key
    - [SEARCH](#search): use regex to search keys
- Commands: [Data Manipulation](#commands-data-manipulation)
    - [INCR](#incr): increment a numeric key
    - [DECR](#decr): decrement a numeric key
    - [APPEND](#append): append to a list
    - [PREPEND](#prepend): prepend to a list

## General syntax

### Tokenization

All tokens are assumed space-delimited and whitespace is ignored.

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

Multiple commands can be completed at once by separating them with semicolons:
```bash
\set a 1 ; \get a
    OK              # confirmation of \set
    a | int: 1      # output of \get
```

---

### Command syntax

Commands are **case-insensitive** and may have alternate syntax. This documentation denotes commands and their arguments as:
```
{set of equivalent invocations} required-argument [optional-arguments]
```

Many commands allow for batching such that multiple of the same command can be invoked:
```bash
\get a b
    a | ...
    b | ...
```

### Identifiers

Identifiers are **case-sensitive** and must begin with an alphabetical character or an underscore, with any alphanumeric or underscore characters permitted afterwards.

#### Examples of allowed identifiers
|              |            |
|:------------:|:----------:|
| `_name`      |   |
| `Age`        | `age` (different from `Age`!)  |
| `_patient0`  |   |

#### Examples of disallowed identifiers
|            |                                                          |
|:----------:|----------------------------------------------------------|
| `0patient` | Cannot start with a number                               |
| `hash#`    | Only alphanumeric characters and underscores are allowed |

## Commands: System 
### QUIT

**`{\q, \quit}`**

Quit the KeplerKV program.

### CLEAR

**`\clear`**

Clear the terminal screen, returning to the `>` prompt.

### SAVE

**`\save [filename]`**

Save the current state of the store into a save file of **`.kep`** extension.

```bash
\set a 1
\save manual
    SAVED
```

#### Valid filenames

Filenames follow the same rules as strings (surrounded by quotes) and [identifiers](#identifiers). If you have spaces or special characters within a filename, it is safer to put quotes around the name.

> Verify what special characters are permitted in your file system.

### LOAD

**`\load [filename]`**

Load in a store state from a valid save file produced from [`SAVE`](#save), denoted by the **`.kep`** extension.

```bash
\load manual
    LOADED
```

### STATS

**`\stats`**

Displays basic statistics about the current instance of KeplerKV, including the total number of keys by type, and the memory usage.

## Commands: Data

### SET

**`{\set, \s} key value [k2 v2 k3 v3 ...]`**

Set a key-value pair to the store.

#### Supported types
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

    To have a quotation mark inside of a string, the inner must be of the opposite kind of the outer.
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

### GET

**`{\get, \g} key [k2 k3 ...]`**

Get the value of a key in the store.

```bash
\set num 1
\get num
    num | int: 1
```

If the key is not present in the store, `NOT FOUND` is returned.

### DEL

**`{\del, \delete \d} key [k2 k3 ...]`**

Delete a key from the store if it is present.
```bash
\set num 1
\del num
    OK
```

If the key is not present in the store, `NOT FOUND` is returned and no effect is done to the store.


### UPDATE

**`{\update, \u} key value [k2 v2 k3 v3 ...]`**

Update the value of a key that is **present** in the store.

Keys must be set in the store to some initial value before they can be updated. The new value can be of a different data type than the old, **keys are not held to a specific type during their lifetime!**

```bash
\set id 123
\update id "an actual name!"
```

### LIST

**`{\list, \ls, \l}`**

List out all items that are currently in the store.

```bash
\set a 1, b "abc", c [1, 2, 3]
\list
    c | list: [int: 1, int: 2, int: 3]
    b | str: "abc"
    a | int: 1
```

### RESOLVE

**`{\resolve, \res, \r} key [k2 k3 ...]`**

Resolve the values of keys that reference another key (or form a key-chain!). **`RESOLVE` works identically to `GET` when keys are not recursive.**

#### Lazy evaluation
Recursive keys are **lazily evaluated**, which means the value of the key being referenced need not be defined until resolution is required.
```bash
\set a b        # a -> b
\get a
    a | id: b   # a references b
\resolve a
    NOT FOUND   # Since b is not defined, no value is found when attempting to resolve
\set b 1
\resolve a
    a | int: 1  # a can be resolved to b, which resolves to an integer
\resolve b
    b | int: 1  # b is not recursive and resolves to its primitive value (same output as GET)
```

In short: `GET` only evaluates one level of the key-value pair, while `RESOLVE` evaluates until a primitive is found.

Similar to C++ references, affecting `b` also affects `a` since `a` is an alias for `b`.
```bash
\update b 2
\resolve b
    b | int: 2
\resolve a
    a | int: 2
```

#### Data manipulation

If manipulation commands such as `[INCR](#incr)` are used upon an alias, the action will affect **the alias and the referenced keys**.

```
\set a 1
\set b a        # b -> a
\incr b
\get a
    a | int: 2
\resolve b
    b | int: 2
```

### RENAME

**`{\rename, \rn} oldKeyName newKeyName [o2 k2 ...]`**

Renames a value's key.

```bash
\set a 1
    a | int: 1
\rename a b
    b | int: 1
```

**Note: if the new key name already exists in the store, you will be asked to confirm if that key should be overwritten with the data from the old key name.**

### SEARCH

**`\search regex1 [r2 r3 ...]`**

Searches for keys using typical [C++ regex](https://en.cppreference.com/w/cpp/regex) rules. For the least buggy experience, format the regex argument as a string with quotes around it.

```bash
\set a 1
\set b 2
\search "."
    . (2)
    b
    a
```

## Commands: Data Manipulation
### INCR

**`\incr key [k2 k3 ...]`**

Increment a numeric (integer or float) key. Throws an error if invoked on other types.

```bash
\set a 1
    a | int: 1
\incr a
    a | int: 2
```

### DECR

**`\decr key [k2 k3 ...]`**

Decrement a numeric (integer or float) key. Throws an error if invoked on other types.

```bash
\set a 1
    a | int: 1
\decr a
    a | int: 0
```

### APPEND

**`\append key value [v2 v3 ...]`**

Append elements to a list. Throws an error if invoked on other types

```bash
\set a [1]
    a | list: [int: 1]
\append a 2
    a | list: [int: 1, int: 2]
```

### PREPEND

**`\prepend key value [v2 v3 ...]`**

Prepend elements to a list. Throws an error if invoked on other types

```bash
\set a [1]
    a | list: [int: 1]
\prepend a 2
    a | list: [int: 2, int: 1]
```
