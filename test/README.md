# TRIC self tests

The correct functionality of TRIC is tested with a number of tests in the files tric_test.c and tric_assert_test.c. These tests are written directly in C using only the assert() macro. Compiling the tests and running the resulting executables should produce no output (i.e. they return 0). Otherwise the execution is aborted and a message describing the failing assert() is displayed.

Compiling and running the tests can be automated by running make. The test executables are generated in the build directory and run automatically. Executing the tests with make outputs messages if all tests were successful.



