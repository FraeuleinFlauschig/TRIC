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
passing a function as argument to the flag assertion should execute the function 
only once
*/
int test_assert_flag_func_function(void) {
    static int flags = 0;
    flags++;
    return flags;
}
SUCCESSFUL_ASSERT_TEST(test_assert_flag_func, ASSERT_FLAG(1, test_assert_flag_func_function()))



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

/*
passing a function as argument to the not flag assertion should execute the 
function only once
*/
int test_assert_not_flag_func_function(void) {
    static int flags = 0;
    flags++;
    return flags;
}
SUCCESSFUL_ASSERT_TEST(test_assert_not_flag_func, ASSERT_NOT_FLAG(test_assert_not_flag_func_function(), 2))



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

/*
passing a function as argument to the string assertion should execute the 
function only once
*/
const char *test_assert_string_equal_func_function(void) {
    static int counter = -1;
    const char *strings[] = { "zero", "one", "two", "more than two" };
    counter++;
    return strings[counter <= 3 ? counter : 3];
}
SUCCESSFUL_ASSERT_TEST(test_assert_string_equal_func, ASSERT_STRING_EQUAL(test_assert_string_equal_func_function(), "zero"))



/* string prefix assertion should fail if both arguments are NULL */
FAILING_ASSERT_TEST(test_assert_string_prefix_null_null, ASSERT_STRING_PREFIX(NULL, NULL))

/* string prefix assertion should fail if the second argument is NULL */
FAILING_ASSERT_TEST(test_assert_string_prefix_string_null, ASSERT_STRING_PREFIX("test", NULL))

/* string prefix assertion should fail if the first argument is NULL */
FAILING_ASSERT_TEST(test_assert_string_prefix_null_string, ASSERT_STRING_PREFIX(NULL, "test"))

/* string prefix assertion should fail if string starts not with prefix */
FAILING_ASSERT_TEST(test_assert_string_prefix_fail, ASSERT_STRING_PREFIX("Testing Rules In C", "Rules In C"))

/* string prefix assertion should fail if prefix is longer than string */
FAILING_ASSERT_TEST(test_assert_string_prefix_too_long, ASSERT_STRING_PREFIX("Testing Rules", "Testing Rules In C"))

/* string prefix assertion should be successful if string starts with prefix */
SUCCESSFUL_ASSERT_TEST(test_assert_string_prefix_ok, ASSERT_STRING_PREFIX("Testing Rules In C", "Testing Rules"))

/* string prefix assertion should be successful if both arguments are empty */
SUCCESSFUL_ASSERT_TEST(test_assert_string_prefix_empty_empty, ASSERT_STRING_PREFIX("", ""))

/* string prefix assertion should be successful if prefix is empty */
SUCCESSFUL_ASSERT_TEST(test_assert_string_prefix_empty, ASSERT_STRING_PREFIX("Testing Rules In C", ""))

/*
passing a function as argument to the string prefix assertion should execute the 
function only once
*/
const char *test_assert_string_prefix_func_function(void) {
    static int counter = -1;
    const char *strings[] = { "zero and one", "two and more", "a lot" };
    counter++;
    return strings[counter <= 2 ? counter : 2];
}
SUCCESSFUL_ASSERT_TEST(test_assert_string_prefix_func, ASSERT_STRING_PREFIX(test_assert_string_prefix_func_function(), "zero"))



/*
string starts with assertion should be successful if string starts with prefix
*/
SUCCESSFUL_ASSERT_TEST(test_assert_string_starts_with_ok, ASSERT_STRING_STARTS_WITH("Testing Rules In C", "Testing Rules"))

/* string starts with assertion should fail if string starts not with prefix */
FAILING_ASSERT_TEST(test_assert_string_starts_with_fail, ASSERT_STRING_STARTS_WITH("Testing Rules In C", "Rules In C"))



/* string suffix assertion should fail if both arguments are NULL */
FAILING_ASSERT_TEST(test_assert_string_suffix_null_null, ASSERT_STRING_SUFFIX(NULL, NULL))

/* string suffix assertion should fail if the second argument is NULL */
FAILING_ASSERT_TEST(test_assert_string_suffix_string_null, ASSERT_STRING_SUFFIX("test", NULL))

/* string suffix assertion should fail if the first argument is NULL */
FAILING_ASSERT_TEST(test_assert_string_suffix_null_string, ASSERT_STRING_SUFFIX(NULL, "test"))

/* string suffix assertion should fail if string ends not with suffix */
FAILING_ASSERT_TEST(test_assert_string_suffix_fail, ASSERT_STRING_SUFFIX("Testing Rules In C", "Testing Rules"))

/* string suffix assertion should fail if suffix is longer than string */
FAILING_ASSERT_TEST(test_assert_string_suffix_too_long, ASSERT_STRING_SUFFIX("Testing Rules", "Testing Rules In C"))

/* string suffix assertion should be successful if string ends with suffix */
SUCCESSFUL_ASSERT_TEST(test_assert_string_suffix_ok, ASSERT_STRING_SUFFIX("Testing Rules In C", "Rules In C"))

