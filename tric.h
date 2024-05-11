/*
TRIC - Minimalistic unit testing framework for c
Copyright 2024 Philip Colombo

This file is part of TRIC.

TRIC is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

TRIC is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with TRIC.  If not, see <https://www.gnu.org/licenses/>.

*/



#ifndef TRIC_H
#define TRIC_H



#include <unistd.h>
#include <sysexits.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>



/**
 * \file tric.h
 *
 * \brief TRIC - Testing Rules In C
 *
 * TRIC is a minimalistic unit testing framework for c. It has no external dependencies and is all contained in a single header file. Tests are automatically detected and are executed in separate processes. Test fixtures can be set per test or for the whole test suite. The default reporting can be easily replaced with a custom reporting.
 *
 * Example test suite:
 *
 * \code
#include "tric.h"

SUITE("simple test suite", NULL, NULL, NULL) {
    TEST("simple failing test", NULL, NULL, NULL) {
        ASSERT(1 < 0);
    }
}
 * \endcode
 *
 * No main function or any further processing is needed - just compile, run and get the following output:
 *
 * \code
test suite "simple test suite" (1 test found):

test 1 of 1 ("simple failing test") failed at line 5

1 test executed, 1 failed, 0 skipped, 1 total
 * \endcode
 *
 * \author Philip Colombo
 * \date 2024
 * \copyright GNU Lesser General Public License
 */



/*
 internally used
create unique name for per test data
*/
#define CONCATENATE_NAME(name1, name2) name1##name2
#define UNIQUE_NAME(name, id) CONCATENATE_NAME(name, id)



/*
internally used
default initialization of suite data
*/
#define NEW_SUITE(DESCRIPTION) \
{ \
    .description = DESCRIPTION, \
    .number_of_tests = 0, \
    .executed_tests = 0, \
    .failed_tests = 0, \
    .skipped_tests = 0, \
    .tests = NULL \
}



/*
internally used
create function to hold global data
*/
#define SUITE_DATA(DESCRIPTION, SETUP, TEARDOWN, DATA) \
const struct tric_suite_data *tric_data(void) { \
    static struct tric_suite suite = NEW_SUITE(DESCRIPTION); \
    static const struct tric_suite_data suite_data = { .suite = &suite, .setup = SETUP, .teardown = TEARDOWN, .data = DATA }; \
    return &suite_data; \
}



/**
 * \brief Create the test suite.
 *
 * There must be exactly one test suite in a file and all tests must be inside the test suite. The description should not be omitted. All other arguments can be omitted by setting them to NULL.
 *
 * \param DESCRIPTION String literal to describe the test suite.
 * \param SETUP Function of type tric_fixture_t that will be executed before the tests will be run. May be NULL.
 * \param TEARDOWN Function of type tric_fixture_t that will be executed after all tests have been run. May be NULL.
 * \param DATA Pointer to user data that is passed to the setup and teardown functions. The pointer must reference data that is compile constant (e.g. static data) or there will be a compilation error.
 */
#define SUITE(DESCRIPTION, SETUP, TEARDOWN, DATA) \
SUITE_DATA(DESCRIPTION, SETUP, TEARDOWN, DATA) \
void tric_suite_function(struct tric_context *tric_context)



/*
internally used
default initialization of test data
*/
#define NEW_TEST(DESCRIPTION) \
{ \
    .id = 0, \
    .description = DESCRIPTION, \
    .before = TRIC_UNDEFINED, \
    .result = TRIC_UNDEFINED, \
    .after = TRIC_UNDEFINED, \
    .line = 0, \
    .signal = 0, \
    .next = NULL \
}



/*
internally used
create test data and prepare for test execution
*/
#define PREPARE_TEST(DESCRIPTION) \
    static struct tric_test UNIQUE_NAME(tric_test, __LINE__) = NEW_TEST(DESCRIPTION); \
    tric_add_test(tric_context, &(UNIQUE_NAME(tric_test, __LINE__))); \
    tric_reset_context(tric_context, &(UNIQUE_NAME(tric_test, __LINE__)));



/**
 * \brief Create a test.
 *
 * The description of the test should not be omitted. All other arguments can be omitted by setting them to NULL. The code of the test as well as the BEFORE and AFTER functions are executed in a separate process.
 *
 * \param DESCRIPTION String literal to describe the test.
 * \param BEFORE Function of type tric_fixture_t that will be executed before the test is run. May be NULL.
 * \param AFTER Function of type tric_fixture_t that will be executed after the test is run. May be NULL.
 * \param DATA User data that is passed to the before and after functions.
 */
