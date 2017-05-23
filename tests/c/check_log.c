#include <stdio.h>
#include <stdlib.h>
#include <check.h>
#include <fcntl.h>

#include "log.h"
#include "to_syslog.h"
#include "to_file.h"

void run_test(evalresp_log_func_t log_func,  void *data, char is_syslog)
{
    ck_assert(EXIT_SUCCESS == evalresp_log_basic(log_func, data, INFO, 0, "This is a TEST"));
    if (is_syslog)
    {
        return;

    }
}

/* test stderr */
START_TEST (test_log_1)
{
    run_test(NULL, NULL, 0);
}
END_TEST

/* test file */
START_TEST (test_log_2)
{
    FILE *fd;
    ck_assert( fd = fopen("log_test.txt", "a+"));
    run_test(evalresp_log_to_file,  (void*)fd, 0);
}
END_TEST

/* test syslog */
START_TEST (test_log_3)
{
    evalresp_syslog_data_t data[1];
    data->ident="test_stuff";
    data->option=LOG_PID;
    data->facility=LOG_USER;
    run_test(evalresp_log_to_syslog, (void *)data, 1);
}
END_TEST

int main (void) {
    int number_failed;
    Suite *s = suite_create("suite");
    TCase *tc = tcase_create ("case");
    tcase_add_test(tc, test_log_1);
    tcase_add_test(tc, test_log_2);
    tcase_add_test(tc, test_log_3);
    suite_add_tcase(s, tc);
    SRunner *sr = srunner_create(s);
    srunner_set_xml(sr, "check-log.xml");
    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return number_failed;
}