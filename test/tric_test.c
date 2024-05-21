/*
TRIC self tests
Copyright 2024 Philip Colombo

This file is part of TRIC.

TRIC is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

TRIC is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with TRIC.  If not, see <https://www.gnu.org/licenses/>.

*/



#include <assert.h>
#include <signal.h>
#include <string.h>



/* system under test */

#define TRIC_SELF_TEST
#include "../tric.h"



/* simple fixture mock */

struct test_fixture_mock_data {
    size_t count;
    void *data;
    bool result;
};

#define TEST_FIXTURE_MOCK_DATA_NEW { .count = 0, .data = NULL, .result = true }



struct test_fixture_mock_data test_setup_mock_data = TEST_FIXTURE_MOCK_DATA_NEW;
bool test_setup_mock(void *data) {
    test_setup_mock_data.count++;
    test_setup_mock_data.data = data;
    return test_setup_mock_data.result;
}



struct test_fixture_mock_data test_teardown_mock_data = TEST_FIXTURE_MOCK_DATA_NEW;
bool test_teardown_mock(void *data) {
    test_teardown_mock_data.count++;
    test_teardown_mock_data.data = data;
    return test_teardown_mock_data.result;
}



struct test_fixture_mock_data test_before_mock_data = TEST_FIXTURE_MOCK_DATA_NEW;
bool test_before_mock(void *data) {
    test_before_mock_data.count++;
    test_before_mock_data.data = data;
    return test_before_mock_data.result;
}



struct test_fixture_mock_data test_after_mock_data = TEST_FIXTURE_MOCK_DATA_NEW;
bool test_after_mock(void *data) {
    test_after_mock_data.count++;
    test_after_mock_data.data = data;
    return test_after_mock_data.result;
}



/* simple log function mock */

struct test_logger_mock_data {
    size_t count;
    struct tric_suite *suite;
    struct tric_test *test;
    void *data;
};

#define TEST_LOGGER_MOCK_DATA_NEW { .count = 0, .suite = NULL, .test = NULL, .data = NULL }



struct test_logger_mock_data test_log_start_mock_data = TEST_LOGGER_MOCK_DATA_NEW;
void test_log_start_mock(struct tric_suite *suite, struct tric_test *test, void *data) {
    test_log_start_mock_data.count++;
    test_log_start_mock_data.suite = suite;
    test_log_start_mock_data.test = test;
    test_log_start_mock_data.data = data;
}



struct test_logger_mock_data test_log_test_mock_data = TEST_LOGGER_MOCK_DATA_NEW;
void test_log_test_mock(struct tric_suite *suite, struct tric_test *test, void *data) {
    test_log_test_mock_data.count++;
    test_log_test_mock_data.suite = suite;
    test_log_test_mock_data.test = test;
    test_log_test_mock_data.data = data;
}



struct test_logger_mock_data test_log_end_mock_data = TEST_LOGGER_MOCK_DATA_NEW;
void test_log_end_mock(struct tric_suite *suite, struct tric_test *test, void *data) {
    test_log_end_mock_data.count++;
    test_log_end_mock_data.suite = suite;
    test_log_end_mock_data.test = test;
    test_log_end_mock_data.data = data;
}



/* simple test suite function mock */

struct test_suite_mock_data {
    size_t count;
    enum tric_mode context_mode;
    struct tric_suite *context_suite;
    struct tric_context *context_reference;
};

#define TEST_SUITE_MOCK_DATA_NEW { .count = 0, .context_mode = MODE_SCAN, .context_suite = NULL, .context_reference = NULL }



struct test_suite_mock_data test_suite_mock_data = TEST_SUITE_MOCK_DATA_NEW;
void tric_suite_function(struct tric_context *tric_context) {
    test_suite_mock_data.count++;
    test_suite_mock_data.context_mode = tric_context->mode;
    test_suite_mock_data.context_suite = tric_context->suite;
    test_suite_mock_data.context_reference = tric_context;
}



/*
globally needed data
see tests test_suite_data, test_main_fail_setup, test_main_fail_teardown and test_main_ok
*/

void *test_suite_data_reference = NULL;
SUITE_DATA("test suite", test_setup_mock, test_teardown_mock, &test_suite_data_reference)



/* start of tests */

void test_new_suite(void) {
    /* description of suite should be set */

    struct tric_suite suite = NEW_SUITE("test");

    assert(strcmp(suite.description, "test") == 0);
}



void test_suite_data(void) {
    /* setup and teardown should be set */

    const struct tric_suite_data *data = tric_data();

    assert(strcmp(data->suite->description, "test suite") == 0);
    assert(data->setup == test_setup_mock);
    assert(data->teardown == test_teardown_mock);
    assert(data->data == &test_suite_data_reference);
}



void test_new_test(void) {
    /* description should be set */

    struct tric_test test = NEW_TEST("test");

    assert(strcmp(test.description, "test") == 0);
}



void test_prepare_test_scan(void) {
    /* new test should be added to suite */

    struct tric_suite suite = { .tests = NULL };
    struct tric_context context = { .mode = MODE_SCAN, .suite = &suite };
    struct tric_context *tric_context = &context;

    PREPARE_TEST("test")

    assert(suite.tests != NULL);
    assert(strcmp(suite.tests->description, "test") == 0);
}



void test_prepare_test_run(void) {
    /* test should be found and context should be reset */

    struct tric_suite suite = { .tests = NULL };
    struct tric_context context = { .mode = MODE_RESET, .suite = &suite };
    struct tric_context *tric_context = &context;

    PREPARE_TEST("test")

    assert(suite.tests == NULL);
    assert(context.test != NULL);
    assert(strcmp(context.test->description, "test") == 0);
    assert(context.mode == MODE_EXECUTE);
}



void test_test_scan(void) {
    /* scanned tests should be contained in linked list of test suite */

    struct tric_suite suite = NEW_SUITE(" test suite");
    struct tric_context context = { .mode = MODE_SCAN, .suite = &suite, .test = NULL };
    struct tric_context *tric_context = &context;
    pid_t parent = getpid();

    TEST("1", NULL, NULL, NULL) {
        assert(getpid() != parent);
    }
    TEST("2", NULL, NULL, NULL) {
        assert(getpid() != parent);
    }
    TEST("3", NULL, NULL, NULL) {
        assert(getpid() != parent);
    }

    assert(context.test != NULL);
    assert(suite.tests != NULL);
    assert(context.test == suite.tests->next->next);
    assert(strcmp(context.test->description, "3") == 0);
    assert(context.test->id == 3);
    assert(suite.number_of_tests == 3);
    assert(strcmp(suite.tests->description, "1") == 0);
    assert(suite.tests->id == 1);
    assert(strcmp(suite.tests->next->description, "2") == 0);
    assert(suite.tests->next->id == 2);
}



void test_test_fail_before(void) {
    /* failing before function should not execute test */

    struct tric_suite suite = NEW_SUITE(" test suite");
    struct tric_context context = { .mode = MODE_SCAN, .suite = &suite, .test = NULL };
    struct tric_context *tric_context = &context;
    test_before_mock_data = (struct test_fixture_mock_data)TEST_FIXTURE_MOCK_DATA_NEW;
    test_before_mock_data.result = false;
    test_after_mock_data = (struct test_fixture_mock_data)TEST_FIXTURE_MOCK_DATA_NEW;
    pid_t parent = getpid();

    size_t i;
    for (i = 0; i < 2; i++) {
        TEST("test", test_before_mock, test_after_mock, &context) {
            assert(getpid() != parent);
        }
        context.mode = MODE_RESET;
    }

    assert(context.test != NULL);
    assert(suite.tests != NULL);
    assert(context.test == suite.tests);
    assert(strcmp(context.test->description, "test") == 0);
    assert(context.test->id == 1);
    assert(context.test->before == TRIC_FAILURE);
    assert(context.test->result == TRIC_SKIPPED);
    assert(context.test->after == TRIC_SKIPPED);
    assert(suite.executed_tests == 1);
    assert(suite.failed_tests == 1);
    assert(suite.skipped_tests == 0);
}



