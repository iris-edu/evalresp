
#ifndef EVALRESP_UGLY_H
#define EVALRESP_UGLY_H

#include <setjmp.h>

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
 * @brief Define a pointer to a channel structure to use in determining the
 *        input and output units if using "default" units and for use in error
 *        output.
 */
//extern struct evalresp_channel_s *GblChanPtr;

/**
 * @private
 * @ingroup evalresp_private
 * @brief Scaling factor to convert from the units in RESP file to MKS units.
 */
//extern float unitScaleFact;

/* define global variables for use in printing error messages */

/**
 * @private
 * @ingroup evalresp_private
 * @brief Sequence number of the current stage.
 */
//extern int curr_seq_no;

/**
 * @private
 * @ingroup evalresp_private
 * @brief Global variable to keep Net-Station-Loc-Channel info in case enabling log-label configure option is used.
 * @author 2007/02/27: IGD.
 */
//extern char myLabel[20];
//char myLabel[20] = "aa";

#define DATIMESIZE 32
#define TODAYSIZE 10

#endif
