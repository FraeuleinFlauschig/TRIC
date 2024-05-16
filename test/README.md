# TRIC self tests

The correct functionality of TRIC is tested with a number of self tests. These tests are written directly in C using only the assert() macro. Compiling tric_test.c and running the resulting executable should produce no output (i.e. it returns 0). Otherwise the execution is aborted and a message describing the failing assert() is displayed.

Compiling and running the tests can be automated by running make. The test executable is generated in the build directory and runs automatically. Executing the tests with make outputs a message if all tests were successful.