#define TEST(DESCRIPTION, BEFORE, AFTER, DATA) \
    PREPARE_TEST(DESCRIPTION) \
    tric_run_test(tric_context, (BEFORE) ? true : false, (AFTER) ? true : false); \
    for ( \
        tric_run_before(tric_context, (BEFORE), (DATA)); \
        (tric_context->mode == MODE_EXECUTE) && ((tric_context->mode = MODE_RESET) == MODE_RESET); \
        tric_run_after((AFTER), (DATA)) \
    )



/**
 * \brief Create a test that is skipped.
 *
 * The description of the test should not be omitted. All other arguments can be omitted by setting them to NULL. The code of the test as well as the BEFORE and AFTER functions are not executed and the test is marked as skipped.
 *
 * \param DESCRIPTION String literal to describe the test.
 * \param BEFORE Function of type tric_fixture_t that would be executed before the test would be run. May be NULL.
 * \param AFTER Function of type tric_fixture_t that would be executed after the test would be run. May be NULL.
 * \param DATA User data that would be passed to the before and after functions.
 */
#define SKIP_TEST(DESCRIPTION, BEFORE, AFTER, DATA) \
    PREPARE_TEST(DESCRIPTION) \
    tric_skip_test_execution(tric_context, (BEFORE), (AFTER)); \
    if (false)



/**
 * \brief Verify an expression.
 *
 * The given expression is executed and if it is false, the test is terminated and marked as failed. If the expression is true, nothing happens and the execution of the test continues.
 *
 * there can be any number of ASSERTs per test. Since ASSERT terminates the calling process, it should not be used outside of a test.
 *
 * \param EXPRESSION Expression to verify.
 */
#define ASSERT(EXPRESSION) \
if ( (EXPRESSION) == false) { \
    lseek(tric_context->self, __LINE__, SEEK_SET); \
    _exit(EXIT_TEST_FAILURE); \
}



/**
 * \brief Mark code inside the test suite as fixture code.
 *
 * Arbitrary code placed inside the test suite but outside of a test is executed twice (during scanning and when executing the tests). To prevent e.g. malloc() from running twice, the code can be placed in a fixture block.
 *
 * \param DESCRIPTION String literal to describe the purpose of the fixture. May be omitted.
 */
#define FIXTURE(DESCRIPTION) \
if (tric_context->mode == MODE_RESET)



struct tric_suite;
struct tric_test;
struct tric_context;
struct tric_reporting_data *tric_report(void);
const struct tric_suite_data *tric_data(void);
void tric_suite_function(struct tric_context *tric_context);



/**
 * \brief Prototype of a log function.
 *
 * \param suite Pointer to the test suite.
 * \param test Pointer to the test that was executed just before the log function is called. If the log function is called at the start or end of the test suite, test is NULL.
 * \param data User data that will be passed to the log function.
 */
typedef void (*tric_logger_t)(struct tric_suite *suite, struct tric_test *test, void *data);



/**
 * \brief Prototype of a test fixture.
 *
 * \param data User data that will be passed to the fixture
 * \return Should return true when the fixture can execute without any failure. Otherwise false should be returned to indicate a problem.
 */
typedef bool (*tric_fixture_t)(void *data);



/*
internally used
function to set the result status of a test
*/
typedef void (*tric_status_t)(struct tric_context *context, bool before, bool after);



/*
internally used
exit status of child process running a test
*/
enum tric_exit {
    EXIT_OK = 0,
    EXIT_TEST_FAILURE,
    EXIT_BEFORE_FAILURE,
    EXIT_AFTER_FAILURE,
    EXIT_SIGNAL,
    EXIT_FORK,
    EXIT_SKIP
};



/**
 * \brief Execution results.
 *
 * The execution result of a test (and its fixtures) indicate whether it ran successfully or not. Initially the execution result is always TRIC_UNDEFINED.
 */
enum tric_result {

    /**
     * \brief Not executed
     */
    TRIC_UNDEFINED = -1,

    /**
     * \brief Execution was successful
     */
    TRIC_OK = 0,

    /**
     * \brief Execution failed
     */
    TRIC_FAILURE,

    /**
     * \brief Execution was skipped
     */
    TRIC_SKIPPED,