void test_test_fail_after(void) {
    /* failing after function should not affect test execution */

    struct tric_suite suite = NEW_SUITE(" test suite");
    struct tric_context context = { .mode = MODE_SCAN, .suite = &suite, .test = NULL };
    struct tric_context *tric_context = &context;
    test_before_mock_data = (struct test_fixture_mock_data)TEST_FIXTURE_MOCK_DATA_NEW;
    test_after_mock_data = (struct test_fixture_mock_data)TEST_FIXTURE_MOCK_DATA_NEW;
    test_after_mock_data.result = false;
    pid_t parent = getpid();

    size_t i;
    for (i = 0; i < 2; i++) {
        TEST("test", test_before_mock, test_after_mock, &context) {
            assert(getpid() != parent);
        }
        context.mode = MODE_RESET;
    }

    assert(context.test != NULL);
    assert(suite.tests != NULL);
    assert(context.test == suite.tests);
    assert(strcmp(context.test->description, "test") == 0);
    assert(context.test->id == 1);
    assert(context.test->before == TRIC_OK);
    assert(context.test->result == TRIC_OK);
    assert(context.test->after == TRIC_FAILURE);
    assert(suite.executed_tests == 1);
    assert(suite.failed_tests == 0);
    assert(suite.skipped_tests == 0);
}



void test_test_ok(void) {
    /* test should be marked as successful */

    struct tric_suite suite = NEW_SUITE(" test suite");
    struct tric_context context = { .mode = MODE_SCAN, .suite = &suite, .test = NULL };
    struct tric_context *tric_context = &context;
    test_before_mock_data = (struct test_fixture_mock_data)TEST_FIXTURE_MOCK_DATA_NEW;
    test_after_mock_data = (struct test_fixture_mock_data)TEST_FIXTURE_MOCK_DATA_NEW;
    pid_t parent = getpid();

    size_t i;
    for (i = 0; i < 2; i++) {
        TEST("test", test_before_mock, test_after_mock, &context) {
            assert(getpid() != parent);
        }
        context.mode = MODE_RESET;
    }

    assert(context.test != NULL);
    assert(suite.tests != NULL);
    assert(context.test == suite.tests);
    assert(strcmp(context.test->description, "test") == 0);
    assert(context.test->id == 1);
    assert(context.test->before == TRIC_OK);
    assert(context.test->result == TRIC_OK);
    assert(context.test->after == TRIC_OK);
    assert(suite.executed_tests == 1);
    assert(suite.failed_tests == 0);
    assert(suite.skipped_tests == 0);
}



void test_test_no_fixtures(void) {
    /* only test result should be marked as successful */

    struct tric_suite suite = NEW_SUITE(" test suite");
    struct tric_context context = { .mode = MODE_SCAN, .suite = &suite, .test = NULL };
    struct tric_context *tric_context = &context;
    pid_t parent = getpid();

    size_t i;
    for (i = 0; i < 2; i++) {
        TEST("test", NULL, NULL, NULL) {
            assert(getpid() != parent);
        }
        context.mode = MODE_RESET;
    }

    assert(context.test != NULL);
    assert(suite.tests != NULL);
    assert(context.test == suite.tests);
    assert(strcmp(context.test->description, "test") == 0);
    assert(context.test->id == 1);
    assert(context.test->before == TRIC_UNDEFINED);
    assert(context.test->result == TRIC_OK);
    assert(context.test->after == TRIC_UNDEFINED);
    assert(suite.executed_tests == 1);
    assert(suite.failed_tests == 0);
    assert(suite.skipped_tests == 0);
}



void test_test_crash(void) {
    /* crashed test should have signal set */

    struct tric_suite suite = NEW_SUITE(" test suite");
    struct tric_context context = { .mode = MODE_SCAN, .suite = &suite, .test = NULL };
    struct tric_context *tric_context = &context;
    test_after_mock_data = (struct test_fixture_mock_data)TEST_FIXTURE_MOCK_DATA_NEW;
    pid_t parent = getpid();

    size_t i;
    for (i = 0; i < 2; i++) {
        TEST("test", NULL, test_after_mock, &context) {
            assert(getpid() != parent);
            int *null_pointer = NULL;
            *null_pointer = 1;
        }
        context.mode = MODE_RESET;
    }

    assert(context.test != NULL);
    assert(suite.tests != NULL);
    assert(context.test == suite.tests);
    assert(strcmp(context.test->description, "test") == 0);
    assert(context.test->id == 1);
    assert(context.test->before == TRIC_UNDEFINED);
    assert(context.test->result == TRIC_CRASHED);
    assert(context.test->after == TRIC_CRASHED);
    assert(context.test->signal != 0);
    assert(suite.executed_tests == 1);
    assert(suite.failed_tests == 1);
    assert(suite.skipped_tests == 0);
}



void test_test_fail(void) {
    /* failing test should have line set */

    struct tric_suite suite = NEW_SUITE(" test suite");
    struct tric_context context = { .mode = MODE_SCAN, .suite = &suite, .test = NULL };
    struct tric_context *tric_context = &context;
    test_before_mock_data = (struct test_fixture_mock_data)TEST_FIXTURE_MOCK_DATA_NEW;
    pid_t parent = getpid();

    size_t i;
    for (i = 0; i < 2; i++) {
        TEST("test", test_before_mock, NULL, &context) {
            assert(getpid() != parent);
            ASSERT(1 != 1);
        }
        context.mode = MODE_RESET;
    }

    assert(context.test != NULL);
    assert(suite.tests != NULL);
    assert(context.test == suite.tests);
    assert(strcmp(context.test->description, "test") == 0);
    assert(context.test->id == 1);
    assert(context.test->before == TRIC_OK);
    assert(context.test->result == TRIC_FAILURE);
    assert(context.test->after == TRIC_UNDEFINED);
    assert(context.test->line != 0);
    assert(suite.executed_tests == 1);
    assert(suite.failed_tests == 1);
    assert(suite.skipped_tests == 0);
}



void test_skip_test_scan(void) {
    /* scanned skipped tests should be contained in linked list of test suite */

    struct tric_suite suite = NEW_SUITE(" test suite");
    struct tric_context context = { .mode = MODE_SCAN, .suite = &suite, .test = NULL };
    struct tric_context *tric_context = &context;
    pid_t parent = getpid();

    SKIP_TEST("1", NULL, NULL, NULL) {
        assert(getpid() != parent);
    }
    SKIP_TEST("2", NULL, NULL, NULL) {
        assert(getpid() != parent);
    }
    SKIP_TEST("3", NULL, NULL, NULL) {
        assert(getpid() != parent);
    }

    assert(context.test != NULL);
    assert(suite.tests != NULL);
    assert(context.test == suite.tests->next->next);
    assert(strcmp(context.test->description, "3") == 0);
    assert(context.test->id == 3);
    assert(suite.number_of_tests == 3);
    assert(strcmp(suite.tests->description, "1") == 0);
    assert(suite.tests->id == 1);
    assert(strcmp(suite.tests->next->description, "2") == 0);
    assert(suite.tests->next->id == 2);
}



void test_skip_test_ok(void) {
    /* test should be marked as skipped */

    struct tric_suite suite = NEW_SUITE(" test suite");
    struct tric_context context = { .mode = MODE_SCAN, .suite = &suite, .test = NULL };
    struct tric_context *tric_context = &context;
    test_before_mock_data = (struct test_fixture_mock_data)TEST_FIXTURE_MOCK_DATA_NEW;
    test_after_mock_data = (struct test_fixture_mock_data)TEST_FIXTURE_MOCK_DATA_NEW;
    pid_t parent = getpid();

    size_t i;
    for (i = 0; i < 2; i++) {
        SKIP_TEST("test", test_before_mock, test_after_mock, &context) {
            assert(getpid() != parent);
        }
        context.mode = MODE_RESET;
    }

    assert(context.test != NULL);
    assert(suite.tests != NULL);
    assert(context.test == suite.tests);
    assert(strcmp(context.test->description, "test") == 0);
    assert(context.test->id == 1);
    assert(context.test->before == TRIC_SKIPPED);
    assert(context.test->result == TRIC_SKIPPED);
    assert(context.test->after == TRIC_SKIPPED);
    assert(suite.executed_tests == 0);
    assert(suite.failed_tests == 0);
    assert(suite.skipped_tests == 1);
}



