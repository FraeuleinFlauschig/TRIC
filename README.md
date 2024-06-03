<!-- start README with comment to help doxygen find the first heading -->

# TRIC - Testing Rules In C

TRIC is a minimalistic unit testing framework for c. It has no external dependencies and is all contained in a single header file. Tests are automatically detected and are executed in separate processes. TRIC features a flexible mechanism for defining test fixtures and supports reporting test results in various formats.



# Features

+ TRIC is contained in a single C header file  requiring only a few POSIX libraries.
+ Minimalistic API consisting of not much more than a few macros.
+ No main function is needed: Including the header tric.h and compiling the tests will produce a ready to run executable.
+ Tests are automatically detected: No need to manually add tests to an execution list, processing the tests with an external tool or the like.
+ Each test is executed in an isolated process.
+ Multiple and flexible options to define test fixtures.
+ Simple mechanism to ignore tests.
+ Collection of additional assertions in the supplementary header tric_assert.h.
+ Simple builtin uncluttered reporting of the test results.
+ Straightforward mechanism to implement a custom test result reporting.
+ Support for various other output formats like TAP, CSV or JSON in the additional header tric_output.h.



# Introduction

TRIC consists of a single C header file and requires only standard C libraries. To use TRIC the header tric.h needs to be  copied into a location where the compiler can find it (either globally or next to the tests source files).

In TRIC tests are contained in a test suite. When compiled and executed, a test suite detects and runs the contained tests automatically. There must be exactly one test suite per executable and all tests must be inside the test suite.

A test suite is created with the SUITE macro. The first argument of the SUITE macro is a string describing the test suite and should not be omitted. The rest of the arguments are used for the setup and teardown fixture of the test suite and may be NULL.

Any number of tests can be created inside the test suite with the TEST macro. The TEST macro has the same parameters as the SUITE macro. The description for the test should also not be omitted, while the arguments for the setup and teardown fixture of the test may be NULL.

Inside a test the test conditions can be defined by using the ASSERT macro. If the expression passed as an argument to the ASSERT macro evaluates to false, the test will fail. Any number of ASSERT macros can be placed inside a test, the first failing ASSERT however will stop the test immediately.

Each test of a test suite runs in an isolated child process. If a test crashes (e.g. by dereferencing a null pointer), the test will fail and the child process is terminated. The execution of the test suite continues even if one or multiple tests crash.

If a test needs to be ignored during the execution of the test suite, the SKIP_TEST macro can be used. This macro has the same signature as the TEST macro. Therefore an existing test can be ignored by just prefixing the TEST macro with "SKIP_". If a test is ignored with the SKIP_TEST macro, it is still detected by the test suite during execution and the skipping of the test is mentioned when the test results are reported.



## Basic example test suite

The following example code shows the basic usage of TRIC.

```
#include "tric.h"

SUITE("tric basics", NULL, NULL, NULL) {

    TEST("a successful test", NULL, NULL, NULL) {
        ASSERT(0 == 0);
    }

    TEST("a failing test", NULL, NULL, NULL) {
        ASSERT(0 < 0);
    }

    TEST("a crashing test", NULL, NULL, NULL) {
        int *null_pointer = NULL;
        ASSERT(*null_pointer == 0);
    }

    SKIP_TEST("an ignored test", NULL, NULL, NULL) {
        ASSERT(0 > 0);
    }

}
```

No main function needs to be written and no further processing is needed. Compiling the above code will create a ready to run executable. Running the executable produces the following output:

```
test suite "tric basics" (4 tests found):

test 2 of 4 ("a failing test") failed at line 10
test 3 of 4 ("a crashing test") crashed with signal 11

3 tests executed, 2 failed, 1 skipped, 4 total
```



## How it works

To understand how TRIC works consider the following simple test suite. Blank lines are inserted for illustration purpose.

```
#include "tric.h"

SUITE("simple suite", NULL, NULL, NULL) {

    TEST("successful test", NULL, NULL, NULL) {

        ASSERT(0 == 0);
    }
}
```