    /**
     * \brief Execution failed due to signal
     */
    TRIC_CRASHED
};



/*
internally used
execution mode of test suite function
*/
enum tric_mode {
    MODE_SCAN,
    MODE_EXECUTE,
    MODE_RESET
};



/*
internally used
structure of global data
*/
struct tric_suite_data {
    struct tric_suite * const suite;
    const tric_fixture_t setup;
    const tric_fixture_t teardown;
    void * const data;
};



/**
 * \brief Test suite data.
 *
 * The description of the test suite is specified by the first argument of the SUITE macro. The number of tests in the test suite is determined before any test  or any reporting is executed. The data of the individual tests is collected in a linked list that can be accessed via the tests property.
 */
struct tric_suite {

    /**
     * \brief Description of the test suite
     */
    const char *description;

    /**
     * \brief Number of tests found in the test suite
     */
    size_t number_of_tests;

    /**
     * \brief Number of tests that were executed
     */
    size_t executed_tests;

    /**
     * \brief Number of tests that failed to execute
     */
    size_t failed_tests;

    /**
     * \brief Number of tests that were found but not executed
     */
    size_t skipped_tests;

    /**
     * \brief Linked list of the tests found in the test suite
     */
    struct tric_test *tests;
};



/**
 * \brief Test data.
 *
 * The description of the test is specified by the first argument of the TEST macro. The line property is only set if the test fails due to a failing ASSERT, otherwise it is 0. The signal property is only set if the test crashes due to a signal, otherwise it is 0.
 */
struct tric_test {

    /**
     * \brief ID of the test (IDs start at 1)
     */
    size_t id;

    /**
     * \brief Description of the test
     */
    const char *description;

    /**
     * \brief Execution result of the before function
     */
    enum tric_result before;

    /**
     * \brief Execution result of the test
     */
    enum tric_result result;

    /**
     * \brief Execution result of the after function
     */
    enum tric_result after;

    /**
     * \brief Line of failing assert
     */
    size_t line;

    /**
     * \brief Signal number that caused the test to crash
     */
    size_t signal;

    /**
     * \brief Next test in the linked list
     *
     * If the test is the last test found in the test suite, next is NULL.
     */
    struct tric_test *next;
};



/*
internally used
execution context of suite function
*/
struct tric_context {

    /* 
   file descriptor to detect the line at which an assertion failed
    */
    int self;
    enum tric_mode mode;
    struct tric_suite * const suite;
    struct tric_test *test;
};



/*
internally used
data used for reporting test results
*/
struct tric_reporting_data {
    tric_logger_t start;
    tric_logger_t test;
    tric_logger_t end;
    void *data;
};



/*
internally used
connect data of individual tests into linked list
*/
void tric_add_test(struct tric_context *context, struct tric_test *test) {
    if (context->mode != MODE_SCAN) {
        return;
    }
    if (context->test) {
        context->test->next = test;
    } else {
        context->suite->tests = test;
    }
    context->test = test;
    context->suite->number_of_tests++;
    test->id = context->suite->number_of_tests;
}



/*
internally used
prepare execution context to run test
*/
void tric_reset_context(struct tric_context *context, struct tric_test *test) {
    if (context->mode != MODE_RESET) {
        return;
    }
    context->mode = MODE_EXECUTE;
    lseek(context->self, 0, SEEK_SET);
    context->test = test;
}



/*
internally used
mark test as successful
*/
void tric_status_ok(struct tric_context *context, bool before, bool after) {
    context->test->before = before ? TRIC_OK : TRIC_UNDEFINED;
    context->test->result = TRIC_OK;
    context->test->after = after ? TRIC_OK : TRIC_UNDEFINED;
}



/*
internally used
mark test as failed
*/
void tric_status_fail(struct tric_context *context, bool before, bool after) {
    context->suite->failed_tests++;
    context->test->line = lseek(context->self, 0, SEEK_CUR);
    context->test->before = before ? TRIC_OK : TRIC_UNDEFINED;
    context->test->result = TRIC_FAILURE;
    context->test->after = after ? TRIC_SKIPPED : TRIC_UNDEFINED;
}



/*
internally used
mark test as failed in before function
*/
void tric_status_fail_before(struct tric_context *context, bool before, bool after) {
    context->suite->failed_tests++;
    context->test->before = TRIC_FAILURE;
    context->test->result = TRIC_SKIPPED;
    context->test->after = after ? TRIC_SKIPPED : TRIC_UNDEFINED;
}



