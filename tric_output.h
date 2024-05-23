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



#include <string.h>



/**
 * \file tric_output.h
 *
 * \brief Report TRIC test results in various output formats
 *
 * TRIC itself provides only a simple default reporting. The header tric_output.h defines functions to report the test results of TRIC in other output formats. The header tric.h must be included before the header tric_output.h can be included. Otherwise the compilation of the test suite will fail.
 *
 * These functions must be called before any test is executed (i.e. in a test suite setup fixture). The following example shows how to report test results in the TAP output format:
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


#define TRIC_OUTPUT_FORMAT "TRIC_OUTPUT_FORMAT"



/*
 internally used
print string representation of execution results
*/
void tric_print_result(enum tric_result result) {
    const char *result_strings[] = { "undefined", "ok", "failure", "skipped", "crashed" };
    printf("%s", result_strings[result + 1]);
}



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



/*
 internally used
print csv header
*/
void tric_csv_header(bool unix_newline) {
    printf("ID,RESULT,LINE,SIGNAL,BEFORE,AFTER,DESCRIPTION%s", unix_newline ? "\n" : "\r\n");
}



/*
 internally used
print csv header with newlines according to specification
*/
void tric_csv_header_standard(struct tric_suite *suite, struct tric_test *test, void *data) {
    tric_csv_header(false);
}



/*
 internally used
print csv header with unix newlines
*/
void tric_csv_header_unix(struct tric_suite *suite, struct tric_test *test, void *data) {
    tric_csv_header(true);
}



/*
 internally used
print csv record
*/
void tric_csv_record(struct tric_test *test, bool unix_newline) {
    printf("%zu,", test->id);
    tric_print_result(test->result);
    printf(",%zu,%zu,", test->line, test->signal);
    tric_print_result(test->before);
    printf(",");
    tric_print_result(test->after);
    printf(",\"%s\"%s", test->description, unix_newline ? "\n" : "\r\n");
}



/*
 internally used
print csv record with newlines according to standard
*/
void tric_csv_record_standard(struct tric_suite *suite, struct tric_test *test, void *data) {
    tric_csv_record(test, false);
}



/*
 internally used
print csv record with unix newlines
*/
void tric_csv_record_unix(struct tric_suite *suite, struct tric_test *test, void *data) {
    tric_csv_record(test, true);
}



/**
 * \brief CSV output
 *
 * Output test results in CSV (Comma Separated Values) format according to the specification in <a href="https://www.rfc-editor.org/rfc/rfc4180">RFC 4180</a>. The output of the csv header may be disabled by setting the header parameter to false..
 *
 * RFC 4180 requires CRLF newlines ("\r\n"). With the parameter unix_newline it is possible to report the test results with unix style LF newlines ("\n").
 *
 * This function must be called before any test in the test suite is executed (i.e. in the test suite setup fixture).
 *
 * \param header If set to true, the optional csv header is included in the test reporting.
 * \param unix_newline If set to true, unix style newlines ("\n") will be used for the csv output. Otherwise CRLF newlines ("\r\n") will be used.
 */
void tric_output_csv(bool header, bool unix_newline) {
    tric_logger_t start = NULL;
    if (header) {
        start = unix_newline ? tric_csv_header_unix : tric_csv_header_standard;
    }
    tric_log(start, unix_newline ? tric_csv_record_unix : tric_csv_record_standard, NULL, NULL);
}



/*
 internally used
print csv summary header
*/
void tric_csv_summary_header(bool unix_newline) {
    printf("DESCRIPTION,TESTS,EXECUTED,FAILED,SKIPPED%s", unix_newline ? "\n" : "\r\n");
}



/*
 internally used
print csv summary header with newlines according to specification
*/
void tric_csv_summary_header_standard(struct tric_suite *suite, struct tric_test *test, void *data) {
    tric_csv_summary_header(false);
}



