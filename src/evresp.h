/**
 * @mainpage Introduction
 *
 * @section purpose Purpose
 *
 * Implements the private interface for evalresp.
 *
 * @section history History
 *
 * Written by <a href="http://www.isti.com/">Instrumental Software
 * Technologies, Inc.</a> (ISTI).
 */

/**
 * @defgroup evalresp_private evalresp Private Interface
 * @ingroup evalresp
 * @brief Private evalresp interface.
 */

/**
 * @defgroup evalresp_private_alloc evalresp Private Memory Allocation Interface
 * @ingroup evalresp_private
 * @brief Private evalresp memory allocation interface.
 */

/**
 * @defgroup evalresp_private_calc evalresp Private Calculation Interface
 * @ingroup evalresp_private
 * @brief Private evalresp calculation interface.
 */

/**
 * @defgroup evalresp_private_error evalresp Private Error Handling Interface
 * @ingroup evalresp_private
 * @brief Private evalresp error handling interface.
 */

/**
 * @defgroup evalresp_private_file evalresp Private File Operations Interface
 * @ingroup evalresp_private
 * @brief Private evalresp file operations interface.
 */

/**
 * @defgroup evalresp_private_parse evalresp Private Parse Interface
 * @ingroup evalresp_private
 * @brief Private evalresp parse interface.
 */

/**
 * @defgroup evalresp_private_print evalresp Private Print Interface
 * @ingroup evalresp_private
 * @brief Private evalresp print interface.
 */

/**
 * @defgroup evalresp_private_resp evalresp Private Response Interface
 * @ingroup evalresp_private
 * @brief Private evalresp response interface.
 */

/**
 * @defgroup evalresp_private_string evalresp Private String Interface
 * @ingroup evalresp_private
 * @brief Private evalresp string interface.
 */

/**
 * @file
 * @brief This file contains declarations and global structures for evalresp.
 */

/*==================================================================
 *                    evresp.h
 *=================================================================*/

/*
 8/28/2001 -- [ET]  Added 'WIN32' directives for Windows compiler
 compatibility; added 'extern' to variable
 declarations at end of file.
 8/2/2001 -- [ET]  Version 3.2.28:  Modified to allow command-line
 parameters for frequency values to be missing
 (default values used).
 10/21/2005 -- [ET]  Version 3.2.29:  Implemented interpolation of
 amplitude/phase values from responses containing
 List blockettes (55); modified message shown when
 List blockette encountered; modified so as not to
 require characters after 'units' specifiers like
 "M" and "COUNTS"; modified to handle case where
 file contains "B052F03 Location:" and nothing
 after it on line; added warnings for unrecognized
 parameters; fixed issue where program would crash
 under Linux/UNIX ("segmentation fault") if input
 response file contained Windows-style "CR/LF"
 line ends; renamed 'strncasecmp()' function
 to 'strnicmp()' when compiling under Windows.
 11/3/2005 -- [ET]  Version 3.2.30:  Modified to unwrap phase values
 before interpolation and re-wrap after (if needed);
 modified to detect when all requested interpolated
 frequencies are out of range.
 1/18/2006 -- [ET]  Version 3.2.31:  Renamed 'regexp' functions to
 prevent name clashes with other libraries.
 2/13/2006 -- [ET]  Version 3.2.32:  Moved 'use_delay()' function from
 'evalresp.c' to 'evresp.c'; modified to close input
 file when a single response file is specified.
 3/27/2006 -- [ID]  Version 3.2.33:  Added include_HEADERS target
 "evr_spline.h" to "Makefile.am"; upgraded "missing"
 script.
 3/28/2006 -- [ET]  Version 3.2.34:  Added 'free()' calls to "evresp.c"
 to fix memory leaks when program functions used
 in external applications.
 4/4/2006 -- [ET]  Version 3.2.35:  Modified to support channel-IDs with
 location codes equal to an empty string.
 8/21/2006 -- [IGD] Version 3.2.36:  Added support for TESLA units.
 10/16/2006 -- [ET]  Version 3.2.37:  Added two more 'free()' calls to
 "evresp.c" to fix memory leaks when program functions
 are used in external applications.
 8/11/2006 -- [IGD] Libtoolized evresp library
 4/03/2007 -- [IGD] Added myLabel global variable which is used to add NSLC
 labels in evalresp logging if --enable-log-label config
 option is used
 5/14/2010 -- [ET]  Version 3.3.3:  Added "#define strcasecmp stricmp"
 if Windows.
 5/31/2013 -- [IGD] Version 3.3.4: Adding polynomial filter
 10/02/2013 -- [IGD] Version 3.3.4: Adding DEGREE CENTIGRADE as units
 */

#ifndef EVRESP_H
#define EVRESP_H

#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <setjmp.h>
#include <stdarg.h>

#include <log.h>

/* if Windows compiler then redefine 'complex' to */
/*  differentiate it from the existing struct,    */
/*  and rename 'strcasecmp' functions:            */
#ifdef _WIN32
#define strcasecmp stricmp
#define strncasecmp strnicmp
#endif

/* IGD 10/16/04 This is for Windows which does not use Makefile.am */
#ifdef VERSION
#define REVNUM VERSION
#else
#define REVNUM "4.0.6"
#endif

// once this is defined (v4.0.6 onwards), some functions take an additional
// axml flag argument
#define EVRESP_XML

/**
 * @private
 * @ingroup evalresp_private
 * @brief True.
 */
#define TRUE 1

/**
 * @private
 * @ingroup evalresp_private
 * @brief False.
 */
#define FALSE 0

/**
 * @private
 * @ingroup evalresp_private
 * @brief Flag to query the value of the flag to use or not use the estimated
 *        delay in response computation.
 * @see use_estimated_delay()
 */
#define QUERY_DELAY -1

/**
 * @private
 * @ingroup evalresp_private
 * @brief Maximum length of station name.

 */
#define STALEN 64

/**
 * @private
 * @ingroup evalresp_private
 * @brief Maximum length of network name.
 */
#define NETLEN 64

/**
 * @private
 * @ingroup evalresp_private
 * @brief Maximum length of location ID.

 */
#define LOCIDLEN 64