When compiled, the preprocessor translates the above example to something similar to the following code. The code below is greatly simplified and shows only the basic concept of TRIC.

```
void test_suite(void);
int main(int argc, char *argv[]) {
    test_suite();
}

void test_suite(void) {

    pid_t child = fork();
    if (child) {
        waitpid(child);
    } else {

        if ( (0 == 0) != true ) {
            exit(EXIT_FAILURE);
        }
    }
}
```

The header tric.h declares the test_suite function and inserts the main function which then calls the test_suite function. The SUITE macro defines the test_suite function up to the parameter list.

Inside the body of the test_suite function the TEST macro inserts the fork() call and the subsequent if statement up to the else statement. Inside the body of the else statement, the ASSERT macro inserts the test condition.

When the test_suite function is executed, a child process is forked for each test in the test suite. The parent process waits for the completion of each child process before forking a new child. Inside a child process is the actual test executed. If a test condition of the test (inserted by an ASSERT macro) fails, the child process is terminated with an error code. If no test condition fails, the child process terminates normally.



# Assertions

TRIC itself has only a basic builtin ASSERT macro to verify test conditions. To simplify the writing of test conditions and to make the tests more readable, the header tric_assert.h can be included in addition to the header tric.h. This header contains a collection of assertion macros that internally use the ASSERT macro of TRIC. In order to use these macros, tric.h must be included before tric_assert.h can be included. Otherwise a compiler error will be generated.

The following example test suite illustrates the usage of some of the assertions in tric_assert.h.

```
#include "tric.h"
#include "tric_assert.h"

SUITE("to illustrate assertions", NULL, NULL, NULL) {

    TEST("some simple assertions", NULL, NULL, NULL) {
        int one = 1;
        ASSERT_NOT(one < 0);
        ASSERT_TRUE(one > 0);
        ASSERT_NOT_ZERO(one);
        ASSERT_POSITIVE(one);
        ASSERT_NEGATIVE(one * -1);
        int *pointer = &one;
        ASSERT_NOT_NULL(pointer);
    }

    TEST("some comparing assertions", NULL, NULL, NULL) {
        ASSERT_NOT_EQUAL(1, -1);
        ASSERT_FLOAT_EQUAL(acos(-1.0), 3.1416, 0.0001);
        ASSERT_STRING_EQUAL("TRIC", "TRIC");
        int m1[] = { 1, 2, 3 };
        int m2[] = { 0, 1, 2, 3 };
        ASSERT_MEMORY_EQUAL(m1, m2 + 1, sizeof(m1));
    }

    TEST("assertions for flags", NULL, NULL, NULL) {
        int flags = O_WRONLY | O_APPEND | O_CREAT | O_EXCL;
        ASSERT_FLAG(flags, O_EXCL);
        ASSERT_NOT_FLAG(flags, O_TRUNC | O_NONBLOCK);
    }

}
```



# Test fixtures

There are 3 methods to define test fixtures in TRIC:

+ As a fixture function that is passed to the SUITE macro
+ As a fixture function that is passed to a TEST macro
+ As a fixture block inside the test suite with the FIXTURE macro

The different types of fixtures can also be mixed. The following sections describe the 3 fixture types in more detail.



## Fixture function passed to the test suite

Code that must run a single time before any test is executed (i.e. setup fixture) or that must run after all tests have finished executing (i.e. teardown fixture) can be defined in a function and can then be passed as an argument to the SUITE macro. If there are any problems during the setup of the test suite that will prevent the tests from running correctly (e.g. if some service can not be connected), the setup fixture function can return false to indicate the problem. In this case, no tests will be executed and the test suite executable will return with an error code.

The SUITE macro can also take as an argument a pointer to some user data which will then be passed to the setup and teardown fixture functions. The pointer however must point to compile constant data. Otherwise the compilation will fail.

The following example shows a simple test suite with a setup and a teardown fixture function passed to the SUITE macro.

