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

#ifndef TRIC_OUTPUT_H
#define TRIC_OUTPUT_H



/**
 * \file tric_output.h
 *
 * \brief Report TRIC test results in various output formats
 *
 * TRIC itself provides only a simple default reporting. The header tric_output.h defines functions to report the test results of TRIC in other output formats. The header tric.h must be included before the header tric_output.h can be included. Otherwise the compilation of the test suite will fail.
 *
 * Most of these functions must be called before the tests are executed (i.e. in a test suite setup fixture). The following example shows how to report test results in the TAP output format:
 *
 * \code
#include "tric.h"
#include "tric_output.h"

bool setup(void *data) {
    tric_output_tap();
    return true;
}

SUITE("with TAP output", setup, NULL, NULL) {
    TEST("a failing test", NULL, NULL, NULL) {
        ASSERT(0 > 1);
    }
}
 * \endcode
 *
 * \author Philip Colombo
 * \date 2024
 * \copyright GNU Lesser General Public License
 */



/*
 internally used
print TAP version
*/
void tric_tap_version(void) {
    printf("TAP version 14\n");
}



/*
 internally used
print TAP plan
*/
void tric_tap_plan(size_t number_of_tests, const char *description) {
    printf("1..%zu # %s\n", number_of_tests, description);
}



/*
 internally used
print TAP version and plan
*/
void tric_tap_header(struct tric_suite *suite, struct tric_test *test, void *data) {
    tric_tap_version();
    tric_tap_plan(suite->number_of_tests, suite->description);
}



/*
 internally used
print TAP test status
*/
void tric_tap_status(struct tric_test *test) {
    char *status = "not ok";
    if (test->result == TRIC_OK
    || (test->before == TRIC_UNDEFINED && test->result == TRIC_SKIPPED)
    || (test->before == TRIC_SKIPPED && test->result == TRIC_SKIPPED)) {
        status = "ok";
    }
    printf("%s", status);
}



/*
 internally used
print TAP test point id
*/
void tric_tap_number(size_t id) {
    printf(" %zu", id);
}



/*
 internally used
print TAP description
*/
void tric_tap_description(const char *description) {
    printf(" - %s", description);
}



/*
 internally used
print TAP directive
*/
void tric_tap_directive(struct tric_test *test) {
    char *directive = "";
    if ((test->before == TRIC_UNDEFINED && test->result == TRIC_SKIPPED)
    || (test->before == TRIC_SKIPPED && test->result == TRIC_SKIPPED)) {
        directive = " # SKIP";
    }
    printf("%s\n", directive);
}



/*
 internally used
print TAP test point
*/
void tric_tap_test_point(struct tric_suite *suite, struct tric_test *test, void *data) {
    tric_tap_status(test);
    tric_tap_number(test->id);
    tric_tap_description(test->description);
    tric_tap_directive(test);
}



/**
 * \brief TAP version 14 output
 *
 * Output test results in the <a href="https://testanything.org/tap-version-14-specification.html">TAP (Test Anything Protocol)</a> format according to version 14 of the TAP specification.
 *
 * This function must be called before any test in the test suite is executed (i.e. in the test suite setup fixture).
 */
void tric_output_tap(void) {
    tric_log(tric_tap_header, tric_tap_test_point, NULL, NULL);
}



#endif