/**
 * @private
 * @ingroup evalresp_private
 * @brief Maximum length of channel name.
 */
#define CHALEN 64

/**
 * @private
 * @ingroup evalresp_private
 * @brief Maximum length of output string.
 */
#define OUTPUTLEN 256

/**
 * @private
 * @ingroup evalresp_private
 * @brief Maximum length of temporary string.
 */
#define TMPSTRLEN 64

/**
 * @private
 * @ingroup evalresp_private
 * @brief Maximum length of unit name strings.
 */
#define UNITS_STR_LEN 16

/**
 * @private
 * @ingroup evalresp_private
 * @brief Maximum length of date-time string.
 */
#define DATIMLEN 23

/**
 * @private
 * @ingroup evalresp_private
 * @brief FIXME.
 * @remark Not used anywhere.
 */
#define UNITSLEN 20

/**
 * @private
 * @ingroup evalresp_private
 * @brief Blockette string length.
 */
#define BLKTSTRLEN 4

/**
 * @private
 * @ingroup evalresp_private
 * @brief Field string length.
 */
#define FLDSTRLEN 3

/**
 * @private
 * @ingroup evalresp_private
 * @brief Maximum field string length.
 */
#define MAXFLDLEN 50

/**
 * @private
 * @ingroup evalresp_private
 * @brief Maximum line string length.
 */
#define MAXLINELEN 256

/**
 * @private
 * @ingroup evalresp_private
 * @brief FIR normal tolerance.
 */
#define FIR_NORM_TOL 0.02

/**
 * @private
 * @ingroup evalresp_private
 * @brief FIXME.
 * @remark Not used anywhere.
 */
#define CORRECTION_APPLIED_FLAG 0

/**
 * @private
 * @ingroup evalresp_private
 * @brief Set flag to use the estimated delay in response computation.
 * @see use_estimated_delay()
 */
#define ESTIMATED_DELAY_FLAG 1

/**
 * @private
 * @ingroup evalresp_private
 * @brief FIXME.
 * @remark Not used anywhere.
 */
#define CALC_DELAY_FLAG 2

/**
 * @private
 * @ingroup evalresp_private
 * @brief Enumeration representing the types of units encountered.
 * @note If default, then the response is just given in input units to output
 *       units, no interpretation is made of the units used).
 * @author 02/03/01: IGD: new unit pressure added.
 * @author 08/21/06: IGD: new unit TESLA added.
 * @author 10/03/13: IGD: new units CENTIGRADE added.
*/
enum units {
    UNDEF_UNITS,  /**< Undefined. */
    DIS,  /**< Displacement. */
    VEL,  /**< Velocity. */
    ACC,  /**< Acceleration. */
    COUNTS,  /**< Counts. */
    VOLTS,  /**< Volts. */
    DEFAULT,  /**< Default. */
    PRESSURE,  /**< Pressure. */
    TESLA,  /**< Tesla. */
    CENTIGRADE  /**< Degrees Celsius. */
};


/**
 * @private
 * @ingroup evalresp_private
 * @brief Enumeration representing the types of filters encountered.
 */
enum filt_types {
    UNDEF_FILT,  /**< FIXME. */
    LAPLACE_PZ,  /**< FIXME. */
    ANALOG_PZ,  /**< FIXME. */
    IIR_PZ,  /**< FIXME. */
    FIR_SYM_1,  /**< FIXME. */
    FIR_SYM_2,  /**< FIXME. */
    FIR_ASYM,  /**< FIXME. */
    LIST,  /**< FIXME. */
    GENERIC,  /**< FIXME. */
    DECIMATION,  /**< FIXME. */
    GAIN,  /**< FIXME. */
    REFERENCE,  /**< FIXME. */
    FIR_COEFFS,  /**< FIXME. */
    IIR_COEFFS,  /**< FIXME. */
    POLYNOMIAL  /**< FIXME. */
};

/**
 * @private
 * @ingroup evalresp_private
 * @brief Enumeration representing the types of stages that are recognized.
 * @author 05/15/02: IGD: added GENERIC_TYPE.
 */
enum stage_types {
    UNDEF_STAGE,  /**< FIXME. */
    PZ_TYPE,  /**< FIXME. */
    IIR_TYPE,  /**< FIXME. */
    FIR_TYPE,  /**< FIXME. */
    GAIN_TYPE,  /**< FIXME. */
    LIST_TYPE,  /**< FIXME. */
    IIR_COEFFS_TYPE,  /**< FIXME. */
    GENERIC_TYPE,  /**< FIXME. */
    POLYNOMIAL_TYPE  /**< FIXME. */
};

/**
 * @private
 * @ingroup evalresp_private
 * @brief Enumeration representing the types of error codes possible.
 */
enum error_codes {
    NON_EXIST_FLD = -2,  /**< FIXME. */
    ILLEGAL_RESP_FORMAT = -5,  /**< FIXME. */
    PARSE_ERROR = -4,  /**< FIXME. */
    UNDEF_PREFIX = -3,  /**< FIXME. */
    UNDEF_SEPSTR = -6,  /**< FIXME. */
    OUT_OF_MEMORY = -1,  /**< FIXME. */
    UNRECOG_FILTYPE = -7,  /**< FIXME. */
    UNEXPECTED_EOF = -8,  /**< FIXME. */
    ARRAY_BOUNDS_EXCEEDED = -9,  /**< FIXME. */
    OPEN_FILE_ERROR = 2,  /**< FIXME. */
    RE_COMP_FAILED = 3,  /**< FIXME. */
    MERGE_ERROR = 4,  /**< FIXME. */
    SWAP_FAILED = 5,  /**< FIXME. */
    USAGE_ERROR = 6,  /**< FIXME. */
    BAD_OUT_UNITS = 7,  /**< FIXME. */
    IMPROP_DATA_TYPE = -10,  /**< FIXME. */
    UNSUPPORT_FILTYPE = -11,  /**< FIXME. */
    ILLEGAL_FILT_SPEC = -12,  /**< FIXME. */
    NO_STAGE_MATCHED = -13,  /**< FIXME. */
    UNRECOG_UNITS = -14  /**< FIXME. */
};

/* define structures for the compound data types used in evalesp */