```
#include "tric.h"



/* setup fixture function */
bool setup(void *data) {
    printf("setting up suite with %s\n", (char *)data);
    return true;
}



/* teardown fixture function */
bool teardown(void *data) {
    printf("tearing down suite with %s\n", (char *)data);
    return true;
}



/*
user data

note:
User data must be passed as compile time constant data to 
the test suite setup and teardown functions. Defining the following string as 
a pointer (i.e. as char *) would produce a compilation error.
*/
char user_data[] = "user data";



SUITE("with fixtures", setup, teardown, user_data) {
    TEST("illustrating failure", NULL, NULL, NULL) {
        ASSERT(1 == -1);
    }
}
```

When the above example is executed, the following output will be produced:

```
setting up suite with user data
test suite "with fixtures" (1 test found):

test 1 of 1 ("illustrating failure") failed at line 35

1 test executed, 1 failed, 0 skipped, 1 total
tearing down suite with user data
```



## Fixture function passed to a test

If some code needs to be called to prepare the execution of one or multiple tests, the code can be defined in a setup fixture function and can then be passed as an argument to the corresponding TEST macros. Likewise code that needs to be called after a test has finished executing can be defined in a teardown fixture function and passed as an argument to the TEST macro.

The setup and teardown fixture functions of a test will be executed in the same isolated process as the test itself. Therefore it is often not necessary to clean up resources allocated in a setup fixture with a corresponding teardown fixture. Memory that was allocated in a setup fixture for example will be cleaned up automatically when the isolated process terminates.

If the setup fixture function of a test returns false (e.g. if some resource could not be allocated), the test as well as the teardown fixture function of the test (if defined) will not execute. The teardown fixture function of a test will also not execute if the test fails.

In the following example a single setup fixture function and a single teardown fixture function are used for multiple tests in a test suite.

```
#include "tric.h"



/* global memory */
int value = 0;



/* setup fixture function */
bool setup(void *data) {
    printf("setting up test with %s\n", (char *)data);
    /* initialize global memory */
    value++;
    return true;
}



/* teardown fixture function */
bool teardown(void *data) {
    printf("tearing down test with %s\n", (char *)data);
    /* change global memory again */
    value++;
    return true;
}



/* user data */
char *user_data = "user data";



SUITE("with test fixtures", NULL, NULL, NULL) {

    TEST("using fixtures", setup, teardown, user_data) {
        /* setup fixture should have changed global memory */
        ASSERT(value == 1);
    }

    TEST("illustrating failure", setup, teardown, user_data) {
        /* last test changed global data in isolation */
        ASSERT(value > 1);
    }

    TEST("using still fresh global data", setup, teardown, user_data) {
        /* global data was changed by other tests in isolation */
        ASSERT(value == 1);
    }

}
```

When the above example is executed in a terminal, the following output is produced:

```
test suite "with test fixtures" (3 tests found):

setting up test with user data
tearing down test with user data
setting up test with user data
test 2 of 3 ("illustrating failure") failed at line 44
setting up test with user data
tearing down test with user data

3 tests executed, 1 failed, 0 skipped, 3 total
```

The second test in the above example fails because the fixture functions of a test run in the same isolated process as the test itself. The memory changed by the fixture functions of a test remains unaffected in the parent process (i.e. the process of the test suite). Since the second test fails, the teardown fixture function of the test is not executed.

The fixture functions of a test should not be used to implement a custom test result logging: When the output of the above example is redirected to a file, the lines printed by the fixture functions are not included in the output. The fixture functions run in separate child processes and only the output of the parent process (i.e. the process of the test suite) is redirected to the file. See below for how to implement a custom test result reporting.



## Fixture block inside the test suite

Test fixture code can also be defined in a fixture block created with the FIXTURE macro. A FIXTURE block must be placed inside the test suite but outside of any test. Any number of fixture blocks can be defined inside the test suite.

Fixture blocks can for example be used if some code must run after a test finished executing, independently of the result of the test: If a teardown fixture function is passed to the test and the test fails, the teardown fixture function is not executed. If some resource must be cleaned up (e.g. disconnect from a service), the code can be defined in a fixture block and placed after the corresponding test.

