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



# Test fixtures

Setup and teardown fixtures can be specified for the test suite and for each test by passing the fixture function as an argument to the macro. A fixture can return false to indicate a problem and will prevent the test (or the whole suite) from running. There is also the possibility to write fixture code directly in the suite.

The following example shows how test fixtures can be defined:

```
#include "tric.h"



/* setup and teardown fixtures for the test suite */

bool setup(void *data) {
    printf("setting up suite with %s\n", (char *)data);
    return true;
}

bool teardown(void *data) {
    printf("tearing down suite with %s\n", (char *)data);
    return true;
}

/*
note:
user data must be passed as compile time constant data to 
the test suite setup and teardown functions
*/
char suite_user_data[] = "suite data";



/* fixtures running before and after a test */

bool before(void *data) {
    printf("run before test with %s\n", (char *)data);
    return true;
}

bool after(void *data) {
    printf("run after test with %s\n", (char *)data);
    return true;
}

char *test_user_data = "test data";



SUITE("test suite with fixtures", setup, teardown, suite_user_data) {

    TEST("successful test", before, after, test_user_data) {
        ASSERT(1 == 1);
    }

    /* if a test fails the fixture after the test will not run */
    TEST("failing test", before, after, test_user_data) {
        ASSERT(1 != 1);
    }

    /*
    note:
    arbitrary code inside the test suite but outside 
    of any test is executed twice
    */
    printf("some arbitrary code\n");
    int one = 0;

    /* use a fixture block to prevent code from running twice */
    FIXTURE("inside suite") {
        printf("arbitrary code inside a fixture block\n");
        one = 1;
    }

    TEST("variable assigned in fixture block", NULL, NULL, NULL) {
        ASSERT(one == 1);
    }

}
```

Running the above code produces the following output:

```

some arbitrary code
setting up suite with suite data
test suite "test suite with fixtures" (3 tests found):

run before test with test data
run after test with test data
run before test with test data
test 2 of 3 ("failing test") failed at line 50
some arbitrary code
arbitrary code inside a fixture block

3 tests executed, 1 failed, 0 skipped, 3 total
tearing down suite with suite data
```

Note the first line of the output: Before any test is run, the test suite is executed a first time to scan for tests. Because of that arbitrary code inside the test suite (that is not contained in a test or a fixture block) will be executed twice.



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



# Assertions

TRIC itself includes only a simple ASSERT macro to verify test conditions. To simplify the writing of test conditions and to make the tests more readable, the header tric_assert.h can be included in addition to TRIC. This header contains a collection of assertion macros that internally use the ASSERT macro of TRIC. In order to use these macros, tric.h must be included before tric_assert.h can be included. Otherwise a compiler error will be generated.

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



# Documentation

The documentation for TRIC can be generated using [doxygen](https://www.doxygen.nl) by running doxygen in the doc directory. The generated html documentation can then be found in the doc/html directory.