void test_skip_test_fail_before(void) {
    /* failing before function should not affect skipped test */

    struct tric_suite suite = NEW_SUITE(" test suite");
    struct tric_context context = { .mode = MODE_SCAN, .suite = &suite, .test = NULL };
    struct tric_context *tric_context = &context;
    test_before_mock_data = (struct test_fixture_mock_data)TEST_FIXTURE_MOCK_DATA_NEW;
    test_before_mock_data.result = false;
    pid_t parent = getpid();

    size_t i;
    for (i = 0; i < 2; i++) {
        SKIP_TEST("test", test_before_mock, NULL, &context) {
            assert(getpid() != parent);
        }
        context.mode = MODE_RESET;
    }

    assert(context.test != NULL);
    assert(suite.tests != NULL);
    assert(context.test == suite.tests);
    assert(strcmp(context.test->description, "test") == 0);
    assert(context.test->id == 1);
    assert(context.test->before == TRIC_SKIPPED);
    assert(context.test->result == TRIC_SKIPPED);
    assert(context.test->after == TRIC_UNDEFINED);
    assert(suite.executed_tests == 0);
    assert(suite.failed_tests == 0);
    assert(suite.skipped_tests == 1);
}



void test_skip_test_fail_after(void) {
    /* failing after function should not affect skipped test */

    struct tric_suite suite = NEW_SUITE(" test suite");
    struct tric_context context = { .mode = MODE_SCAN, .suite = &suite, .test = NULL };
    struct tric_context *tric_context = &context;
    test_after_mock_data = (struct test_fixture_mock_data)TEST_FIXTURE_MOCK_DATA_NEW;
    test_after_mock_data.result = false;
    pid_t parent = getpid();

    size_t i;
    for (i = 0; i < 2; i++) {
        SKIP_TEST("test", NULL, test_after_mock, &context) {
            assert(getpid() != parent);
        }
        context.mode = MODE_RESET;
    }

    assert(context.test != NULL);
    assert(suite.tests != NULL);
    assert(context.test == suite.tests);
    assert(strcmp(context.test->description, "test") == 0);
    assert(context.test->id == 1);
    assert(context.test->before == TRIC_UNDEFINED);
    assert(context.test->result == TRIC_SKIPPED);
    assert(context.test->after == TRIC_SKIPPED);
    assert(suite.executed_tests == 0);
    assert(suite.failed_tests == 0);
    assert(suite.skipped_tests == 1);
}



void test_skip_test_no_fixtures(void) {
    /* only test result should be marked as skipped */

    struct tric_suite suite = NEW_SUITE(" test suite");
    struct tric_context context = { .mode = MODE_SCAN, .suite = &suite, .test = NULL };
    struct tric_context *tric_context = &context;
    pid_t parent = getpid();

    size_t i;
    for (i = 0; i < 2; i++) {
        SKIP_TEST("test", NULL, NULL, NULL) {
            assert(getpid() != parent);
        }
        context.mode = MODE_RESET;
    }

    assert(context.test != NULL);
    assert(suite.tests != NULL);
    assert(context.test == suite.tests);
    assert(strcmp(context.test->description, "test") == 0);
    assert(context.test->id == 1);
    assert(context.test->before == TRIC_UNDEFINED);
    assert(context.test->result == TRIC_SKIPPED);
    assert(context.test->after == TRIC_UNDEFINED);
    assert(suite.executed_tests == 0);
    assert(suite.failed_tests == 0);
    assert(suite.skipped_tests == 1);
}



void test_assert_fail(char *argv0) {
    /* failing assert should exit subprocess */

    struct tric_suite suite;
    struct tric_context context = { .suite = &suite };
    context.self = open(argv0, O_RDONLY);
    assert(context.self != -1);
    struct tric_context *tric_context = &context;
    int status = 0;

    size_t line_before = __LINE__;
    pid_t child = fork();
    assert(child != -1);
    if (child) {
        waitpid(child, &status, 0);
    } else {
        ASSERT(0 > 1);
        _exit(EXIT_OK);
    }

    assert(lseek(context.self, 0, SEEK_CUR) == line_before + 6);
    assert(WEXITSTATUS(status) == EXIT_TEST_FAILURE);

    close(context.self);
}



void test_assert_pass(char *argv0) {
    /* successful assert should not exit subprocess */

    struct tric_suite suite;
    struct tric_context context = { .suite = &suite };
    context.self = open(argv0, O_RDONLY);
    assert(context.self != -1);
    struct tric_context *tric_context = &context;
    int status = 0;

    pid_t child = fork();
    assert(child != -1);
    if (child) {
        waitpid(child, &status, 0);
    } else {
        ASSERT(0 < 1);
        _exit(EXIT_OK);
    }

    assert(lseek(context.self, 0, SEEK_CUR) == 0);
    assert(WEXITSTATUS(status) == EXIT_OK);

    close(context.self);
}



void test_fixture_scan(void) {
    /* code in fixture should not execute when scanning for tests */

    struct tric_context context = { .mode = MODE_SCAN };
    struct tric_context *tric_context = &context;
    bool test = true;

    FIXTURE("test") {
        test = false;
    }

    assert(test == true);
}



void test_fixture_execute(void) {
    /* code in fixture should not execute when executing tests */

    struct tric_context context = { .mode = MODE_EXECUTE };
    struct tric_context *tric_context = &context;
    bool test = true;

    FIXTURE("test") {
        test = false;
    }

    assert(test == true);
}



void test_fixture_reset(void) {
    /* code in fixture should execute when resetting */

    struct tric_context context = { .mode = MODE_RESET };
    struct tric_context *tric_context = &context;
    bool test = false;

    FIXTURE("test") {
        test = true;
    }

    assert(test == true);
}



void test_add_test_not(void) {
    /* do not add tests when not scanning */

    struct tric_test test;
    struct tric_suite suite = { .number_of_tests = 0, .tests = NULL };
    struct tric_context context = { .mode = MODE_RESET, .suite = &suite, .test = NULL };

    tric_add_test(&context, &test);

    assert(suite.number_of_tests == 0);
    assert(suite.tests == NULL);
    assert(context.test == NULL);
    assert(context.mode == MODE_RESET);
}



void test_add_test_first(void) {
    /* add the first test to the list of tests */

    struct tric_test test = { .id = 0, .next = NULL };
    struct tric_suite suite = { .number_of_tests = 0, .tests = NULL };
    struct tric_context context = { .mode = MODE_SCAN, .suite = &suite, .test = NULL };

    tric_add_test(&context, &test);

    assert(test.id == 1);
    assert(test.next == NULL);
    assert(suite.number_of_tests == 1);
    assert(suite.tests == &test);
    assert(context.test == &test);
}



void test_add_test_second(void) {
    /* add an additional test to the list of tests */

    struct tric_test first = { .id = 1, .next = NULL };
    struct tric_test second = { .id = 0, .next = NULL };
    struct tric_suite suite = { .number_of_tests = 1, .tests = &first };
    struct tric_context context = { .mode = MODE_SCAN, .suite = &suite, .test = &first };

    tric_add_test(&context, &second);

    assert(first.id == 1);
    assert(first.next == &second);
    assert(second.id == 2);
    assert(second.next == NULL);
    assert(suite.number_of_tests == 2);
    assert(suite.tests == &first);
    assert((suite.tests)->next == &second);
    assert(context.suite == &suite);
    assert(context.test == &second);
}



void test_reset_context_not(void) {
    /* do not reset context when scanning for tests */

    struct tric_test test;
    struct tric_context context = { .mode = MODE_SCAN, .test = NULL };

    tric_reset_context(&context, &test);

    assert(context.mode == MODE_SCAN);
    assert(context.test == NULL);
}



void test_reset_context_run(char *argv0){
    /* resetting the context must set mode to execute */

    int self = open(argv0, O_RDONLY);
    assert(self != -1);
    assert(lseek(self, 1, SEEK_SET) == 1);
    struct tric_test test;
    struct tric_context context = { .self = self, .mode = MODE_RESET, .test = NULL };

    tric_reset_context(&context, &test);

    assert(lseek(self, 0, SEEK_CUR) == 0);
    assert(context.test == &test);
    assert(context.mode == MODE_EXECUTE);

    close(self);
}



void test_status_ok_all(void) {
    /* all result states should be set */

    struct tric_test test = { .before = TRIC_UNDEFINED, .result = TRIC_UNDEFINED, .after = TRIC_UNDEFINED };
    struct tric_context context = { .test = &test };

    tric_status_ok(&context, true, true);

    assert(test.before == TRIC_OK);
    assert(test.result == TRIC_OK);
    assert(test.after == TRIC_OK);
}



void test_status_ok_none(void) {
    /* only test result state should be set */

    struct tric_test test = { .before = TRIC_OK, .result = TRIC_UNDEFINED, .after = TRIC_OK };
    struct tric_context context = { .test = &test };

    tric_status_ok(&context, false, false);

    assert(test.before == TRIC_UNDEFINED);
    assert(test.result == TRIC_OK);
    assert(test.after == TRIC_UNDEFINED);
}



