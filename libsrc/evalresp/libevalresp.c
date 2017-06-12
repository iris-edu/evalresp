#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/*===================================================================
Name:   evresp_1
Purpose:
        FORTRAN callable interface to the evresp routine (below)
Reference:
        SEED. Standard for the Exchange of Earthquake Data
        Reference Manual
        SEED Format Version 2.3 or later
        ??? 1995
Author:    Thomas J. McSweeney, Andrew Cooke

Usage (from FORTRAN):
        See tests/fortran/evresp.f

Notes:
        This routine was updated in release 4.0.0 to support Fortran
        95.  Previous versions were clearly broken (parameters had
        been added with no respect to the implicit lengths from
        Fortran character arrays), so backwards compatibility is not
        provided.

        Given the cleaner interface supported by Fortran 95, this
        routine can also be called from C.

        Whereas the other function returns a linked list of responses
        (one for each response that matched the user's request), this
        routine returns the response for one (1)
        station-channel-network for one (1) effective time.  If more
        than one match is found for a given
        station-channel-network-time, an error condition is raised
        (and a value of -1 is returned to the calling routine to
        indicate failure).  Likewise, a value of 1 is returned if no
        match is found for the given station-channel-network-time.  If
        a unique match is found, a value of 0 is returned to the
        calling routine

 *=================================================================*/
/*
    8/28/2001 -- [ET]  Moved several variable definitions from 'evresp.h'
                       into this file.
   10/19/2005 -- [ET]  Added 'evresp_itp()' function with support for
                       List blockette interpolation; made 'evresp()'
                       call 'evresp_itp()' function with default
                       values for List blockette interpolation parameters.
    2/13/2006 -- [ET]  Moved 'use_delay()' function from 'evalresp.c'
                       to 'evresp.c'; modified to close input file
                       when a single response file is specified.
    3/28/2006 -- [ET]  Added "free(freqs_orig)" at end of 'evresp_itp()'
                       function; added "free_matched_files(output_files)"
                       in 'evresp_itp()' function.
    8/21/2006 -- [IGD] Version 3.2.36: Added support for TESLA units.
    8/23/2006 -- [ET]  Improved detection of pressure ("PA") input units
                       for in/out units check.
   10/16/2006 -- [ET]  Modified to free array allocated in 'evresp_itp()'
                       function.
   02/27/2007 -- [IGD] Added return (#ifdef LIB_MODE) if the input file is not
                       found
   2015-05-04 -- [AC]  Simplifed / fixed for fortran 95
*/

#include <evalresp/evalresp.h>
#include <evalresp/stationxml2resp/ws.h>
#include <stdlib.h>
#include <string.h>
#include <evalresp_log/log.h>

/* define a global flag to use if using "default" units */
int def_units_flag;

/* define a pointer to a channel structure to use in determining the input and
 output units if using "default" units and for use in error output*/
struct channel *GblChanPtr;
float unitScaleFact;

/* define global variables for use in printing error messages */
char *curr_file;
int curr_seq_no;

/* and set a global variable to contain the environment for the setjmp/longjmp
 combination for error handling */
jmp_buf jump_buffer;

char myLabel[20];

int evresp_1(char *sta, char *cha, char *net, char *locid, char *datime,
        char *units, char *file, double *freqs, int nfreqs, double *resp,
        char *rtype, char *verbose, int start_stage, int stop_stage,
        int stdio_flag, int useTotalSensitivityFlag, double x_for_b62,
        int xml_flag) {
    struct response *first = (struct response *) NULL;
    int i, j;
    evalresp_log_t *log = NULL;

    // some eyeball checks to make sure fortran is passing things ok
    // printf("freqs: %f-%f\n", freqs[0], freqs[nfreqs-1]);
    // printf("x_for_b62: %f\n", x_for_b62);

    first = evresp(sta, cha, net, locid, datime, units, file, freqs, nfreqs,
            rtype, verbose, start_stage, stop_stage, stdio_flag, useTotalSensitivityFlag,
            x_for_b62, xml_flag, log);

    /* check the output.  If no response found, return 1, else if more than one response
     found, return -1 */

    if (first == (struct response *) NULL) {
        return (1);
    } else if (first->next != (struct response *) NULL) {
        free_response(first);
        return (-1);
    }

    /* if only one response found, convert from complex output vector into multiplexed
     real output for FORTRAN (real1, imag1, real2, imag2, ..., realN, imagN) */

    for (i = 0, j = 0; i < nfreqs; i++) {
        resp[j++] = (float) first->rvec[i].real;
        resp[j++] = (float) first->rvec[i].imag;
    }

    /* free up dynamically allocated space */

    free_response(first);

    /* and return to FORTRAN program */

    return (0);
}

/* 2/6/2006 -- [ET]  Moved from 'evalresp.c' to 'evresp.c' */
int use_estimated_delay(int flag) {
    /* WE USE THOSE WEIRD magic numbers here because
     * there is a chance that use_delay_flag is not
     * defined: in user program which uses evresp()
     * when use_estimated_delay() is not used before evresp().
     */
    int magic_use_delay = 35443647;
    int magic_dont_use_delay = -90934324;
    static int use_delay_flag = FALSE;
    if (TRUE == flag)
        use_delay_flag = magic_use_delay;
    if (FALSE == flag)
        use_delay_flag = magic_dont_use_delay;

    if (use_delay_flag == magic_use_delay)
        return TRUE;
    return FALSE;
}

/*===================================================================

 Name:      evresp

 Purpose:
        Extract channel response parameters from either ASCII files
        produced by rdseed -r ("response" file) or rdseed -d
        ("sta-cha" files) and calculate the complex response.

 Reference:
        SEED. Standard for the Exchange of Earthquake Data Reference
        Manual SEED Format Version 2.3 or later ??? 1995

 Author:
        Thomas J. McSweeney

 Modifications:
        Ilya Dricker (i.dricker@isti.com) IGD for versions of evalresp 3.2.17

 Notes:

         ???. Version 3.0

        - modified to parse "new" rdseed RESP file output (includes a
        new field that contains the blockette and field numbers for
        each of the items in the RESP file)

        - is a very substantial change over the previous releases of
        evresp.  The code has been completely rewritten from the
        original form authored by Jean-Francios Fels to support
        several new features.  among them are:

        (a) a "new" RESP file format that contains the blockette and
        field numbers as prefixes to each line.  This allows for quick
        determination of whether or not the program is parsing the
        correct information without relying on searching for
        non-standardized character strings in the RESP file

        (b) support for the blockette [61] responses

        (c) support for the response-reference style responses (i.e.
        a blockette [60] followed by a series of blockette [41] or
        blockette [43] through blockette [48] responses)

        - the code has been rewritten so that the calculations are all
        confined to this function and the functions that it calls.
        All the user has to do us supply the appropriate control
        parameters to this function

        - the parsing has been entirely reworked so that each
        blockette style is parsed in a seperate.  This should make the
        code easier to maintain and allow for changes in the output
        from RDSEED (either in number of fields on a line or in which
        fields are output from a given blockette)

        - the code has been converted to ANSI standard C, rather than
          K&R style C

 Thomas J. McSweeney:  tjm@iris.washington.edu

 *=================================================================*/

double Pi;
double twoPi;
/* IGD 08/21/06 Added Tesla */
char SEEDUNITS[][UNITS_STR_LEN] =
        { "Undef Units", "Displacement", "Velocity", "Acceleration", "Counts",
                "Volts", "", "Pascals", "Tesla", "Centigrade" };

char FirstLine[MAXLINELEN];
int FirstField;

/* This version of the function includes
 the 'listinterp...' parameters  */

/* IGD 10/03/13 This version of the function includes x_for_b62 parameter. See comment to calc_resp() function
 for further explanation */

/* IGD 09/30/13 reformatted the code */
struct response *evresp_itp(char *stalst, char *chalst, char *net_code,
        char *locidlst, char *date_time, char *units, char *file, double *freqs,
        int nfreqs, char *rtype, char *verbose, int start_stage, int stop_stage,
        int stdio_flag, int listinterp_out_flag, int listinterp_in_flag,
        double listinterp_tension, int useTotalSensitivityFlag,
        double x_for_b62, int xml_flag, evalresp_log_t *log) {
    struct channel this_channel;
    struct scn *scn;
    struct string_array *sta_list, *chan_list;
    struct string_array *locid_list;
    // TODO - new_file assigned 0 blindly to fix compiler warning.  bug?
    int i, j, k, count = 0, which_matched, test = 1, mode, new_file = 0;
    int err_type;
    char out_name[MAXLINELEN], locid[LOCIDLEN + 1];
    char *locid_ptr, *end_locid_ptr;
    struct matched_files *flst_head = (struct matched_files *) NULL;
    struct matched_files *flst_ptr = NULL, *output_files = NULL;
    struct file_list *lst_ptr = NULL, *tmp_ptr = NULL, *out_file = NULL,
            *tmp_file = NULL;
    struct response *resp = NULL, *next_ptr = NULL;
    struct response *prev_ptr = (struct response *) NULL;
    struct response *first_resp = (struct response *) NULL;
    struct evr_complex *output = NULL;
    struct scn_list *scns = NULL;
    FILE *fptr = NULL;
    double *freqs_orig = NULL; /* for saving the original frequencies */
    int nfreqs_orig;

    /* Let's save the original frequencies requested by a user since they can be overwritten */
    /* if we process blockette 55 IGD for version 3.2.17 of evalresp*/

    nfreqs_orig = nfreqs;
    freqs_orig = (double *) malloc(sizeof(double) * nfreqs_orig);
    memcpy(freqs_orig, freqs, sizeof(double) * nfreqs_orig);

    /* set 'GblChanPtr' to point to 'this_channel' */

    GblChanPtr = &this_channel;

    /* clear out the FirstLine buffer */

    memset(FirstLine, 0, sizeof(FirstLine));

    /* if the verbose flag is set, then print some diagnostic output (other than errors) */

    if (verbose && !strcmp(verbose, "-v")) {
        evalresp_log(log, INFO, 0, "<< EVALRESP RESPONSE OUTPUT V%s >>", REVNUM);
        /*XXX fprintf(stderr, "<< EVALRESP RESPONSE OUTPUT V%s >>\n", REVNUM);
        fflush(stderr); */
    }

    /* first, determine the values of Pi and twoPi for use in evaluating
     the instrument responses later */

    Pi = acos(-1.0);
    twoPi = 2.0 * Pi;

    /* set the values of first_units and last_units to null strings */

    strncpy(this_channel.staname, "", STALEN);
    strncpy(this_channel.network, "", NETLEN);
    strncpy(this_channel.locid, "", LOCIDLEN);
    strncpy(this_channel.chaname, "", CHALEN);
    strncpy(this_channel.beg_t, "", DATIMLEN);
    strncpy(this_channel.end_t, "", DATIMLEN);
    strncpy(this_channel.first_units, "", MAXLINELEN);
    strncpy(this_channel.last_units, "", MAXLINELEN);

    /* and initialize the linked list of pointers to filters */

    this_channel.first_stage = (struct stage *) NULL;

    /* parse the "stalst" string to form a list of stations */

    for (i = 0; i < (int) strlen(stalst); i++) {
        if (stalst[i] == ',')
            stalst[i] = ' ';
    }
    if (!(sta_list = ev_parse_line(stalst, log)))
    {
        /*TODO free things */
        return NULL;
    }

    /* remove any blank spaces from the beginning and end of the string */

    locid_ptr = locidlst;
    strncpy(locid, "", LOCIDLEN);
    while (*locid_ptr && *locid_ptr == ' ')
        locid_ptr++;
    end_locid_ptr = locid_ptr + strlen(locid_ptr) - 1;
    while (end_locid_ptr > locid_ptr && *end_locid_ptr == ' ')
        end_locid_ptr--;
    strncpy(locid, locid_ptr, (end_locid_ptr - locid_ptr + 1));

    /* parse the "locidlst" string to form a list of locations  */
    if (!(locid_list = parse_delim_line(locid, ",", log)))
    {
        /*TODO free things */
        return NULL;
    }

    /* parse the "chalst" string to form a list of channels */
    for (i = 0; i < (int) strlen(chalst); i++) {
        if (chalst[i] == ',')
            chalst[i] = ' ';
    }

    if (!(chan_list = ev_parse_line(chalst, log)))
    {
        /*TODO free things */
        return NULL;
    }

    /* then form a set of network-station-locid-channel tuples to search for */
    scns = alloc_scn_list(
            chan_list->nstrings * sta_list->nstrings * locid_list->nstrings, log);
    if (!scns)
    {
        return NULL;
    }
    for (i = 0; i < sta_list->nstrings; i++) {
        for (j = 0; j < locid_list->nstrings; j++) {
            for (k = 0; k < chan_list->nstrings; k++, count++) {
                scn = scns->scn_vec[count];
                strncpy(scn->station, sta_list->strings[i], STALEN);
                // treat '??' as '*' after long discussion w rob, ilya and eric
                if (strlen(locid_list->strings[j]) == strspn(locid_list->strings[j], "?")) {
                    strcpy(scn->locid, "*");
                } else if (strlen(locid_list->strings[j]) == strspn(locid_list->strings[j], " ")) {
                    memset(scn->locid, 0, LOCIDLEN);
                } else {
                    strncpy(scn->locid, locid_list->strings[j], LOCIDLEN);
                }
                strncpy(scn->channel, chan_list->strings[k], CHALEN);
                strncpy(scn->network, net_code, NETLEN);
            }
        }
    }
#ifdef LOG_LABEL
    sprintf(myLabel, "[%s.%s.%s.%s]", scn->network, scn->station, scn->locid, scn->channel);
#else
    myLabel[0] = '\0';
#endif
    /* if input is from stdin, set fptr to stdin, else find whatever matching files there are */
    if (stdio_flag) {
        fptr = stdin;
        mode = 0;
    } else {
        flst_head = find_files(file, scns, &mode, log);
        flst_ptr = flst_head;
    }

    /* find the responses for each of the station channel pairs as they occur in the file */

    if (!mode && !stdio_flag) {
        curr_file = file;
        if (!(fptr = fopen(file, "r"))) {
            evalresp_log(log, ERROR, 0, "%s failed to open file %s\n", myLabel, file);
            return NULL;
            /*XXX
#ifdef LIB_MODE
            fprintf(stderr, "%s failed to open file %s\n", myLabel, file);
            return NULL;
#else
            error_exit(OPEN_FILE_ERROR, "failed to open file %s", file);
#endif
*/
        }
    }

    /* allocate space for the first response */
    resp = alloc_response(nfreqs, log);
    if (!resp)
    {
        return NULL;
    }

    for (i = 0; i < scns->nscn && (mode || test); i++) {
        /* allocate space for 'matched_files' pointer used to determine if a file has already been read */

        if (!stdio_flag)
            output_files = alloc_matched_files(log);

        /* then check the mode to determine if are parsing one file or a list
         of files (note: if input is from stdin, is one file) */

        if (!mode) {

            /* convert from xml format if necessary, logging error messages to stderr. */
            if (x2r_xml2resp_on_flag(&fptr, xml_flag, X2R_ERROR, log)) return NULL;
            //if (x2r_xml2resp_auto(&fptr, X2R_ERROR)) return NULL;

            which_matched = 0;
            while (test && which_matched >= 0) {
                if (!(err_type = setjmp(jump_buffer))) {
                    new_file = 0;
                    which_matched = find_resp(fptr, scns, date_time,
                            &this_channel, log);

                    /* found a station-channel-network that matched.  First construct
                     an output filename and compare to other output files. If this
                     filename doesn't match any of them, (or if it is the first
                     file found) parse the channel's response information.
                     Otherwise skip it (since a match has already been found) */

                    sprintf(out_name, "%s.%s.%s.%s", this_channel.network,
                            this_channel.staname, this_channel.locid,
                            this_channel.chaname);
#ifdef LOG_LABEL
                    sprintf(myLabel, "[%s]", out_name);
#else
                    myLabel[0] = '\0';
#endif
                    if (!stdio_flag) {
                        tmp_file = output_files->first_list;
                        for (k = 0; k < output_files->nfiles; k++) {
                            out_file = tmp_file;
                            if (!strcmp(out_file->name, out_name))
                                break;
                            tmp_file = out_file->next_file;
                        }
                    }
                    if ((stdio_flag && !new_file) || !output_files->nfiles) {
                        if (!stdio_flag) {
                            output_files->nfiles++;
                            out_file = alloc_file_list(log);
                            output_files->first_list = out_file;
                            out_file->name = alloc_char(strlen(out_name) + 1, log);
                            strcpy(out_file->name, out_name);
                        }
                        new_file = 1;
                    } else if ((stdio_flag && !new_file)
                            || k == output_files->nfiles) {
                        if (!stdio_flag) {
                            output_files->nfiles++;
                            out_file->next_file = alloc_file_list(log);
                            tmp_file = out_file->next_file;
                            out_file = tmp_file;
                            out_file->name = alloc_char(strlen(out_name) + 1, log);
                            strcpy(out_file->name, out_name);
                        }
                        new_file = 1;
                    } else
                        new_file = 0;

                    if (new_file && which_matched >= 0) {
                        /* fill in station-channel-net information for the response */

                        strncpy(resp->station, this_channel.staname, STALEN);
                        strncpy(resp->locid, this_channel.locid, LOCIDLEN);
                        strncpy(resp->channel, this_channel.chaname, CHALEN);
                        strncpy(resp->network, this_channel.network, NETLEN);
                        output = resp->rvec;

                        /* found a station channel pair that matched a response, so parse
                         the response into a channel/filter list */

                        test = parse_channel(fptr, &this_channel, log);

                        if (listinterp_in_flag
                                && this_channel.first_stage->first_blkt->type
                                        == LIST) { /* flag set for interpolation and stage type is "List" */
                            interpolate_list_blockette(
                                    &(this_channel.first_stage->first_blkt->blkt_info.list.freq),
                                    &(this_channel.first_stage->first_blkt->blkt_info.list.amp),
                                    &(this_channel.first_stage->first_blkt->blkt_info.list.phase),
                                    &(this_channel.first_stage->first_blkt->blkt_info.list.nresp),
                                    freqs, nfreqs, listinterp_tension, log);
                        }

                        /* check the filter sequence that was just read */
                        check_channel(&this_channel, log);

                        /* If we process blockette 55, we should recompute resp->rvec */
                        /* because the number of output responses is generally different from */
                        /* what is the user requested */
                        /*if we don't use blockette 55, we should set the frequencies to the original */
                        /* user defined position if we did mess up with frequencies in -possible - blockette 55*/
                        /* containing previous file. Modifications by I.Dricker IGD*/

                        free(resp->rvec);
                        /* 'freqs' array is passed in and should not be freed -- 10/18/2005 -- [ET] */
                        /*        free(freqs); */

                        if (this_channel.first_stage->first_blkt != NULL
                                && this_channel.first_stage->first_blkt->type
                                        == LIST) {
                            /*to prevent segmentation in case of bogus input files */
                            nfreqs =
                                    this_channel.first_stage->first_blkt->blkt_info.list.nresp;
                            freqs = (double *) malloc(sizeof(double) * nfreqs); /* malloc a new vector */
                            memcpy(freqs,
                                    this_channel.first_stage->first_blkt->blkt_info.list.freq,
                                    sizeof(double) * nfreqs); /*cp*/
                            resp->rvec = alloc_complex(nfreqs, log);
                            output = resp->rvec;
                            resp->nfreqs = nfreqs;
                            resp->freqs = (double *) malloc(
                                    sizeof(double) * nfreqs); /* malloc a new vector */
                            memcpy(resp->freqs,
                                    this_channel.first_stage->first_blkt->blkt_info.list.freq,
                                    sizeof(double) * nfreqs); /*cp*/
                        } else {
                            nfreqs = nfreqs_orig;
                            freqs = (double *) malloc(sizeof(double) * nfreqs); /* malloc a new vector */
                            memcpy(freqs, freqs_orig, sizeof(double) * nfreqs); /*cp*/
                            resp->rvec = alloc_complex(nfreqs, log);
                            output = resp->rvec;
                            resp->nfreqs = nfreqs;
                            resp->freqs = (double *) malloc(
                                    sizeof(double) * nfreqs); /* malloc a new vector */
                            memcpy(resp->freqs, freqs_orig,
                                    sizeof(double) * nfreqs); /*cp*/
                        }

                        /* normalize the response of the filter sequence */
                        norm_resp(&this_channel, start_stage, stop_stage, log);

                        /* calculate the response at the requested frequencies */
                        calc_resp(&this_channel, freqs, nfreqs, output, units,
                                start_stage, stop_stage,
                                useTotalSensitivityFlag, x_for_b62, log);

                        /* diagnostic output, if the user requested it */

                        if (verbose && !strcmp(verbose, "-v")) {
                            print_chan(&this_channel, start_stage, stop_stage,
                                    stdio_flag, listinterp_out_flag,
                                    listinterp_in_flag,
                                    useTotalSensitivityFlag, log);
                        }

                        free(freqs); /* free array that was allocated above */

                        /* and, finally, free the memory associated with this channel/filter
                         list and continue searching for the next match */

                        free_channel(&this_channel);
                        if (first_resp == (struct response *) NULL) {
                            first_resp = resp;
                        }
                        next_ptr = alloc_response(nfreqs, log);
                        resp->next = next_ptr;
                        prev_ptr = resp;
                        resp = next_ptr;
                    } else {
                        strncpy(FirstLine, "", MAXLINELEN);
                        test = next_resp(fptr, log);
                    }
                } else {
                    if (new_file)
                        output_files->nfiles--;
                    free_channel(&this_channel);
                    /* catch errors that cause parsing to fail midstream */
                    if (err_type == PARSE_ERROR || err_type == UNRECOG_FILTYPE
                            || err_type == UNDEF_SEPSTR
                            || err_type == IMPROP_DATA_TYPE
                            || err_type == RE_COMP_FAILED
                            || err_type == UNRECOG_UNITS) {
                        strncpy(FirstLine, "", MAXLINELEN);
                        test = next_resp(fptr, log);
                    } else if (err_type == UNDEF_PREFIX) {
                        test = 0;
                    }
                }
            }
            if (!stdio_flag)
                free_matched_files(output_files); /* added 3/28/2006 -- [ET] */

            /* allocated one too many responses */

            free_response(resp);
            if (prev_ptr != (struct response *) NULL)
                prev_ptr->next = (struct response *) NULL;
            break;

        } else if (mode) {
            lst_ptr = flst_ptr->first_list;
            scn = scns->scn_vec[i];
            next_scn: for (j = 0; j < flst_ptr->nfiles; j++) {
                if (!stdio_flag) {
                    fptr = fopen(lst_ptr->name, "r");
                }
                if (fptr) {

                    /* convert from xml format if necessary, logging error messages to stderr. */
                    if (x2r_xml2resp_on_flag(&fptr, xml_flag, X2R_ERROR, log)) return NULL;
                    //if (x2r_xml2resp_auto(&fptr, X2R_ERROR)) return NULL;

                    curr_file = lst_ptr->name;
                    look_again: if (!(err_type = setjmp(jump_buffer))) {
                        new_file = 0;
                        which_matched = get_resp(fptr, scn, date_time, &this_channel, log);
                        if (which_matched >= 0) {

                            /* found a station-channel-network that matched.  First construct
                             an output filename and compare to other output files. If this
                             filename doesn't match any of them, (or if it is the first
                             file found) parse the channel's response information.
                             Otherwise skip it (since a match has already been found) */

                            sprintf(out_name, "%s.%s.%s.%s",
                                    this_channel.network, this_channel.staname,
                                    this_channel.locid, this_channel.chaname);
#ifdef LOG_LABEL
                            sprintf (myLabel, "[%s]", out_name);
#else
                            myLabel[0] = '\0';
#endif
                            tmp_file = output_files->first_list;
                            for (k = 0; k < output_files->nfiles; k++) {
                                out_file = tmp_file;
                                if (!strcmp(out_file->name, out_name))
                                    break;
                                tmp_file = out_file->next_file;
                            }
                            if (!output_files->nfiles) {
                                output_files->nfiles++;
                                out_file = alloc_file_list(log);
                                output_files->first_list = out_file;
                                out_file->name = alloc_char(
                                        strlen(out_name) + 1, log);
                                strcpy(out_file->name, out_name);
                                new_file = 1;
                            } else if (k == output_files->nfiles) {
                                output_files->nfiles++;
                                out_file->next_file = alloc_file_list(log);
                                tmp_file = out_file->next_file;
                                out_file = tmp_file;
                                out_file->name = alloc_char(
                                        strlen(out_name) + 1, log);
                                strcpy(out_file->name, out_name);
                                new_file = 1;
                            } else
                                new_file = 0;

                            if (new_file) {
                                /* fill in station-channel-net information for the response */

                                strncpy(resp->station, this_channel.staname,
                                        STALEN);
                                strncpy(resp->locid, this_channel.locid,
                                        LOCIDLEN);
                                strncpy(resp->channel, this_channel.chaname,
                                        CHALEN);
                                strncpy(resp->network, this_channel.network,
                                        NETLEN);
                                output = resp->rvec;

                                /* parse the response into a channel/filter list */

                                test = parse_channel(fptr, &this_channel, log);

                                /* IGD 01/04/01 Add code preventing a user from defining output units as DIS and ACC if
                                 the input units are PRESSURE after */
                                if (strncmp(this_channel.first_units, "PA -", 4)
                                        == 0) {
                                    if (strcmp(units, "VEL") != 0) {
                                        if (strcmp(units, "DEF") != 0) {
                                            evalresp_log(log, WARN, 0,
                                                    "%s: OUTPUT %s does not make sense if INPUT is PRESSURE\n",
                                                    myLabel, units);
                                            /*XXX fprintf(stderr,
                                                    "%s WARNING: OUTPUT %s does not make sense if INPUT is PRESSURE\n",
                                                    myLabel, units); */
                                            strcpy(units, "VEL");
                                            evalresp_log(log, WARN, 0,
                                                    "%s      OUTPUT units are reset and interpreted as PRESSURE\n",
                                                    myLabel);
                                            /*XXX fprintf(stderr,
                                                    "%s      OUTPUT units are reset and interpreted as PRESSURE\n",
                                                    myLabel); */
                                        }
                                    }
                                }
                                /* IGD 08/21/06 Add code preventing a user from defining output units as DIS and ACC if
                                 the input units are TESLA */
                                if (strncmp(this_channel.first_units, "T -", 3)
                                        == 0) {
                                    if (strcmp(units, "VEL") != 0) {
                                        if (strcmp(units, "DEF") != 0) {
                                            evalresp_log(log, WARN, 0,
                                                    "%s: OUTPUT %s does not make sense if INPUT is MAGNETIC FLUX\n",
                                                    myLabel, units);
                                            /*XXX fprintf(stderr,
                                                    "%s WARNING: OUTPUT %s does not make sense if INPUT is MAGNETIC FLUX\n",
                                                    myLabel, units); */
                                            strcpy(units, "VEL");
                                            evalresp_log(log, WARN, 0,
                                                    "%s      OUTPUT units are reset and interpreted as TESLA\n",
                                                    myLabel);
                                            /*XXX fprintf(stderr,
                                                    "%s      OUTPUT units are reset and interpreted as TESLA\n",
                                                    myLabel); */
                                        }
                                    }
                                }

                                /* IGD 10/03/13 Add code preventing a user from defining output units as DIS and ACC if
                                 the input units are CENTIGRADE */
                                if (strncmp(this_channel.first_units, "C -", 3)
                                        == 0) {
                                    if (strcmp(units, "VEL") != 0) {
                                        if (strcmp(units, "DEF") != 0) {
                                            evalresp_log(log, WARN, 0,
                                                    "%s: OUTPUT %s does not make sense if INPUT is TEMPERATURE\n",
                                                    myLabel, units);
                                            /*XXX fprintf(stderr,
                                                    "%s WARNING: OUTPUT %s does not make sense if INPUT is TEMPERATURE\n",
                                                    myLabel, units); */
                                            strcpy(units, "VEL");
                                            evalresp_log(log, WARN, 0,
                                                    "%s      OUTPUT units are reset and interpreted as  DEGREES CENTIGRADE\n",
                                                    myLabel);
                                            /*XXX fprintf(stderr,
                                                    "%s      OUTPUT units are reset and interpreted as  DEGREES CENTIGRADE\n",
                                                    myLabel); */
                                        }
                                    }
                                }

                                if (listinterp_in_flag
                                        && this_channel.first_stage->first_blkt->type
                                                == LIST) {
                                    /* flag set for interpolation and stage type is "List" */
                                    interpolate_list_blockette(
                                            &(this_channel.first_stage->first_blkt->blkt_info.list.freq),
                                            &(this_channel.first_stage->first_blkt->blkt_info.list.amp),
                                            &(this_channel.first_stage->first_blkt->blkt_info.list.phase),
                                            &(this_channel.first_stage->first_blkt->blkt_info.list.nresp),
                                            freqs, nfreqs, listinterp_tension, log);
                                }

                                /* check the filter sequence that was just read */
                                check_channel(&this_channel, log);

                                /* If we process blockette 55, we should recompute resp->rvec */
                                /* because the number of output responses is generally different from */
                                /* what is the user requested */
                                /*if we don't use blockette 55, we should set the frequencies to the original */
                                /* user defined position if we did mess up with frequencies in -possible - blockette 55*/
                                /* containing previous file. Modifications by I.Dricker / IGD */

                                free(resp->rvec);
                                /* 'freqs' array is passed in and should not be freed -- 10/18/2005 -- [ET] */
                                /* free(freqs); */
                                if (this_channel.first_stage->first_blkt != NULL
                                        && this_channel.first_stage->first_blkt->type
                                                == LIST) {
                                    /* This is to prevent segmentation if the response input is bogus responses */
                                    nfreqs =
                                            this_channel.first_stage->first_blkt->blkt_info.list.nresp;
                                    freqs = (double *) malloc(
                                            sizeof(double) * nfreqs); /* malloc a new vector */
                                    memcpy(freqs,
                                            this_channel.first_stage->first_blkt->blkt_info.list.freq,
                                            sizeof(double) * nfreqs); /*cp*/
                                    resp->rvec = alloc_complex(nfreqs, log);
                                    output = resp->rvec;
                                    resp->nfreqs = nfreqs;
                                    resp->freqs = (double *) malloc(
                                            sizeof(double) * nfreqs); /* malloc a new vector */
                                    memcpy(resp->freqs,
                                            this_channel.first_stage->first_blkt->blkt_info.list.freq,
                                            sizeof(double) * nfreqs); /*cp*/
                                } else {
                                    nfreqs = nfreqs_orig;
                                    freqs = (double *) malloc(
                                            sizeof(double) * nfreqs); /* malloc a new vector */
                                    memcpy(freqs, freqs_orig,
                                            sizeof(double) * nfreqs); /*cp*/
                                    resp->rvec = alloc_complex(nfreqs, log);
                                    output = resp->rvec;
                                    resp->nfreqs = nfreqs;
                                    resp->freqs = (double *) malloc(
                                            sizeof(double) * nfreqs); /* malloc a new vector */
                                    memcpy(resp->freqs, freqs_orig,
                                            sizeof(double) * nfreqs); /*cp*/
                                }

                                /* normalize the response of the filter sequence */
                                norm_resp(&this_channel, start_stage,
                                        stop_stage, log);

                                /* calculate the response at the requested frequencies */
                                calc_resp(&this_channel, freqs, nfreqs, output,
                                        units, start_stage, stop_stage,
                                        useTotalSensitivityFlag, x_for_b62, log);

                                /* diagnostic output, if the user requested it */

                                if (verbose && !strcmp(verbose, "-v")) {
                                    print_chan(&this_channel, start_stage,
                                            stop_stage, stdio_flag,
                                            listinterp_out_flag,
                                            listinterp_in_flag,
                                            useTotalSensitivityFlag, log);
                                }

                                free(freqs); /* free array that was allocated above */

                                /* and, finally, free the memory associated with this
                                 channel/filter list and continue searching for the
                                 next match */

                                free_channel(&this_channel);
                                if (first_resp == (struct response *) NULL) {
                                    first_resp = resp;
                                }
                                next_ptr = alloc_response(nfreqs, log);
                                resp->next = next_ptr;
                                prev_ptr = resp;
                                resp = next_ptr;
                            }
                            FirstField = 0;
                            strncpy(FirstLine, "", MAXLINELEN);
                            if (!stdio_flag) {
                                fclose(fptr);
                            }
                        } else {
                            strncpy(FirstLine, "", MAXLINELEN);
                            test = next_resp(fptr, log);
                            if (!test) {
                                if (!stdio_flag) {
                                    fclose(fptr);
                                }
                            }
                        }

                        /* if not the last file in the list, move on to the next one */
                        if (lst_ptr->next_file != (struct file_list *) NULL) {
                            tmp_ptr = lst_ptr->next_file;
                            lst_ptr = tmp_ptr;
                        }
                    } else {
                        if (new_file)
                            output_files->nfiles--;
                        /* catch errors that cause parsing to fail midstream */
                        if (err_type == PARSE_ERROR
                                || err_type == UNRECOG_FILTYPE
                                || err_type == UNDEF_SEPSTR
                                || err_type == IMPROP_DATA_TYPE
                                || err_type == RE_COMP_FAILED
                                || err_type == UNRECOG_UNITS) {
                            strncpy(FirstLine, "", MAXLINELEN);
                            test = next_resp(fptr, log);
                        } else if (err_type == UNDEF_PREFIX) {
                            test = 0;
                        }
                        free_channel(&this_channel);
                        if (!test) {
                            FirstField = 0;
                            strncpy(FirstLine, "", MAXLINELEN);
                            if (!stdio_flag)
                                fclose(fptr);
                        } else
                            goto look_again;

                        /* if not the last file in the list, move on to the next one */
                        if (lst_ptr->next_file != (struct file_list *) NULL) {
                            tmp_ptr = lst_ptr->next_file;
                            lst_ptr = tmp_ptr;
                        }
                    }
                }
            }
            /* if not the last station-channel-network in the list, move on to the next one */
            if (i < (scns->nscn - 1)) {
                flst_ptr = flst_ptr->ptr_next;
                lst_ptr = flst_ptr->first_list;
                i++;
                scn = scns->scn_vec[i];
                goto next_scn;
            }
            if (!stdio_flag)
                free_matched_files(output_files);

            /* allocated one too many responses */

            free_response(resp);
            if (prev_ptr != (struct response *) NULL)
                prev_ptr->next = (struct response *) NULL;

        } /* end else if mode */

    } /* end for loop */

    /* added file close if single input file -- 2/13/2006 -- [ET]: */
    if (!mode && !stdio_flag) /* if single file was opened then */
        fclose(fptr); /* close input file */

    /* and print a list of WARNINGS about the station-channel pairs that were not
       found in the input RESP files */

    for (i = 0; i < scns->nscn; i++) {
        scn = scns->scn_vec[i];
        if (!scn->found) {
            evalresp_log(log, WARN, 0,
                    "%s: no response found for NET=%s,STA=%s,LOCID=%s,CHAN=%s,DATE=%s\n",
                    myLabel, scn->network, scn->station, scn->locid,
                    scn->channel, date_time);
            /*XXX fprintf(stderr,
                    "%s WARNING: no response found for NET=%s,STA=%s,LOCID=%s,CHAN=%s,DATE=%s\n",
                    myLabel, scn->network, scn->station, scn->locid,
                    scn->channel, date_time);
            fflush(stderr); */
        }
    }
    free_scn_list(scns);
    if (flst_head != (struct matched_files *) NULL)
        free_matched_files(flst_head);
    free_string_array(chan_list);
    free_string_array(locid_list);
    free_string_array(sta_list);

    free(freqs_orig); /* added 3/28/2006 -- [ET] */

    return (first_resp);

}

/* This version of the function does not include
 the 'listinterp...' parameters  */

struct response *evresp(char *stalst, char *chalst, char *net_code,
        char *locidlst, char *date_time, char *units, char *file, double *freqs,
        int nfreqs, char *rtype, char *verbose, int start_stage, int stop_stage,
        int stdio_flag, int useTotalSensitivityFlag, double x_for_b62,
        int xml_flag, evalresp_log_t *log) {
    return evresp_itp(stalst, chalst, net_code, locidlst, date_time, units,
            file, freqs, nfreqs, rtype, verbose, start_stage, stop_stage,
            stdio_flag, 0, 0, 0.0, 0, x_for_b62, xml_flag, log);
}
