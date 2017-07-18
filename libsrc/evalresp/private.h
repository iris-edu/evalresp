/**
 * @mainpage Introduction
 *
 * @section purpose Purpose
 *
 * Implements the interfaces for evalresp.
 *
 * @section history History
 *
 * Written by <a href="http://www.isti.com/">Instrumental Software
 * Technologies, Inc.</a> (ISTI).
 */

/**
 * @defgroup evalresp evalresp
 * @ingroup iris
 * @brief Application Programming Interface (API) for evalresp.
 */

/**
 * @defgroup evalresp_public_compat evalresp Legacy Public Interface
 * @ingroup evalresp
 * @brief Legacy public evalresp interface (predating version 5.0) is included for comatibility purpose.
 */

/**
 * @defgroup evalresp_public evalresp Public Interface
 * @ingroup evalresp
 * @brief Public evalresp interface.
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

#ifndef EVALRESP_PRIVATE_H
#define EVALRESP_PRIVATE_H

#include <ctype.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>

#include "evalresp/public_api.h"
#include "evalresp/public_channels.h"
#include "evalresp/public_responses.h"
#include "evalresp/constants.h"
#include "evalresp_log/log.h"

/* if Windows compiler then redefine 'complex' to */
/*  differentiate it from the existing struct,    */
/*  and rename 'strcasecmp' functions:            */
#ifdef _WIN32
#define strcasecmp stricmp
#define strncasecmp strnicmp
#endif

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
 * @brief FIR normal tolerance.
 */
#define FIR_NORM_TOL 0.02

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
 * @brief Enumeration representing the types of units encountered.
 * @note If default, then the response is just given in input units to output
 *       units, no interpretation is made of the units used).
 * @author 02/03/01: IGD: new unit pressure added.
 * @author 08/21/06: IGD: new unit TESLA added.
 * @author 10/03/13: IGD: new units CENTIGRADE added.
*/
enum units
{
  UNDEF_UNITS, /**< Undefined. */
  DIS, /**< Displacement. */
  VEL, /**< Velocity. */
  ACC, /**< Acceleration. */
  COUNTS, /**< Counts. */
  VOLTS, /**< Volts. */
  DEFAULT, /**< Default. */
  PRESSURE, /**< Pressure. */
  TESLA, /**< Tesla. */
  CENTIGRADE /**< Degrees Celsius. */
};

/**
 * @private
 * @ingroup evalresp_private
 * @brief Enumeration representing the types of filters encountered with corresponding SEED blockettes.
 */
enum filt_types
{
  UNDEF_FILT, /**< Undefined filter. */
  LAPLACE_PZ, /**< Laplace transform filter: poles and zeros representation B53 . */
  ANALOG_PZ, /**< Analog filter: poles and zeros representation B53. */
  IIR_PZ, /**< Infinite Impulse Response: polez and zeros representation B53. */
  FIR_SYM_1, /**< Finite Impulse Response Filter (symmetrical with odd number of weights) B61. */
  FIR_SYM_2, /**< Finie Impulse Response Filter (symmetrical with even number of weights) B61. */
  FIR_ASYM, /**< Finite Impulse Response Filter (assymetrical) B54. */
  LIST, /**< Filter presented as a list B55 (Frequency/amplitude). */
  GENERIC, /**< Filter presented as a generi B56 (via Corener frequencies/slopes. */
  DECIMATION, /**< Decimation B57. */
  GAIN, /**< Channel Sensitiity/Gain B58. */
  REFERENCE, /**< Response Reference B60 to replace B53-58,61 with the dictionary counterparts. */
  FIR_COEFFS, /**< FIR response: coefficients representation B61. */
  IIR_COEFFS, /**< Infinite Impulse response represented in B54. */
  POLYNOMIAL /**< Polynomial filter via B62. */
};

/**
 * @private
 * @ingroup evalresp_private
 * @brief Enumeration representing the types of stages in RESP fi;es that are recognized.
 */
enum stage_types
{
  UNDEF_STAGE, /**< Undefined stage. */
  PZ_TYPE, /**< Polez and zeros stage. */
  IIR_TYPE, /**< Infinite Impulse response stage. */
  FIR_TYPE, /**< Finite Impulse response stage. */
  GAIN_TYPE, /**< Channel Sensitivity/Gain stage. */
  LIST_TYPE, /**< List response representation stage. */
  IIR_COEFFS_TYPE, /**< Infinite Impulse response in coefficient form stage. */
  GENERIC_TYPE, /**< Generic response stage. */
  POLYNOMIAL_TYPE /**< Polynomial type stage. */
};

/**
 * @private
 * @ingroup evalresp_private
 * @brief Enumeration representing the types of error codes possible.
 */
enum error_codes
{
  NON_EXIST_FLD = -2, /**< Field does not exist. */
  ILLEGAL_RESP_FORMAT = -5, /**< Illegal response format. */
  PARSE_ERROR = -4, /**< Parse error. */
  UNDEF_PREFIX = -3, /**< Undefined prefix. */
  UNDEF_SEPSTR = -6, /**< Undefined separator. */
  OUT_OF_MEMORY = -1, /**< Out of memory */
  UNRECOG_FILTYPE = -7, /**< Unrecognized filter type */
  UNEXPECTED_EOF = -8, /**< Unexpected EOF. */
  ARRAY_BOUNDS_EXCEEDED = -9, /**< Array bound exceeded. */
  OPEN_FILE_ERROR = 2, /**< Failure to open the file. */
  RE_COMP_FAILED = 3, /**< Failure to compile the pattern. */
  MERGE_ERROR = 4, /**< lure to merge blockettes. */
  SWAP_FAILED = 5, /**< Swapping failure: not used. */
  USAGE_ERROR = 6, /**< User errors detencted on teh command line. */
  BAD_OUT_UNITS = 7, /**< Bad output units error. */
  IMPROP_DATA_TYPE = -10, /**< Unsupported data type. */
  UNSUPPORT_FILTYPE = -11, /**< Unsupported filter type. */
  ILLEGAL_FILT_SPEC = -12, /**< Illigal filter specs. */
  NO_STAGE_MATCHED = -13, /**< No stage matched error. */
  UNRECOG_UNITS = -14 /**< Unrecognized units. */
};

/* define structures for the compound data types used in evalesp */

/**
 * @private
 * @ingroup evalresp_private
 * @brief Array of string objects.
 */
