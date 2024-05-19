/*
TRIC assertion tests
Copyright 2024 Philip Colombo

This file is part of TRIC.

TRIC is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

TRIC is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with TRIC.  If not, see <https://www.gnu.org/licenses/>.

*/



#include <assert.h>



/* system under test */

#define TRIC_SELF_TEST
#include "../tric.h"
#include "../tric_assert.h"



/* macros for reusing code */

#define FAILING_ASSERT_CHECK \
    assert(lseek(context.self, 0, SEEK_CUR) == line); \
    assert(WEXITSTATUS(status) == EXIT_TEST_FAILURE);



#define SUCCESSFUL_ASSERT_CHECK \
    line = 0; \
    assert(lseek(context.self, 0, SEEK_CUR) == line); \
    assert(WEXITSTATUS(status) == EXIT_OK);



#define ASSERT_TEST(CHECK, NAME, TEST) \
void NAME(char *argv0) { \
    struct tric_suite suite; \
    struct tric_context context = { .suite = &suite }; \
    context.self = open(argv0, O_RDONLY); \
    assert(context.self != -1); \
    struct tric_context *tric_context = &context; \
    int status = 0; \
    size_t line = __LINE__; \
    pid_t child = fork(); \
    assert(child != -1); \
    if (child) { \
        waitpid(child, &status, 0); \
    } else { \
        TEST; \
        _exit(EXIT_OK); \
    } \
    CHECK; \
    close(context.self); \
}



#define FAILING_ASSERT_TEST(NAME, TEST) \
ASSERT_TEST(FAILING_ASSERT_CHECK, NAME, TEST)



#define SUCCESSFUL_ASSERT_TEST(NAME, TEST) \
ASSERT_TEST(SUCCESSFUL_ASSERT_CHECK, NAME, TEST)



/* globally needed data */

SUITE_DATA("test suite", NULL, NULL, NULL)
void tric_suite_function(struct tric_context *tric_context) { return; }



/* start of tests */

/* not assertion should fail if expression is true */
FAILING_ASSERT_TEST(test_assert_not_fail, ASSERT_NOT(0 < 1))

/* not assertion should be successful if expression is false */
SUCCESSFUL_ASSERT_TEST(test_assert_not_ok, ASSERT_NOT(0 > 1))



/* equal assertion should fail if the arguments are not equal */
FAILING_ASSERT_TEST(test_assert_equal_fail, ASSERT_EQUAL(0, 1))

/* equal assertion should be successful if the arguments are equal */
SUCCESSFUL_ASSERT_TEST(test_assert_equal_ok, ASSERT_EQUAL(1, 1))



/* not equal assertion should fail if the arguments are equal */
FAILING_ASSERT_TEST(test_assert_not_equal_fail, ASSERT_NOT_EQUAL(1, 1))

/* not equal assertion should be successful if the arguments are not equal */
SUCCESSFUL_ASSERT_TEST(test_assert_not_equal_ok, ASSERT_NOT_EQUAL(0, 1))



/* true assertion should fail if the argument is not true */
FAILING_ASSERT_TEST(test_assert_true_fail, ASSERT_TRUE(false))

/* true assertion should be successful if the argument is true */
SUCCESSFUL_ASSERT_TEST(test_assert_true_ok, ASSERT_TRUE(true))



/* false assertion should fail if the argument is not false */
FAILING_ASSERT_TEST(test_assert_false_fail, ASSERT_FALSE(true))

/* false assertion should be successful if the argument is false */
SUCCESSFUL_ASSERT_TEST(test_assert_false_ok, ASSERT_FALSE(false))



/* null assertion should fail if the argument is not NULL */
int test_assert_null_fail_value;
FAILING_ASSERT_TEST(test_assert_null_fail, ASSERT_NULL(&test_assert_null_fail_value))

/* null assertion should be successful if the argument is NULL */
SUCCESSFUL_ASSERT_TEST(test_assert_null_ok, ASSERT_NULL(NULL))



/* not null assertion should fail if the argument is NULL */
FAILING_ASSERT_TEST(test_assert_not_null_fail, ASSERT_NOT_NULL(NULL))