/**
 * @private
 * @ingroup evalresp_private
 * @brief Complex data type.
 */
struct evr_complex {
    double real;  /**< Real part. */
    double imag;  /**< Imaginary part. */
};

/**
 * @private
 * @ingroup evalresp_private
 * @brief Array of string objects.
 */
struct string_array {
    int nstrings;  /**< Number of strings. */
    char **strings;  /**< String array. */
};

/**
 * @private
 * @ingroup evalresp_private
 * @brief Network-station-locid-channel object.
 */
struct scn {
    char *station;  /**< Station name. */
    char *network;  /**< Network name. */
    char *locid;  /**< Location ID. */
    char *channel;  /**< Channel name. */
    int found;  /**< Flag (true/false) if found in the input RESP files. */
};

/**
 * @private
 * @ingroup evalresp_private
 * @brief Response object.
 */
struct response {
    char station[STALEN];  /**< Station name. */
    char network[NETLEN];  /**< Network name. */
    char locid[LOCIDLEN];  /**< Location ID. */
    char channel[CHALEN];  /**< Channel name. */
    struct evr_complex *rvec;  /**< Output vector. */
    int nfreqs;   /**< Number of frequencies. */ /* Add by I.Dricker IGD to support blockette 55 */
    double *freqs;  /**< Array of frequencies. */ /* Add by I.Dricker IGD to support blockette 55 */
    struct response *next;  /**< Pointer to next response object. */
};

/**
 * @private
 * @ingroup evalresp_private
 * @brief File list object.
 */
struct file_list {
    char *name;  /**< File name. */
    struct file_list *next_file;  /**< Pointer to next file list object. */
};

/**
 * @private
 * @ingroup evalresp_private
 * @brief Matches files object.
 */
struct matched_files {
    int nfiles;  /**< Number of files. */
    struct file_list *first_list;  /**< Array of file list objects. */
    struct matched_files *ptr_next;  /**< Pointer to next matches files object. */
};

/**
 * @private
 * @ingroup evalresp_private
 * @brief List of network-station-locid-channel objects.
 */
struct scn_list {
    int nscn;  /**< Number of network-station-locid-channel objects. */
    struct scn **scn_vec;  /**< Array of network-station-locid-channel objects. */
};

/* define structures for the various types of filters defined in seed */

/**
 * @private
 * @ingroup evalresp_private
 * @brief A Response (Poles & Zeros) blockette.
 */
struct pole_zeroType {
    int nzeros;  /**< Number of zeros (blockettes [43] or [53]). */
    int npoles;  /**< Number of poles. */
    double a0;  /**< FIXME. */
    double a0_freq;  /**< FIXME. */
    struct evr_complex *zeros;  /**< Array of zeros (complex). */
    struct evr_complex *poles;  /**< Array of poles (complex). */
};

/**
 * @private
 * @ingroup evalresp_private
 * @brief A Response (Coefficients) blockette.
 */
struct coeffType {
    int nnumer;  /**< FIXME. (blockettes [44] or [54]) */
    int ndenom;  /**< FIXME. */
    double *numer;  /**< FIXME. */
    double *denom;  /**< FIXME. */
    double h0;  /**< FIXME. */  /* IGD this field is new v 3.2.17 */
};

/**
 * @private
 * @ingroup evalresp_private
 * @brief A Response (Coefficients) blockette.
 */
struct polynomialType {
    unsigned char approximation_type;  /**< FIXME. (blockettes [42] or [62]) */  /* IGD 05/31/2013 */
    unsigned char frequency_units;  /**< FIXME. */
    double lower_freq_bound;  /**< FIXME. */
    double upper_freq_bound;  /**< FIXME. */
    double lower_approx_bound;  /**< FIXME. */
    double upper_approx_bound;  /**< FIXME. */
    double max_abs_error;  /**< FIXME. */
    int ncoeffs;  /**< FIXME. */
    double *coeffs;  /**< FIXME. */
    double *coeffs_err;  /**< FIXME. */
};

/**
 * @private
 * @ingroup evalresp_private
 * @brief A FIR Response blockette.
 */
struct firType {
    int ncoeffs;  /**< Number of coefficients (blockettes [41] or [61]). */
    double *coeffs;  /**< Array of coefficients. */
    double h0;  /**< FIXME. */
};

/**
 * @private
 * @ingroup evalresp_private
 * @brief A Response (List) blockette.
 */
struct listType {
    int nresp;  /**< Number of responses (blockettes [45] or [55]). */
    double *freq;  /**< Array of freqencies. */
    double *amp;  /**< Array of amplitudes. */
    double *phase;  /**< Array of phases. */
};

/**
 * @private
 * @ingroup evalresp_private
 * @brief A Generic Response blockette.
 */
struct genericType {
    int ncorners;  /**< FIXME. (blockettes [46] or [56]) */
    double *corner_freq;  /**< FIXME. */
    double *corner_slope;  /**< FIXME. */
};

/**
 * @private
 * @ingroup evalresp_private
 * @brief A Decimation blockette.
 */
struct decimationType {
    double sample_int;  /**< FIXME. (blockettes [47] or [57]) */
    int deci_fact;  /**< FIXME. */
    int deci_offset;  /**< FIXME. */
    double estim_delay;  /**< FIXME. */
    double applied_corr;  /**< FIXME. */
};

/**
 * @private
 * @ingroup evalresp_private
 * @brief A Channel Sensitivity/Gain blockette.
 */
struct gainType {
    double gain;  /**< FIXME. (blockettes [48] or [58]) */
    double gain_freq;  /**< FIXME. */
};

/**
 * @private
 * @ingroup evalresp_private
 * @brief A Response Reference blockette.
 */
struct referType {
    int num_stages;  /**< FIXME. */
    int stage_num;  /**< FIXME. */
    int num_responses;  /**< FIXME. */
};

/**
 * @private
 * @ingroup evalresp_private
 * @brief Define a blkt as a stucture containing the blockette type, a union
 *        (blkt_info) containing the blockette info, and a pointer to the
 *        next blockette in the filter sequence.
 * @details The structures will be assembled to form a linked list of
 *          blockettes that make up a filter, with the last blockette
 *          containing a '(struct blkt *)NULL' pointer in the 'next_blkt'
 *          position.
 */
