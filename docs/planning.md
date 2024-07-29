# planning

---

## purpose

A NoSQL key-value pair store for quick access.
* A sister project to [PlutoDB](https://github.com/sneha-afk/PlutoDB) to focus on developing a lexer-parser-evaluator system.

## structure

* `class Handler`: processes and executes commands to the store
    * `class Lexer`: lexical analyzer for commands
        * `class Token`: container of a string token read and its inferred type (i.e command, value, unknown)
    * `class Parser`: parses semantic meaning out of the tokens generated from the lexer
        * `class ASTNode`: represents a `Token`'s meaning and any contained data
    * Execution and validation done within `Handler`: may consider exporting this to a class if too unwiedly
* `class Store`: in-memory representation of the store
    * `class StoreValue`: variant type to hold different types of data

## goals

### overall
- [X] Build a parser system to implement a user CLI
- [ ] Manage key-values in efficent data structures
- [ ] Serialization/deserialization from file
- [ ] Allow for data manipulation to supported types

### reach
- [ ] Allow for folders/indices to group together keys
- [ ] User credentials and encryption of data
- [ ] Concurrency

### baseline
- [X] Inserting and deleting key-values
- [X] Validate user input and operations

---

## resources used

1. [How to Write a Programming Language](https://accu.org/journals/overload/26/145/balaam_2510/) series by Andy Balaam
2. [TosLang](https://faouellet.github.io/categories/toslang/) series by Félix-Antoine Ouellet

---
## dev journal

### July 28, 2024

* Working on deserialization
    * Reducing the delimiters would be more helpful (and less space)
    * Only a few instances are of variable type

### July 27, 2024

* Binary I/O to save into a file
    * Type of the data is written first to know if any size considerations need to be made
    * Lists start with a < and end with a >, might not need this later
* 

### July 26, 2024

* Added a manual `Makefile`, for the few out there who would appreciate an easier manual build process :)

### July 18, 2024

* `LIST` for listing out all values that are currently in the store
* Added a new command called `RESOLVE` which allows for keys to be references to other keys
    * `RESOLVE` is a recursive `GET` call until a primitive is found
    * Reach: should `RESOLVE` handle references that are in list primitives?

### July 10, 2024

* Finished `Handler` on the four basic command types
* Fixed the parsing issues mentioned yesterday
* Next steps:
    * Work on recursive identifier handling
        * Thinking about using lazy evaluation for these
        * Can set a currently unbinded identifier, but must be specified before doing a `GET`
    * Start plannning serialization to file

### July 9, 2024

I had been working on this incrementally and continously getting stuck on how to effectively parse so many types of `ValueNodes`. I took a step back and read more about abstract syntax trees and decided a more "flat" approach to parsing since I am only handling very primiive types. After doing so, I had to delve into `std::variant` to make operations easier on the store. This coupling between a `StoreValue` type and its containing `ValueNode` type is hopefully as seamless as I can make it for now.
* Next steps:
    * Debug parsing issues:
        * Segmentation fault on improperly formed lists, should instead throw an error (or infer end of list scope)
        * Floating type values are not being parsed correctly
    * Complete the implemenation of `Handler` which interacts with the store

### July 5, 2024

* Worked on finishing up the `handleQuery` method
* Made a barebones header for the store
* Smaller refactorings for the syntax nodes to identify values more easily

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