/* string suffix assertion should be successful if both arguments are empty */
SUCCESSFUL_ASSERT_TEST(test_assert_string_suffix_empty_empty, ASSERT_STRING_SUFFIX("", ""))

/* string suffix assertion should be successful if suffix is empty */
SUCCESSFUL_ASSERT_TEST(test_assert_string_suffix_empty, ASSERT_STRING_SUFFIX("Testing Rules In C", ""))

/*
passing a function as argument to the string suffix assertion should execute the 
function only once
*/
const char *test_assert_string_suffix_func_function(void) {
    static int counter = -1;
    const char *strings[] = { "zero and one", "two and more", "a lot" };
    counter++;
    return strings[counter <= 2 ? counter : 2];
}
SUCCESSFUL_ASSERT_TEST(test_assert_string_suffix_func, ASSERT_STRING_SUFFIX(test_assert_string_suffix_func_function(), "one"))



/* string ends with assertion should be successful if string ends with suffix */
SUCCESSFUL_ASSERT_TEST(test_assert_string_ends_with_ok, ASSERT_STRING_ENDS_WITH("Testing Rules In C", "Rules In C"))

/* string ends with assertion should fail if string ends not with suffix */
FAILING_ASSERT_TEST(test_assert_string_ends_with_fail, ASSERT_STRING_ENDS_WITH("Testing Rules In C", "Testing Rules"))



/* string contains assertion should fail if both arguments are NULL */
FAILING_ASSERT_TEST(test_assert_string_contains_null_null, ASSERT_STRING_CONTAINS(NULL, NULL))

/* string contains assertion should fail if the second argument is NULL */
FAILING_ASSERT_TEST(test_assert_string_contains_string_null, ASSERT_STRING_CONTAINS("test", NULL))

/* string contains assertion should fail if the first argument is NULL */
FAILING_ASSERT_TEST(test_assert_string_contains_null_string, ASSERT_STRING_CONTAINS(NULL, "test"))

/* string contains assertion should fail if string contains not content */
FAILING_ASSERT_TEST(test_assert_string_contains_fail, ASSERT_STRING_CONTAINS("Testing Rules In C", "TRIC"))

/* string contains assertion should fail if content is longer than string */
FAILING_ASSERT_TEST(test_assert_string_contains_too_long, ASSERT_STRING_CONTAINS("Testing Rules", "Testing Rules In C"))

/* string contains assertion should be successful if string contains content */
SUCCESSFUL_ASSERT_TEST(test_assert_string_contains_ok, ASSERT_STRING_CONTAINS("Testing Rules In C", "Rules"))

/* string contains assertion should be successful if content equals string */
SUCCESSFUL_ASSERT_TEST(test_assert_string_contains_equal, ASSERT_STRING_CONTAINS("Testing Rules In C", "Testing Rules In C"))

/* string contains assertion should be successful if both arguments are empty */
SUCCESSFUL_ASSERT_TEST(test_assert_string_contains_empty_empty, ASSERT_STRING_CONTAINS("", ""))

/* string contains assertion should be successful if content is empty */
SUCCESSFUL_ASSERT_TEST(test_assert_string_contains_empty, ASSERT_STRING_CONTAINS("Testing Rules In C", ""))

/*
passing a function as argument to the string contains assertion should execute 
the function only once
*/
const char *test_assert_string_contains_func_function(void) {
    static int counter = -1;
    const char *strings[] = { "zero and one", "two or more", "a lot" };
    counter++;
    return strings[counter <= 2 ? counter : 2];
}
SUCCESSFUL_ASSERT_TEST(test_assert_string_contains_func, ASSERT_STRING_CONTAINS(test_assert_string_contains_func_function(), "and"))



/* substring assertion should be successful if string contains substring */
SUCCESSFUL_ASSERT_TEST(test_assert_substring_ok, ASSERT_SUBSTRING("Testing Rules In C", "Rules"))

/* substring assertion should fail if string contains not substring */
FAILING_ASSERT_TEST(test_assert_substring_fail, ASSERT_SUBSTRING("Testing Rules In C", "TRIC"))



int test_assert_memory_equal_value1[] = { 1, 2, 3, 4 };
int test_assert_memory_equal_value2[] = { 1, 2, 3, 4 };

/* memory assertion should fail if both arguments are NULL */
FAILING_ASSERT_TEST(test_assert_memory_equal_null_null, ASSERT_MEMORY_EQUAL(NULL, NULL, sizeof(test_assert_memory_equal_value1)))

/* memory assertion should fail if the second argument is NULL */
FAILING_ASSERT_TEST(test_assert_memory_equal_memory_null, ASSERT_MEMORY_EQUAL(test_assert_memory_equal_value1, NULL, sizeof(test_assert_memory_equal_value1)))

/* memory assertion should fail if the first argument is NULL */
FAILING_ASSERT_TEST(test_assert_memory_equal_null_memory, ASSERT_MEMORY_EQUAL(NULL, test_assert_memory_equal_value1, sizeof(test_assert_memory_equal_value1)))