struct blkt {
    int type;  /**< Blockette type. */
    union {
        struct pole_zeroType pole_zero;  /**< FIXME. */
        struct coeffType coeff;  /**< FIXME. */
        struct firType fir;  /**< FIXME. */
        struct listType list;  /**< FIXME. */
        struct genericType generic;  /**< FIXME. */
        struct decimationType decimation;  /**< FIXME. */
        struct gainType gain;  /**< FIXME. */
        struct referType reference;  /**< FIXME. */
        struct polynomialType polynomial;  /**< FIXME. */
    } blkt_info;  /**< Blockette info. */
    struct blkt *next_blkt;  /**< Pointer to next blockette. */
};

/**
 * @private
 * @ingroup evalresp_private
 * @brief Define a stage as a structure that contains the sequence number, the
 *        input and output units, a pointer to the first blockette of the
 *        filter, and a pointer to the next stage in the response.
 * @details Again, the last stage in the response will be indicated by a
 *          '(struct stage *)NULL pointer in the 'next_stage' position.
 */
struct stage {
    int sequence_no;  /**< Sequence number. */
    int input_units;  /**< Input units. */
    int output_units;  /**< Output units. */
    struct blkt *first_blkt;  /**< Pointer to first blockette of the filter. */
    struct stage *next_stage;  /**< Pointer to the next stage in the response. */
};

/**
 * @private
 * @ingroup evalresp_private
 * @brief And define a channel as a structure containing a pointer to the head
 *        of a linked list of stages.
 * @details Will access the pieces one stages at a time in the same order that
 *          they were read from the input file, so a linked list is the
 *          easiest way to do this (since we don't have to worry about the
 *          time penalty inherent in following the chain). As an example, if
 *          the first stage read was a pole-zero stage, then the parts of that
 *          stage contained in a channel structure called "ch" would be
 *          accessed as:

@verbatim
struct stage *stage_ptr;
struct blkt *blkt_ptr;

stage_ptr = ch.first_stage;
blkt_ptr = stage_ptr->first_blkt;
if(blkt_ptr->type == LAPLACE_PZ || blkt_ptr->type == ANALOG_PZ ||
blkt_ptr->type == IIR_PZ){
nzeros = blkt_ptr->blkt_info.poles_zeros.nzeros;
........
}
@endverbatim

 */
struct channel {
    char staname[STALEN];  /**< FIXME. */
    char network[NETLEN];  /**< FIXME. */
    char locid[LOCIDLEN];  /**< FIXME. */
    char chaname[CHALEN];  /**< FIXME. */
    char beg_t[DATIMLEN];  /**< FIXME. */
    char end_t[DATIMLEN];  /**< FIXME. */
    char first_units[MAXLINELEN];  /**< FIXME. */
    char last_units[MAXLINELEN];  /**< FIXME. */
    double sensit;  /**< FIXME. */
    double sensfreq;  /**< FIXME. */
    double calc_sensit;  /**< FIXME. */
    double calc_delay;  /**< FIXME. */
    double estim_delay;  /**< FIXME. */
    double applied_corr;  /**< FIXME. */
    double sint;  /**< FIXME. */
    int nstages;  /**< FIXME. */
    struct stage *first_stage;  /**< Pointer to the head of a linked list of
                                   stage. */
};

/**
 * @private
 * @ingroup evalresp_private
 * @brief Structure used for time comparisons.
 */
struct dateTime {
    int year;  /**< Year. */
    int jday;  /**< Day of year. */
    int hour;  /**< Hour. */
    int min;  /**< Minutes. */
    float sec;  /**< Seconds. */
};

/**
 * @private
 * @ingroup evalresp_private
 * @brief FIXME.
 * @author 2007/02/27: IGD.
 */
extern char myLabel[20];
//char myLabel[20] = "aa";

/* utility routines that are used to parse the input file line by line and
 convert the input to what the user wants */

/**
 * @private
 * @ingroup evalresp_private_string
 * @brief Parses the fields on a line into separate strings.
 * @details The definition of a field is a sequence of any non-white space
 *          characters with bordering white space. The result is a structure
 *          containing the number of fields on the line and an array of
 *          character strings (which are easier to deal with than the original
 *          line).
 * @param[in] line String to parse.
 * @returns Array of string objects.
 */
struct string_array *ev_parse_line(char *line);

/**
 * @private
 * @ingroup evalresp_private_string
 * @brief Parses the fields on a line into separate strings using delimeter.
 * @details The definition of a field is a sequence of any non-delimeter
 *          characters with bordering delimiter. The result is a structure
 *          containing the number of fields on the line and an array of
 *          character strings (which are easier to deal with than the original
 *          line).
 * @param[in] line String to parse.
 * @param[in] delim Delimiter string.
 * @returns Array of string objects.
 */
struct string_array *parse_delim_line(char *line, char *delim);

/**
 * @private
 * @ingroup evalresp_private_string
 * @brief Returns the indicated field from the next 'non-comment' line from a
 *        RESP file using blockette number and field number.
 * @details Exits with error if no non-comment lines left in file or if
 *          expected blockette and field numbers do not match those found in
 *          the next non-comment line.
 * @param[in] fptr FILE pointer.
 * @param[out] return_field Return field.
 * @param[in] blkt_no Blockette number.
 * @param[in] fld_no Field number.
 * @param[in] sep Field separator.
 * @param[in] fld_wanted Field wanted.
 * @returns Length of the resulting field if successful.
 * @note Here a field is any string of non-white characters surrounded by
 *       white space.
 */
int get_field(FILE *fptr, char *return_field, int blkt_no, int fld_no,
              char *sep, int fld_wanted);

/**
 * @private
 * @ingroup evalresp_private_string
 * @brief Returns the indicated field from the next 'non-comment' line from a
 *        RESP file.
 * @details Returns with a value of zero if no non-comment lines left in file
 *          or if expected blockette and field numbers do not match those
 *          found in the next non-comment line.
 * @param[in] fptr FILE pointer.
 * @param[out] return_field Return field.
 * @param[out] blkt_no Blockette number.
 * @param[out] fld_no Field number.
 * @param[in] sep Field separator.
 * @param[in] fld_wanted Field wanted.
 * @returns Length of the resulting field if successful.
 * @returns Value of zero otherwise.
 * @note Here a field is any string of non-white characters surrounded by
 *       white space.
 */
