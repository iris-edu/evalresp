
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./private.h"
#include "evalresp/public_api.h"

int
evalresp_response_to_char (evalresp_log_t *log, const evalresp_response *response,
                           evalresp_file_format format, char **output)
{
  int status = EVALRESP_OK;
  int num_of_points = 0;
  int i, length, offset;
  double *amp_arr = NULL;
  double *pha_arr = NULL;
  double *freq_arr = NULL;

  if (*output)
  {
    /* don't want to get bad memory location to write too
         * or we don't want to zombify stuff */
    evalresp_log (log, EV_ERROR, EV_ERROR, "cannot out put to an already allocated output");
    return EVALRESP_ERR;
  }

  if (!response)
  {
    evalresp_log (log, EV_ERROR, EV_ERROR, "Cannot Process Empty Response");
    return EVALRESP_ERR;
  }

  num_of_points = response->nfreqs;
  freq_arr = response->freqs;

  switch (format)
  {
  case evalresp_fap_file_format:
    if (!(status = calloc_doubles (log, "amplitude", num_of_points, &amp_arr)))
    {
      status = calloc_doubles (log, "phase", num_of_points, &pha_arr);
    }
    break;
  case evalresp_amplitude_file_format:
    status = calloc_doubles (log, "amplitude", num_of_points, &amp_arr);
    break;
  case evalresp_phase_file_format:
    status = calloc_doubles (log, "phase", num_of_points, &pha_arr);
    break;
  case evalresp_complex_file_format:
    break;
  default:
    evalresp_log (log, EV_ERROR, EV_ERROR, "Invalid format sent to evalresp_response_to_char");
    status = EVALRESP_ERR;
  }

  if (!status)
  {
    for (i = 0; i < num_of_points; i++)
    {
      if (amp_arr)
      {
        amp_arr[i] = sqrt (response->rvec[i].real * response->rvec[i].real + response->rvec[i].imag * response->rvec[i].imag);
      }
      if (pha_arr)
      {
        pha_arr[i] = atan2 (response->rvec[i].imag, response->rvec[i].real) * 180.0 / M_PI;
      }
    }
    /* Get Size of output string */
    for (i = 0, length = 0; i < num_of_points; i++)
    {
      switch (format)
      {
      case evalresp_fap_file_format:
        length += snprintf (NULL, 0, "%.6E  %.6E  %.6E\n", freq_arr[i], amp_arr[i], pha_arr[i]);
        break;
      case evalresp_amplitude_file_format:
        length += snprintf (NULL, 0, "%.6E  %.6E\n", freq_arr[i], amp_arr[i]);
        break;
      case evalresp_phase_file_format:
        length += snprintf (NULL, 0, "%.6E  %.6E\n", freq_arr[i], pha_arr[i]);
        break;
      case evalresp_complex_file_format:
        length += snprintf (NULL, 0, "%.6E  %.6E  %.6E\n", freq_arr[i], response->rvec[i].real, response->rvec[i].imag);
        break;
      }
    }
    length++; /* Adding one for NULL termination */
  }

  if (!status)
  {
    if (!(*output = (char *)calloc (length, sizeof (char))))
    {
      evalresp_log (log, EV_ERROR, EV_ERROR, "Cannot allocate output");
      status = EVALRESP_MEM;
    }
  }

  if (!status)
  {
    for (i = 0, offset = 0; i < num_of_points; i++)
    {
      switch (format)
      {
      case evalresp_fap_file_format:
        offset += snprintf (*output + offset, length - offset, "%.6E  %.6E  %.6E\n", freq_arr[i], amp_arr[i], pha_arr[i]);
        break;
      case evalresp_amplitude_file_format:
        offset += snprintf (*output + offset, length - offset, "%.6E  %.6E\n", freq_arr[i], amp_arr[i]);
        break;
      case evalresp_phase_file_format:
        offset += snprintf (*output + offset, length - offset, "%.6E  %.6E\n", freq_arr[i], pha_arr[i]);
        break;
      case evalresp_complex_file_format:
        offset += snprintf (*output + offset, length - offset, "%.6E  %.6E  %.6E\n", freq_arr[i], response->rvec[i].real, response->rvec[i].imag);
        break;
      }
    }
  }

  free (amp_arr);
  free (pha_arr);

  return status;
}


int
evalresp_response_to_stream (evalresp_log_t *log, const evalresp_response *response,
                             evalresp_file_format format, const FILE *file)
{
  // TODO - call above and then write char to FILE
  return 0;
}

int
evalresp_response_to_file (evalresp_log_t *log, const evalresp_response *response,
                           evalresp_file_format format, const char *filename)
{
  // TODO - open file then call above.
  return 0;
}