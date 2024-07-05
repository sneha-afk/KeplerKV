# planning

---

## purpose

A NoSQL key-value pair store for quick access.
* A sister project to [PlutoDB](https://github.com/sneha-afk/PlutoDB) to focus on developing a lexer-parser-evaluator system.

## structure

* `class Handler`: processes and executes commands to the store
    * `class Lexer`: lexical analyzer for commands
    * `class Parser`: parses semantic meaning out of the tokens generated from the lexer
    * Execution and validation done within `Handler`: may consider exporting this to a class if too unwiedly

## goals

### overall
- [ ] Build a parser system to implement a user CLI
- [ ] Manage key-values in efficent data structures
- [ ] Serialization/deserialization from file
- [ ] Allow for data manipulation to supported types

### reach
- [ ] Allow for folders/indices to group together keys
- [ ] User credentials and encryption of data
- [ ] Concurrency

### baseline
- [ ] Inserting and deleting key-values
- [ ] Validate user input and operations

---
## dev journal

### July 4, 2024

* Continued work on the parser, which takes in tokens from `Lexer` and outputs `SyntaxNode` structures
    * Finished primitives (numbers, identifiers, strings), lists, and commands
    * Mapping the command token to the appropriate `CommandType` to be evaluated
* Using smart pointers to use dynamic memory safely
* Next steps:
    * Evaluator to enforce syntax rules and command specifications
    * Planning for the store: how to bridge the gap betweent the evaluator and store itself

### July 1, 2024

* Done with the first iteration of the lexer, moving onto the parser
* [Parse tree](https://en.wikipedia.org/wiki/Parse_tree), related to CSE103!
    * Need to build a structure from the lexer's tokens
* Commands: have one keyword in the beginning, then a list of arguments
* Primitive values: simple need to typecast the value
* List values: denoted with LIST_START and LIST_END
    * Turn on an "inside list" flag

### June 30, 2024

* Starting the lexer and planning out what command syntax to support
* Identifiers: starts with a letter or underscore
    * Should only contain letters, numbers, and underscores
    * No special characters
* `\SET, \set, \s key value [key value]*`: set a key value pair(s)
    * Support basic primitives (numbers and strings) and lists
    * To ease the interpretation of identifiers and values, strings must be surrounded by quotes
        * Inserting a string with quotes? Use the opposite quote outside
            * e.g. "i said 'hi'" -> i said 'hi', and 'she said "bye"' -> she said "bye"
    * Lists are denoted with [] and elements are comma delimited
        * Allows for differently typed list elements
* `\DEL, \del, \d key`: delete a key
    * No effect if the key does not exist in the store
* `\UPDATE, \upd, \u key new_value`: updates the value of a key
    * `new_value` can have a different data type from the previous value
* `\LIST, \ls`: list all key-values currently stored
* `\HELP, \h`: help message

### June 28, 2024

A key-value store is far less coupled than a relational database and I intend to focus my efforts in learning how to make an effective CLI that is easy to expand and interpret.

## resources used

1. [How to Write a Programming Language](https://accu.org/journals/overload/26/145/balaam_2510/) series by Andy Balaam
2. [TosLang](https://faouellet.github.io/categories/toslang/) series by Félix-Antoine Ouellet