/* memory assertion should fail if the arguments are not equal */
FAILING_ASSERT_TEST(test_assert_memory_equal_fail, ASSERT_MEMORY_EQUAL(test_assert_memory_equal_value1, test_assert_memory_equal_value2 + 1, sizeof(test_assert_memory_equal_value1) - sizeof(int)))

/* memory assertion should be successful if both arguments are equal */
SUCCESSFUL_ASSERT_TEST(test_assert_memory_equal_ok, ASSERT_MEMORY_EQUAL(test_assert_memory_equal_value1, test_assert_memory_equal_value2, sizeof(test_assert_memory_equal_value1)))

/* memory assertion should be successful if the size argument is 0 */
SUCCESSFUL_ASSERT_TEST(test_assert_memory_equal_zero, ASSERT_MEMORY_EQUAL(test_assert_memory_equal_value1, test_assert_memory_equal_value2, 0))

/*
memory assertion should be not successful if the size argument is larger 
than the size of the memory arguments
*/
FAILING_ASSERT_TEST(test_assert_memory_equal_fail_size, ASSERT_MEMORY_EQUAL(test_assert_memory_equal_value1, test_assert_memory_equal_value2, sizeof(test_assert_memory_equal_value1) + 1))

/*
passing a function as argument to the memory equal assertion should execute the 
function only once
*/
void *test_assert_memory_equal_func_function(void) {
    static int counter = -1;
    static int memory[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    counter++;
    return counter < 5 ? memory + counter : memory + 5;
}
SUCCESSFUL_ASSERT_TEST(test_assert_memory_equal_func, ASSERT_MEMORY_EQUAL(test_assert_memory_equal_func_function(), test_assert_memory_equal_value1, sizeof(test_assert_memory_equal_value1)))



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
    test_assert_flag_func(argv[0]);

    test_assert_not_flag_fail(argv[0]);
    test_assert_not_flag_fail_multiple(argv[0]);
    test_assert_not_flag_ok(argv[0]);
    test_assert_not_flag_ok_multiple(argv[0]);
    test_assert_not_flag_ok_zero(argv[0]);
    test_assert_not_flag_func(argv[0]);

    test_assert_string_equal_null_null(argv[0]);
    test_assert_string_equal_string_null(argv[0]);
    test_assert_string_equal_null_string(argv[0]);
    test_assert_string_equal_fail(argv[0]);
    test_assert_string_equal_ok(argv[0]);
    test_assert_string_equal_empty(argv[0]);
    test_assert_string_equal_func(argv[0]);

    test_assert_string_prefix_null_null(argv[0]);
    test_assert_string_prefix_string_null(argv[0]);
    test_assert_string_prefix_null_string(argv[0]);
    test_assert_string_prefix_fail(argv[0]);
    test_assert_string_prefix_too_long(argv[0]);
    test_assert_string_prefix_ok(argv[0]);
    test_assert_string_prefix_empty_empty(argv[0]);
    test_assert_string_prefix_empty(argv[0]);
    test_assert_string_prefix_func(argv[0]);

    test_assert_string_starts_with_ok(argv[0]);
    test_assert_string_starts_with_fail(argv[0]);

    test_assert_string_suffix_null_null(argv[0]);
    test_assert_string_suffix_string_null(argv[0]);
    test_assert_string_suffix_null_string(argv[0]);
    test_assert_string_suffix_fail(argv[0]);
    test_assert_string_suffix_too_long(argv[0]);
    test_assert_string_suffix_ok(argv[0]);
    test_assert_string_suffix_empty_empty(argv[0]);
    test_assert_string_suffix_empty(argv[0]);
    test_assert_string_suffix_func(argv[0]);

    test_assert_string_ends_with_ok(argv[0]);
    test_assert_string_ends_with_fail(argv[0]);

    test_assert_string_contains_null_null(argv[0]);
    test_assert_string_contains_string_null(argv[0]);
    test_assert_string_contains_null_string(argv[0]);
    test_assert_string_contains_fail(argv[0]);
    test_assert_string_contains_too_long(argv[0]);
    test_assert_string_contains_ok(argv[0]);
    test_assert_string_contains_equal(argv[0]);
    test_assert_string_contains_empty_empty(argv[0]);
    test_assert_string_contains_empty(argv[0]);
    test_assert_string_contains_func(argv[0]);

    test_assert_substring_ok(argv[0]);
    test_assert_substring_fail(argv[0]);

    test_assert_memory_equal_null_null(argv[0]);
    test_assert_memory_equal_memory_null(argv[0]);
    test_assert_memory_equal_null_memory(argv[0]);
    test_assert_memory_equal_fail(argv[0]);
    test_assert_memory_equal_ok(argv[0]);
    test_assert_memory_equal_zero(argv[0]);
    test_assert_memory_equal_fail_size(argv[0]);
    test_assert_memory_equal_func(argv[0]);

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



