<!-- start README with comment to help doxygen find the first heading -->

# TRIC - Testing Rules In C

TRIC is a minimalistic unit testing framework for c. It has no external dependencies and is all contained in a single header file. Tests are automatically detected and are executed in separate processes. TRIC supports test fixtures and has a builtin and customizable reporting.



# Features

+ Everything is contained in a single header file. No linking of other resources is needed.
+ No further dependencies. Just include tric.h and write tests.
+ Minimalistic API. Only a hand full of macros.
+ Automatic test registration. No need to manually add test functions to an execution list or process the tests with an external tool.
+ A main function is automatically inserted. Compiling the test suite produces a ready to run executable.
+ Tests run isolated in their own processes. Even crashing tests do not stop the test suite.
+ Setup and teardown fixtures can be set per test or for the entire test suite.
+ Fixtures for a test run in the same isolated process as the test itself.
+ Simple builtin default reporting of the test results.
+ Custom reporting routines can be defined or reporting can be turned off completely.



# Example test suite

The following example code shows the basic usage of TRIC. Tests need to be placed inside a test suite. A test suite is created with the SUITE macro and a test is created with the TEST macro. The ASSERT macro is used to verify an expression and lets the test fail if the expression is false.

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



# Assertions

TRIC itself includes only a basic ASSERT macro to verify test conditions. To simplify the writing of test conditions and to make the tests more readable, in addition to the header tric.h the header tric_assert.h can be included. This header contains a collection of assertion macros that internally use the ASSERT macro of TRIC. In order to use these macros, tric.h must be included before tric_assert.h can be included. Otherwise a compiler error will be generated.

The following example test suite shows how to use the assertions in tric_assert.h:

```
#include "tric.h"
#include "tric_assert.h"

SUITE("assertion overview", NULL, NULL, NULL) {

    TEST("single value assertions", NULL, NULL, NULL) {
        int one = 1;
        ASSERT_NOT(one < 0);
        ASSERT_TRUE(one > 0);
        ASSERT_FALSE(one != -1 && one < -1);
        ASSERT_ZERO(one + -1);
        ASSERT_NOT_ZERO(one + one);
        ASSERT_POSITIVE(one);
        ASSERT_NEGATIVE(0 - one);
        int *pointer = NULL;
        ASSERT_NULL(pointer);
        pointer = &one;
        ASSERT_NOT_NULL(pointer);
    }

    TEST("equality assertions", NULL, NULL, NULL) {
        ASSERT_EQUAL(1, 1);
        ASSERT_NOT_EQUAL(1, 0);
        ASSERT_STRING_EQUAL("TRIC", "TRIC");
        int m1[] = { 1, 2, 3 };
        int m2[] = { 0, 1, 2, 3 };
        ASSERT_MEMORY_EQUAL(m1, m2 + 1, sizeof(m1));
        ASSERT_FLOAT_EQUAL(acos(-1.0), 3.1416, 0.0001);
    }

    TEST("flag assertions", NULL, NULL, NULL) {
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

The fixture functions of a test should not be used to implement a custom test result logging: When the output of the above example is redirected to a file, the lines printed by the fixture functions are not included in the output. The fixture functions run in separate child processes and only the output of the parent process (i.e. the process of the test suite) is redirected to the file. See below for how to implement a custom logging.



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

The following example shows a test suite that uses a fixture block to terminate before all tests found in the test suite have finished executing. The example illustrates also that test data defined in the process of the test suite is not affected by the isolated execution of a test.

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



# Reporting test results in other output formats

TRIC itself provides only a simple builtin reporting. To output the test results in other formats the header tric_output.h can be included in addition to tric.h. This header provides functions to output the test results in formats like TAP, CSV or JSON. To use these functions, tric.h must be included before tric_output.h can be included. Otherwise the compilation of the test suite will fail.

The functions in tric_output.h must be called before any test is executed (i.e. in the setup fixture of the test suite). The following example shows how to output the test results in the TAP format:

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

When the above example is executed, it will produce the following output:

```
TAP version 14
1..3 # TAP output for TRIC
ok 1 - a successful test
not ok 2 - a failing test
ok 3 - a skipped test # SKIP
```

TRIC can only output a single test report at a time. To avoid editing and recompiling a test suite if multiple  output formats are needed, the function tric_output_environment() can be used. The output format can then be specified with the environment variable TRIC_OUTPUT_FORMAT when the test suite is executed.

If for example a test suite executable called "list_test" using the function tric_output_environment() should output a summary of the test results in CSV format, it can be executed as follows:

```
$ TRIC_OUTPUT_FORMAT=csv_summary ./list_test
```



# Custom reporting of the test results

The default reporting of the test results can be replaced with custom logging functions. Reporting can be done in 3 situations: When the suite starts, after the execution of each test and at the end of the suite. To specify custom logging functions tric_log() needs to be called with the logging functions as arguments. To run tric_log() before the suite starts, a setup fixture for the test suite can be defined that contains the call to tric_log().

In the following example the default reporting is replaced with a custom reporting that produces a simple markdown like table for the test results:

```
#include "tric.h"



/* logging functions to produce a markdown like table */

void table_header(struct tric_suite *suite, struct tric_test *test, void *data) {
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
    TEST("successful test", NULL, NULL, NULL) {
        ASSERT(0 == 0);
    }

    TEST("failing test", NULL, NULL, NULL) {
        ASSERT(0 < 0);
    }

    TEST("crashing test", NULL, NULL, NULL) {
        int *null_pointer = NULL;
    ASSERT(*null_pointer == 0);
    }
}
```

When the above code is run it produces the following output:

```
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