void test_status_ok_before(void) {
    /* before result state should be set */

    struct tric_test test = { .before = TRIC_UNDEFINED, .result = TRIC_UNDEFINED, .after = TRIC_OK };
    struct tric_context context = { .test = &test };

    tric_status_ok(&context, true, false);

    assert(test.before == TRIC_OK);
    assert(test.result == TRIC_OK);
    assert(test.after == TRIC_UNDEFINED);
}



void test_status_ok_after(void) {
    /* after result state should be set */

    struct tric_test test = { .before = TRIC_OK, .result = TRIC_UNDEFINED, .after = TRIC_UNDEFINED };
    struct tric_context context = { .test = &test };

    tric_status_ok(&context, false, true);

    assert(test.before == TRIC_UNDEFINED);
    assert(test.result == TRIC_OK);
    assert(test.after == TRIC_OK);
}



void test_status_fail_all(char *argv0) {
    /* all result states should be set */

    int self = open(argv0, O_RDONLY);
    assert(self != -1);
    assert(lseek(self, 1, SEEK_SET) == 1);
    struct tric_suite suite = { .failed_tests = 0 };
    struct tric_test test = { .before = TRIC_UNDEFINED, .result = TRIC_UNDEFINED, .after = TRIC_UNDEFINED, .line = 0 };
    struct tric_context context = { .self = self, .suite = &suite, .test = &test };

    tric_status_fail(&context, true, true);
    close(self);

    assert(suite.failed_tests == 1);
    assert(test.line == 1);
    assert(test.before == TRIC_OK);
    assert(test.result == TRIC_FAILURE);
    assert(test.after == TRIC_SKIPPED);
}



void test_status_fail_none(char *argv0) {
    /* only test result state should be set */

    int self = open(argv0, O_RDONLY);
    assert(self != -1);
    struct tric_suite suite = { .failed_tests = 0 };
    struct tric_test test = { .before = TRIC_OK, .result = TRIC_UNDEFINED, .after = TRIC_OK, .line = 0 };
    struct tric_context context = { .self = self, .suite = &suite, .test = &test };

    tric_status_fail(&context, false, false);
    close(self);

    assert(test.before == TRIC_UNDEFINED);
    assert(test.result == TRIC_FAILURE);
    assert(test.after == TRIC_UNDEFINED);
}



void test_status_fail_before(char *argv0) {
    /* before result state should be set */

    int self = open(argv0, O_RDONLY);
    assert(self != -1);
    struct tric_suite suite = { .failed_tests = 0 };
    struct tric_test test = { .before = TRIC_UNDEFINED, .result = TRIC_UNDEFINED, .after = TRIC_OK, .line = 0 };
    struct tric_context context = { .self = self, .suite = &suite, .test = &test };

    tric_status_fail(&context, true, false);
    close(self);

    assert(test.before == TRIC_OK);
    assert(test.result == TRIC_FAILURE);
    assert(test.after == TRIC_UNDEFINED);
}



void test_status_fail_after(char *argv0) {
    /* after result state should be set */

    int self = open(argv0, O_RDONLY);
    assert(self != -1);
    struct tric_suite suite = { .failed_tests = 0 };
    struct tric_test test = { .before = TRIC_OK, .result = TRIC_UNDEFINED, .after = TRIC_UNDEFINED, .line = 0 };
    struct tric_context context = { .self = self, .suite = &suite, .test = &test };

    tric_status_fail(&context, false, true);
    close(self);

    assert(test.before == TRIC_UNDEFINED);
    assert(test.result == TRIC_FAILURE);
    assert(test.after == TRIC_SKIPPED);
}



void test_status_fail_before_all(void) {
    /* all result states should be set */

    struct tric_suite suite = { .failed_tests = 0 };
    struct tric_test test = { .before = TRIC_UNDEFINED, .result = TRIC_UNDEFINED, .after = TRIC_UNDEFINED };
    struct tric_context context = { .suite = &suite, .test = &test };

    tric_status_fail_before(&context, true, true);

    assert(suite.failed_tests == 1);
    assert(test.before == TRIC_FAILURE);
    assert(test.result == TRIC_SKIPPED);
    assert(test.after == TRIC_SKIPPED);
}



void test_status_fail_before_none(void) {
    /* test and before result state should be set */

    /* note that this situation will never happen */

    struct tric_suite suite = { .failed_tests = 0 };
    struct tric_test test = { .before = TRIC_UNDEFINED, .result = TRIC_UNDEFINED, .after = TRIC_OK };
    struct tric_context context = { .suite = &suite, .test = &test };

    tric_status_fail_before(&context, false, false);

    assert(test.before == TRIC_FAILURE);
    assert(test.result == TRIC_SKIPPED);
    assert(test.after == TRIC_UNDEFINED);
}



void test_status_fail_before_before(void) {
    /* test and before result state should be set */

    struct tric_suite suite = { .failed_tests = 0 };
    struct tric_test test = { .before = TRIC_UNDEFINED, .result = TRIC_UNDEFINED, .after = TRIC_OK };
    struct tric_context context = { .suite = &suite, .test = &test };

    tric_status_fail_before(&context, true, false);

    assert(test.before == TRIC_FAILURE);
    assert(test.result == TRIC_SKIPPED);
    assert(test.after == TRIC_UNDEFINED);
}



void test_status_fail_before_after(void) {
    /* all result states should be set */

    /* note that this situation will never happen */

    struct tric_suite suite = { .failed_tests = 0 };
    struct tric_test test = { .before = TRIC_UNDEFINED, .result = TRIC_UNDEFINED, .after = TRIC_UNDEFINED };
    struct tric_context context = { .suite = &suite, .test = &test };

    tric_status_fail_before(&context, false, true);

    assert(test.before == TRIC_FAILURE);
    assert(test.result == TRIC_SKIPPED);
    assert(test.after == TRIC_SKIPPED);
}



void test_status_fail_after_all(void) {
    /* all result states should be set */

    struct tric_test test = { .before = TRIC_UNDEFINED, .result = TRIC_UNDEFINED, .after = TRIC_UNDEFINED };
    struct tric_context context = { .test = &test };

    tric_status_fail_after(&context, true, true);

    assert(test.before == TRIC_OK);
    assert(test.result == TRIC_OK);
    assert(test.after == TRIC_FAILURE);
}



void test_status_fail_after_none(void) {
    /* test and after result state should be set */

    /* note that this situation will never happen */

    struct tric_test test = { .before = TRIC_OK, .result = TRIC_UNDEFINED, .after = TRIC_UNDEFINED };
    struct tric_context context = { .test = &test };

    tric_status_fail_after(&context, false, false);

    assert(test.before == TRIC_UNDEFINED);
    assert(test.result == TRIC_OK);
    assert(test.after == TRIC_FAILURE);
}



void test_status_fail_after_before(void) {
    /* all result states should be set */

    /* note that this situation will never happen */

    struct tric_test test = { .before = TRIC_UNDEFINED, .result = TRIC_UNDEFINED, .after = TRIC_UNDEFINED };
    struct tric_context context = { .test = &test };

    tric_status_fail_after(&context, true, false);

    assert(test.before == TRIC_OK);
    assert(test.result == TRIC_OK);
    assert(test.after == TRIC_FAILURE);
}



void test_status_fail_after_after(void) {
    /* test and after result state should be set */

    struct tric_test test = { .before = TRIC_OK, .result = TRIC_UNDEFINED, .after = TRIC_UNDEFINED };
    struct tric_context context = { .test = &test };

    tric_status_fail_after(&context, false, true);

    assert(test.before == TRIC_UNDEFINED);
    assert(test.result == TRIC_OK);
    assert(test.after == TRIC_FAILURE);
}



void test_status_crash_all(void) {
    /* all result states should be set */

    struct tric_suite suite = { .failed_tests = 0 };
    struct tric_test test = { .before = TRIC_UNDEFINED, .result = TRIC_UNDEFINED, .after = TRIC_UNDEFINED };
    struct tric_context context = { .suite = &suite, .test = &test };

    tric_status_crash(&context, true, true);

    assert(suite.failed_tests == 1);
    assert(test.before == TRIC_CRASHED);
    assert(test.result == TRIC_CRASHED);
    assert(test.after == TRIC_CRASHED);
}