struct string_array
{
  int nstrings; /**< Number of strings. */
  char **strings; /**< String array. */
};

/**
 * @private
 * @ingroup evalresp_private
 * @brief File list object.
 */
struct file_list
{
  char *name; /**< File name. */
  struct file_list *next_file; /**< Pointer to next file list object. */
};

/**
 * @private
 * @ingroup evalresp_private
 * @brief Matches files object.
 */
struct matched_files
{
  int nfiles; /**< Number of files. */
  struct file_list *first_list; /**< Array of file list objects. */
  struct matched_files *ptr_next; /**< Pointer to next matches files object. */
};

/**
 * @private
 * @ingroup evalresp_private_string
 * @brief A function that tests whether a string can be converted into an
 *        integer using string_match().
 * @param[in] test String to test.
 * @param[in] log Logging structure.
 * @returns 0 if false.
 * @returns >0 if true.
 */
int is_int (const char *test, evalresp_log_t *log);

/**
 * @private
 * @ingroup evalresp_private_string
 * @brief A function that tests whether a string can be converted into an
 *        double using string_match().
 * @param[in] test String to test.
 * @param[in] log Logging structure.
 * @returns 0 if false.
 * @returns >0 if true.
*/
int is_real (const char *test, evalresp_log_t *log);

/**
 * @private
 * @ingroup evalresp_private_parse
 * @brief Finds the location of a response for the input station-channel
 *        at the specified date.
 * @details If no response is available in the file pointed to by @p fptr,
 *          then a -1 value is returned (indicating failure), otherwise a
 *          value of 1 is returned (indicating success). The matching beg_t,
 *          end_t and station info are returned as part of the input
 *          structure @p this_channel. The pointer to the file (@p fptr) is
 *          left in position for the parse_channel() routine to grab the
 *          response information for that station.
 * @param[in,out] fptr FILE pointer.
 * @param[in] scn Network-station-locid-channel object.
 * @param[in] datime Date-time string.
 * @param[out] this_channel Channel structure.
 * @param[in] log Logging structure.
 * @returns 1 on success.
 * @returns -1 on failure.
 * @note The station information is preloaded into @p this_channel, so the
 *       file pointer does not need to be repositioned to allow for this
 *       information to be reread.
 */
int get_resp (FILE *fptr, evalresp_sncl *scn, char *datime,
              evalresp_channel *this_channel, evalresp_log_t *log);

/**
 * @private
 * @ingroup evalresp_private_parse
 * @brief Retrieves the info from the RESP file blockettes for a channel.
 * @details Errors cause the program to terminate. The blockette and field
 *          numbers are checked as the file is parsed. Only the
 *          station/channel/date info is returned. The file pointer is left at
 *          the end of the date info, where it can be used to read the
 *          response information into the filter structures.
 * @param[in,out] fptr File pointer.
 * @param[out] chan Channel structure.
 * @param[in] log Logging structure.
 * @returns 1 on success.
 * @returns 0 on failure.
 */
 int get_channel (FILE *fptr, evalresp_channel *chan, evalresp_log_t *log);

/**
 * @private
 * @ingroup evalresp_private_parse
 * @brief Finds the location of the start of the next response in the file
 *        and positions the file pointer there.
 * @details If no more responses are available then a zero value is returned
 *          (indicating failure to reposition the file pointer), otherwise a
 *          that first line is returned in the global variable FirstLine. The
 *          pointer to the file (fptr) is left in position for the
 *          get_channel() routine to grab the channel information.
 * @param[in,out] fptr FILE pointer.
 * @param[in] log Logging structure.
 * @returns 1 on success.
 * @returns 0 on failure.
 */
int next_resp (FILE *fptr, evalresp_log_t *log);

/* routines used to create a list of files matching the users request */

/**
 * @private
 * @ingroup evalresp_private_file
 * @brief Creates a linked list of files to search based on the @p file name
 *        and @p scn_lst input arguments, i.e. based on the filename (if it
 *        is non-NULL) and the list of stations and channels.
 * @details If the filename exists as a directory, then that directory is
 *          searched for a list of files that look like
 *          'RESP.NETCODE.STA.CHA'. The names of any matching files will be
 *          placed in the linked list of file names to search for matching
 *          response information.  If no match is found for a requested
 *          'RESP.NETCODE.STA.CHA' file in that directory, then the search
 *          for a matching file will stop (see discussion of SEEDRESP case
 *          below).
 *
 *          If the filename is a file in the current directory, then a
 *          (matched_files *)NULL will be returned.
 *
 *          If the filename is NULL the current directory and the directory
 *          indicated by the environment variable 'SEEDRESP' will be searched
 *          for files of the form 'RESP.NETCODE.STA.CHA'. Files in the
 *          current directory will override matching files in the directory
 *          pointed to by 'SEEDRESP'. The routine will behave exactly as if
 *          the filenames contained in these two directories had been
 *          specified.
 *
 *          The mode is set to zero if the user named a specific filename and
 *          to one if the user named a directory containing RESP files (or if
 *          the SEEDRESP environment variable was used to find RESP files.
 *
 *          If a pattern cannot be found, then a value of NULL is set for the
 *          'names' pointer of the linked list element representing that
 *          station-channel-network pattern.
 * @param[in] file File name.
 * @param[in] scn_lst List of network-station-locid-channel objects.
 * @param[out] mode 1 for directory search or 0 if file was specified.
 * @param[in] log Logging structure.
 * @returns Pointer to the head of the linked list of matches files.
 * @returns @c NULL if no files were found that match request.
 */
struct matched_files *find_files (char *file, evalresp_sncls *scn_lst,
                                  int *mode, evalresp_log_t *log);

/**
 * @private
 * @ingroup evalresp_private_file
 * @brief Get filenames matching the expression in @p in_file.
 * @param[in] in_file File name matching expression.
 * @param[out] file Pointer to the head of the linked list of matches files.
 * @param[in] log Logging structure.
 * @returns Number of files found matching the expression.
 */
int get_names (char *in_file, struct matched_files *file, evalresp_log_t *log);

/* routines used to allocate vectors of the basic data types used in the
 filter stages */

/**
 * @private
 * @ingroup evalresp_private_alloc
 * @brief Allocates space for an array of complex numbers.
 * @param[in] npts Number of complex numbers to allocate in array.
 * @param[in] log Logging structure.
 * @returns Pointer to allocated array.
 * @returns @c NULL if @p npts is zero.
 * @warning Exits with error if allocation fails.
 */