/*
internally used
mark test as failed in after function
*/
void tric_status_fail_after(struct tric_context *context, bool before, bool after) {
    context->test->before = before ? TRIC_OK : TRIC_UNDEFINED;
    context->test->result = TRIC_OK;
    context->test->after = TRIC_FAILURE;
}



/*
internally used
mark test as failed due to signal
*/
void tric_status_crash(struct tric_context *context, bool before, bool after) {
    context->suite->failed_tests++;
    context->test->before = before ? TRIC_CRASHED : TRIC_UNDEFINED;
    context->test->result = TRIC_CRASHED;
    context->test->after = after ? TRIC_CRASHED : TRIC_UNDEFINED;
}



/*
internally used
mark test as failed due to fork error
*/
void tric_status_fail_fork(struct tric_context *context, bool before, bool after) {
    context->suite->skipped_tests++;
    context->test->before = TRIC_UNDEFINED;
    context->test->result = TRIC_UNDEFINED;
    context->test->after = TRIC_UNDEFINED;
}



/*
internally used
mark test as skipped
*/
void tric_status_skip(struct tric_context *context, bool before, bool after) {
    context->suite->skipped_tests++;
    context->test->before = before ? TRIC_SKIPPED : TRIC_UNDEFINED;
    context->test->result = TRIC_SKIPPED;
    context->test->after = after ? TRIC_SKIPPED : TRIC_UNDEFINED;
}



/*
internally used
mark test according to exit status
*/
void tric_set_status(struct tric_context *context, enum tric_exit status, bool before, bool after) {
    static const tric_status_t states[] = {
        [EXIT_OK] = tric_status_ok,
        [EXIT_TEST_FAILURE] = tric_status_fail,
        [EXIT_BEFORE_FAILURE] = tric_status_fail_before,
        [EXIT_AFTER_FAILURE] = tric_status_fail_after,
        [EXIT_SIGNAL] = tric_status_crash,
        [EXIT_FORK] = tric_status_fail_fork,
        [EXIT_SKIP] = tric_status_skip
    };
    states[status](context, before, after);
}



/*
internally used
execute test in separate process
*/
void tric_run_test(struct tric_context *context, bool before, bool after) {
    if (context->mode != MODE_EXECUTE) {
        return;
    }
    pid_t child = fork();
    if (child == 0) {
        return;
    }
    context->mode = MODE_RESET;
    if (child == -1) {
        tric_set_status(context, EXIT_FORK, before, after);
        tric_report()->test(context->suite, context->test, tric_report()->data);
        return;
    }
    context->suite->executed_tests++;
    int status;
    waitpid(child, &status, 0);
    if (WIFSIGNALED(status)) {
        tric_set_status(context, EXIT_SIGNAL, before, after);
        context->test->signal = WTERMSIG(status);
    } else {
        tric_set_status(context, WEXITSTATUS(status), before, after);
    }
    tric_report()->test(context->suite, context->test, tric_report()->data);
}



/*
internally used
execute before function of test
*/
void tric_run_before(struct tric_context *context, tric_fixture_t before, void *data) {
    if (context->mode != MODE_EXECUTE
    || before == NULL
    || before(data)) {
        return;
    }
    _exit(EXIT_BEFORE_FAILURE);
}



/*
internally used
execute after function of test
*/
void tric_run_after(tric_fixture_t after, void *data) {
    if (after == NULL
    || after(data)) {
        _exit(EXIT_OK);
    }
    _exit(EXIT_AFTER_FAILURE);
}



/*
internally used
skip execution of test
*/
void tric_skip_test_execution(struct tric_context *context, tric_fixture_t before, tric_fixture_t after) {
    if (context->mode != MODE_EXECUTE) {
        return;
    }
    tric_set_status(context, EXIT_SKIP, before ? true : false, after ? true : false);
    tric_report()->test(context->suite, context->test, tric_report()->data);
}



/*
internally used
default log function running at start of suite
*/
void tric_log_start(struct tric_suite *suite, struct tric_test *test, void *data) {
    printf("test suite \"%s\" (%zu %s found):\n\n", suite->description, suite->number_of_tests, suite->number_of_tests == 1 ? "test" : "tests");
}