void test_status_crash_none(void) {
    /* only test result state should be set */

    struct tric_suite suite = { .failed_tests = 0 };
    struct tric_test test = { .before = TRIC_OK, .result = TRIC_UNDEFINED, .after = TRIC_OK };
    struct tric_context context = { .suite = &suite, .test = &test };

    tric_status_crash(&context, false, false);

    assert(test.before == TRIC_UNDEFINED);
    assert(test.result == TRIC_CRASHED);
    assert(test.after == TRIC_UNDEFINED);
}



void test_status_crash_before(void) {
    /* before result state should be set */

    struct tric_suite suite = { .failed_tests = 0 };
    struct tric_test test = { .before = TRIC_UNDEFINED, .result = TRIC_UNDEFINED, .after = TRIC_OK };
    struct tric_context context = { .suite = &suite, .test = &test };

    tric_status_crash(&context, true, false);

    assert(test.before == TRIC_CRASHED);
    assert(test.result == TRIC_CRASHED);
    assert(test.after == TRIC_UNDEFINED);
}



void test_status_crash_after(void) {
    /* after result state should be set */

    struct tric_suite suite = { .failed_tests = 0 };
    struct tric_test test = { .before = TRIC_OK, .result = TRIC_UNDEFINED, .after = TRIC_UNDEFINED };
    struct tric_context context = { .suite = &suite, .test = &test };

    tric_status_crash(&context, false, true);

    assert(test.before == TRIC_UNDEFINED);
    assert(test.result == TRIC_CRASHED);
    assert(test.after == TRIC_CRASHED);
}



void test_status_fail_fork_all(void) {
    /* all result states should be undefined */

    struct tric_test test = { .before = TRIC_OK, .result = TRIC_OK, .after = TRIC_OK };
    struct tric_suite suite = { .skipped_tests = 0 };
    struct tric_context context = { .suite = &suite, .test = &test };

    tric_status_fail_fork(&context, true, true);

    assert(suite.skipped_tests == 1);
    assert(test.before == TRIC_UNDEFINED);
    assert(test.result == TRIC_UNDEFINED);
    assert(test.after == TRIC_UNDEFINED);
}



void test_status_fail_fork_none(void) {
    /* all result states should be undefined */

    struct tric_test test = { .before = TRIC_OK, .result = TRIC_OK, .after = TRIC_OK };
    struct tric_suite suite = { .skipped_tests = 0 };
    struct tric_context context = { .suite = &suite, .test = &test };

    tric_status_fail_fork(&context, false, false);

    assert(suite.skipped_tests == 1);
    assert(test.before == TRIC_UNDEFINED);
    assert(test.result == TRIC_UNDEFINED);
    assert(test.after == TRIC_UNDEFINED);
}



void test_status_fail_fork_before(void) {
    /* all result states should be undefined */

    struct tric_test test = { .before = TRIC_OK, .result = TRIC_OK, .after = TRIC_OK };
    struct tric_suite suite = { .skipped_tests = 0 };
    struct tric_context context = { .suite = &suite, .test = &test };

    tric_status_fail_fork(&context, true, false);

    assert(suite.skipped_tests == 1);
    assert(test.before == TRIC_UNDEFINED);
    assert(test.result == TRIC_UNDEFINED);
    assert(test.after == TRIC_UNDEFINED);
}



void test_status_fail_fork_after(void) {
    /* all result states should be undefined */

    struct tric_test test = { .before = TRIC_OK, .result = TRIC_OK, .after = TRIC_OK };
    struct tric_suite suite = { .skipped_tests = 0 };
    struct tric_context context = { .suite = &suite, .test = &test };

    tric_status_fail_fork(&context, false, true);

    assert(suite.skipped_tests == 1);
    assert(test.before == TRIC_UNDEFINED);
    assert(test.result == TRIC_UNDEFINED);
    assert(test.after == TRIC_UNDEFINED);
}



void test_status_skip_all(void) {
    /* all result states should be set */

    struct tric_test test = { .before = TRIC_UNDEFINED, .result = TRIC_UNDEFINED, .after = TRIC_UNDEFINED };
    struct tric_suite suite = { .skipped_tests = 0 };
    struct tric_context context = { .suite = &suite, .test = &test };

    tric_status_skip(&context, true, true);

    assert(suite.skipped_tests == 1);
    assert(test.before == TRIC_SKIPPED);
    assert(test.result == TRIC_SKIPPED);
    assert(test.after == TRIC_SKIPPED);
}



void test_status_skip_none(void) {
    /* only test result state should be set */

    struct tric_test test = { .before = TRIC_OK, .result = TRIC_UNDEFINED, .after = TRIC_OK };
    struct tric_suite suite = { .skipped_tests = 0 };
    struct tric_context context = { .suite = &suite, .test = &test };

    tric_status_skip(&context, false, false);

    assert(suite.skipped_tests == 1);
    assert(test.before == TRIC_UNDEFINED);
    assert(test.result == TRIC_SKIPPED);
    assert(test.after == TRIC_UNDEFINED);
}



void test_status_skip_before(void) {
    /* before result state should be set */

    struct tric_test test = { .before = TRIC_UNDEFINED, .result = TRIC_UNDEFINED, .after = TRIC_OK };
    struct tric_suite suite = { .skipped_tests = 0 };
    struct tric_context context = { .suite = &suite, .test = &test };

    tric_status_skip(&context, true, false);

    assert(suite.skipped_tests == 1);
    assert(test.before == TRIC_SKIPPED);
    assert(test.result == TRIC_SKIPPED);
    assert(test.after == TRIC_UNDEFINED);
}



void test_status_skip_after(void) {
    /* after result state should be set */

    struct tric_test test = { .before = TRIC_OK, .result = TRIC_UNDEFINED, .after = TRIC_UNDEFINED };
    struct tric_suite suite = { .skipped_tests = 0 };
    struct tric_context context = { .suite = &suite, .test = &test };

    tric_status_skip(&context, false, true);

    assert(suite.skipped_tests == 1);
    assert(test.before == TRIC_UNDEFINED);
    assert(test.result == TRIC_SKIPPED);
    assert(test.after == TRIC_SKIPPED);
}



void test_set_status_ok(void) {
    /* ok status should be called */

    struct tric_test test = { .before = TRIC_UNDEFINED, .result = TRIC_UNDEFINED, .after = TRIC_UNDEFINED };
    struct tric_context context = { .test = &test };

    tric_set_status(&context, EXIT_OK, true, true);

    assert(test.before == TRIC_OK);
    assert(test.result == TRIC_OK);
    assert(test.after == TRIC_OK);
}



void test_set_status_test_failure(char *argv0) {
    /* fail test status should be called */

    int self = open(argv0, O_RDONLY);
    assert(self != -1);
    struct tric_suite suite = { .failed_tests = 0 };
    struct tric_test test = { .before = TRIC_UNDEFINED, .result = TRIC_UNDEFINED, .after = TRIC_UNDEFINED, .line = 0 };
    struct tric_context context = { .self = self, .suite = &suite, .test = &test };

    tric_set_status(&context, EXIT_TEST_FAILURE, true, true);
    close(self);

    assert(suite.failed_tests == 1);
    assert(test.before == TRIC_OK);
    assert(test.result == TRIC_FAILURE);
    assert(test.after == TRIC_SKIPPED);
}



void test_set_status_before_failure(void) {
    /* fail before status should be called */

    struct tric_suite suite = { .failed_tests = 0 };
    struct tric_test test = { .before = TRIC_UNDEFINED, .result = TRIC_UNDEFINED, .after = TRIC_UNDEFINED };
    struct tric_context context = { .suite = &suite, .test = &test };

    tric_set_status(&context, EXIT_BEFORE_FAILURE, true, true);

    assert(suite.failed_tests == 1);
    assert(test.before == TRIC_FAILURE);
    assert(test.result == TRIC_SKIPPED);
    assert(test.after == TRIC_SKIPPED);
}



void test_set_status_after_failure(void) {
    /* fail after status should be called */

    struct tric_test test = { .before = TRIC_UNDEFINED, .result = TRIC_UNDEFINED, .after = TRIC_UNDEFINED };
    struct tric_context context = { .test = &test };

    tric_set_status(&context, EXIT_AFTER_FAILURE, true, true);

    assert(test.before == TRIC_OK);
    assert(test.result == TRIC_OK);
    assert(test.after == TRIC_FAILURE);
}



