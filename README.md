<!-- start README with comment to help doxygen find the first heading -->

# TRIC - Testing Rules In C

TRIC is a minimalistic unit testing framework for c. It has no external dependencies and is all contained in a single header file. Tests are automatically detected and are executed in separate processes. TRIC supports test fixtures and has a builtin and customizable reporting.



## Features

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



## Example test suite

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



## Test fixtures

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



## Custom reporting of the test results

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