int test_field(FILE *fptr, char *return_field, int *blkt_no, int *fld_no,
               char *sep, int fld_wanted);
/**
 * @private
 * @ingroup evalresp_private_string
 * @brief Returns the next 'non-comment' line from a RESP file using blockette
 *        number and field number.
 * @details Exits with error if no non-comment lines left in file or if
 *          expected blockette and field numbers do not match those found in
 *          the next non-comment line.
 * @param[in] fptr FILE pointer.
 * @param[out] return_line Return line.
 * @param[in] blkt_no Blockette number.
 * @param[in] fld_no Field number.
 * @param[in] sep Separator.
 * @returns length of the resulting line if successful
 * @note Exits with error if no non-comment lines left in file or if expected
 *       blockette and field numbers do not match those found in the next
 *       non-comment line.
 * @author 2004.079: SBH: Added code to skip over valid lines that we didn't
 *                        expect. Support for SHAPE formatte RESP files, and
 *                        to skip blank lines.
 */
int get_line(FILE *fptr, char *return_line, int blkt_no, int fld_no, char *sep);

/**
 * @private
 * @ingroup evalresp_private_string
 * @brief Returns the next 'non-comment' line from a RESP file.
 * @param[in] fptr FILE pointer.
 * @param[out] return_line Return line.
 * @param[out] blkt_no Blockette number.
 * @param[out] fld_no Field number.
 * @param[in] sep Separator.
 * @returns The fld_no of the resulting line if successful.
 * @returns 0 if no non-comment lines left in file, regardless of the
 *          blockette and field numbers for the line (these values are
 *          returned as the values of the input pointer variables @p fld_no
 *          and @p blkt_no).
 * @author 2004.079: SBH: Added code to skip blank lines.
 */
int next_line(FILE *fptr, char *return_line, int *blkt_no, int *fld_no, char *sep);

/**
 * @private
 * @ingroup evalresp_private_string
 * @brief FIXME.
 */
int count_fields(char *);

/**
 * @private
 * @ingroup evalresp_private_string
 * @brief FIXME.
 */
int count_delim_fields(char *, char *);

/**
 * @private
 * @ingroup evalresp_private_string
 * @brief FIXME.
 */
int parse_field(char *, int, char *);

/**
 * @private
 * @ingroup evalresp_private_string
 * @brief FIXME.
 */
int parse_delim_field(char *, int, char *, char *);

/**
 * @private
 * @ingroup evalresp_private_string
 * @brief FIXME.
 */
int check_line(FILE *, int *, int *, char *);

/**
 * @private
 * @ingroup evalresp_private_string
 * @brief Uses get_fld() to return the integer value of the input string.
 * @details If the requested field is not a proper representation of a number,
 *          then 'IMPROPER DATA TYPE' error is signaled.
 * @param[in] in_line Input string.
 * @returns Integer value on success.
 */
int get_int(char *in_line);

/**
 * @private
 * @ingroup evalresp_private_string
 * @brief Uses get_fld() to return the double-precision value of the input
 *        string.
 * @details If the requested field is not a proper representation of a number,
 *          then 'IMPROPER DATA TYPE' error is signaled.
 * @param[in] in_line Input string.
 * @returns Double value on success.
 */
double get_double(char *in_line);

/**
 * @private
 * @ingroup evalresp_private_string
 * @brief Checks an incoming line for keys that indicate the units represented
 *        by a filter.
 * @details If the units are not recognized, an 'UNDEFINED UNITS' error
 *          condition is signaled. If the user specified that 'default' unit
 *          should be used, then the line is simply saved as the value of
 *          'SEEDUNITS[DEFAULT]' and no check of the units is made.
 * @param[in] line Incoming line.
 * @see units
 */
int check_units(char *line);

/**
 * @private
 * @ingroup evalresp_private_string
 * @brief Compares an input string (string) with a regular expression or
 *        glob-style "pattern" (expr) using the re_comp() and re_exec()
 *        functions (from stdlib.h).
 * @details - First, if the type-flag is set to the string "-g", the
 *            glob-style 'expr' is changed so that any '*' characters are
 *            converted to '.*' combinations and and '?' characters are
 *            converted to '.' characters.
 *
 *          - If the type-flag is set to "-r" then no conversions are
 *            necessary (the string is merely copied to the new location).
 *
 *          - Finally, the 'regexp_pattern' argument is passed through the
 *            re_comp() routine (compiling the pattern), and the value of
 *            re_exec(string) is returned to the calling function.
 *
 * @param[in] string Input string.
 * @param[in] expr Regularion expression or glob-style pattern expression.
 * @param[in] type_flag Type flag, @c -g or @c -r.
 * @returns 0 if false.
 * @returns >0 if true.
 */
int string_match(const char *string, char *expr, char *type_flag);

/**
 * @private
 * @ingroup evalresp_private_string
 * @brief A function that tests whether a string can be converted into an
 *        integer using string_match().
 * @param[in] test String to test.
 * @returns 0 if false.
 * @returns >0 if true.
 */
int is_int(const char *test);

/**
 * @private
 * @ingroup evalresp_private_string
 * @brief A function that tests whether a string can be converted into an
 *        double using string_match().
 * @param[in] test String to test.
 * @returns 0 if false.
 * @returns >0 if true.
*/
int is_real(const char *);

/**
 * @private
 * @ingroup evalresp_private_string
 * @brief Used to check out if we are using a FIR or IIR coefficients in
 *        blockette 54.
 * @details This information is contained in the 10th field of this blockette
 *          (number of denominators); if the number of denominators is 0: we
 *          got FIR. Otherwise, it is IIR. is_IIR_coeff() reads the text
 *          response file; finds the proper line and decides if the response is
 *          IIR or FIR. The text file is then fseek() to the original position
 *          and the function returns.
 * @param[in] fp FILE pointer.
 * @param[in] position Position.
 * @returns 1 if it is IIR.
 * @returns 0 if it is not IIR.
 * @returns 0 in case of error.
 * @author 07/00: IGD I.Dricker ISTI i.dricker@isti.com for evalresp 3.2.17.
 */