/*
 internally used
print csv summary header with unix newlines
*/
void tric_csv_summary_header_unix(struct tric_suite *suite, struct tric_test *test, void *data) {
    tric_csv_summary_header(true);
}



/*
 internally used
print csv summary record
*/
void tric_csv_summary_record(struct tric_suite *suite, bool unix_newline) {
    printf("\"%s\",%zu,%zu,%zu,%zu%s", suite->description, suite->number_of_tests, suite->executed_tests, suite->failed_tests, suite->skipped_tests, unix_newline ? "\n" : "\r\n");
}



/*
 internally used
print csv summary record with newlines according to standard
*/
void tric_csv_summary_record_standard(struct tric_suite *suite, struct tric_test *test, void *data) {
    tric_csv_summary_record(suite, false);
}



/*
 internally used
print csv summary record with unix newlines
*/
void tric_csv_summary_record_unix(struct tric_suite *suite, struct tric_test *test, void *data) {
    tric_csv_summary_record(suite, true);
}



/**
 * \brief CSV summary output
 *
 * Output a summary of the test results in CSV (Comma Separated Values) format according to the specification in <a href="https://www.rfc-editor.org/rfc/rfc4180">RFC 4180</a>. The output of the csv header may be disabled by setting the header parameter to false..
 *
 * RFC 4180 requires CRLF newlines ("\r\n"). With the parameter unix_newline it is possible to report the test summary with unix style LF newlines ("\n").
 *
 * This function must be called before any test in the test suite is executed (i.e. in the test suite setup fixture).
 *
 * \param header If set to true, the optional csv header is included in the test summary reporting.
 * \param unix_newline If set to true, unix style newlines ("\n") will be used for the csv output. Otherwise CRLF newlines ("\r\n") will be used.
 */
void tric_output_csv_summary(bool header, bool unix_newline) {
    tric_logger_t start = NULL;
    if (header) {
        start = unix_newline ? tric_csv_summary_header_unix : tric_csv_summary_header_standard;
    }
    tric_log(start, NULL, unix_newline ? tric_csv_summary_record_unix : tric_csv_summary_record_standard, NULL);
}



/*
 internally used
print test as json
*/
void tric_json_test(struct tric_test *test) {
    printf("{ \"id\": %zu, \"description\": \"%s\", \"before\": \"", test->id, test->description);
    tric_print_result(test->before);
    printf("\", \"result\": \"");
    tric_print_result(test->result);
    printf("\", \"after\": \"");
    tric_print_result(test->after);
    printf("\", \"line\": %zu, \"signal\": %zu }", test->line, test->signal);
}



/*
 internally used
print all tests as json list
*/
void tric_json_tests(struct tric_suite *suite) {
    printf("[ ");
    struct tric_test *test = suite->tests;
    while (test != NULL) {
        tric_json_test(test);
        if (test->next == NULL) {
            printf(" ");
            break;
        }
        printf(", ");
        test = test->next;
    }
    printf("] ");
}



/*
 internally used
print suite as json
*/
void tric_json_suite(struct tric_suite *suite, struct tric_test *test, void *data) {
    printf("{ \"description\": \"%s\", \"number_of_tests\": %zu, \"executed_tests\": %zu, \"failed_tests\": %zu, \"skipped_tests\": %zu, \"tests\": ", suite->description, suite->number_of_tests, suite->executed_tests, suite->failed_tests, suite->skipped_tests);
    tric_json_tests(suite);
    printf("}\n");
}



/**
 * \brief JSON output
 *
 * Output the test results in <a href="https://en.wikipedia.org/wiki/JSON">JSON (JavaScript Object Notation)</a> format.
 *
 * This function must be called before any test in the test suite is executed (i.e. in the test suite setup fixture).
 */
void tric_output_json(void) {
    tric_log(NULL, NULL, tric_json_suite, NULL);
}



