/*
TRIC - Minimalistic unit testing framework for c
Copyright 2024 Philip Colombo

This file is part of TRIC.

TRIC is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

TRIC is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with TRIC.  If not, see <https://www.gnu.org/licenses/>.

*/



#ifndef TRIC_H
#error "TRIC is not defined"
#endif

#ifndef TRIC_ASSERT_H
#define TRIC_ASSERT_H



#include <string.h>
#include <math.h>



/**
 * \file tric_assert.h
 *
 * \brief A collection of assertions for TRIC
 *
 * TRIC itself provides only the ASSERT macro. This collection of assertions can be used to simplify testing and to increase readability of the tests. These macros are implemented using the ASSERT macro from the header tric.h. The header tric.h must be included before the header tric_assert.h can be included. Otherwise the compilation fails.
 *
 * A simple test suite with some assertion examples:
 *
 * \code
#include "tric.h"
#include "tric_assert.h"

SUITE("some assertions", NULL, NULL, NULL) {
    TEST("descriptive assertions", NULL, NULL, NULL) {
        ASSERT_NOT(1 < 0);
        ASSERT_EQUAL(1, 1);
        ASSERT_NOT_ZERO(1);
        ASSERT_POSITIVE(1);
        ASSERT_FALSE(1 != -1 && 1 < -1);
    }
}
 * \endcode
 *
 * \author Philip Colombo
 * \date 2024
 * \copyright GNU Lesser General Public License
 */



/**
 * \brief Inversion of the ASSERT macro.
 *
 * The given expression is executed and if it is true, the test is terminated and marked as failed. If the expression is false, nothing happens and the execution of the test continues.
 *
 * \param E Expression to verify.
 */
#define ASSERT_NOT(E) \
ASSERT( (E) ? false : true)



/**
 * \brief Test for equality.
 *
 * If the given arguments are not equal by using the == operator for comparison, the test is terminated and marked as failed. If the arguments are equal, nothing happens and the execution of the test continues.
 *
 * \param V1 First value to test.
 * \param V2 Second value to test.
 */
#define ASSERT_EQUAL(V1, V2) \
ASSERT( (V1) == (V2) )



/**
 * \brief Test for inequality.
 *
 * If the given arguments are equal, the test is terminated and marked as failed. If the arguments are not equal, nothing happens and the execution of the test continues.
 *
 * \param V1 First value to test.
 * \param V2 Second value to test.
 */
#define ASSERT_NOT_EQUAL(V1, V2) \
ASSERT( (V1) != (V2) )



/**
 * \brief Test for true.
 *
 * If the given boolean argument is false, the test is terminated and marked as failed. If the value is true, nothing happens and the execution of the test continues.
 *
 * \param B Boolean value to test.
 */
#define ASSERT_TRUE(B) \
ASSERT( (B) == true)



/**
 * \brief Test for false.
 *
 * If the given boolean argument is true, the test is terminated and marked as failed. If the value is false, nothing happens and the execution of the test continues.
 *
 * \param B Boolean value to test.
 */
#define ASSERT_FALSE(B) \
ASSERT( (B) == false)



/**
 * \brief Test for NULL.
 *
 * If the given argument is not equal to NULL, the test is terminated and marked as failed. If the value is NULL, nothing happens and the execution of the test continues.
 *
 * \param P Value to test.
 */
#define ASSERT_NULL(P) \
ASSERT( (P) == NULL )



/**
 * \brief Test for a non NULL value.
 *
 * If the given argument is NULL, the test is terminated and marked as failed. Otherwise nothing happens and the execution of the test continues.
 *
 * \param P Value to test.
 */
#define ASSERT_NOT_NULL(P) \
ASSERT( (P) != NULL)



/**
 * \brief Test for 0.
 *
 * If the given argument is not equal to 0, the test is terminated and marked as failed. If the value is 0, nothing happens and the execution of the test continues.
 *
 * \param V Value to test.
 */
#define ASSERT_ZERO(V) \
ASSERT( (V) == 0 )