/* not null assertion should be successful if the argument is not NULL */
int test_assert_not_null_ok_value;
SUCCESSFUL_ASSERT_TEST(test_assert_not_null_ok, ASSERT_NOT_NULL(&test_assert_not_null_ok_value))



/* zero assertion should fail if the argument is not 0 */
FAILING_ASSERT_TEST(test_assert_zero_fail, ASSERT_ZERO(1))

/* zero assertion should be successful if the argument is 0 */
SUCCESSFUL_ASSERT_TEST(test_assert_zero_ok, ASSERT_ZERO(0))



/* not zero assertion should fail if the argument is 0 */
FAILING_ASSERT_TEST(test_assert_not_zero_fail, ASSERT_NOT_ZERO(0))

/* not zero assertion should be successful if the argument is not 0 */
SUCCESSFUL_ASSERT_TEST(test_assert_not_zero_ok, ASSERT_NOT_ZERO(1))



/* positive assertion should fail if the argument is less than 0 */
FAILING_ASSERT_TEST(test_assert_positive_fail, ASSERT_POSITIVE(-1))

/* positive assertion should fail if the argument is 0 */
FAILING_ASSERT_TEST(test_assert_positive_zero, ASSERT_POSITIVE(0))

/* positive assertion should be successful if the argument is greater than 0 */
SUCCESSFUL_ASSERT_TEST(test_assert_positive_ok, ASSERT_POSITIVE(1))



/* negative assertion should fail if the argument is greater than 0 */
FAILING_ASSERT_TEST(test_assert_negative_fail, ASSERT_NEGATIVE(1))

/* negative assertion should fail if the argument is 0 */
FAILING_ASSERT_TEST(test_assert_negative_zero, ASSERT_NEGATIVE(0))

/* negative assertion should be successful if the argument is less than 0 */
SUCCESSFUL_ASSERT_TEST(test_assert_negative_ok, ASSERT_NEGATIVE(-1))



/*
flag assertion should fail if the flag argument is not set in the bits 
argument
*/
FAILING_ASSERT_TEST(test_assert_flag_fail, ASSERT_FLAG(5, 2))

/*
flag assertion should fail if the flag argument is 0 and the bit argument is 
not 0
*/
FAILING_ASSERT_TEST(test_assert_flag_fail_zero, ASSERT_FLAG(1, 0))

/*
flag assertion should be successful if the flag argument is set in the 
bits argument
*/
SUCCESSFUL_ASSERT_TEST(test_assert_flag_ok, ASSERT_FLAG(7, 2))

/*
flag assertion should be successful if all flags of the flag argument are set 
in the bits argument
*/
SUCCESSFUL_ASSERT_TEST(test_assert_flag_ok_multiple, ASSERT_FLAG(7, 5))

/* flag assertion should be successful if both arguments are 0 */
SUCCESSFUL_ASSERT_TEST(test_assert_flag_ok_zero, ASSERT_FLAG(0, 0))



/*
not flag assertion should fail if the flag argument is set in the bits 
argument
*/
FAILING_ASSERT_TEST(test_assert_not_flag_fail, ASSERT_NOT_FLAG(7, 2))

/*
not flag assertion should fail if any of the flags in the flag argument are 
set in the bits argument
*/
FAILING_ASSERT_TEST(test_assert_not_flag_fail_multiple, ASSERT_NOT_FLAG(6, 5))

/*
not flag assertion should be successful if the flag argument is not set in 
the bits argument
*/
SUCCESSFUL_ASSERT_TEST(test_assert_not_flag_ok, ASSERT_NOT_FLAG(5, 2))

/*
not flag assertion should be successful if none of the flags in the 
flag argument is set in the bits argument
*/
SUCCESSFUL_ASSERT_TEST(test_assert_not_flag_ok_multiple, ASSERT_NOT_FLAG(2, 5))

/* not flag assertion should be successful if both arguments are 0 */
SUCCESSFUL_ASSERT_TEST(test_assert_not_flag_ok_zero, ASSERT_NOT_FLAG(0, 0))