In the following example two fixture blocks are used to setup and teardown a resource for a test.

```
#include "tric.h"

SUITE("with fixture blocks", NULL, NULL, NULL) {

    int file;
    FIXTURE("to setup resource") {
        file = open("/dev/zero", O_RDONLY);
    }
    TEST("resource setup by fixture block", NULL, NULL, NULL) {
        int value = 1;
        read(file, &value, sizeof(int));
        ASSERT(value == 0);
    }
    FIXTURE("to teardown resource") {
        close(file);
    }

}
```

Fixture blocks can also be used to place arbitrary code inside a test suite that is not contained in a test: When the executable of a test suite is run, the code inside the test suite is actually executed twice. A first time when scanning for tests and a second time when the tests are executed. If arbitrary code is not contained in a fixture block, it will also be executed twice and may for example lead to unwanted side effects.

In the following example arbitrary code is placed in a test suite to illustrate that the code is executed twice if it is not protected inside a fixture block.

```
#include "tric.h"

SUITE("with arbitrary code", NULL, NULL, NULL) {

    /* will run twice */
    printf("arbitrary code\n");

    FIXTURE("to protect arbitrary code") {
        /* runs only once */
        printf("arbitrary code protected by fixture block\n");
    }

}
```

Executing the above example will produce the following output:

```
arbitrary code
test suite "with arbitrary code" (0 tests found):

arbitrary code
arbitrary code protected by fixture block

0 tests executed, 0 failed, 0 skipped, 0 total
```

The first line of the above output is produced by the unprotected code inside the test suite. The code is executed a first time when the test suite is scanned for tests and before the reporting starts. It is then executed a second time during test execution.

The code inside a fixture block runs in the same process as the test suite. If a fixture block for example allocates memory, tests that use this memory run in separate processes and thus always get a fresh copy of the memory. It is therefore often not necessary to cleanup and reallocate resources allocated by a fixture block after each test that uses the resources.

When a fixture block contains a return statement, it will immediately terminate the test suite. This can be used to terminate a test suite early if for example some fatal error occurs.

The following example shows a test suite that uses a fixture block to terminate execution before all of the tests found are executed. The example illustrates also that test data defined in the process of the test suite is not affected by the isolated execution of a test.

```
#include "tric.h"

SUITE("terminating early", NULL, NULL, NULL) {

    int test_data = 0;
    FIXTURE("setting up test data") {
        test_data = 1;
    }

    TEST("modifying test data", NULL, NULL, NULL) {
        ASSERT(test_data == 1);
        test_data = -1;
        ASSERT(test_data < 0);
    }

    TEST("using unmodified test data", NULL, NULL, NULL) {
        ASSERT(test_data == 1);
    }

    FIXTURE("to exit early") {
        return;
    }

    TEST("that is not executed", NULL, NULL, NULL) {
        ASSERT(test_data == 1);
    }

}
```

When the above example is executed, the following output will be produced:

```
test suite "terminating early" (3 tests found):


2 tests executed, 0 failed, 0 skipped, 3 total
```

The above output shows that although there were 3 tests found in the test suite, only 2 of them were executed. this is because the test suite is terminated with a fixture block placed before the third test.



# Reporting of the test results

TRIC has a simple builtin reporting to output the test results. To change the output format of the test results either the reporting functions in the additional header tric_output.h can be used or a custom reporting can be implemented.



## Reporting test results in other output formats

To output the test results in other formats the header tric_output.h can be included in addition to tric.h. This header provides functions to output the test results in formats like TAP, CSV or JSON. To use these functions, tric.h must be included before tric_output.h can be included. Otherwise a compiler error will be generated.

The functions in tric_output.h must be called before any test is executed (i.e. in the setup fixture of the test suite). The following example shows how to output the test results in the TAP format.