/**
 * \brief Test for a non 0 value.
 *
 * If the given argument is equal to 0, the test is terminated and marked as failed. If the value is not equal to 0, nothing happens and the execution of the test continues.
 *
 * \param V Value to test.
 */
#define ASSERT_NOT_ZERO(V) \
ASSERT( (V) != 0 )



/**
 * \brief Test for a positive value.
 *
 * If the given argument is equal to 0 or less than 0, the test is terminated and marked as failed. If the value is greater than 0, nothing happens and the execution of the test continues.
 *
 * \param V Value to test.
 */
#define ASSERT_POSITIVE(V) \
ASSERT( (V) > 0 )



/**
 * \brief Test for a negative value.
 *
 * If the given argument is equal to 0 or greater than 0, the test is terminated and marked as failed. If the value is less than 0, nothing happens and the execution of the test continues.
 *
 * \param V Value to test.
 */
#define ASSERT_NEGATIVE(V) \
ASSERT( (V) < 0 )



/**
 * \brief Test if a flag is set.
 *
 * If not all flags given by F are set in the bits given by B, the test is terminated and marked as failed. If all flags are set, nothing happens and the execution of the test continues. If F is 0 and B is not 0, the assertion fails.
 *
 * \param B Bits to test.
 * \param F Flags to check if set in B.
 */
#define ASSERT_FLAG(B, F) \
ASSERT( tric_assert_flag((B), (F)) == true )



/**
 * \brief Test if a flag is not set.
 *
 * If any of the flags given by F is set in the bits given by B, the test is terminated and marked as failed. If no flags are set, nothing happens and the execution of the test continues.
 *
 * \param B Bits to test.
 * \param F Flags to check if not set in B.
 */
#define ASSERT_NOT_FLAG(B, F) \
ASSERT( tric_assert_not_flag((B), (F)) == true )



/**
 * \brief Test for string equality.
 *
 * If the given string arguments are not equal, the test is terminated and marked as failed. If the strings are equal, nothing happens and the execution of the test continues. If any of the arguments is NULL, the assertion fails.
 *
 * \param S1 First string to test.
 * \param S2 Second string to test.
 */
#define ASSERT_STRING_EQUAL(S1, S2) \
ASSERT( tric_assert_string_equal((S1), (S2)) == true )



/**
 * \brief Test a string to start with a prefix.
 *
 * If the string argument does not start with the prefix string argument, the test is terminated and marked as failed. If the string starts with the prefix, nothing happens and the execution of the test continues. If any of the arguments is NULL or the prefix is longer than the string, the assertion fails.
 *
 * \param S String to test.
 * \param X Prefix string to find in S.
 */
#define ASSERT_STRING_PREFIX(S, X) \
ASSERT( tric_assert_string_prefix((S), (X)) == true )



/**
 * \brief Test a string to start with a prefix.
 *
 * Alias for ASSERT_STRING_PREFIX.
 *
 * \param S String to test.
 * \param X prefix string to find in S.
 */
#define ASSERT_STRING_STARTS_WITH(S, X) ASSERT_STRING_PREFIX(S, X)



/**
 * \brief Test a string to end with a suffix.
 *
 * If the string argument does not end with the suffix string argument, the test is terminated and marked as failed. If the string ends with the suffix, nothing happens and the execution of the test continues. If any of the arguments is NULL or the suffix is longer than the string, the assertion fails.
 *
 * \param S String to test.
 * \param X Suffix string to find in S.
 */
#define ASSERT_STRING_SUFFIX(S, X) \
ASSERT( tric_assert_string_suffix((S), (X)) == true )



/**
 * \brief Test a string to end with a suffix.
 *
 * Alias for ASSERT_STRING_SUFFIX.
 *
 * \param S String to test.
 * \param X Suffix string to find in S.
 */
#define ASSERT_STRING_ENDS_WITH(S, X) ASSERT_STRING_SUFFIX(S, X)