evalresp_complex *alloc_complex (int npts, evalresp_log_t *log);

/**
 * @private
 * @ingroup evalresp_private_alloc
 * @brief Allocates space for an array of responses.
 * @details A 'response' is a combination of a complex array, a
 *          station-channel-network, and a pointer to the next 'response' in
 *          the list.
 * @param[in] npts Number of responses to allocate in array.
 * @param[in] log Logging structure.
 * @returns Pointer to allocated array.
 * @returns @c NULL if @p npts is zero.
 * @warning Exits with error if allocation fails.
 */
evalresp_response *alloc_response (int npts, evalresp_log_t *log);

/**
 * @private
 * @ingroup evalresp_private_alloc
 * @brief Allocates space for an array of strings.
 * @param[in] nstrings Number of strings to allocate in array.
 * @param[in] log Logging structure.
 * @param[in] log Logging structure.
 * @returns Pointer to allocated array.
 * @returns @c NULL if @p nstrings is zero.
 * @warning Exits with error if allocation fails.
 */
struct string_array *alloc_string_array (int nstrings, evalresp_log_t *log);

/**
 * @private
 * @ingroup evalresp_private_alloc
 * @brief Allocates space for a station-channel structure
 * @returns Pointer to allocated structure.
 * @param[in] log Logging structure.
 * @warning Exits with error if allocation fails.
 */
evalresp_sncl *alloc_scn (evalresp_log_t *log);

/**
 * @private
 * @ingroup evalresp_private_alloc
 * @brief Allocates space for an array of station/channel pairs.
 * @param[in] nscn Number of station/channel pairs to allocate in array.
 * @param[in] log Logging structure.
 * @returns Pointer to allocated array.
 * @returns @c NULL if @p nscn is zero.
 * @warning Exits with error if allocation fails.
 */
evalresp_sncls *alloc_scn_list (int nscn, evalresp_log_t *log);

/**
 * @private
 * @ingroup evalresp_private_alloc
 * @brief Allocates space for an element of a linked list of filenames.
 * @param[in] log Logging structure.
 * @returns Pointer to allocated structure.
 * @warning Exits with error if allocation fails.
 */
struct file_list *alloc_file_list (evalresp_log_t *log);

/**
 * @private
 * @ingroup evalresp_private_alloc
 * @brief Allocates space for an element of a linked list of matching files.
 * @param[in] log Logging structure.
 * @returns Pointer to allocated structure.
 * @warning Exits with error if allocation fails.
 */
struct matched_files *alloc_matched_files (evalresp_log_t *log);

/**
 * @private
 * @ingroup evalresp_private_alloc
 * @brief Allocates space for an array of double precision numbers
 * @param[in] npts Number of double precision numbers to allocate in array.
 * @param[in] log Logging structure.
 * @returns Pointer to allocated array.
 * @returns @c NULL if @p npts is zero.
 * @warning Exits with error if allocation fails.
 */
double *alloc_double (int npts, evalresp_log_t *log);

/**
 * @private
 * @ingroup evalresp_private_alloc
 * @brief Allocates space for an array of characters.
 * @param[in] len Number of characters to allocate in array.
 * @param[in] log Logging structure.
 * @returns Pointer to allocated array.
 * @returns @c NULL if @p len is zero.
 * @warning Exits with error if allocation fails.
 */
char *alloc_char (int len, evalresp_log_t *log);

/**
 * @private
 * @ingroup evalresp_private_alloc
 * @brief Allocates space for an array of char pointers.
 * @param[in] len Number of char pointers to allocate in array.
 * @param[in] log Logging structure.
 * @returns Pointer to allocated array.
 * @returns @c NULL if @p len is zero.
 * @warning Exits with error if allocation fails.
 */
char **alloc_char_ptr (int len, evalresp_log_t *log);

/* allocation routines for the various types of filters */

/**
 * @private
 * @ingroup evalresp_private_alloc
 * @brief Allocates space for a pole-zero type filter structure.
 * @param[in] log Logging structure.
 * @returns Pointer to allocated structure.
 * @note The space for the complex poles and zeros is not allocated here, the
 *       space for these vectors must be allocated as they are read, since the
 *       number of poles and zeros is unknown until the blockette is partially
 *       parsed.
 * @warning Exits with error if allocation fails.
 */
evalresp_blkt *alloc_pz (evalresp_log_t *log);

/**
 * @private
 * @ingroup evalresp_private_alloc
 * @brief Allocates space for a coefficients-type filter.
 * @param[in] log Logging structure.
 * @returns Pointer to allocated structure.
 * @note See alloc_pz() for details (like alloc_pz(), this does not allocate
 *       space for the numerators and denominators, that is left until
 *       parse_fir()).
 * @warning Exits with error if allocation fails.
 */
evalresp_blkt *alloc_coeff (evalresp_log_t *log);

/**
 * @private
 * @ingroup evalresp_private_alloc
 * @brief Allocates space for a fir-type filter.
 * @param[in] log Logging structure.
 * @returns Pointer to allocated structure.
 * @note See alloc_pz() for details (like alloc_pz(), this does not allocate
 *       space for the numerators and denominators, that is left until
 *       parse_fir()).
 * @warning Exits with error if allocation fails.
 */
evalresp_blkt *alloc_fir (evalresp_log_t *log);

/**
 * @private
 * @ingroup evalresp_private_alloc
 * @brief Allocates space for a response reference type filter structure.
 * @param[in] log Logging structure.
 * @returns Pointer to allocated structure.
 * @warning Exits with error if allocation fails.
 */
evalresp_blkt *alloc_ref (evalresp_log_t *log);

/**
 * @private
 * @ingroup evalresp_private_alloc
 * @brief Allocates space for a gain type filter structure.
 * @param[in] log Logging structure.
 * @returns Pointer to allocated structure.
 * @note The space for the calibration vectors is not allocated here, the
 *       space for these vectors must be allocated as they are read, since the
 *       number of calibration points is unknown until the blockette is
 *       partially parsed.
 * @warning Exits with error if allocation fails.
 */
evalresp_blkt *alloc_gain (evalresp_log_t *log);