void test_set_status_signal(void) {
    /* crash status should be called */

    struct tric_suite suite = { .failed_tests = 0 };
    struct tric_test test = { .before = TRIC_UNDEFINED, .result = TRIC_UNDEFINED, .after = TRIC_UNDEFINED };
    struct tric_context context = { .suite = &suite, .test = &test };

    tric_set_status(&context, EXIT_SIGNAL, true, true);

    assert(suite.failed_tests == 1);
    assert(test.before == TRIC_CRASHED);
    assert(test.result == TRIC_CRASHED);
    assert(test.after == TRIC_CRASHED);
}



void test_set_status_fork(void) {
    /* fail fork status should be called */

    struct tric_test test = { .before = TRIC_OK, .result = TRIC_OK, .after = TRIC_OK };
    struct tric_suite suite = { .skipped_tests = 0 };
    struct tric_context context = { .suite = &suite, .test = &test };

    tric_set_status(&context, EXIT_FORK, true, true);

    assert(suite.skipped_tests == 1);
    assert(test.before == TRIC_UNDEFINED);
    assert(test.result == TRIC_UNDEFINED);
    assert(test.after == TRIC_UNDEFINED);
}



void test_set_status_skip(void) {
    /* skip status should be called */

    struct tric_test test = { .before = TRIC_UNDEFINED, .result = TRIC_UNDEFINED, .after = TRIC_UNDEFINED };
    struct tric_suite suite = { .skipped_tests = 0 };
    struct tric_context context = { .suite = &suite, .test = &test };

    tric_set_status(&context, EXIT_SKIP, true, true);

    assert(suite.skipped_tests == 1);
    assert(test.before == TRIC_SKIPPED);
    assert(test.result == TRIC_SKIPPED);
    assert(test.after == TRIC_SKIPPED);
}



void test_run_test_not(void) {
    /* test should not run when scanning for tests */

    struct tric_context context = { .mode = MODE_SCAN };

    tric_run_test(&context, true, true);

    assert(context.mode == MODE_SCAN);
}



void test_run_test_ok(void) {
    /* test should run successfully */

    struct tric_suite suite = { .executed_tests = 0 };
    struct tric_test test = { .before = TRIC_UNDEFINED, .result = TRIC_UNDEFINED, .after = TRIC_OK };
    struct tric_context context = { .mode = MODE_EXECUTE, .suite = &suite, .test = &test };
    tric_log(NULL, test_log_test_mock, NULL, &context);
    test_log_test_mock_data = (struct test_logger_mock_data)TEST_LOGGER_MOCK_DATA_NEW;
    pid_t parent = getpid();

    tric_run_test(&context, true, false);
    if (context.mode == MODE_EXECUTE) {
        assert(getpid() != parent);
        _exit(EXIT_OK);
    }

    assert(context.mode == MODE_RESET);
    assert(suite.executed_tests == 1);
    assert(test.before == TRIC_OK);
    assert(test.result == TRIC_OK);
    assert(test.after == TRIC_UNDEFINED);
    assert(test_log_test_mock_data.count == 1);
    assert(test_log_test_mock_data.suite == &suite);
    assert(test_log_test_mock_data.test == &test);
    assert(test_log_test_mock_data.data == &context);
}



void test_run_test_signal(void) {
    /* test should crash */

    struct tric_suite suite = { .executed_tests = 0, .failed_tests = 0 };
    struct tric_test test = { .before = TRIC_UNDEFINED, .result = TRIC_UNDEFINED, .after = TRIC_UNDEFINED, .signal = 0 };
    struct tric_context context = { .mode = MODE_EXECUTE, .suite = &suite, .test = &test };
    tric_log(NULL, test_log_test_mock, NULL, &context);
    test_log_test_mock_data = (struct test_logger_mock_data)TEST_LOGGER_MOCK_DATA_NEW;
    pid_t parent = getpid();

    tric_run_test(&context, true, true);
    if (context.mode == MODE_EXECUTE) {
        assert(getpid() != parent);
        int *void_pointer = NULL;
        *void_pointer += 1;
        _exit(EXIT_OK);
    }

    assert(context.mode == MODE_RESET);
    assert(suite.executed_tests == 1);
    assert(suite.failed_tests == 1);
    assert(test.before == TRIC_CRASHED);
    assert(test.result == TRIC_CRASHED);
    assert(test.after == TRIC_CRASHED);
    assert(test.signal == SIGSEGV);
    assert(test_log_test_mock_data.count == 1);
    assert(test_log_test_mock_data.suite == &suite);
    assert(test_log_test_mock_data.test == &test);
    assert(test_log_test_mock_data.data == &context);
}



void test_run_before_not(void) {
    /* before function should not run */

    struct tric_context context = { .mode = MODE_SCAN };
    test_before_mock_data = (struct test_fixture_mock_data)TEST_FIXTURE_MOCK_DATA_NEW;

    tric_run_before(&context, test_before_mock, &context);

    assert(test_before_mock_data.count == 0);
    assert(test_before_mock_data.data == NULL);
}



void test_run_before_null(void) {
    /* nothing should run */

    struct tric_context context = { .mode = MODE_EXECUTE };

    int status;
    pid_t child = fork();
    assert(child != -1);
    if (child) {
        waitpid(child, &status, 0);
    } else {
        tric_run_before(&context, NULL, NULL);
        _exit(EXIT_OK);
    }

    assert(WEXITSTATUS(status) == EXIT_OK);
}



void test_run_before_ok(void) {
    /* before function should run */

    struct tric_context context = { .mode = MODE_EXECUTE };
    test_before_mock_data = (struct test_fixture_mock_data)TEST_FIXTURE_MOCK_DATA_NEW;

    tric_run_before(&context, test_before_mock, &context);

    assert(test_before_mock_data.count == 1);
    assert(test_before_mock_data.data == &context);
}



void test_run_before_fail(void) {
    /* failing before function should exit with failure status */

    struct tric_context context = { .mode = MODE_EXECUTE };
    test_before_mock_data = (struct test_fixture_mock_data)TEST_FIXTURE_MOCK_DATA_NEW;
    test_before_mock_data.result = false;

    int status;
    pid_t child = fork();
    assert(child != -1);
    if (child) {
        waitpid(child, &status, 0);
    } else {
        tric_run_before(&context, test_before_mock, NULL);
        _exit(EXIT_OK);
    }

    assert(WEXITSTATUS(status) == EXIT_BEFORE_FAILURE);
}



void test_run_after_null(void) {
    /* function should exit successfully */

    int status;
    pid_t child = fork();
    assert(child != -1);
    if (child) {
        waitpid(child, &status, 0);
    } else {
        tric_run_after(NULL, NULL);
        _exit(EXIT_AFTER_FAILURE);
    }

    assert(WEXITSTATUS(status) == EXIT_OK);
}



void test_run_after_ok(void) {
    /* function should exit successfully */

    test_after_mock_data = (struct test_fixture_mock_data)TEST_FIXTURE_MOCK_DATA_NEW;

    int status;
    pid_t child = fork();
    assert(child != -1);
    if (child) {
        waitpid(child, &status, 0);
    } else {
        tric_run_after(test_after_mock, NULL);
        _exit(EXIT_AFTER_FAILURE);
    }

    assert(WEXITSTATUS(status) == EXIT_OK);
}



void test_run_after_fail(void) {
    /* function should exit failure status */

    test_after_mock_data = (struct test_fixture_mock_data)TEST_FIXTURE_MOCK_DATA_NEW;
    test_after_mock_data.result = false;

    int status;
    pid_t child = fork();
    assert(child != -1);
    if (child) {
        waitpid(child, &status, 0);
    } else {
        tric_run_after(test_after_mock, NULL);
        _exit(EXIT_OK);
    }

    assert(WEXITSTATUS(status) == EXIT_AFTER_FAILURE);
}



void test_skip_test_execution_not(void) {
    /* status should not be set */

    struct tric_suite suite = { .skipped_tests = 0 };
    struct tric_test test = { .before = TRIC_UNDEFINED, .result = TRIC_UNDEFINED, .after = TRIC_UNDEFINED };
    struct tric_context context = { .mode = MODE_SCAN, .suite = &suite, .test = &test };

    tric_skip_test_execution(&context, NULL, NULL);

    assert(suite.skipped_tests == 0);
    assert(test.before == TRIC_UNDEFINED);
    assert(test.result == TRIC_UNDEFINED);
    assert(test.after == TRIC_UNDEFINED);
    assert(context.mode == MODE_SCAN);
}



