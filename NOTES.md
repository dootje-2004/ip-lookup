# Testing C code with Google test

## References

Simple setup: <https://terminalroot.com/how-to-install-and-get-started-with-google-test/>

Somewhat more advanced: <https://notes.eatonphil.com/unit-testing-c-code-with-gtest.html>

## Installation

`sudo apt-get install googletest libgtest-dev`

## Usage

Filter tests with `build/test/ip-test --gtest_filter=HelperSuite.*`.

Stop at first fail with `build/test/ip-test --gtest_break_on_failure`.

## CMake kits

See [this post](https://mcuoneclipse.com/2023/11/05/no-kit-selected-fixing-vs-code-cmake-kit-assignment/).

## The ARPA_INET library

Contains functions for manipulating IP addresses, potentially replacing much of our code.
Note that its internal format is very different from ours.

AF_INET6 appears to maintain word order, but uses LSB-first within a word.
Thus, byte order is 2-1-4-3-6-5-...

## White-box testing

See [this post](https://www.exchangetuts.com/unit-testing-c-with-functions-not-in-header-1640858345575752).

TODO: Not all helper functions have tests yet.