/* string assertion should fail if both arguments are NULL */
FAILING_ASSERT_TEST(test_assert_string_equal_null_null, ASSERT_STRING_EQUAL(NULL, NULL))

/* string assertion should fail if the second argument is NULL */
FAILING_ASSERT_TEST(test_assert_string_equal_string_null, ASSERT_STRING_EQUAL("test", NULL))

/* string assertion should fail if the first argument is NULL */
FAILING_ASSERT_TEST(test_assert_string_equal_null_string, ASSERT_STRING_EQUAL(NULL, "test"))

/* string assertion should fail if the arguments are not equal */
FAILING_ASSERT_TEST(test_assert_string_equal_fail, ASSERT_STRING_EQUAL("test", "TEST"))

/* string assertion should be successful if both arguments are equal */
SUCCESSFUL_ASSERT_TEST(test_assert_string_equal_ok, ASSERT_STRING_EQUAL("test", "test"))

/* string assertion should be successful if both arguments are empty strings */
SUCCESSFUL_ASSERT_TEST(test_assert_string_equal_empty, ASSERT_STRING_EQUAL("", ""))



int test_assert_memory_equal_value1[] = { 1, 2, 3, 4 };
int test_assert_memory_equal_value2[] = { 1, 2, 3, 4 };

/* memory assertion should fail if both arguments are NULL */
FAILING_ASSERT_TEST(test_assert_memory_equal_null_null, ASSERT_MEMORY_EQUAL(NULL, NULL, sizeof(test_assert_memory_equal_value1)))

/* memory assertion should fail if the second argument is NULL */
FAILING_ASSERT_TEST(test_assert_memory_equal_memory_null, ASSERT_MEMORY_EQUAL(test_assert_memory_equal_value1, NULL, sizeof(test_assert_memory_equal_value1)))

/* memory assertion should fail if the first argument is NULL */
FAILING_ASSERT_TEST(test_assert_memory_equal_null_memory, ASSERT_MEMORY_EQUAL(NULL, test_assert_memory_equal_value1, sizeof(test_assert_memory_equal_value1)))

/* memory assertion should fail if the arguments are not equal */
FAILING_ASSERT_TEST(test_assert_memory_equal_fail, ASSERT_MEMORY_EQUAL(test_assert_memory_equal_value1, test_assert_memory_equal_value1 + 1, sizeof(test_assert_memory_equal_value1) - sizeof(int)))

/* memory assertion should be successful if both arguments are equal */
SUCCESSFUL_ASSERT_TEST(test_assert_memory_equal_ok, ASSERT_MEMORY_EQUAL(test_assert_memory_equal_value1, test_assert_memory_equal_value2, sizeof(test_assert_memory_equal_value1)))

/* memory assertion should be successful if the size argument is 0 */
SUCCESSFUL_ASSERT_TEST(test_assert_memory_equal_zero, ASSERT_MEMORY_EQUAL(test_assert_memory_equal_value1, test_assert_memory_equal_value2, 0))

/*
memory assertion should be not successful if the size argument is larger 
than the size of the memory arguments
*/
FAILING_ASSERT_TEST(test_assert_memory_equal_fail_size, ASSERT_MEMORY_EQUAL(test_assert_memory_equal_value1, test_assert_memory_equal_value2, sizeof(test_assert_memory_equal_value1) + 1))



float test_assert_float_equal_precision = 0.00001;

/*
float assertion should be able to fail if the difference of the arguments 
is positive
*/
FAILING_ASSERT_TEST(test_assert_float_equal_positive_zero, ASSERT_FLOAT_EQUAL(0.1, 0.0, test_assert_float_equal_precision))

/*
float assertion should be able to fail if the difference of the arguments 
is negative
*/
FAILING_ASSERT_TEST(test_assert_float_equal_negative_zero, ASSERT_FLOAT_EQUAL(-0.1, 0.0, test_assert_float_equal_precision))

/* float assertion should fail if not both arguments are positive */
FAILING_ASSERT_TEST(test_assert_float_equal_positive_negative, ASSERT_FLOAT_EQUAL(0.1, -0.1, test_assert_float_equal_precision))