/**
 * @private
 * @ingroup evalresp_private_alloc
 * @brief Allocates space for a list type filter structure.
 * @param[in] log Logging structure.
 * @returns Pointer to allocated structure.
 * @note The space for the amplitude, phase and frequency vectors is not
 *       allocated here the user must allocate space for these parameters once
 *       the number of frequencies is known.
 * @warning Exits with error if allocation fails.
 */
evalresp_blkt *alloc_list (evalresp_log_t *log);

/**
 * @private
 * @ingroup evalresp_private_alloc
 * @brief Allocates space for a generic type filter structure.
 * @param[in] log Logging structure.
 * @returns Pointer to allocated structure.
 * @note The space for the corner_freq, and corner_slope vectors is not
 *       allocated here the user must allocate space for these parameters once
 *       the number of frequencies is known.
 * @warning Exits with error if allocation fails.
 */
evalresp_blkt *alloc_generic (evalresp_log_t *log);

/**
 * @private
 * @ingroup evalresp_private_alloc
 * @brief Allocates space for a decimation type filter structure.
 * @param[in] log Logging structure.
 * @returns Pointer to allocated structure.
 * @warning Exits with error if allocation fails.
 */
evalresp_blkt *alloc_deci (evalresp_log_t *log);

/**
 * @private
 * @ingroup evalresp_private_alloc
 * @brief Allocates space for a polynomial Blockette 62.
 * @param[in] log Logging structure.
 * @returns Pointer to allocated structure.
 * @warning Exits with error if allocation fails.
 * @author 05/31/2013: IGD.
 */
evalresp_blkt *alloc_polynomial (evalresp_log_t *log);

/**
 * @private
 * @ingroup evalresp_private_alloc
 * @brief Allocates space for a decimation type filter structure.
 * @param[in] log Logging structure.
 * @returns Pointer to allocated structure.
 * @warning Exits with error if allocation fails.
 */
evalresp_stage *alloc_stage (evalresp_log_t *log);

/* routines to free up space associated with dynamically allocated
 structure members */

/**
 * @private
 * @ingroup evalresp_private_alloc
 * @brief A routine that frees up the space associated with a string list type
 *        structure.
 * @param[in,out] lst String list type structure.
 */
void free_string_array (struct string_array *lst);

/**
 * @private
 * @ingroup evalresp_private_alloc
 * @brief A routine that frees up the space associated with a station-channel
 *        type structure.
 * @param[in,out] ptr Station-channel type structure.
 */
void free_scn (evalresp_sncl *ptr);

/**
 * @private
 * @ingroup evalresp_private_alloc
 * @brief A routine that frees up the space associated with a matched files
 *        type structure.
 * @param[in,out] lst Matched files type structure.
 */
void free_matched_files (struct matched_files *lst);

/**
 * @private
 * @ingroup evalresp_private_alloc
 * @brief A routine that frees up the space associated with a file list type
 *        structure.
 * @param[in,out] lst Matched files type structure.
 */
void free_file_list (struct file_list *lst);

/**
 * @private
 * @ingroup evalresp_private_alloc
 * @brief A routine that frees up the space associated with a pole-zero type
 *        filter.
 * @param[in,out] blkt_ptr Pole-zero type filter blockette structure.
 */
void free_pz (evalresp_blkt *blkt_ptr);

/**
 * @private
 * @ingroup evalresp_private_alloc
 * @brief A routine that frees up the space associated with a coefficients
 *        type filter.
 * @param[in,out] blkt_ptr Coefficients type filter blockette structure.
 */
void free_coeff (evalresp_blkt *blkt_ptr);

/**
 * @private
 * @ingroup evalresp_private_alloc
 * @brief A routine that frees up the space associated with a fir type filter.
 * @param[in,out] blkt_ptr Fir type filter blockette structure.
 */
void free_fir (evalresp_blkt *blkt_ptr);

/**
 * @private
 * @ingroup evalresp_private_alloc
 * @brief A routine that frees up the space associated with a list type
 *        filter.
 * @param[in,out] blkt_ptr List type filter blockette structure.
 */
void free_list (evalresp_blkt *blkt_ptr);

/**
 * @private
 * @ingroup evalresp_private_alloc
 * @brief A routine that frees up the space associated with a generic type
 *        filter.
 * @param[in,out] blkt_ptr Generic type filter blockette structure.
 */
void free_generic (evalresp_blkt *blkt_ptr);

/**
 * @private
 * @ingroup evalresp_private_alloc
 * @brief A routine that frees up the space associated with a gain type
 *        filter.
 * @param[in,out] blkt_ptr Gain type filter blockette structure.
 */
void free_gain (evalresp_blkt *blkt_ptr);

/**
 * @private
 * @ingroup evalresp_private_alloc
 * @brief A routine that frees up the space associated with a decimation type
 *        filter.
 * @param[in,out] blkt_ptr Decimation type filter blockette structure.
 */
void free_deci (evalresp_blkt *blkt_ptr);

/**
 * @private
 * @ingroup evalresp_private_alloc
 * @brief A routine that frees up the space associated with a response
 *        reference type filter.
 * @param[in,out] blkt_ptr Response reference type filter blockette structure.
 */
void free_ref (evalresp_blkt *blkt_ptr);

/**
 * @private
 * @ingroup evalresp_private_alloc
 * @brief A routine that frees up the space associated with a stages in a
 *        channel's response.
 * @param[in,out] stage_ptr Stage structure.
 */
void free_stages (evalresp_stage *stage_ptr);

/**
 * @private
 * @ingroup evalresp_private_alloc
f * @brief A routine that frees up the space associated with a channel's filter
 *        sequence.
 * @param[in,out] chan_ptr Channel structure.
 */
void free_channel (evalresp_channel *chan_ptr);

/* simple error handling routines to standardize the output error values and
 allow for control to return to 'evresp' if a recoverable error occurs */

/* a simple routine that parses the station information from the input file */

/**
 * @private
 * @ingroup evalresp_private_parse
 * @brief Parses the RESP file blockettes for COEFF filters (in a Blockette
 *        [54] or [44]).
 * @details Errors cause the program to terminate. The blockette and field
 *          numbers are checked as the file is parsed. As with parse_pz(), for
 *          this routine to work, the lines must contain evalresp-3.0 style
 *          prefixes.
 * @param[in,out] fptr FILE pointer.
 * @param[in,out] blkt_ptr Blockette structure.
 * @param[in,out] stage_ptr Stage structure.
 * @param[in] log Logging structure.
 */