int is_IIR_coeffs(FILE *fp, int position);

/* routines used to load a channel's response information into a linked
 list of filter stages, each containing a linked list of blockettes */

/**
 * @private
 * @ingroup evalresp_private_parse
 * @brief FIXME.
 */
int find_resp(FILE *, struct scn_list *, char *, struct channel *);

/**
 * @private
 * @ingroup evalresp_private_parse
 * @brief FIXME.
 */
int get_resp(FILE *, struct scn *, char *, struct channel *);

/**
 * @private
 * @ingroup evalresp_private_parse
 * @brief FIXME.
 */
int get_channel(FILE *, struct channel *);

/**
 * @private
 * @ingroup evalresp_private_parse
 * @brief FIXME.
 */
int next_resp(FILE *);

/* routines used to create a list of files matching the users request */

/**
 * @private
 * @ingroup evalresp_private_file
 * @brief FIXME.
 */
struct matched_files *find_files(char *, struct scn_list *, int *);

/**
 * @private
 * @ingroup evalresp_private_file
 * @brief FIXME.
 */
int get_names(char *, struct matched_files *);

/* routines used to allocate vectors of the basic data types used in the
 filter stages */

/**
 * @private
 * @ingroup evalresp_private_alloc
 * @brief FIXME.
 */
struct evr_complex *alloc_complex(int);

/**
 * @private
 * @ingroup evalresp_private_alloc
 * @brief FIXME.
 */
struct response *alloc_response(int);

/**
 * @private
 * @ingroup evalresp_private_alloc
 * @brief FIXME.
 */
struct string_array *alloc_string_array(int);

/**
 * @private
 * @ingroup evalresp_private_alloc
 * @brief FIXME.
 */
struct scn *alloc_scn(void);

/**
 * @private
 * @ingroup evalresp_private_alloc
 * @brief FIXME.
 */
struct scn_list *alloc_scn_list(int);

/**
 * @private
 * @ingroup evalresp_private_alloc
 * @brief FIXME.
 */
struct file_list *alloc_file_list(void);

/**
 * @private
 * @ingroup evalresp_private_alloc
 * @brief FIXME.
 */
struct matched_files *alloc_matched_files(void);

/**
 * @private
 * @ingroup evalresp_private_alloc
 * @brief FIXME.
 */
double *alloc_double(int);

/**
 * @private
 * @ingroup evalresp_private_alloc
 * @brief FIXME.
 */
char *alloc_char(int);

/**
 * @private
 * @ingroup evalresp_private_alloc
 * @brief FIXME.
 */
char **alloc_char_ptr(int);

/* allocation routines for the various types of filters */

/**
 * @private
 * @ingroup evalresp_private_alloc
 * @brief FIXME.
 */
struct blkt *alloc_pz(void);

/**
 * @private
 * @ingroup evalresp_private_alloc
 * @brief FIXME.
 */
struct blkt *alloc_coeff(void);

/**
 * @private
 * @ingroup evalresp_private_alloc
 * @brief FIXME.
 */
struct blkt *alloc_fir(void);

/**
 * @private
 * @ingroup evalresp_private_alloc
 * @brief FIXME.
 */
struct blkt *alloc_ref(void);

/**
 * @private
 * @ingroup evalresp_private_alloc
 * @brief FIXME.
 */
struct blkt *alloc_gain(void);

/**
 * @private
 * @ingroup evalresp_private_alloc
 * @brief FIXME.
 */
struct blkt *alloc_list(void);

/**
 * @private
 * @ingroup evalresp_private_alloc
 * @brief FIXME.
 */
struct blkt *alloc_generic(void);

/**
 * @private
 * @ingroup evalresp_private_alloc
 * @brief FIXME.
 */
struct blkt *alloc_deci(void);

/**
 * @private
 * @ingroup evalresp_private_alloc
 * @brief FIXME.
 */
struct blkt *alloc_polynomial(void); /*IGD 05/31/2013 */

/**
 * @private
 * @ingroup evalresp_private_alloc
 * @brief FIXME.
 */
struct stage *alloc_stage(void);

/* routines to free up space associated with dynamically allocated
 structure members */

/**
 * @private
 * @ingroup evalresp_private_alloc
 * @brief FIXME.
 */
void free_string_array(struct string_array *);

/**
 * @private
 * @ingroup evalresp_private_alloc
 * @brief FIXME.
 */
void free_scn(struct scn *);

/**
 * @private
 * @ingroup evalresp_private_alloc
 * @brief FIXME.
 */
void free_scn_list(struct scn_list *);

/**
 * @private
 * @ingroup evalresp_private_alloc
 * @brief FIXME.
 */
void free_matched_files(struct matched_files *);

/**
 * @private
 * @ingroup evalresp_private_alloc
 * @brief FIXME.
 */
void free_file_list(struct file_list *);

/**
 * @private
 * @ingroup evalresp_private_alloc
 * @brief FIXME.
 */
void free_pz(struct blkt *);

/**
 * @private
 * @ingroup evalresp_private_alloc
 * @brief FIXME.
 */
void free_coeff(struct blkt *);

/**
 * @private
 * @ingroup evalresp_private_alloc
 * @brief FIXME.
 */
void free_fir(struct blkt *);

/**
 * @private
 * @ingroup evalresp_private_alloc
 * @brief FIXME.
 */
void free_list(struct blkt *);

/**
 * @private
 * @ingroup evalresp_private_alloc
 * @brief FIXME.
 */
void free_generic(struct blkt *);

/**
 * @private
 * @ingroup evalresp_private_alloc
 * @brief FIXME.
 */
void free_gain(struct blkt *);

/**
 * @private
 * @ingroup evalresp_private_alloc
 * @brief FIXME.
 */
void free_deci(struct blkt *);

/**
 * @private
 * @ingroup evalresp_private_alloc
 * @brief FIXME.
 */
void free_ref(struct blkt *);

/**
 * @private
 * @ingroup evalresp_private_alloc
 * @brief FIXME.
 */