/*
internally used
default log function running after each test
*/
void tric_log_test(struct tric_suite *suite, struct tric_test *test, void *data) {
    if (test->result == TRIC_FAILURE) {
        printf("test %zu of %zu (\"%s\") failed at line %zu\n", test->id, suite->number_of_tests, test->description, test->line);
    } else if (test->result == TRIC_CRASHED) {
        printf("test %zu of %zu (\"%s\") crashed with signal %zu\n", test->id, suite->number_of_tests, test->description, test->signal);
    }
}



/*
internally used
default log function running at end of suite
*/
void tric_log_end(struct tric_suite *suite, struct tric_test *test, void *data) {
    printf("\n%zu %s executed, %zu failed, %zu skipped, %zu total\n", suite->executed_tests, suite->executed_tests == 1 ? "test" : "tests", suite->failed_tests, suite->skipped_tests, suite->number_of_tests);
}



/*
internally used
log function producing no output
*/
void tric_log_nothing(struct tric_suite *suite, struct tric_test *test, void *data) {
    return;
}



/*
internally used
function to hold global reporting data
*/
struct tric_reporting_data *tric_reporting(bool set, tric_logger_t start, tric_logger_t test, tric_logger_t end, void *data) {
    static struct tric_reporting_data reporting = { .start = tric_log_start, .test = tric_log_test, .end = tric_log_end, .data = NULL };
    if (set) {
        reporting.start = start ? start : tric_log_nothing;
        reporting.test = test ? test : tric_log_nothing;
        reporting.end = end ? end : tric_log_nothing;
        reporting.data = data;
    }
    return &reporting;
}



/*
internally used
retrieve global reporting data
*/
struct tric_reporting_data *tric_report(void) {
    return tric_reporting(false, NULL, NULL, NULL, NULL);
}



/**
 * \brief Set the log functions to report the test results.
 *
 * Logging can take place in 3 situations: At the start of the test suite (before any test is executed), directly after the execution of each test and at the end of the test suite (after all tests have been executed). When a log function is called at the start or end of the test suite, the test argument passed to the log function is NULL.
 *
 * \param start Function of type tric_logger_t that is executed after the setup fixture of the test suite has run and after the tests have been scanned but before any test is executed. May be NULL.
 * \param test Function of type tric_logger_t that is executed each time after a test was executed. May be NULL.
 * \param end Function of type tric_logger_t that is executed  after all tests have been executed but before the teardown fixture of the test suite is run. May be NULL.
 * \param data Pointer to user data that is passed to the logging functions.
 */
void tric_log(tric_logger_t start, tric_logger_t test, tric_logger_t end, void *data) {
    tric_reporting(true, start, test, end, data);
}



/*
internally used
execute setup or teardown function of test suite
*/
bool tric_run_fixture(tric_fixture_t fixture, void *data) {
    if (fixture == NULL) {
        return true;
    }
    return fixture(data);
}



/*
internally used
scan suite for tests
*/
void tric_scan_tests(struct tric_context *context) {
    context->mode = MODE_SCAN;
    context->test = NULL;
    tric_suite_function(context);
    context->mode = MODE_RESET;
}



/*
internally used
execute tests of suite
*/
int tric_run_tests(struct tric_context *context) {
    context->mode = MODE_RESET;
    if (tric_run_fixture(tric_data()->setup, tric_data()->data) == false) {
        return EX_UNAVAILABLE;
    }
    tric_report()->start(context->suite, NULL, tric_report()->data);
    tric_suite_function(context);
    tric_report()->end(context->suite, NULL, tric_report()->data);
    return tric_run_fixture(tric_data()->teardown, tric_data()->data) ? EX_OK : EX_TEMPFAIL;
}



#ifdef TRIC_SELF_TEST
int tric_main(int argc, char *argv[]) {
#else

/**
 * \brief Main function of the test suite.
 *
 * The main function is automatically added to the file containing the test suite.
 *
 * \return If all tests of the test suite as well as the setup and teardown functions were executed successfully, main returns 0 (or EX_OK). Otherwise main returns EX_NOINPUT if the executable can not be opened for reading, EX_UNAVAILABLE if the setup function fails or EX_TEMPFAIL if the teardown function fails.
 */
int main(int argc, char *argv[]) {
#endif
    struct tric_context context = { .suite = tric_data()->suite };
    if ((context.self = open(argv[0], O_RDONLY)) == -1) {
        return EX_NOINPUT;
    }
    tric_scan_tests(&context);
    int result = tric_run_tests(&context);
    close(context.self);
    return result;
}



#endif