void parse_coeff (FILE *fptr, evalresp_blkt *blkt_ptr, evalresp_stage *stage_ptr, evalresp_log_t *log);

/**
 * @private
 * @ingroup evalresp_private_parse
 * @brief Parses the RESP file blockettes for COEFF filters (in a Blockette
 *        [54] or [44]).
 * @details Handles the case of a digital IIR filtering. Derived from
 *          parse_coeff(). Errors cause the program to terminate. The
 *          blockette and field numbers are checked as the file is parsed. As
 *          with parse_pz(), for this routine to work,  the lines must contain
 *          evalresp-3.0 style prefixes.
 * @param[in,out] fptr FILE pointer, evalresp_log_t *log.
 * @param[in,out] blkt_ptr Blockette structure.
 * @param[in,out] stage_ptr Stage structure.
 * @param[in] log Logging structure.
 * @author 06/27/00: I.Dricker (i.dricker@isti.com) for 2.3.17 iir.
 */
void parse_iir_coeff (FILE *fptr, evalresp_blkt *blkt_ptr, evalresp_stage *stage_ptr, evalresp_log_t *log);

/**
 * @private
 * @ingroup evalresp_private_parse
 * @brief Parses the RESP file blockettes for list filters (in a Blockette
 *        [55] or [45]).
 * @details Errors cause the program to terminate. The blockette and field
 *          numbers are checked as the file is parsed. As with parse_pz(), for
 *          this routine to work, the lines must contain evalresp-3.0 style
 *          prefixes.
 * @param[in,out] fptr FILE pointer.
 * @param[in,out] blkt_ptr Blockette structure.
 * @param[in,out] stage_ptr Stage structure.
 * @param[in] log Logging structure.
 * @author 06/21/00: Ilya Dricker ISTI: I modify this routine to accomodate
 *         the form of the parsed blockette 55 generated by SeismiQuery.
 *         Since currently the blockette 55 is not supported, we do not
 *         anticipate problems caused by this change.
 */
void parse_list (FILE *fptr, evalresp_blkt *blkt_ptr, evalresp_stage *stage_ptr, evalresp_log_t *log);

/**
 * @private
 * @ingroup evalresp_private_parse
 * @brief Parses the RESP file for generic filters (in a Blockette [56] or
 *        [46]).
 * @details Errors cause the program to terminate. The blockette and field
 *          numbers are checked as the file is parsed. As with parse_pz(), for
 *          this routine to work, the lines must contain evalresp-3.0 style
 *          prefixes.
 * @param[in,out] fptr FILE pointer.
 * @param[in,out] blkt_ptr Blockette structure.
 * @param[in,out] stage_ptr Stage structure.
 * @param[in] log Logging structure.
 */
void parse_generic (FILE *fptr, evalresp_blkt *blkt_ptr, evalresp_stage *stage_ptr, evalresp_log_t *log);

/**
 * @private
 * @ingroup evalresp_private_parse
 * @brief Parses the RESP file for decimation filters (in a Blockette [57] or
 *        [47]).
 * @details Errors cause the program to terminate. The blockette and field
 *          numbers are checked as the file is parsed. As with parse_pz(), for
 *          this routine to work, the lines must contain evalresp-3.0 style
 *          prefixes.
 * @param[in,out] fptr FILE pointer.
 * @param[in,out] blkt_ptr Blockette structure.
 * @param[in] log Logging structure.
 * @returns Sequence number of the stage for verification.
 */
int parse_deci (FILE *fptr, evalresp_blkt *blkt_ptr, evalresp_log_t *log);

/**
 * @private
 * @ingroup evalresp_private_parse
 * @brief Parses the RESP file blockettes for gain filters (in a Blockette
 *        [58] or [48]).
 * @details Errors cause the program to terminate. The blockette and field
 *          numbers are checked as the file is parsed. As with parse_pz(), for
 *          this routine to work, the lines must contain evalresp-3.0 style
 *          prefixes.
 * @param[in,out] fptr FILE pointer.
 * @param[in,out] blkt_ptr Blockette structure.
 * @param[in] log Logging structure.
 * @returns Sequence number of the stage for verification.
 */
int parse_gain (FILE *fptr, evalresp_blkt *blkt_ptr, evalresp_log_t *log);

/**
 * @private
 * @ingroup evalresp_private_parse
 * @brief Parses the RESP file blockettes for FIR filters (in a Blockette [61]
 *        or [41]).
 * @details Errors cause the program to terminate. The blockette and field
 *          numbers are checked as the file is parsed. As with parse_pz(), for
 *          this routine to work, the lines must contain evalresp-3.0 style
 *          prefixes.
 * @param[in,out] fptr FILE pointer.
 * @param[in,out] blkt_ptr Blockette structure.
 * @param[in,out] stage_ptr Stage structure.
 * @param[in] log Logging structure.
 */
void parse_fir (FILE *fptr, evalresp_blkt *blkt_ptr, evalresp_stage *stage_ptr, evalresp_log_t *log);

/**
 * @private
 * @ingroup evalresp_private_parse
 * @brief Parses the RESP file blockettes for Response Reference type filters
 *        (in a Blockette [60]).
 * @details Errors cause the program to terminate.  The blockette and field
 *          numbers are checked as the file is parsed. As with parse_pz(), for
 *          this routine to work, the lines must contain evalresp-3.0 style
 *          prefixes.
 * @param[in,out] fptr FILE pointer.
 * @param[in,out] blkt_ptr Blockette structure.
 * @param[in,out] stage_ptr Stage structure.
 * @param[in] log Logging structure.
 */
//XXX void parse_ref (FILE *fptr, evalresp_blkt *blkt_ptr, evalresp_stage *stage_ptr, evalresp_log_t *log);

/**
 * @private
 * @ingroup evalresp_private_parse
 * @brief Parses the RESP file blockettes for polynomial filters (in a
 *        Blockette [62] or [42]).
 * @details Errors cause the program to terminate. The blockette and field
 *          numbers are checked as the file is parsed. The lines must contain
 *          evalresp-3.0 style prefixes
 * @param[in,out] fptr FILE pointer.
 * @param[in,out] blkt_ptr Blockette structure.
 * @param[in,out] stage_ptr Stage structure.
 * @param[in] log Logging structure.
 * @author 05/31/2013: IGD.
 */