void free_stages(struct stage *);

/**
 * @private
 * @ingroup evalresp_private_alloc
 * @brief FIXME.
 */
void free_channel(struct channel *);

/**
 * @private
 * @ingroup evalresp_private_alloc
 * @brief FIXME.
 */
void free_response(struct response *);

/* simple error handling routines to standardize the output error values and
 allow for control to return to 'evresp' if a recoverable error occurs */

/**
 * @private
 * @ingroup evalresp_private_error
 * @brief Wrapper function which prints a message and exits the program.
 * @warning Do not use in library calls.
 */
void error_exit(int, char *, ...);

/**
 * @private
 * @ingroup evalresp_private_error
 * @brief FIXME.
 */
void error_return(int, char *, ...);

/* a simple routine that parses the station information from the input file */

/**
 * @private
 * @ingroup evalresp_private_parse
 * @brief FIXME.
 */
int parse_channel(FILE *, struct channel *);

/* parsing routines for various types of filters */

/**
 * @private
 * @ingroup evalresp_private_parse
 * @brief FIXME.
 */
int parse_pref(int *, int *, char *);

/**
 * @private
 * @ingroup evalresp_private_parse
 * @brief FIXME.
 */
void parse_pz(FILE *, struct blkt *, struct stage *); /* pole-zero */

/**
 * @private
 * @ingroup evalresp_private_parse
 * @brief FIXME.
 */
void parse_coeff(FILE *, struct blkt *, struct stage *); /* fir */

/**
 * @private
 * @ingroup evalresp_private_parse
 * @brief FIXME.
 */
void parse_iir_coeff(FILE *, struct blkt *, struct stage *); /*I.Dricker IGD for
 2.3.17 iir */

/**
 * @private
 * @ingroup evalresp_private_parse
 * @brief FIXME.
 */
void parse_list(FILE *, struct blkt *, struct stage *); /* list */

/**
 * @private
 * @ingroup evalresp_private_parse
 * @brief FIXME.
 */
void parse_generic(FILE *, struct blkt *, struct stage *); /* generic */

/**
 * @private
 * @ingroup evalresp_private_parse
 * @brief FIXME.
 */
int parse_deci(FILE *, struct blkt *); /* decimation */

/**
 * @private
 * @ingroup evalresp_private_parse
 * @brief FIXME.
 */
int parse_gain(FILE *, struct blkt *); /* gain/sensitivity */

/**
 * @private
 * @ingroup evalresp_private_parse
 * @brief FIXME.
 */
void parse_fir(FILE *, struct blkt *, struct stage *); /* fir */

/**
 * @private
 * @ingroup evalresp_private_parse
 * @brief FIXME.
 */
void parse_ref(FILE *, struct blkt *, struct stage *); /* response reference */

/**
 * @private
 * @ingroup evalresp_private_parse
 * @brief FIXME.
 */
void parse_polynomial(FILE *, struct blkt *, struct stage *); /* polynomial B42 B62 IGD 05/31/2013 */

/* remove trailing white space from (if last arg is 'a') and add null character to
 end of (if last arg is 'a' or 'e') input (FORTRAN) string */

/**
 * @private
 * @ingroup evalresp_private_string
 * @brief Add a null character to the end of a string.
 * @details @p where is a character that specifies where the null character
 *          should be placed, the possible values are:
 *
 *          - @c a - remove extra spaces from end of string, then adds null
 *                   character;
 *
 *          - @c e - adds null character to end of character string.
 *
 * @param[in,out] s String.
 * @param[in] len Length of string.
 * @param[in] where Where to place null character, @c a or @c e.
 * @returns New length of string.
 */
int add_null(char *s, int len, char where);

/* run a sanity check on the channel's filter sequence */

/**
 * @private
 * @ingroup evalresp_private_response
 * @brief FIXME.
 */
void merge_coeffs(struct blkt *, struct blkt **);

/**
 * @private
 * @ingroup evalresp_private_response
 * @brief FIXME.
 */
void merge_lists(struct blkt *, struct blkt **); /* Added by I.Dricker IGD for v
 3.2.17 of evalresp */

/**
 * @private
 * @ingroup evalresp_private_response
 * @brief FIXME.
 */
void check_channel(struct channel *);

/**
 * @private
 * @ingroup evalresp_private_response
 * @brief FIXME.
 */
void check_sym(struct blkt *, struct channel *);

/* routines used to calculate the instrument responses */

/*void calc_resp(struct channel *, double *, int, struct evr_complex *,char *, int, int);*/
/**
 * @private
 * @ingroup evalresp_private_calc
 * @brief FIXME.
 */
void calc_resp(struct channel *chan, double *freq, int nfreqs,
        struct evr_complex *output, char *out_units, int start_stage,
        int stop_stage, int useTotalSensitivityFlag, double x_for_b62);

/**
 * @private
 * @ingroup evalresp_private_calc
 * @brief FIXME.
 */
void convert_to_units(int, char *, struct evr_complex *, double);

/**
 * @private
 * @ingroup evalresp_private_calc
 * @brief FIXME.
 */
void analog_trans(struct blkt *, double, struct evr_complex *);

/**
 * @private
 * @ingroup evalresp_private_calc
 * @brief FIXME.
 */
void fir_sym_trans(struct blkt *, double, struct evr_complex *);

/**
 * @private
 * @ingroup evalresp_private_calc
 * @brief FIXME.
 */
void fir_asym_trans(struct blkt *, double, struct evr_complex *);

/**
 * @private
 * @ingroup evalresp_private_calc
 * @brief FIXME.
 */
void iir_pz_trans(struct blkt *, double, struct evr_complex *);

/**
 * @private
 * @ingroup evalresp_private_calc
 * @brief FIXME.
 */
void calc_time_shift(double, double, struct evr_complex *);

/**
 * @private
 * @ingroup evalresp_private_calc
 * @brief FIXME.
 */
void zmul(struct evr_complex *, struct evr_complex *);

/**
 * @private
 * @ingroup evalresp_private_calc
 * @brief FIXME.
 */
void norm_resp(struct channel *, int, int);

/**
 * @private
 * @ingroup evalresp_private_calc
 * @brief FIXME.
 */