void test_skip_test_execution_ok(void) {
    /* skipped status should be set and mode should be reset */

    struct tric_suite suite = { .skipped_tests = 0 };
    struct tric_test test = { .before = TRIC_UNDEFINED, .result = TRIC_UNDEFINED, .after = TRIC_UNDEFINED };
    struct tric_context context = { .mode = MODE_EXECUTE, .suite = &suite, .test = &test };
    test_before_mock_data = (struct test_fixture_mock_data)TEST_FIXTURE_MOCK_DATA_NEW;
    test_after_mock_data = (struct test_fixture_mock_data)TEST_FIXTURE_MOCK_DATA_NEW;
    tric_log(NULL, test_log_test_mock, NULL, &context);
    test_log_test_mock_data = (struct test_logger_mock_data)TEST_LOGGER_MOCK_DATA_NEW;

    tric_skip_test_execution(&context, test_before_mock, test_after_mock);

    assert(suite.skipped_tests == 1);
    assert(test.before == TRIC_SKIPPED);
    assert(test.result == TRIC_SKIPPED);
    assert(test.after == TRIC_SKIPPED);
    assert(test_before_mock_data.count == 0);
    assert(test_after_mock_data.count == 0);
    assert(test_log_test_mock_data.count == 1);
    assert(test_log_test_mock_data.suite == &suite);
    assert(test_log_test_mock_data.test == &test);
    assert(test_log_test_mock_data.data == &context);
    assert(context.mode == MODE_RESET);
}



void test_reporting_all(void) {
    /* all logging functions should be set */

    /* ensure default configuration */
    tric_reporting(true, tric_log_start, tric_log_test, tric_log_end, NULL);
    assert(tric_reporting(false, NULL, NULL, NULL, NULL)->start == tric_log_start);
    assert(tric_reporting(false, NULL, NULL, NULL, NULL)->test == tric_log_test);
    assert(tric_reporting(false, NULL, NULL, NULL, NULL)->end == tric_log_end);
    assert(tric_reporting(false, NULL, NULL, NULL, NULL)->data == NULL);

    void *test_reference = NULL;
    tric_reporting(true, test_log_start_mock, test_log_test_mock, test_log_end_mock, &test_reference);

    assert(tric_reporting(false, NULL, NULL, NULL, NULL)->start == test_log_start_mock);
    assert(tric_reporting(false, NULL, NULL, NULL, NULL)->test == test_log_test_mock);
    assert(tric_reporting(false, NULL, NULL, NULL, NULL)->end == test_log_end_mock);
    assert(tric_reporting(false, NULL, NULL, NULL, NULL)->data == &test_reference);
}



void test_reporting_none(void) {
    /* logging functions should be set to no logger */

    /* ensure default configuration */
    tric_reporting(true, tric_log_start, tric_log_test, tric_log_end, NULL);
    assert(tric_reporting(false, NULL, NULL, NULL, NULL)->start == tric_log_start);
    assert(tric_reporting(false, NULL, NULL, NULL, NULL)->test == tric_log_test);
    assert(tric_reporting(false, NULL, NULL, NULL, NULL)->end == tric_log_end);
    assert(tric_reporting(false, NULL, NULL, NULL, NULL)->data == NULL);

    tric_reporting(true, NULL, NULL, NULL, NULL);

    assert(tric_reporting(false, NULL, NULL, NULL, NULL)->start == tric_log_nothing);
    assert(tric_reporting(false, NULL, NULL, NULL, NULL)->test == tric_log_nothing);
    assert(tric_reporting(false, NULL, NULL, NULL, NULL)->end == tric_log_nothing);
    assert(tric_reporting(false, NULL, NULL, NULL, NULL)->data == NULL);
}



void test_report(void) {
    /* logger functions should be accessible */

    /* ensure default configuration */
    tric_reporting(true, tric_log_start, tric_log_test, tric_log_end, NULL);
    assert(tric_reporting(false, NULL, NULL, NULL, NULL)->start == tric_log_start);
    assert(tric_reporting(false, NULL, NULL, NULL, NULL)->test == tric_log_test);
    assert(tric_reporting(false, NULL, NULL, NULL, NULL)->end == tric_log_end);
    assert(tric_reporting(false, NULL, NULL, NULL, NULL)->data == NULL);

    struct tric_reporting_data *result = tric_report();

    assert(result->start == tric_log_start);
    assert(result->test == tric_log_test);
    assert(result->end == tric_log_end);
    assert(result->data == NULL);
}



void test_log(void) {
    /* all logger functions should be set */

    /* ensure default configuration */
    tric_reporting(true, tric_log_start, tric_log_test, tric_log_end, NULL);
    assert(tric_reporting(false, NULL, NULL, NULL, NULL)->start == tric_log_start);
    assert(tric_reporting(false, NULL, NULL, NULL, NULL)->test == tric_log_test);
    assert(tric_reporting(false, NULL, NULL, NULL, NULL)->end == tric_log_end);
    assert(tric_reporting(false, NULL, NULL, NULL, NULL)->data == NULL);

    void *test_reference = NULL;
    tric_log(test_log_start_mock, test_log_test_mock, test_log_end_mock, &test_reference);

    assert(tric_reporting(false, NULL, NULL, NULL, NULL)->start == test_log_start_mock);
    assert(tric_reporting(false, NULL, NULL, NULL, NULL)->test == test_log_test_mock);
    assert(tric_reporting(false, NULL, NULL, NULL, NULL)->end == test_log_end_mock);
    assert(tric_reporting(false, NULL, NULL, NULL, NULL)->data == &test_reference);
}



void test_run_fixture_not(void) {
    // no fixture to execute should be ok */

    bool result = tric_run_fixture(NULL, NULL);

    assert(result == true);
}



void test_run_fixture_ok(void) {
    /* successfully run fixture should return true */

    test_setup_mock_data = (struct test_fixture_mock_data)TEST_FIXTURE_MOCK_DATA_NEW;

    void *test_reference = NULL;
    bool result = tric_run_fixture(test_setup_mock, &test_reference);

    assert(result == true);
    assert(test_setup_mock_data.count == 1);
    assert(test_setup_mock_data.data == &test_reference);
}



void test_run_fixture_fail(void) {
    /* failing fixture should return false */

    test_setup_mock_data = (struct test_fixture_mock_data)TEST_FIXTURE_MOCK_DATA_NEW;
    test_setup_mock_data.result = false;

    void *test_reference = NULL;
    bool result = tric_run_fixture(test_setup_mock, &test_reference);

    assert(result == false);
    assert(test_setup_mock_data.count == 1);
    assert(test_setup_mock_data.data == &test_reference);
}



void test_scan_tests(void) {
    /* suite function should execute scan */

    struct tric_suite suite;
    struct tric_test test;
    struct tric_context context = { .mode = MODE_RESET, .suite = &suite, .test = &test };
    test_suite_mock_data = (struct test_suite_mock_data)TEST_SUITE_MOCK_DATA_NEW;

    tric_scan_tests(&context);

    assert(test_suite_mock_data.count == 1);
    assert(test_suite_mock_data.context_mode == MODE_SCAN);
    assert(test_suite_mock_data.context_suite == &suite);
    assert(test_suite_mock_data.context_reference == &context);
    assert(context.mode == MODE_RESET);
    assert(context.test == NULL);
}



void test_run_tests_fail_setup(void) {
    /* failing setup should return failure */

    struct tric_context context = { .mode = MODE_SCAN };
    test_setup_mock_data = (struct test_fixture_mock_data)TEST_FIXTURE_MOCK_DATA_NEW;
    test_setup_mock_data.result = false;

    int result = tric_run_tests(&context);

    assert(result == EX_UNAVAILABLE);
    assert(context.mode == MODE_RESET);
    assert(test_setup_mock_data.count == 1);
    assert(test_setup_mock_data.data == &test_suite_data_reference);
}