void parse_polynomial (FILE *fptr, evalresp_blkt *blkt_ptr,
                       evalresp_stage *stage_ptr, evalresp_log_t *log);

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
 * @remark Practical to use with input FORTRAN strings.
 */
int add_null (char *s, int len, char where);

/* run a sanity check on the channel's filter sequence */

/**
 * @private
 * @ingroup evalresp_private_response
 * @brief A routine that merges two fir filters.
 * @details The coefficients from the second filter are copied into the first
 *          filter and the number of coefficients in the first filter is
 *          adjusted to reflect the new filter size. Then the next_blkt
 *          pointer for the first filter is reset to the value of the
 *          next_blkt pointer of the second filter and the space associated
 *          with the second filter is free'd. Finally, the second filter
 *          pointer is reset to the next_blkt pointer value of the first
 *          filter
 * @param[in,out] first_blkt First filter.
 * @param[in,out] second_blkt Second filter.
 * @param[in] log Logging structure.
 */
//void merge_coeffs (evalresp_blkt *first_blkt, evalresp_blkt **second_blkt, evalresp_log_t *log);

/**
 * @private
 * @ingroup evalresp_private_response
 * @brief A routine that merges two lists filters (blockettes 55).
 * @details The frequencies, amplitudes and phases from the second filter are
 *          copied into the first filter and the number of coefficients in the
 *          first filter is adjusted to reflect the new filter size. Then the
 *          next_blkt pointer for the first filter is reset to the value of
 *          the next_blkt pointer of the second filter and the space
 *          associated with the second filter is free'd. Finally, the second
 *          filter pointer is reset to the next_blkt pointer value of the
 *          first filter.
 * @param[in,out] first_blkt First filter.
 * @param[in,out] second_blkt Second filter.
 * @param[in] log Logging structure.
 * @author 07/07/00: Ilya Dricker IGD (i.dricker@isti.com): Modified from
 *         merge_coeffs() for 3.2.17 of evalresp.
 */
//void merge_lists (evalresp_blkt *first_blkt, evalresp_blkt **second_blkt, evalresp_log_t *log);

/**
 * @private
 * @ingroup evalresp_private_response
 * @brief A routine that checks a channel's filter stages.
 * @details (1) Run a sanity check on the filter sequence. And that LAPLACE_PZ
 *              and ANALOG_PZ filters will be followed by a GAIN blockette
 *              only. REFERENCE blockettes are ignored (since they contain no
 *              response information).
 *
 *          (2) As the routine moves through the stages in the filter
 *              sequence, several other checks are made. First, the output
 *              units of this stage are compared with the input units of the
 *              next on to ensure that no stages have been skipped. Second,
 *              the filter type of this blockette is compared with the filter
 *              type of the next. If they are the same and have the same
 *              stage-sequence number, then they are merged to form one
 *              filter.  At the present time this is only implemented for the
 *              FIR type filters (since those are the only filter types that
 *              typically are continued to a second filter blockette). The new
 *              filter will have the combined number of coefficients and a new
 *              vector containing the coefficients for both filters, one after
 *              the other.
 *
 *          (3) the expected delay from the FIR filter stages in the channel's
 *              filter sequence is calculated and stored in the filter
 *              structure.
 * @param[in] log Logging structure.
 * @param[in] chan Channel structure.
 * @retval EVALRESP_OK on success
 */
int check_channel (evalresp_log_t *log, evalresp_channel *chan);

/**
 * @private
 * @ingroup evalresp_private_response
 * @brief Checks to see if a FIR filter can be converted to a symmetric FIR
 *        filter.
 * @details If so, the conversion is made and the filter type is redefined.
 * @param[in,out] f FIR filter.
 * @param[in] chan Channel structure.
 * @param[in] log Logging structure.
 */
void check_sym (evalresp_blkt *f, evalresp_channel *chan, evalresp_log_t *log);

/* routines used to calculate the instrument responses */
/**
 * @private
 * @ingroup evalresp_private_calc
 * @brief Calculate response.
 * @param[in] log Logging structure.
 * @param[in] chan Channel structure.
 * @param[in] freq Frequency array.
 * @param[in] nfreqs Number if numbers in @p freq.
 * @param[in] output Output.
 */
int calculate_response (evalresp_log_t *log, evalresp_options *options, evalresp_channel *chan, double *freq, int nfreqs, evalresp_complex *output);

/**
 * @private
 * @ingroup evalresp_private_calc
 * @brief Convert response to velocity first, then to specified units.
 * @param[in] inp Input units. See units constants.
 * @param[in] out_units Output units. @c DEF, @c DIS, @c VEL, @c ACC.
 * @param[in,out] data Data.
 * @param[in] w Frequency.
 * @param[in] log Logging structure.
 */
int convert_to_units (int inp, const evalresp_unit units, evalresp_complex *data, double w, evalresp_log_t *log);

/**
 * @private
 * @ingroup evalresp_private_calc
 * @brief Response of analog filter.
 * @param[in] blkt_ptr Filter.
 * @param[in] freq Frequency.
 * @param[out] out Response.
 */
void analog_trans (evalresp_blkt *blkt_ptr, double freq, evalresp_complex *out);

/**
 * @private
 * @ingroup evalresp_private_calc
 * @brief Response of symetrical FIR filters.
 * @param[in] blkt_ptr Filter.
 * @param[in] w Frequency.
 * @param[out] out Response.
 */
void fir_sym_trans (evalresp_blkt *blkt_ptr, double w, evalresp_complex *out);

/**
 * @private
 * @ingroup evalresp_private_calc
 * @brief Response of asymetrical FIR filters.
 * @param[in] blkt_ptr Filter.
 * @param[in] w Frequency.
 * @param[out] out Response.
 */
void fir_asym_trans (evalresp_blkt *blkt_ptr, double w, evalresp_complex *out);

/**
 * @private
 * @ingroup evalresp_private_calc
 * @brief Response of IIR filters.
 * @param[in] blkt_ptr Filter.
 * @param[in] w Frequency.
 * @param[out] out Response.
 */
void iir_pz_trans (evalresp_blkt *blkt_ptr, double w, evalresp_complex *out);

/**
 * @private
 * @ingroup evalresp_private_calc
 * @brief Calculate the phase shift equivalent to the time shift.
 * @details Delta at the frequency w (rads/sec).
 * @param[in] delta Delta.
 * @param[in] w Frequency.
 * @param[out] out Phase shift equivalent.
 */