```
#include "tric.h"
#include "tric_output.h"



/* setup fixture for the test suite */
bool setup(void *data) {
    tric_output_tap();
    return true;
}



SUITE("TAP output for TRIC", setup, NULL, NULL) {
    TEST("a successful test", NULL, NULL, NULL) {
        ASSERT(0 == 0);
    }
    TEST("a failing test", NULL, NULL, NULL) {
        ASSERT(0 > 1);
    }
    SKIP_TEST("a skipped test", NULL, NULL, NULL) {
        ASSERT(0 < -1);
    }
}
```

When the above example is executed, the output will look as follows:

```
TAP version 14
1..3 # TAP output for TRIC
ok 1 - a successful test
not ok 2 - a failing test
ok 3 - a skipped test # SKIP
```

TRIC can only output a single report of the test results at a time. To avoid editing and recompiling a test suite if multiple  output formats are needed, the function tric_output_environment() can be used. The output format can then be specified with the environment variable TRIC_OUTPUT_FORMAT when the test suite is executed.

If for example a test suite executable called "list_test" using the function tric_output_environment() should output a summary of the test results in CSV format, it can be executed as follows:

```
$ TRIC_OUTPUT_FORMAT=csv_summary ./list_test
```



## Custom reporting of the test results

The simple builtin test result reporting of TRIC can be replaced with a custom reporting by implementing logging functions. These logging functions need to be passed as arguments to the tric_log() function.

Reporting of the test results can be done in 3 situations during the execution of a test suite: When the suite starts executing, directly after the execution of each test and before the end of the execution of the test suite. When the test suite starts executing, the number of tests in the suite has been determined and the reporting data for each test is set to initial values. After a test was executed, the reporting data of the corresponding test contains the result of the test execution as well as additional information (e.g. the reason why the test failed). The complete reporting data of the test suite and all tests contained in the suite is available at the end of the execution of the test suite.

Separate logging functions for the start of the test suite, the end of a test and the end of the test suite can be passed to tric_log(). If an argument of tric_log() is NULL, no logging will be performed in the corresponding situation. If all arguments of tric_log() are NULL, test result reporting is disabled completely.

To activate a custom test result reporting before the test suite starts executing, a setup fixture function for the test suite needs to be defined that contains the call to tric_log(). When tric_log() is called later in the test suite, logging starts with the default reporting.

In the following example the default test result reporting of TRIC is replaced with custom logging functions that produce a simple markdown like table.

```
#include "tric.h"



/* logging functions to produce a markdown like table */

void table_header(struct tric_suite *suite, struct tric_test *test, void *data) {
    printf("|----|--------|\n");
    printf("| ID | RESULT |\n");
    printf("|----|--------|\n");
}

void table_row(struct tric_suite *suite, struct tric_test *test, void *data) {
    printf("| %zu  | %s |\n", test->id, test->result == TRIC_OK ? "OK    " : "FAIL  ");
}

void table_footer(struct tric_suite *suite, struct tric_test *test, void *data) {
    printf("|----|--------|\n");
}



/* setup fixture to activate table reporting */
bool setup(void *data) {
    tric_log(table_header, table_row, table_footer, NULL);
    return true;
}



SUITE("printing markdown like table", setup, NULL, NULL) {
    TEST("is successful", NULL, NULL, NULL) {
        ASSERT(0 == 0);
    }
    TEST("is failing", NULL, NULL, NULL) {
        ASSERT(0 < 0);
    }
    TEST("is crashing", NULL, NULL, NULL) {
        int *null_pointer = NULL;
    ASSERT(*null_pointer == 0);
    }
}
```

Running the above example produces the following output:

```
|----|--------|
| ID | RESULT |
|----|--------|
| 1  | OK     |
| 2  | FAIL   |
| 3  | FAIL   |
|----|--------|
```



# Documentation

The documentation for TRIC can be generated using [doxygen](https://www.doxygen.nl) by running doxygen in the doc directory. The generated html documentation can then be found in the doc/html directory.



# License

TRIC is licensed under the GNU Lesser General Public License. A copy of the license can be found in the files COPYING and COPYING.LESSER or at the [GNU website](https://www.gnu.org/licenses/lgpl-3.0).