/* float assertion should fail if not both arguments are negative */
FAILING_ASSERT_TEST(test_assert_float_equal_negative_positive, ASSERT_FLOAT_EQUAL(-0.1, 0.1, test_assert_float_equal_precision))

/*
float assertion should fail if the absolute difference of the arguments 
is larger than the precision argument
*/
FAILING_ASSERT_TEST(test_assert_float_equal_fail_precision, ASSERT_FLOAT_EQUAL(0.1, 0.0, 0.1))

/* float assertion should be able to succeed with both arguments positive */
SUCCESSFUL_ASSERT_TEST(test_assert_float_equal_ok_positive, ASSERT_FLOAT_EQUAL(0.1, 0.1, test_assert_float_equal_precision))

/* float assertion should be able to succeed with both arguments negative */
SUCCESSFUL_ASSERT_TEST(test_assert_float_equal_ok_negative, ASSERT_FLOAT_EQUAL(-0.1, -0.1, test_assert_float_equal_precision))

/* float assertion should be successful with both arguments 0 */
SUCCESSFUL_ASSERT_TEST(test_assert_float_equal_ok_zero, ASSERT_FLOAT_EQUAL(0.0, 0.0, test_assert_float_equal_precision))



int main(int argc, char *argv[]) {

    test_assert_not_fail(argv[0]);
    test_assert_not_ok(argv[0]);

    test_assert_equal_fail(argv[0]);
    test_assert_equal_ok(argv[0]);

    test_assert_not_equal_fail(argv[0]);
    test_assert_not_equal_ok(argv[0]);

    test_assert_true_fail(argv[0]);
    test_assert_true_ok(argv[0]);

    test_assert_false_fail(argv[0]);
    test_assert_false_ok(argv[0]);

    test_assert_null_fail(argv[0]);
    test_assert_null_ok(argv[0]);

    test_assert_not_null_fail(argv[0]);
    test_assert_not_null_ok(argv[0]);

    test_assert_zero_fail(argv[0]);
    test_assert_zero_ok(argv[0]);

    test_assert_not_zero_fail(argv[0]);
    test_assert_not_zero_ok(argv[0]);

    test_assert_positive_fail(argv[0]);
    test_assert_positive_zero(argv[0]);
    test_assert_positive_ok(argv[0]);

    test_assert_negative_fail(argv[0]);
    test_assert_negative_zero(argv[0]);
    test_assert_negative_ok(argv[0]);

    test_assert_flag_fail(argv[0]);
    test_assert_flag_fail_zero(argv[0]);
    test_assert_flag_ok(argv[0]);
    test_assert_flag_ok_multiple(argv[0]);
    test_assert_flag_ok_zero(argv[0]);

    test_assert_not_flag_fail(argv[0]);
    test_assert_not_flag_fail_multiple(argv[0]);
    test_assert_not_flag_ok(argv[0]);
    test_assert_not_flag_ok_multiple(argv[0]);
    test_assert_not_flag_ok_zero(argv[0]);

    test_assert_string_equal_null_null(argv[0]);
    test_assert_string_equal_string_null(argv[0]);
    test_assert_string_equal_null_string(argv[0]);
    test_assert_string_equal_fail(argv[0]);
    test_assert_string_equal_ok(argv[0]);
    test_assert_string_equal_empty(argv[0]);

    test_assert_memory_equal_null_null(argv[0]);
    test_assert_memory_equal_memory_null(argv[0]);
    test_assert_memory_equal_null_memory(argv[0]);
    test_assert_memory_equal_fail(argv[0]);
    test_assert_memory_equal_ok(argv[0]);
    test_assert_memory_equal_zero(argv[0]);
    test_assert_memory_equal_fail_size(argv[0]);

    test_assert_float_equal_positive_zero(argv[0]);
    test_assert_float_equal_negative_zero(argv[0]);
    test_assert_float_equal_positive_negative(argv[0]);
    test_assert_float_equal_negative_positive(argv[0]);
    test_assert_float_equal_fail_precision(argv[0]);
    test_assert_float_equal_ok_positive(argv[0]);
    test_assert_float_equal_ok_negative(argv[0]);
    test_assert_float_equal_ok_zero(argv[0]);

    return 0;
}