void calc_time_shift (double delta, double w, evalresp_complex *out);

/**
 * @private
 * @ingroup evalresp_private_calc
 * @brief Complex multiplication.
 * @details Complex version of val1 *= val2.
 * @param[in,out] val1 Complex number 1.
 * @param[in] val2 Complex number 2.
 */
void zmul (evalresp_complex *val1, evalresp_complex *val2);

/**
 * @private
 * @ingroup evalresp_private_calc
 * @brief Normalize response.
 * @param[in,out] chan Channel structure.
 * @param[in] start_stage Start stage.
 * @param[in] stop_stage Stop stage.
 * @param[in] log Logging structure.
 */
int norm_resp (evalresp_channel *chan, int start_stage, int stop_stage, evalresp_log_t *log);

/**
 * @private
 * @ingroup evalresp_private_calc
 * @brief Response of blockette 55 (Response List Blockette) for a given frequency.
 * @param[in] blkt_ptr Response List Blockette (55).
 * @param[in] i Index in the frequency input vector.
 * @param[out] out Response.
 * @author 06/22/00: Ilya Dricker ISTI (.dricker@isti.com): Function
 *         introduced in version 3.2.17 of evalresp.
 */
void calc_list (evalresp_blkt *blkt_ptr, int i, evalresp_complex *out);

/**
 * @private
 * @ingroup evalresp_private_calc
 * @brief Response of blockette 62 (Polynomial)i for a given frequency.
 * @param[in] blkt_ptr Polynomial Blockette (62).
 * @param[out] out Response.
 * @param[in] x_for_b62 Frequency for response computation.
 * @param[in] log Logging structure.
 * @author 06/01/13: Ilya Dricker ISTI (.dricker@isti.com): Function
 *         introduced in version 3.3.4 of evalresp
 */
int calc_polynomial (evalresp_blkt *blkt_ptr, evalresp_complex *out,
                      double x_for_b62, evalresp_log_t *log);

/**
 * @private
 * @ingroup evalresp_private_calc
 * @brief Response of a digital IIR filter.
 * @details This code is modified from the FORTRAN subroutine written and
 *          tested by Bob Hutt (ASL USGS). Evaluates phase directly from
 *          imaginary and real parts of IIR filter coefficients.
 * @param[in] blkt_ptr Digital IIR filter.
 * @param[in] wint Circular frequency (2*PI*f).
 * @param[out] out Response.
 * @author 07/12/00: Ilya Dricker (ISTI), i.dricker@isti.com: C translation
 *         from FORTRAN function. Version 0.2. For version 3.2.17.
 */
void iir_trans (evalresp_blkt *blkt_ptr, double wint, evalresp_complex *out);

/**
 * @private
 * @ingroup evalresp_private_string
 * @brief A function that tests whether a string looks like a time string
 *        using string_match().
 * @details Time strings must be in the format 'hh:mm:ss[.#####]', so more
 *          than 14 characters is an error (too many digits).
 * @param[in] test String to test.
 * @param[in] log Logging structure.
 * @returns 0 if false.
 * @returns >0 if true.
 */
int is_time (const char *test, evalresp_log_t *log);

/**
 * @private
 * @ingroup evalresp_private_parse
 * @brief Compare two times and determine if the first is greater, equal to,
 *        or less than the second.
 * @param[in] dt1 Date-time 1.
 * @param[in] dt2 Date-time 2.
 * @returns An integer indicating whether the time in the input argument
 *          @p dt1 is greater than (1), equal to (0), or less than (-1) the
 *          time in the input argument @p dt2.
 */
 int timecmp (evalresp_datetime *dt1, evalresp_datetime *dt2);

/**
 * @private
 * @ingroup evalresp_private_print
 * @brief Print the channel info, followed by the list of filters.
 * @param[in] chan Channel structure.
 * @param[in] start_stage Start stage.
 * @param[in] stop_stage Stop stage.
 * @param[in] stdio_flag Flag if standard input was used.
 * @param[in] listinterp_out_flag Flag if interpolated output was used.
 * @param[in] listinterp_in_flag Flag if interpolated input was used.
 * @param[in] useTotalSensitivityFlag Flag if reported sensitivity was used to
 *                                  compute response.
 * @param[in] log Logging structure.
 */
void print_chan (evalresp_channel *chan, int start_stage, int stop_stage,
                 int stdio_flag, int listinterp_out_flag, int listinterp_in_flag,
                 int useTotalSensitivityFlag, evalresp_log_t *log);

/**
 * @private
 * @ingroup evalresp_private_print
 * @brief Print the response information to the output files.
 * @details Prints the response information in the fashion that the user
 *          requested it.  The response is either in the form of a complex
 *          spectra (freq, real_resp, imag_resp) to the file
 *          SPECTRA.NETID.STANAME.CHANAME (if rtype = "cs") or in the form of
 *          seperate amplitude and phase files (if rtype = "ap") with names
 *          like AMP.NETID.STANAME.CHANAME and PHASE.NETID.STANAME.CHANAME. In
 *          all cases, the pointer to the channel is used to obtain the NETID,
 *          STANAME, and CHANAME values. If the 'stdio_flag' is set to 1, then
 *          the response information will be output to stdout, prefixed by a
 *          header that includes the NETID, STANAME, and CHANAME, as well as
 *          whether the response given is in amplitude/phase or complex
 *          response (real/imaginary) values. If either case, the output to
 *          stdout will be in the form of three columns of real numbers, in
 *          the former case they will be freq/amp/phase tuples, in the latter
 *          case freq/real/imaginary tuples.
 * @param[in] freqs Array of frequencies.
 * @param[in] nfreqs Number of frequencies.
 * @param[in] first Pointer to first response in chain.
 * @param[in] rtype Reponse type.
 * @param[in] stdio_flag Flag controlling output.
 * @param[in] log Logging structure.
 * @see print_resp_itp().
 * @note This version of the function does not include the 'listinterp...'
 *       parameters.
 */
void print_resp (double *freqs, int nfreqs, evalresp_response *first, char *rtype,
                 int stdio_flag, evalresp_log_t *log);

