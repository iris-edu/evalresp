
#ifndef EVALRESP_INPUT_H
#define EVALRESP_INPUT_H

#include <stdio.h>

#include "evalresp_log/log.h"

// private functions exposed only for testing

void
slurp_line (const char **seed, char *line, int maxlen);

int
find_line (evalresp_log_t *log, const char **seed, char *sep,
           int blkt_no, int fld_no, char *return_line);

int
find_field (evalresp_log_t *log, const char **seed, char *sep,
            int blkt_no, int fld_no, int fld_wanted, char *return_field);

int
open_file (evalresp_log_t *log, const char *filename, FILE **in);

int
file_to_char (evalresp_log_t *log, FILE *in, char **seed);

#endif