/**
 * \brief Test a string to contain a substring.
 *
 * If the string argument does not contain the substring argument, the test is terminated and marked as failed. If the string contains the substring, nothing happens and the execution of the test continues. If any of the arguments is NULL or the substring is longer than the string, the assertion fails.
 *
 * \param S String to test.
 * \param X Substring to find in S.
 */
#define ASSERT_STRING_CONTAINS(S, X) \
ASSERT( tric_assert_string_contains((S), (X)) == true )



/**
 * \brief Test a string to contain a substring.
 *
 * Alias for ASSERT_STRING_CONTAINS.
 *
 * \param S String to test.
 * \param X Substring to find in S.
 */
#define ASSERT_SUBSTRING(S, X) ASSERT_STRING_CONTAINS(S, X)



/**
 * \brief Test for memory equality.
 *
 * If the given memory pointer arguments are not equal in the first SIZE bytes, the test is terminated and marked as failed. If the arguments are equal, nothing happens and the execution of the test continues. If any of the memory pointer arguments is NULL, the assertion fails.
 *
 * \param M1 First memory pointer to test.
 * \param M2 Second memory pointer to test.
 * \param SIZE Number of bytes to test.
 */
#define ASSERT_MEMORY_EQUAL(M1, M2, SIZE) \
ASSERT( tric_assert_memory_equal(M1, M2, SIZE) == true )



/**
 * \brief Test for floating point equality.
 *
 * If the absolute difference of the given floating point arguments is not smaller than PRECISION, the test is terminated and marked as failed. Otherwise nothing happens and the execution of the test continues. All arguments are cast to double before the comparison is performed.
 *
 * \param F1 First floating point value to test.
 * \param F2 Second floating point value to test.
 * \param PRECISION If the absolute difference of F1 and F2 is greater or equal to this value, the assertion fails.
 */
#define ASSERT_FLOAT_EQUAL(F1, F2, PRECISION) \
ASSERT( fabs( (double)(F1) - (double)(F2) ) < (double)(PRECISION) )



/*
 internally used
test if certain flags are set in bits
*/
bool tric_assert_flag(size_t bits, size_t flags) {
    if ((flags == 0 && bits == 0) || (bits & flags) != 0) {
        return true;
    }
    return false;
}



/*
 internally used
test if certain flags are not set in bits
*/
bool tric_assert_not_flag(size_t bits, size_t flags) {
    if (bits == 0 || (bits & flags) == 0) {
        return true;
    }
    return false;
}



/*
 internally used
test if strings are equal
*/
bool tric_assert_string_equal(const char *s1, const char *s2) {
    if (s1 != NULL && s2 != NULL && strcmp(s1, s2) == 0) {
        return true;
    }
    return false;
}



/*
 internally used
test if string starts with prefix
*/
bool tric_assert_string_prefix(const char *string, const char *prefix) {
    if (string != NULL && prefix != NULL
    && strlen(string) >= strlen(prefix)
    && strncmp(string, prefix, strlen(prefix)) == 0) {
        return true;
    }
    return false;
}



/*
 internally used
test if string ends with suffix
*/
bool tric_assert_string_suffix(const char *string, const char *suffix) {
    if (string != NULL && suffix != NULL
    && strlen(string) >= strlen(suffix)
    && strncmp((char *)(string) + (strlen(string) - strlen(suffix)), suffix, strlen(suffix)) == 0) {
        return true;
    }
    return false;
}



/*
 internally used
test if string contains substring
*/
bool tric_assert_string_contains(const char *string, const char *substring) {
    if (string != NULL && substring != NULL
    && strlen(string) >= strlen(substring)
    && strstr(string, substring) != NULL) {
        return true;
    }
    return false;
}



/*
 internally used
test if memory areas are equal
*/
bool tric_assert_memory_equal(void *m1, void *m2, size_t size) {
    if (m1 != NULL && m2 != NULL && memcmp(m1, m2, size) == 0) {
        return true;
    }
    return false;
}



#endif