/**
 * @private
 * @ingroup evalresp_private_print
 * @brief Print the response information to the output files.
 * @details Prints the response information in the fashion that the user
 *          requested it. The response is either in the form of a complex
 *          spectra (freq, real_resp, imag_resp) to the file
 *          SPECTRA.NETID.STANAME.CHANAME (if rtype = "cs") or in the form of
 *          seperate amplitude and phase files (if rtype = "ap") with names
 *          like AMP.NETID.STANAME.CHANAME and PHASE.NETID.STANAME.CHANAME.
 *          In all cases, the pointer to the channel is used to obtain the
 *          NETID, STANAME, and CHANAME values. If the 'stdio_flag' is set to
 *          1, then the response information will be output to stdout,
 *          prefixed by a header that includes the NETID, STANAME, and
 *          CHANAME, as well as whether the response given is in
 *          amplitude/phase or complex response (real/imaginary) values. If
 *          either case, the output to stdout will be in the form of three
 *          columns of real numbers, in the former case they will be
 *          freq/amp/phase tuples, in the latter case freq/real/imaginary
 *          tuples.
 * @param[in] freqs Array of frequencies.
 * @param[in] nfreqs Number of frequencies.
 * @param[in] first Pointer to first response in chain.
 * @param[in] rtype Reponse type.
 * @param[in] stdio_flag Flag controlling output.
 * @param[in] listinterp_out_flag Flag if interpolated output was used.
 * @param[in] listinterp_tension Interpolation tension used.
 * @param[in] unwrap_flag Flag if phases are unwrapped.
 * @param[in] log Logging structure.
 * @see print_resp().
 * @note This version of the function includes the 'listinterp...' parameters.
 */
void print_resp_itp (double *freqs, int nfreqs, evalresp_response *first,
                     char *rtype, int stdio_flag, int listinterp_out_flag,
                     double listinterp_tension, int unwrap_flag, evalresp_log_t *log);

/**
 * @private
 * @ingroup evalresp_private_response
 * @brief Interpolates amplitude and phase values from the set of frequencies
 *        in the List blockette to the requested set of frequencies.
 * @details The given frequency, amplitude and phase arrays are deallocated
 *          and replaced with newly allocated arrays containing the
 *          interpolated values. The @p p_number_points value is also updated.
 * @param[in,out] frequency_ptr Reference to array of frequency values.
 * @param[in,out]  amplitude_ptr Reference to array of amplitude values.
 * @param[in,out] phase_ptr Reference to array of phase values.
 * @param[in,out] p_number_points Reference to number of points value.
 * @param[in] req_freq_arr Array of requested frequency values.
 * @param[in] eq_num_freqs Number values in @p req_freq_arr array.
 * @param[in] log Logging structure.
 */
int interpolate_list_blockette (double **frequency_ptr,
                                 double **amplitude_ptr, double **phase_ptr,
                                 int *p_number_points, double *req_freq_arr,
                                 int req_num_freqs, evalresp_log_t *log);

/**
 * @private
 * @ingroup evalresp_private_calc
 * @brief Phase unwrapping function.
 * @details It works only inside a loop over phases.
 * @param[in] phase Phase value to process.
 * @param[in] prev_phase Previous phase.
 * @param[in] range Range value to use.
 * @param[in,out] added_value Pointer to offset value used for each call.
 * @returns "Unwrapped" version of the given phase value.
 * @author 04/05/04: IGD.
 */
double unwrap_phase (double phase, double prev_phase, double range,
                     double *added_value);

/**
 * @private
 * @ingroup evalresp_private_calc
 * @brief Wraps a set of phase values so that all of the values are between
 *        "-range" and "+range" (inclusive).
 * @details This function is called iteratively, once for each phase value.
 * @param[in] phase Phase value to process.
 * @param[in] range Range value to use.
 * @param[in,out] added_value Pointer to offset value used for each call.
 * @returns "Wrapped" version of the given phase value.
 */
double wrap_phase (double phase, double range, double *added_value);

/**
 * @private
 * @ingroup evalresp_private
 * @param[in] log logging structure
 * @param[in] name label to use if needing to log error
 * @param[in] str string to convert
 * @param[out] value the result of the conversion
 * @brief convert string to integar value
 * @retval EVALRESP_OK on success
 */
int
parse_int (evalresp_log_t *log, const char *name, const char *str, int *value);

/**
 * @private
 * @ingroup evalresp_private
 * @param[in] log logging structure
 * @param[in] name label to use if needing to log error
 * @param[in] str string to convert
 * @param[out] value the result of the conversion
 * @brief convert string to double value
 * @retval EVALRESP_OK on success
 */
int
parse_double (evalresp_log_t *log, const char *name, const char *str, double *value);

/**
 * @private
 * @ingroup evalresp_private
 * @param[in] log logging structure
 * @param[in] name label to use if needing to log error
 * @param[in] n number of elements to allocate
 * @param[out] array the returned array
 * @brief allocate a array of doubles with length n
 * @retval EVALRESP_OK on success
 */
int
calloc_doubles (evalresp_log_t *log, const char *name, int n, double **array);

/**
 * @private
 * @ingroup evalresp_private
 * @param[in] log logging structure
 * @param[in] options object to control the flow of the conversion to responses
 * @param[in] filter object on how to filter the inputed files
 * @param[out] responses object pointer containing responses
 * @brief take information from file and converthem into responses
 * @retval EVALRESP_OK on success
 */
int process_cwd (evalresp_log_t *log, evalresp_options *options,
                 evalresp_filter *filter, evalresp_responses **responses);

/**
 * @private
 * @ingroup evalresp_private
 * @param[in] log logging structure
 * @param[in] options object to control the flow of the conversion to responses
 * @param[in] filter object on how to filter the inputed files
 * @param[out] responses object pointer containing responses
 * @brief take information form stdin and convert them to responses
 * @retval EVALRESP_OK on success
 */
int process_stdio (evalresp_log_t *log, evalresp_options *options,
                   evalresp_filter *filter, evalresp_responses **responses);

/**
 * @private
 * @ingroup evalresp_private
 * @param[in] log logging structure
 * @param[in] responses evalresp_responses object to be printed out
 * @param[in] format evalresp_output_format that determines what files are outputed
 * @param[in] use_stdio flag to determine if printing to stdio instead of to files
 * @brief create files in the cwd (or stdio) based on the output formats selected
 * @post files created in the current working directory
 * @retval EVALRESP_OK on success
 */
int responses_to_cwd (evalresp_log_t *log, const evalresp_responses *responses,
                      evalresp_output_format format, int use_stdio);
#endif
