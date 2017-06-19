
#ifndef EVALRESP_PUBLIC_API_H
#define EVALRESP_PUBLIC_API_H

#include <stdio.h>

#include "evalresp/public_channels.h"
#include "evalresp/public_responses.h"
#include "evalresp_log/log.h"
#if 0
#define EVALRESP_OK 0
#define EVALRESP_MEM 1
#define EVALRESP_IO 2
#endif
enum evalresp_status_enum
{
    EVALRESP_OK = 0,
    EVALRESP_MEM,
    EVALRESP_IO
};

// TODO - see design doc for details that should go into comments

typedef struct
{
} evalresp_filter;

// TODO - sncl may want expanding
int evalresp_new_filter (evalresp_log_t *log, const char *sncl, int year, int julian_day,
                         evalresp_filter **filter);

int evalresp_free_filter (evalresp_filter **filter);

int evalresp_add_constraint (evalresp_log_t *log, const char *snclq, evalresp_filter **filter);

int evalresp_char_to_channels (evalresp_log_t *log, const char *seed_or_xml,
                               const evalresp_filter *filter, evalresp_channels **channels);

int evalresp_file_to_channels (evalresp_log_t *log, FILE *file,
                               const evalresp_filter *filter, evalresp_channels **channels);

int evalresp_filename_to_channels (evalresp_log_t *log, const char *filename,
                                   const evalresp_filter *filter, evalresp_channels **channels);

// TODO - everything below here needs implementing

typedef struct
{

} evalresp_eval_options;

int evalresp_channel_to_response (evalresp_log_t *log, const evalresp_channel *channel,
                                  const evalresp_eval_options *eval_options,
                                  evalresp_responses **responses);

int evalresp_channels_to_responses (evalresp_log_t *log, const evalresp_channels *channels,
                                    const evalresp_eval_options *eval_options,
                                    evalresp_responses **responses);

typedef enum {
  evalresp_fap_format,
  evalresp_amplitude_format,
  evalresp_phase_format,
  evalresp_complex_format
} evalresp_format;

int evalresp_response_to_char (evalresp_log_t *log, const evalresp_response *response,
                               evalresp_format format, char **output);

int evalresp_response_to_file (evalresp_log_t *log, const evalresp_response *response,
                               evalresp_format format, const char *filename);

typedef struct
{

} evalresp_output_options;

int evalresp_responses_to_dir (evalresp_log_t *log, const evalresp_responses *responses,
                               evalresp_output_options *output_options, const char *dir);

int evalresp_dir_to_dir (evalresp_log_t *log, const char *dir,
                         evalresp_eval_options *eval_options, evalresp_output_options *output_options);

#endif