void test_run_tests_ok(void) {
    /* setup, reporting and teardown should be called */

    struct tric_suite suite;
    struct tric_context context = { .mode = MODE_SCAN, .suite = &suite };
    test_suite_mock_data = (struct test_suite_mock_data)TEST_SUITE_MOCK_DATA_NEW;
    test_setup_mock_data = (struct test_fixture_mock_data)TEST_FIXTURE_MOCK_DATA_NEW;
    test_teardown_mock_data = (struct test_fixture_mock_data)TEST_FIXTURE_MOCK_DATA_NEW;
    test_log_start_mock_data = (struct test_logger_mock_data)TEST_LOGGER_MOCK_DATA_NEW;
    test_log_end_mock_data = (struct test_logger_mock_data)TEST_LOGGER_MOCK_DATA_NEW;
    tric_log(test_log_start_mock, NULL, test_log_end_mock, &context);

    int result = tric_run_tests(&context);

    assert(result == EX_OK);
    assert(context.mode == MODE_RESET);
    assert(test_suite_mock_data.count == 1);
    assert(test_suite_mock_data.context_mode == MODE_RESET);
    assert(test_suite_mock_data.context_suite == &suite);
    assert(test_suite_mock_data.context_reference == &context);
    assert(test_setup_mock_data.count == 1);
    assert(test_setup_mock_data.data == &test_suite_data_reference);
    assert(test_teardown_mock_data.count == 1);
    assert(test_teardown_mock_data.data == &test_suite_data_reference);
    assert(test_log_start_mock_data.count == 1);
    assert(test_log_start_mock_data.suite == &suite);
    assert(test_log_start_mock_data.test == NULL);
    assert(test_log_start_mock_data.data == &context);
    assert(test_log_end_mock_data.count == 1);
    assert(test_log_end_mock_data.suite == &suite);
    assert(test_log_end_mock_data.test == NULL);
    assert(test_log_end_mock_data.data == &context);
}



void test_run_tests_fail_teardown(void) {
    /* failing teardown should return failure */

    struct tric_suite suite;
    struct tric_context context = { .mode = MODE_SCAN, .suite = &suite };
    test_suite_mock_data = (struct test_suite_mock_data)TEST_SUITE_MOCK_DATA_NEW;
    test_setup_mock_data = (struct test_fixture_mock_data)TEST_FIXTURE_MOCK_DATA_NEW;
    test_teardown_mock_data = (struct test_fixture_mock_data)TEST_FIXTURE_MOCK_DATA_NEW;
    test_teardown_mock_data.result = false;
    test_log_start_mock_data = (struct test_logger_mock_data)TEST_LOGGER_MOCK_DATA_NEW;
    test_log_end_mock_data = (struct test_logger_mock_data)TEST_LOGGER_MOCK_DATA_NEW;
    tric_log(test_log_start_mock, NULL, test_log_end_mock, &context);

    int result = tric_run_tests(&context);

    assert(result == EX_TEMPFAIL);
    assert(context.mode == MODE_RESET);
    assert(test_suite_mock_data.count == 1);
    assert(test_suite_mock_data.context_mode == MODE_RESET);
    assert(test_suite_mock_data.context_suite == &suite);
    assert(test_suite_mock_data.context_reference == &context);
    assert(test_setup_mock_data.count == 1);
    assert(test_setup_mock_data.data == &test_suite_data_reference);
    assert(test_teardown_mock_data.count == 1);
    assert(test_teardown_mock_data.data == &test_suite_data_reference);
    assert(test_log_start_mock_data.count == 1);
    assert(test_log_start_mock_data.suite == &suite);
    assert(test_log_start_mock_data.test == NULL);
    assert(test_log_start_mock_data.data == &context);
    assert(test_log_end_mock_data.count == 1);
    assert(test_log_end_mock_data.suite == &suite);
    assert(test_log_end_mock_data.test == NULL);
    assert(test_log_end_mock_data.data == &context);
}



void test_main_fail_open(void) {
    /* failing to open itself should return error */

    char *arguments[] = { "/dev/null/no/file" };

    int result = tric_main(1, arguments);

    assert(result == EX_NOINPUT);
}



void test_main_fail_setup(int argc, char *argv[]) {
    /* failing setup should return error */

    test_suite_mock_data = (struct test_suite_mock_data)TEST_SUITE_MOCK_DATA_NEW;
    test_setup_mock_data = (struct test_fixture_mock_data)TEST_FIXTURE_MOCK_DATA_NEW;
test_setup_mock_data.result = false;

    int result = tric_main(argc, argv);

    assert(result == EX_UNAVAILABLE);
    assert(test_suite_mock_data.count == 1);
    assert(test_suite_mock_data.context_suite == tric_data()->suite);
    assert(test_setup_mock_data.count == 1);
}



void test_main_fail_teardown(int argc, char *argv[]) {
    /* failing teardown should return error */

    test_suite_mock_data = (struct test_suite_mock_data)TEST_SUITE_MOCK_DATA_NEW;
    test_setup_mock_data = (struct test_fixture_mock_data)TEST_FIXTURE_MOCK_DATA_NEW;
    test_teardown_mock_data = (struct test_fixture_mock_data)TEST_FIXTURE_MOCK_DATA_NEW;
test_teardown_mock_data.result = false;

    int result = tric_main(argc, argv);

    assert(result == EX_TEMPFAIL);
    assert(test_suite_mock_data.count == 2);
    assert(test_suite_mock_data.context_suite == tric_data()->suite);
    assert(test_setup_mock_data.count == 1);
    assert(test_teardown_mock_data.count == 1);
}



void test_main_ok(int argc, char *argv[]) {
    /* successful execution should return 0 */

    test_suite_mock_data = (struct test_suite_mock_data)TEST_SUITE_MOCK_DATA_NEW;
    test_setup_mock_data = (struct test_fixture_mock_data)TEST_FIXTURE_MOCK_DATA_NEW;
    test_teardown_mock_data = (struct test_fixture_mock_data)TEST_FIXTURE_MOCK_DATA_NEW;

    int result = tric_main(argc, argv);

    assert(result == 0);
    assert(result == EX_OK);
    assert(test_suite_mock_data.count == 2);
    assert(test_suite_mock_data.context_suite == tric_data()->suite);
    assert(test_setup_mock_data.count == 1);
    assert(test_teardown_mock_data.count == 1);
}



int main(int argc, char *argv[]) {
    /* turn off default logging */
    tric_log(NULL, NULL, NULL, NULL);

    test_new_suite();

    test_suite_data();

    test_new_test();

    test_prepare_test_scan();
    test_prepare_test_run();

    test_test_scan();
    test_test_fail_before();
    test_test_fail_after();
    test_test_ok();
    test_test_no_fixtures();
    test_test_crash();
    test_test_fail();

    test_skip_test_scan();
    test_skip_test_ok();
    test_skip_test_fail_before();
    test_skip_test_fail_after();
    test_skip_test_no_fixtures();

    test_assert_fail(argv[0]);
    test_assert_pass(argv[0]);

    test_fixture_scan();
    test_fixture_execute();
    test_fixture_reset();

    test_add_test_not();
    test_add_test_first();
    test_add_test_second();

    test_reset_context_not();
    test_reset_context_run(argv[0]);

    test_status_ok_all();
    test_status_ok_none();
    test_status_ok_before();
    test_status_ok_after();

    test_status_fail_all(argv[0]);
    test_status_fail_none(argv[0]);
    test_status_fail_before(argv[0]);
    test_status_fail_after(argv[0]);

    test_status_fail_before_all();
    test_status_fail_before_none();
    test_status_fail_before_before();
    test_status_fail_before_after();

    test_status_fail_after_all();
    test_status_fail_after_none();
    test_status_fail_after_before();
    test_status_fail_after_after();

    test_status_crash_all();
    test_status_crash_none();
    test_status_crash_before();
    test_status_crash_after();

    test_status_fail_fork_all();
    test_status_fail_fork_none();
    test_status_fail_fork_before();
    test_status_fail_fork_after();

    test_status_skip_all();
    test_status_skip_none();
    test_status_skip_before();
    test_status_skip_after();

    test_set_status_ok();
    test_set_status_test_failure(argv[0]);
    test_set_status_before_failure();
    test_set_status_after_failure();
    test_set_status_signal();
    test_set_status_fork();
    test_set_status_skip();

    test_run_test_not();
    test_run_test_ok();
    test_run_test_signal();

    test_run_before_not();
    test_run_before_null();
    test_run_before_ok();
    test_run_before_fail();

    test_run_after_null();
    test_run_after_ok();
    test_run_after_fail();

    test_skip_test_execution_not();
    test_skip_test_execution_ok();

    test_reporting_all();
    test_reporting_none();

    test_report();

    test_log();

    test_run_fixture_not();
    test_run_fixture_ok();
    test_run_fixture_fail();

    test_scan_tests();

    test_run_tests_fail_setup();
    test_run_tests_ok();
    test_run_tests_fail_teardown();

    test_main_fail_open();
    test_main_fail_setup(argc, argv);
    test_main_fail_teardown(argc, argv);
    test_main_ok(argc, argv);

    return 0;
}



