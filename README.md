Chess engine library
====================

This is a library that contains everything necessary to build a Chess AI (see
[chess_uci](https://github.com/fjswaugh/chess_uci) for an example).

The main functionality is found in `evaluate.h`.

Building
--------

The library definitely builds on Linux, and with some adjustment should be cross-platform.

Building consists of *two* stages:
1. `make lookup_tables`
2. `make`

The second command will produce a `libchess.a` file in the `lib` directory.

To build and run the tests, use `make test`. These use Boost's unit test framework.
