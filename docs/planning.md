# planning

---

## purpose

A NoSQL key-value pair store for quick access.
* A sister project to [PlutoDB](https://github.com/sneha-afk/PlutoDB) to focus on developing a lexer-parser-evaluator system.

## structure

* [`class Handler`](/include/handler.h): processes and executes commands to the store
    * [`class Lexer`](/include/lexer.h): lexical analyzer for commands
        * [`class Token`](/include/token.h): container of a string token read and its inferred type (i.e command, value, unknown)
    * [`class Parser`](/include/parser.h): parses semantic meaning out of the tokens generated from the lexer
        * [`class ASTNode`](/include/syntax_tree.h): represents a `Token`'s meaning and any contained data
    * Execution and validation done within `Handler`: may consider exporting this to a class if too unwiedly
* [`class Store`](/include/store.h): in-memory representation of the store
    * [`class StoreValue`](/include/store_value.h): base class representing a value within the store, from which specific types inherit from, such as `IntValue`

### procedure for adding new commands
1. Register a new value in the `CommandType` enum in [`syntax_tree.h`](/include/syntax_tree.h)
2. Add the corresponding tokens mapped to the enum in `mapToCmd` in that file
3. Add to the `CommandToFunction` map in [`handler.cpp`](/src/handler.cpp) if the new command will need a larger handler function, else it can be added to the `switch` statement in the main `handleQuery()` function
4. Implement the necessary functionality within `Store` if applicable

## goals

### overall
- [X] Build a parser system to implement a user CLI
- [X] Serialization/deserialization from file
- [X] Allow for data manipulation to supported types

### baseline
- [X] Inserting and deleting key-values
- [X] Validate user input and operations

### feature ideas
- [ ] Allow for transactions, commits, rollbacks
- [ ] API to a common language?
- [ ] Allow for folders/indices to group together keys
- [ ] User credentials and encryption of data
- [ ] Cache system for larger amounts of keys
- [ ] Concurrency

---

## current limitations
* No nested commands allowed, i.e `\SET a \GET b + 1`
* No commands allowed within a list definition, i.e `\SET l [\GET a]
* Arithmetic expressions are not allowed when setting numbers

## feature list
* Expandable parser system
    * For data: SET, GET, RESOLVE, DELETE, UPDATE, SEARCH
    * For manipulation: INCR, DECR, APPEND, PREPEND
    * Other: LIST, CLEAR, SAVE, LOAD, STATS
* Most commands can be batched, i.e setting multiple keys at once
* Data types supported: integers, floats, strings, hetereogenous and multidimensional lists
* Saving and loading state from a `.kep` file
* Cycle detection in RESOLVE to prevent infinite loops
    * RESOLVE will also recursively find identifiers to resolve within lists if requested to
* Data types can be changed if UDPATE is used with a different type
* Data manipulation for certain types
    * Integers, floats: INCR, DECR
    * Lists: APPEND, PREPEND

## sources
- [How to Write a Programming Language](https://accu.org/journals/overload/26/145/balaam_2510/) series by Andy Balaam
- [TosLang](https://faouellet.github.io/categories/toslang/) series by Félix-Antoine Ouellet
- [Wiki: Abstract syntax trees](https://en.wikipedia.org/wiki/Abstract_syntax_tree)
- File I/O
    - [GH Gist by molpopgen](https://gist.github.com/molpopgen/9123133)
    - [UMich - binary file I/O](https://www.eecs.umich.edu/courses/eecs380/HANDOUTS/cppBinaryFileIO-2.html)
- StackOverflow
    - [Using `sed` to remove ANSI escape sequences](https://superuser.com/a/380778)
        - [List of color ANSI escape sequences](https://stackoverflow.com/q/5947742)
    - [Using `std::dynamic_pointer_cast` on `std::shared_ptr`](https://stackoverflow.com/a/14545746)
    - [Virtual destructors in inheritance](https://stackoverflow.com/q/461203)
    - [Making custom classes iterable](https://stackoverflow.com/a/46432381)

---
## dev journal
### September 7, 2024
- Bugs found and fixed:
    - Lone quotes can cause segfaults: not checking the iterator when appending the end quote (or, lack thereof)
    - `removeQuotations()` had similar quote finding issues (and was wrong on cases like `'"'`)
- Downgraded to standard 11 from 17 since `std::variant` is gone
- Lowered some heap usage:
    - `Lexer` didn't need to store the input
    - `const &`, love them
    - Going forward, being more mindful on strings
- `inline`d smaller functions, especially the returning enum ones
- Moved macros to more sensible locations
- Lots of minor code quallity issues
- Up next: learning more about `std::move` and move constructors, potentially very beneficial for a program so reliant on passing strings through a pipeline!

### September 6, 2024
- Reworked serialization and deserialization for individual `StoreValue` types
    - Strings and identifiers utilize the same string functionality: identifiers will now inherit from string classes (enums distinguish them)
- New `evaluate()` functions for each `ASTNode` to create appropriate `StoreValue` type
    - More clear, forward conversion from the parser to actual values
- Revamped `Parser` to not return anything related to `StoreValue`
    - `Parser` only deals with the `ASTNode` representation of a value
- Let `Identifier` abstractions (AST and Value) inherit from `String` since they are functionally the same and an enum is used to distinguish them during use
- Corresponding fixes to `Handler` and `Store` for the new scheme

### September 5, 2024
The current system using `std::variant` for the `StoreValue` and generic `ValueNode`s introduces a significant overhead within the program and bloated lines of code for type-checking and reducing the true type of a value.

Adding more types and operations will only add onto this bloat, so I believe the next course of action would be implementing the type erasure paradigm and going back to more rigid type specifications.
1. Make `ValueNode` a base class that specific types need to inherit from.
2. Similarly, make `StoreValue` a base class.

### August 29, 2024
- Implemented `SEARCH` that allows for searching by regex patterns
    - An $O(n)$ operation, oh dear
- Should start looking into better ways of structuring this data

### August 18, 2024
Launching version 1.0!
* Added `STATS` to display some basic statistics on how many keys, how much memory usage, etc.

### August 17, 2024
* `RESOLVE` will now iterate over base lists and resolve any identifiers present within them
    * Added a default `false` option for resolving nested lists since this returns a copy of the full resolution to display
    * Effect: I anticipate resolving very nested identifiers may be slow when done to the extreme!

### August 15, 2024
* Implementing new commands to mutate the keys
* `INCR` and `DECR` for integers and floats: throws errors on other types
    * Also works if invoked on an alias to an numeric value!
    * Using `RESOLVE` internally
* `APPEND` and `PREPEND` for lists

### August 10, 2024

* Added a new command `RENAME` to renae keys to new names
* Added documentation and a test file

### August 8, 2024

* Created `tests/execute_all.sh` which takes input and expected output files as test cases
* Refactored `handler.cpp` to use a function map instead of an unweidly `switch` statement for longer command processes
* Next steps:
    * Aforementioned code review
    * Add some small utility commands: `RENAME`, `STATS`
    * Data manipulation: perhaps starting with incrementing decrementing integers

### July 29, 2024

* Introduced the `CLEAR` method similar to Linux's `clear`
* Reduced the number of delimiters in the save files to save some bytes
* Next steps:
    * Formal testing (most likely through bash scripts)
    * Review codebase and improve on some redundancy
    * Plan on the data manipulation goal

### July 28, 2024

* Worked on deserialization, currently working for basic types, lists not fully tested
* Considering reducing the number of delimiters to be more space-efficient
* More extensive testing mechanisms would be beneficial
* `.kep` file extension to mark which files are Kepler saves

### July 27, 2024

* Designed a preliminary serialization scheme for saving
* Encoding into bytes to be more efficient
* Datatype indicator is written first, followed by the data or other information (such as size)

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