void calc_list(struct blkt *, int, struct evr_complex *); /*IGD i.dricker@isti.c
om for version 3.2.17 */
/**
 * @private
 * @ingroup evalresp_private_calc
 * @brief FIXME.
 */

/**
 * @private
 * @ingroup evalresp_private_calc
 * @brief FIXME.
 */
void calc_polynomial(struct blkt *, int, struct evr_complex *, double); /*IGD 06/01/2013 */

/**
 * @private
 * @ingroup evalresp_private_calc
 * @brief FIXME.
 */
void iir_trans(struct blkt *, double, struct evr_complex *); /* IGD for version 3.2.17 */

/**
 * @private
 * @ingroup evalresp_private_string
 * @brief A function that tests whether a string looks like a time string
 *        using string_match().
 * @detailed Time strings must be in the format 'hh:mm:ss[.#####]', so more
 *          than 14 characters is an error (too many digits).
 * @param[in] test String to test.
 * @returns 0 if false.
 * @returns >0 if true.
 */
int is_time(const char *);

/**
 * @private
 * @ingroup evalresp_private_parse
 * @brief Compare two times and determine if the first is greater, equal to,
 *        or less than the second.
 */
int timecmp(struct dateTime *dt1, struct dateTime *dt2);

/**
 * @private
 * @ingroup evalresp_private_print
 * @brief Print the channel info, followed by the list of filters.
 */
void print_chan(struct channel *, int, int, int, int, int, int);

/**
 * @private
 * @ingroup evalresp_private_print
 * @brief Print the response information to the output files.
 */
void print_resp(double *, int, struct response *, char *, int);

/**
 * @private
 * @ingroup evalresp_private_print
 * @brief Print the response information to the output files.
 */
void print_resp_itp(double *, int, struct response *, char *, int, int, double,
        int);

/**
 * @private
 * @ingroup evalresp_private
 * @brief Evaluate responses for user requested station/channel/network tuple
 *        at the frequencies requested by the user.
 * @remark Calls evresp_itp() but with listinterp_tension set to 0.
 */
struct response *evresp(char *, char *, char *, char *, char *, char *, char *,
        double *, int, char *, char *, int, int, int, int, double, int,
        evalresp_log_t *);

/**
 * @private
 * @ingroup evalresp_private
 * @brief Evaluate responses for user requested station/channel/network tuple
 *        at the frequencies requested by the user.
 */
struct response *evresp_itp(char *, char *, char *, char *, char *, char *,
        char *, double *, int, char *, char *, int, int, int, int, int, double,
        int, double, int, evalresp_log_t *);
/**
 * @private
 * @ingroup evalresp_private
 * @brief Evaluate responses for user requested station/channel/network tuple
 *        at the frequencies requested by the user.
 * @remark Fortran interface.
 */
int evresp_1(char *sta, char *cha, char *net, char *locid, char *datime,
        char *units, char *file, double *freqs, int nfreqs, double *resp,
        char *rtype, char *verbose, int start_stage, int stop_stage,
        int stdio_flag, int useTotalSensitivityFlag, double x_for_b62,
        int xml_flag, evalresp_log_t *);

/**
 * @private
 * @ingroup evalresp_private_response
 * @brief Interpolates amplitude and phase values from the set of frequencies
 *        in the List blockette to the requested set of frequencies.
 */
void interpolate_list_blockette(double **freq, double **amp, double **phase,
        int *p_number_points, double *req_freq_arr, int req_num_freqs,
        double tension);

/**
 * @private
 * @ingroup evalresp_private
 * @brief FIXME.
 */
extern char SEEDUNITS[][UNITS_STR_LEN];

/**
 * @private
 * @ingroup evalresp_private
 * @brief Define the "first line" and "first field" arguments that are used by
 *        the parsing routines (they are used to compensate for the fact that
 *        in parsing the RESP files, one extra line is always read because the
 *        "end" of a filter sequence cannot be determined until the first line
 *        of the "next" filter sequence or station-channel pair is read from
 *        the file.
 */
extern char FirstLine[];

/**
 * @private
 * @ingroup evalresp_private
 * @brief Define the "first line" and "first field" arguments that are used by
 *        the parsing routines (they are used to compensate for the fact that
 *        in parsing the RESP files, one extra line is always read because the
 *        "end" of a filter sequence cannot be determined until the first line
 *        of the "next" filter sequence or station-channel pair is read from
 *        the file.
 */
extern int FirstField;

/* values for use in calculating responses (defined in 'evresp_') */

/**
 * @private
 * @ingroup evalresp_private
 * @brief FIXME.
 */
extern double Pi;

/**
 * @private
 * @ingroup evalresp_private
 * @brief FIXME.
 */
extern double twoPi;

/**
 * @private
 * @ingroup evalresp_private
 * @brief Define a global flag to use if using "default" units.
 */
extern int def_units_flag;

/**
 * @private
 * @ingroup evalresp_private
 * @brief Define a pointer to a channel structure to use in determining the
 *        input and output units if using "default" units and for use in error
 *        output.
 */
extern struct channel *GblChanPtr;

/**
 * @private
 * @ingroup evalresp_private
 * @brief FIXME.
 */
extern float unitScaleFact;

/* define global variables for use in printing error messages */

/**
 * @private
 * @ingroup evalresp_private
 * @brief FIXME.
 */
extern char *curr_file;

/**
 * @private
 * @ingroup evalresp_private
 * @brief FIXME.
 */
extern int curr_seq_no;

/* and set a global variable to contain the environment for the setjmp/longjmp
 combination for error handling */

/**
 * @private
 * @ingroup evalresp_private
 * @brief FIXME.
 */
extern jmp_buf jump_buffer;

/**
 * @private
 * @ingroup evalresp_private_calc
 * @brief FIXME.
 */
double unwrap_phase(double phase, double prev_phase, double range,
        double *added_value);

/**
 * @private
 * @ingroup evalresp_private_calc
 * @brief FIXME.
 */
double wrap_phase(double phase, double range, double *added_value);

/**
 * @private
 * @ingroup evalresp_private
 * @brief FIXME.
 */
int use_estimated_delay(int flag);

#endif /* !EVRESP_H */