/*
 internally used
read TRIC_OUTPUT_FORMAT environment variable
*/
char *tric_environment_format(void) {
    extern char **environ;
    size_t i;
    char *current;
    for (i = 0; (current = environ[i]) != NULL; i++) {
        char *separator = strchr(current, '=');
        if (separator == NULL
        || (separator - current) != strlen(TRIC_OUTPUT_FORMAT)) {
            continue;
        }
        if (strncmp(current, TRIC_OUTPUT_FORMAT, strlen(TRIC_OUTPUT_FORMAT)) == 0) {
            return separator + 1;
        }
    }
    return NULL;
}



/*
 internally used
test if string of environment variable matches output format string
*/
bool tric_environment_match(char *environment_value, char *format) {
    if (strlen(environment_value) != strlen(format)
    || strcmp(environment_value, format) != 0) {
        return false;
    }
    return true;
}



/**
 * \brief Set output format with environment variable
 *
 * Output the test results in the format determined by the environment variable TRIC_OUTPUT_FORMAT. The following values for TRIC_OUTPUT_FORMAT are recognized:
 *
 * - tap
 *
 *   Output TAP format (i.e. tric_output_tap() will be called).
 *
 * - csv
 *
 *   Output CSV format without header (i.e. tric_output_csv(false, false) will be called).
 *
 * - csv_header
 *
 *   Output CSV format with header (i.e. tric_output_csv(true, false) will be called).
 *
 * - csv_unix
 *
 *   Output CSV format without header and with unix style newlines (i.e.  tric_output_csv(false, true) will be called).
 *
 * - csv_header_unix
 *
 *   Output CSV format with header and with unix style newlines (i.e. tric_output_csv(true, true) will be called).
 *
 * - csv_summary
 *
 *   Output CSV summary format without header (i.e. tric_output_csv_summary(false, false) will be called).
 *
 * - csv_summary_header
 *
 *   Output CSV summary format with header (i.e. tric_output_csv_summary(true, false) will be called).
 *
 * - csv_summary_unix
 *
 *   Output CSV summary format without header and with unix style newlines (i.e. tric_output_csv_summary(false, true) will be called).
 *
 * - csv_summary_header_unix
 *
 *   Output CSV summary format with header and with unix style newlines (i.e. tric_output_csv_summary(true, true) will be called).
 *
 * - json
 *
 *   Output JSON format (i.e. tric_output_json() will be called).
 *
 * - none
 *
 *   Do not report any test results (i.e. tric_log(NULL, NULL, NULL, NULL) will be called).
 *
 * Any other value for TRIC_OUTPUT_FORMAT or if TRIC_OUTPUT_FORMAT is not defined will set the default reporting of TRIC.
 *
 * This function must be called before any test in the test suite is executed (i.e. in the test suite setup fixture).
 */
void tric_output_environment(void) {
    char *format = tric_environment_format();
    if (format == NULL) {
        return;
    } else if (tric_environment_match(format, "tap")) {
        tric_output_tap();
    } else if (tric_environment_match(format, "csv")) {
        tric_output_csv(false, false);
    } else if (tric_environment_match(format, "csv_header")) {
        tric_output_csv(true, false);
    } else if (tric_environment_match(format, "csv_unix")) {
        tric_output_csv(false, true);
    } else if (tric_environment_match(format, "csv_header_unix")) {
        tric_output_csv(true, true);
    } else if (tric_environment_match(format, "csv_summary")) {
        tric_output_csv_summary(false, false);
    } else if (tric_environment_match(format, "csv_summary_header")) {
        tric_output_csv_summary(true, false);
    } else if (tric_environment_match(format, "csv_summary_unix")) {
        tric_output_csv_summary(false, true);
    } else if (tric_environment_match(format, "csv_summary_header_unix")) {
        tric_output_csv_summary(true, true);
    } else if (tric_environment_match(format, "json")) {
        tric_output_json();
    } else if (tric_environment_match(format, "none")) {
        tric_log(NULL, NULL, NULL, NULL);
    }
}



#endif



