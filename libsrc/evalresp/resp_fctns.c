/* This file is modified by I.Dricker , ISTI, NY 06/21/00 */

/*
    8/28/2001 -- [ET]  Added quotes to two 'blockette 55' error messages
                       to avoid having an open string traverse into the
                       next line.
    11/2/2005 -- [ET]  Implemented 'interpolate_list_blockette()' function.
*/

#include <stdlib.h>
#include <string.h>
#include <evalresp/evalresp.h>
#include "evr_spline.h"
#include <evalresp_log/log.h>

void merge_lists(struct blkt *first_blkt, struct blkt **second_blkt, evalresp_log_t *log) {
    int new_ncoeffs, ncoeffs1, ncoeffs2, i, j;
    double *amp1, *amp2, *phase1, *phase2, *freq1, *freq2;
    struct blkt *tmp_blkt;

    tmp_blkt = *second_blkt;
    switch (first_blkt->type) {
    case LIST:
        break;
    default:
        evalresp_log(log, ERROR, 0, "merge_lists; filter types must be LIST");
        return; /*TODO MERGE_ERROR */
        /*XXX error_return(MERGE_ERROR, "merge_lists; filter types must be LIST"); */
    }

    if (first_blkt->type != tmp_blkt->type)
    {
        evalresp_log(log, ERROR, 0,
                "merge_lists; both filters must have the same type");
        return; /*TODO MERGE_ERROR */
        /*XXX error_return(MERGE_ERROR,
                "merge_lists; both filters must have the same type"); */
    }

    /* set up some local pointers and values */

    ncoeffs1 = first_blkt->blkt_info.list.nresp;

    amp1 = first_blkt->blkt_info.list.amp;
    phase1 = first_blkt->blkt_info.list.phase;
    freq1 = first_blkt->blkt_info.list.freq;

    ncoeffs2 = tmp_blkt->blkt_info.list.nresp;
    amp2 = tmp_blkt->blkt_info.list.amp;
    phase2 = tmp_blkt->blkt_info.list.phase;
    freq2 = tmp_blkt->blkt_info.list.freq;

    new_ncoeffs = ncoeffs1 + ncoeffs2;

    /* attempt to reallocate space for the new (combined) coefficients vector */

    if ((amp1 = (double *) realloc(amp1, new_ncoeffs * sizeof(double)))
            == (double *) NULL)
    {
        evalresp_log(log, ERROR, 0,
                "merge_lists; insufficient memory for combined amplitudes");
        return; /*TODO OUT_OF_MEMORY */
        /*XXX error_exit(OUT_OF_MEMORY,
                "merge_lists; insufficient memory for combined amplitudes"); */
    }

    if ((phase1 = (double *) realloc(phase1, new_ncoeffs * sizeof(double)))
            == (double *) NULL)
    {
        evalresp_log(log, ERROR, 0,
                "merge_lists; insufficient memory for combined phases");
        return; /*TODO OUT_OF_MEMORY */
        /*XXX error_exit(OUT_OF_MEMORY,
                "merge_lists; insufficient memory for combined phases"); */
    }

    if ((freq1 = (double *) realloc(freq1, new_ncoeffs * sizeof(double)))
            == (double *) NULL)
    {
        evalresp_log(log, ERROR, 0,
                "merge_lists; insufficient memory for combined frequencies");
        return; /*TODO OUT_OF_MEMORY */
        /*XXX error_exit(OUT_OF_MEMORY,
                "merge_lists; insufficient memory for combined frequencies"); */
    }
    ;

    /* copy the coeff values to the new space */

    for (i = 0, j = (ncoeffs1); i < ncoeffs2; i++, j++) {
        amp1[j] = amp2[i];
        phase1[j] = phase2[i];
        freq1[j] = freq2[i];
    }

    /* set the new values for the combined filter, free second_blkt and reset to the next_blkt
     value for first_blkt (i.e. to the next filter in the sequence) */

    first_blkt->blkt_info.list.nresp = new_ncoeffs;
    first_blkt->blkt_info.list.amp = amp1;
    first_blkt->blkt_info.list.freq = freq1;
    first_blkt->blkt_info.list.phase = phase1;
    first_blkt->next_blkt = tmp_blkt->next_blkt;
    free_fir(tmp_blkt);
    *second_blkt = first_blkt->next_blkt;

}

void merge_coeffs(struct blkt *first_blkt, struct blkt **second_blkt, evalresp_log_t *log) {
    int new_ncoeffs, ncoeffs1, ncoeffs2, i, j;
    double *coeffs1, *coeffs2;
    struct blkt *tmp_blkt;

    tmp_blkt = *second_blkt;
    switch (first_blkt->type) {
    case FIR_SYM_1:
    case FIR_SYM_2:
    case FIR_ASYM:
        break;
    default:
        evalresp_log(log, ERROR, 0, "merge_coeffs; filter types must be FIR");
        return; /*TODO MERGE_ERROR */
        /*XXX error_return(MERGE_ERROR, "merge_coeffs; filter types must be FIR");*/
    }

    if (first_blkt->type != tmp_blkt->type)
    {
        evalresp_log(log, ERROR, 0,
                "merge_coeffs; both filters must have the same type");
        return; /*TODO MERGE_ERROR */
        /*XXX error_return(MERGE_ERROR,
                "merge_coeffs; both filters must have the same type"); */
    }

    /* set up some local pointers and values */

    ncoeffs1 = first_blkt->blkt_info.fir.ncoeffs;
    coeffs1 = first_blkt->blkt_info.fir.coeffs;

    ncoeffs2 = tmp_blkt->blkt_info.fir.ncoeffs;
    coeffs2 = tmp_blkt->blkt_info.fir.coeffs;

    new_ncoeffs = ncoeffs1 + ncoeffs2;

    /* attempt to reallocate space for the new (combined) coefficients vector */

    if ((coeffs1 = (double *) realloc(coeffs1, new_ncoeffs * sizeof(double)))
            == (double *) NULL)
    {
        evalresp_log(log, ERROR, 0,
                "merge_coeffs; insufficient memory for combined coeffs");
        return; /*TODO OUT_OF_MEMORY */
        /*XXX error_exit(OUT_OF_MEMORY,
                "merge_coeffs; insufficient memory for combined coeffs"); */
    }

    /* copy the coeff values to the new space */

    for (i = 0, j = (ncoeffs1); i < ncoeffs2; i++, j++) {
        coeffs1[j] = coeffs2[i];
    }

    /* set the new values for the combined filter, free second_blkt and reset to the next_blkt
     value for first_blkt (i.e. to the next filter in the sequence) */

    first_blkt->blkt_info.fir.ncoeffs = new_ncoeffs;
    first_blkt->blkt_info.fir.coeffs = coeffs1;
    first_blkt->next_blkt = tmp_blkt->next_blkt;
    free_fir(tmp_blkt);
    *second_blkt = first_blkt->next_blkt;

}

void check_channel(struct channel *chan, evalresp_log_t *log) {
    // TODO - assignments below (0 + NULL) made blindly to fix compiler warning.  bug?
    struct stage *stage_ptr, *next_stage, *prev_stage;
    struct blkt *blkt_ptr, *next_blkt;
    struct blkt *filt_blkt = NULL, *deci_blkt = NULL, *gain_blkt = NULL, *ref_blkt = NULL;
    int stage_type;
    int gain_flag, deci_flag, ref_flag;
    int i, j, nc = 0;

    /* first run a 'sanity-check' of the filter sequence, making sure
     that the units match and that the proper blockettes are found
     where they are 'expected'.  At the same time, continuation filters
     from the FIR filters are merged and out of order filters are
     moved to ensure that the order for blockettes in a stage will be

     FILT_TYPE   ( ->  DECIMATION  )->  GAIN

     where FILT_TYPE is one of ANALOG_PZ, LAPLACE_PZ (which are both called
     PZ_TYPE here), IIR_PZ (called IIR_TYPE here), FIR_ASYM, FIR_SYM1 or
     FIR_SYM2 (which are all called FIR_TYPE in this routine).
     This sequence will order will be established, regardless of the order
     in which the blockettes appeared in the RESP file.  One other 'type' of
     stage is allowed (in addition to the PZ_TYPE and FIR_TYPE):  A 'gain only'
     stage.  This stage type consists of a gain blockette only (with no other
     blockettes).  Use of this type of filter is discouraged, since there is no
     information about units into and out of the stage in a gain blockette, but
     evalresp will allow this type of stage to be used by assuming that the
     units into and out of this stage are identical (i.e. that the units are
     continous between the stage preceeding the 'GAIN_TYPE' stage and the stage
     following it (essentially, this means that 'Units Checking' is skipped
     when this type of stage is encountered).

     Note:  The Decimation stages in any 'type' of filter are optional, except
            for FIR filters, where they are required if there are a non-zero
            number of FIR coefficients of filter or IIR filters with a non-zero
            number of poles or zeros */

  stage_ptr = chan->first_stage;
  prev_stage = (struct stage *)NULL;
  for(i = 0; i < chan->nstages; i++) {
    j = 0;
    next_stage = stage_ptr->next_stage;
    stage_type = gain_flag = deci_flag = ref_flag = 0;
    blkt_ptr = stage_ptr->first_blkt;
    curr_seq_no = stage_ptr->sequence_no;
    while(blkt_ptr) {
      j++;
      next_blkt = blkt_ptr->next_blkt;
      switch (blkt_ptr->type) {
      case POLYNOMIAL: /* IGS 06/01/2013 */
      /* FIXME: Add checks for the stage */
        stage_type = POLYNOMIAL_TYPE;
        filt_blkt = blkt_ptr;
        break;
      case ANALOG_PZ:
      case LAPLACE_PZ:
        if(stage_type && stage_type != GAIN_TYPE)
        {
          evalresp_log(log, ERROR, 0, "check_channel; %s in stage %d",
                       "more than one filter type",i);
          return; /*TODO ILLEGAL_RESP_FORMAT */
          /*XXX error_return(ILLEGAL_RESP_FORMAT, "check_channel; %s in stage %d",
                       "more than one filter type",i); */
        }
        stage_type = PZ_TYPE;
        filt_blkt = blkt_ptr;
        break;
      case IIR_PZ:
        if(stage_type && stage_type != GAIN_TYPE)
        {
          evalresp_log(log, ERROR, 0, "check_channel; %s in stage %d",
                       "more than one filter type",i);
          return; /*TODO ILLEGAL_RESP_FORMAT */
          /*XXX error_return(ILLEGAL_RESP_FORMAT, "check_channel; %s in stage %d",
                       "more than one filter type",i); */
        }
        stage_type = IIR_TYPE;
        filt_blkt = blkt_ptr;
        break;
      case FIR_COEFFS:
          evalresp_log(log, ERROR, 0, "check_channel; unsupported filter type");
        return; /*TODO UNSUPPORT_FILTYPE */
        /*XXX error_return(UNSUPPORT_FILTYPE, "check_channel; unsupported filter type"); */
        break;
      case LIST:
/* IGD         error_return(UNSUPPORT_FILTYPE, "check_channel; unsupported filter type"); */
/* We are going to support this blockette starting with version 3.2.17 IGD */
/* Nevertheless, this support is limited . Clearly, if we allow to mix the blockette 55 with */
/* only selected number of known frequencies with the other blockettes with arbitrary frequency */
/* sampling, there will be a mess */
/* In this version of evalresp (2.3.17), we allow to get a response for the blockette 55 if it is */
/* the only one filter blockette in the response file . To simplify , we allow the program to process */
/* the LIST response if and only if the blkt_ptr->next_blkt == NULL; */

/* First we merge blocketes if the list spans for more than a single blockette */

      while(next_blkt != (struct blkt *)NULL && next_blkt->type == blkt_ptr->type)
      {
            merge_lists(blkt_ptr,&next_blkt, log);
      }
    if (stage_ptr->next_stage != NULL || prev_stage != NULL)
    {
        if (!stage_ptr->next_stage && 0 != chan->first_stage->next_stage->sequence_no)
        {
            evalresp_log(log, ERROR, 0,
                    "blockette 55 cannot be mixed with other filter blockettes\n");
            return; /*TODO UNSUPPORT_FILTYPE */
            /*XXX error_return(UNSUPPORT_FILTYPE,
                    "blockette 55 cannot be mixed with other filter blockettes\n"); */
        }
    }
    else
    {
        /* There are still situations which we want to avoid */
        /* In particular, the next stage can be hidden in */
        /* chan->first_stage->next_stage->first_blkt->type */
        /* If it is GAIN, we will continue. If it is not gain, we generate error */

        if ( chan->first_stage->next_stage != NULL)
        {
            if (chan->first_stage->next_stage->first_blkt != NULL)
            {
                /* If the next stage is overall sesitivity, it's OK */
                if (chan->first_stage->next_stage->first_blkt->type != GAIN)
                {
                        evalresp_log(log, ERROR, 0,
                       "blockette 55 cannot be mixed with other filter blockettes\n");
                        return; /*TODO UNSUPPORT_FILTYPE */
                        /*XXX error_return(UNSUPPORT_FILTYPE,
                       "blockette 55 cannot be mixed with other filter blockettes\n"); */
                }

            }
        }
    }
    stage_type = LIST_TYPE;
    filt_blkt = blkt_ptr;
    break;
      case GENERIC:
/*        error_return(UNSUPPORT_FILTYPE, "check_channel; unsupported filter type"); */
    /* IGD 05/16/02 Added support for Generic type */
    if(stage_type && stage_type != GAIN_TYPE)
    {
            evalresp_log(log, ERROR, 0, "check_channel; %s in stage %d",
                              "more than one filter type",i+1);
            return; /*TODO ILLEGAL_RESP_FORMAT */
            /*XXX error_return(ILLEGAL_RESP_FORMAT, "check_channel; %s in stage %d",
                              "more than one filter type",i+1); */
    }
        /* check to see if next blockette(s) is(are) a continuation of this one.
        If so, merge them into one blockette */
    if(next_blkt != (struct blkt *)NULL && next_blkt->type == blkt_ptr->type)
    {
                evalresp_log(log, ERROR, 0,
                        "check_channel; multiple 55 blockettes in GENERIC stages are not supported yet");
                return; /*TODO ILLEGAL_RESP_FORMAT */
                /*XXX error_return(ILLEGAL_RESP_FORMAT,
                        "check_channel; multiple 55 blockettes in GENERIC stages are not supported yet"); */
    }
         stage_type = GENERIC_TYPE;
/*    nc = 1; */ /*for calc_delay to be 0 in decimation blockette */
    evalresp_log(log, WARN, 0, "%s WARNING: Generic blockette is detected in stage %d; content is ignored\n", myLabel, i+1);
    /*XXX fprintf(stdout, "%s WARNING: Generic blockette is detected in stage %d; content is ignored\n", myLabel, i+1);
    fflush(stdout); */
    filt_blkt = blkt_ptr;
        break;

      case FIR_SYM_1:
      case FIR_SYM_2:
      case FIR_ASYM:
        if(stage_type && stage_type != GAIN_TYPE)
        {
          evalresp_log(log, ERROR, 0, "check_channel; %s in stage %d",
                       "more than one filter type",i);
          return; /*TODO ILLEGAL_RESP_FORMAT */
          /*XXX error_return(ILLEGAL_RESP_FORMAT, "check_channel; %s in stage %d",
                       "more than one filter type",i); */
        }

        /* check to see if next blockette(s) is(are) a continuation of this one.
           If so, merge them into one blockette */
        while(next_blkt != (struct blkt *)NULL && next_blkt->type == blkt_ptr->type)
          merge_coeffs(blkt_ptr,&next_blkt, log);

        /* set the stage type to be FIR_TYPE */
        stage_type = FIR_TYPE;

        /* make FIR filters symmetric if possible */
        if(blkt_ptr->type == FIR_ASYM)
          check_sym(blkt_ptr, chan, log);

        /* increment the channel delay for this stage using the number of coefficients
           and the filter type */
        if(blkt_ptr->type == FIR_SYM_1)
          nc = (double) blkt_ptr->blkt_info.fir.ncoeffs*2 - 1;
        else if(blkt_ptr->type == FIR_SYM_2)
          nc = (double) blkt_ptr->blkt_info.fir.ncoeffs*2;
        else if(blkt_ptr->type == FIR_ASYM)
          nc = (double) blkt_ptr->blkt_info.fir.ncoeffs;
        filt_blkt = blkt_ptr;
        break;
      case IIR_COEFFS:  /* IGD New type evalresp supports in 3.2.17 */
     if(stage_type && stage_type != GAIN_TYPE)
     {
            evalresp_log(log, ERROR, 0, "check_channel; %s in stage %d",
                              "more than one filter type",i);
            return; /*TODO ILLEGAL_RESP_FORMAT */
            /*XXX error_return(ILLEGAL_RESP_FORMAT, "check_channel; %s in stage %d",
                              "more than one filter type",i); */
     }
        /* check to see if next blockette(s) is(are) a continuation of this one.
        If so, merge them into one blockette */
    if(next_blkt != (struct blkt *)NULL && next_blkt->type == blkt_ptr->type)
    {
                evalresp_log(log, ERROR, 0,
                        "check_channel; multiple 55 blockettes in IIR stages are not supported yet");
                return; /*TODO ILLEGAL_RESP_FORMAT */
                /*XXX error_return(ILLEGAL_RESP_FORMAT,
                        "check_channel; multiple 55 blockettes in IIR stages are not supported yet"); */
    }
        /* merge_coeffs(blkt_ptr,&next_blkt);  */ /* Leave it alone for now ! */
        /* set the stage type to be FIR_TYPE */
        stage_type = IIR_COEFFS_TYPE;
    nc = 1; /*for calc_delay to be 0 in decimation blockette */
        filt_blkt = blkt_ptr;
        break;
      case GAIN:
        if(!stage_ptr->sequence_no) {
          chan->sensit = blkt_ptr->blkt_info.gain.gain;
          chan->sensfreq = blkt_ptr->blkt_info.gain.gain_freq;
        }
        if(!stage_type)
          stage_type = GAIN_TYPE;
        gain_flag = j;
        gain_blkt = blkt_ptr;
        break;
      case DECIMATION:
        /* if stage is a FIR filter, increment the estimated delay and applied
           correction for the channel */
        if(stage_type) {
      if (stage_type == FIR_TYPE && nc > 0)
            chan->calc_delay += ((nc-1)/2.0) * blkt_ptr->blkt_info.decimation.sample_int;
          chan->estim_delay += (double) blkt_ptr->blkt_info.decimation.estim_delay;
          chan->applied_corr += (double) blkt_ptr->blkt_info.decimation.applied_corr;
          chan->sint = blkt_ptr->blkt_info.decimation.sample_int*
            (double)blkt_ptr->blkt_info.decimation.deci_fact;  /* channel's sint is the last stage's */
        }
        else
        {
          evalresp_log(log, ERROR, 0,
                     "check_channel; decimation blockette with no associated filter");
          return; /*TODO ILLEGAL_RESP_FORMAT */
          /*XXX error_return(ILLEGAL_RESP_FORMAT,
                     "check_channel; decimation blockette with no associated filter"); */
        }
        deci_blkt = blkt_ptr;
        deci_flag = j;
        break;
      case REFERENCE:
        ref_flag = j;
        ref_blkt = blkt_ptr;
        break;
      default:
        error_return(UNSUPPORT_FILTYPE, "check_channel; unrecognized blkt type (type=%d)",
                     blkt_ptr->type);
      }
      blkt_ptr = next_blkt;
    }

    /* If not a 'gain-only' stage, run a 'sanity check' for this stage's filter
       sequence.  Do this by reorganizing the filters (regardless of input
       order) so that they point to each other in the following manner within
       a particular stage:

        STAGE_FIRST_BLKT -> (REF_BLKT) -> FILT_BLKT -> (DECI_BLKT) -> GAIN_BLKT

       where the blockette types are defined above (blockettes given in
       parentheses are optional), the STAGE_FIRST_BLKT indicates the pointer
       to the first blockette for this stage and the REF_BLKT indicates the
       pointer to the REFERENCE blockette, if there is one */
    if(stage_type != GAIN_TYPE) {
      if(ref_flag && deci_flag) {
        stage_ptr->first_blkt = ref_blkt;
        ref_blkt->next_blkt = filt_blkt;
        filt_blkt->next_blkt = deci_blkt;
        deci_blkt->next_blkt = gain_blkt;
        gain_blkt->next_blkt = (struct blkt *)NULL;
      }
      else if(deci_flag) {
        stage_ptr->first_blkt = filt_blkt;
        filt_blkt->next_blkt = deci_blkt;
        deci_blkt->next_blkt = gain_blkt;
        gain_blkt->next_blkt = (struct blkt *)NULL;
      }
      else if(ref_flag) {
        stage_ptr->first_blkt = ref_blkt;
        ref_blkt->next_blkt = filt_blkt;
        filt_blkt->next_blkt = gain_blkt;
        gain_blkt->next_blkt = (struct blkt *)NULL;
      }
      else if(gain_flag) {
        stage_ptr->first_blkt = filt_blkt;
        filt_blkt->next_blkt = gain_blkt;
        gain_blkt->next_blkt = (struct blkt *)NULL;
      }
    }

    /* check for mismatch in units between this stage and the previous one
       (or in the case where this is a gain-only stage, between the next stage
       and the previous stage).  If it is a gain-only stage, this check will be
       skipped and the 'prev_stage' pointer will be left in it's existing
       position (essentially, this 'disables' the units check for 'gain-only'
       stages */
    /* IGD in version 3.2.17, there are two new stage types are in the next check */
    if(stage_type == PZ_TYPE || stage_type == FIR_TYPE  ||
    stage_type == IIR_TYPE || stage_type == IIR_COEFFS_TYPE || stage_type == LIST_TYPE) {
      if(prev_stage != (struct stage *)NULL && prev_stage->output_units !=
         stage_ptr->input_units)
      {
        evalresp_log(log, ERROR, 0, "check_channel; units mismatch between stages");
        exit(1); /*IGD 06/06/2017  TODO ILLEGAL_RESP_FORMAT */
        return; /*TODO ILLEGAL_RESP_FORMAT */
        /*XXX error_return(ILLEGAL_RESP_FORMAT, "check_channel; units mismatch between stages"); */
      }
    }

    /* if the main filter type is either FIR with a non-zero number of coefficients
       or IIR with a non-zero number of poles or zeros, then it is a fatal error
       if the following blockette for this stage is not a decimation blockette
       (i.e. if the deci_flag has not been set) */

    /* IGD : new stage type IIR_COEFFS_TYPE is processed in v 3.2.17 */
    if((stage_type == IIR_TYPE || stage_type == FIR_TYPE || stage_type == IIR_COEFFS_TYPE) && !deci_flag)
    {
        evalresp_log(log, ERROR, 0, "check_channel; required decimation blockette for IIR or FIR filter missing");
	exit(1); /* IGD 06/06/2017 To satisfy failure of RESP.BK.BDM..UCD; need return error and process it */
       /* return; */ /*TODO ILLEGAL_RESP_FORMAT */
        /*XXX error_return(ILLEGAL_RESP_FORMAT, "check_channel; required decimation blockette for IIR or FIR filter missing"); */
    }

    /* if this wasn't a gain-only stage, save it for comparison of units between
       stages.  Otherwise, keep the previous stage from this iteration and move
       on to the next stage */
    if(stage_type != GAIN_TYPE) {
      if(stage_ptr->sequence_no)
        prev_stage = stage_ptr;
    }
    stage_ptr = next_stage;
  }
}

void check_sym(struct blkt *f, struct channel *chan, evalresp_log_t *log) {
  int nc, n0, k;
  double sum = 0.0;

  nc = f->blkt_info.fir.ncoeffs;

  /* CHECK IF IF FILTER IS NORMALIZED TO 1 AT FREQ 0 */

  for (k=0; k<nc; k++) sum += f->blkt_info.fir.coeffs[k];
  if (nc && (sum < (1.0-FIR_NORM_TOL) || sum > (1.0+FIR_NORM_TOL))) {
    evalresp_log(log, WARN, 0,"%s WARNING: FIR normalized: sum[coef]=%E; ", myLabel, sum);
    evalresp_log(log, WARN, 0,"%s %s %s %s %s\n", myLabel, chan->network, chan->staname, chan->locid, chan->chaname);
    /*XXX fprintf(stderr,"%s WARNING: FIR normalized: sum[coef]=%E; ", myLabel, sum);
    fprintf(stderr,"%s %s %s %s %s\n", myLabel, chan->network, chan->staname, chan->locid, chan->chaname);
    fflush(stderr); */
    for (k=0; k<nc; k++) f->blkt_info.fir.coeffs[k] /= sum;
  }

  if (f->type != FIR_ASYM) return;

  /* CHECK IF FILTER IS SYMETRICAL WITH EVEN NUM OF WEIGHTS */

  if ((nc%2) == 0) {
    n0 = nc / 2;
    for (k=0; k < n0; k++) {
      if (f->blkt_info.fir.coeffs[n0+k] != f->blkt_info.fir.coeffs[n0-k-1]) return;
    }
    f->type = FIR_SYM_2;
    f->blkt_info.fir.ncoeffs = n0;
  }

  /* CHECK IF FILTER IS SYMETRICAL WITH ODD NUM OF WEIGHTS */

  else {
    n0 = (nc - 1) / 2;
    for (k=1; k<nc-n0; k++) {
      if (f->blkt_info.fir.coeffs[n0+k] != f->blkt_info.fir.coeffs[n0-k]) return;
    }
    f->type = FIR_SYM_1;
    f->blkt_info.fir.ncoeffs =  nc-n0;
  }
}

/*********** function free for freeing of double arrays *******************************************/
/*
void sscdns_free_double (double *array)
{
  if (array != NULL)
  {
    free (array);
    array = NULL;
  }
}
*/

void interpolate_list_blockette(double **frequency_ptr,
                                double **amplitude_ptr, double **phase_ptr,
                                int *p_number_points, double *req_freq_arr,
                                int req_num_freqs, double tension, evalresp_log_t *log)
{
  int i, num;
  double first_freq, last_freq, val, min_ampval;
  int fix_first_flag, fix_last_flag, unwrapped_flag;
  double *used_req_freq_arr;
  int used_req_num_freqs;
  double *retvals_arr, *retamps_arr;
  int num_retvals;
  char *retstr;
  double old_pha, new_pha, added_value, prev_phase;
  double *local_pha_arr;

         /* get first and last values in freq array from list blockette */
  first_freq = (*frequency_ptr)[0];
  last_freq = (*frequency_ptr)[(*p_number_points)-1];
  if(first_freq > last_freq)
  {      /* first is larger than last; swap values */
    val = first_freq;
    first_freq = last_freq;
    last_freq = val;
  }
  i = 0;      /* check if any requested frequency values are out of range */
  while(i < req_num_freqs && (req_freq_arr[i] < first_freq ||
                                               req_freq_arr[i] > last_freq))
  {      /* for each out-of-range value at beginning of req freqs */
    ++i;
  }
         /* if out of range values found at beginning of req freqs and last
            clipped value is within 0.0001% of first List freq value
            then setup to replace it with first List frequency value: */
  if(i > 0 && fabs(first_freq - req_freq_arr[i-1]) < first_freq*1e-6)
  {
    --i;                          /* restore clipped value */
    fix_first_flag = 1;           /* indicate value should be "fixed" */
  }
  else
    fix_first_flag = 0;
  if(i > 0)
  {      /* some requested frequency values were clipped */
    if(i >= req_num_freqs)
    {    /* all requested frequency values were clipped */
      evalresp_log(log, ERROR, 0,"Error interpolating amp/phase values:  %s",
                                  "All requested freqencies out of range\n");
      /*XXX error_exit(IMPROP_DATA_TYPE,"Error interpolating amp/phase values:  %s",
                                  "All requested freqencies out of range\n"); */
      return; /*TODO return error */
    }
    evalresp_log(log, INFO, 0,
         "%s Note:  %d frequenc%s clipped from beginning of requested range\n",
                                               myLabel, i, ((i == 1) ? "y" : "ies"));
    /*XXX fprintf(stderr,
         "%s Note:  %d frequenc%s clipped from beginning of requested range\n",
                                               myLabel, i, ((i == 1) ? "y" : "ies"));*/
  }
  used_req_num_freqs = req_num_freqs;
  while(used_req_num_freqs > 0 &&
                          (req_freq_arr[used_req_num_freqs-1] > last_freq ||
                           req_freq_arr[used_req_num_freqs-1] < first_freq))
  {      /* for each out-of-range value at end of req freqs */
    --used_req_num_freqs;
  }
         /* if out of range values found at end of req freqs and last
            clipped value is within 0.0001% of last List freq value
            then replace it with last List frequency value: */
  if(used_req_num_freqs < req_num_freqs &&
        fabs(req_freq_arr[used_req_num_freqs] - last_freq) < last_freq*1e-6)
  {
    used_req_num_freqs++;         /* restore clipped value */
    fix_last_flag = 1;            /* indicate value should be "fixed" */
  }
  else
    fix_last_flag = 0;
  if((num=req_num_freqs-used_req_num_freqs) > 0)
  {      /* some requested frequency values were clipped; show message */
    req_num_freqs = used_req_num_freqs;
    evalresp_log(log, INFO, 0,
    /*XXX fprintf(stderr, */
               "%s Note:  %d frequenc%s clipped from end of requested range\n",
                                           myLabel, num, ((num == 1) ? "y" : "ies"));
  }
  if(i > 0)                       /* if freq entries were skipped then */
    req_num_freqs -= i;           /* subtract # of freqs clipped from beg */
         /* allocate new array for requested frequency values */
  used_req_freq_arr = (double *)calloc(req_num_freqs,sizeof(double));
         /* copy over freq values (excluding out-of-bounds values) */
  memcpy(used_req_freq_arr,&req_freq_arr[i],req_num_freqs*sizeof(double));
  req_freq_arr = used_req_freq_arr;    /* setup to use new array */
  if(fix_first_flag)
    req_freq_arr[0] = first_freq;
  if(fix_last_flag)
    req_freq_arr[req_num_freqs-1] = last_freq;

         /* interpolate amplitude values */
  if((retstr=evr_spline(*p_number_points,*frequency_ptr,*amplitude_ptr,
                                     tension,1.0,req_freq_arr,req_num_freqs,
                                        &retvals_arr,&num_retvals, log)) != NULL)
  {
    evalresp_log(log, ERROR, 0,"Error interpolating amplitudes:  %s",retstr);
    /*XXX error_exit(IMPROP_DATA_TYPE,"Error interpolating amplitudes:  %s",retstr); */
    return; /*TODO This should return somethin */
  }
  if(num_retvals != req_num_freqs)
  {      /* # of generated values != # requested (shouldn't happen) */
    evalresp_log(log, ERROR, 0,"Error interpolating amplitudes:  %s",
                                "Bad # of values");
    /*XXX error_exit(IMPROP_DATA_TYPE,"Error interpolating amplitudes:  %s",
                                "Bad # of values"); */
    return; /*TODO This should return somethin */
  }
  retamps_arr = retvals_arr;      /* save ptr to interpolated amplitudes */

         /* make sure all interpolated amplitude values are positive */
              /* first find minimum value in "source" amplitudes */
  min_ampval = (*amplitude_ptr)[0];
  for(i=1; i<*p_number_points; ++i)
  {      /* for each remaining "source" amplitude value */
    if((val=(*amplitude_ptr)[i]) < min_ampval)
      min_ampval = val;           /* if new mininum then save value */
  }
  if(min_ampval > 0.0)
  {      /* all "source" amplitude values are positive */
    min_ampval /= 10.0;           /* bring minimum a bit closer to zero */
              /* substitude minimum for any non-positive values */
    for(i=0; i<num_retvals; ++i)
    {    /* for each interpolated amplitude value */
      if(retamps_arr[i] <= 0.0)        /* if value not positive then */
        retamps_arr[i] = min_ampval;   /* use minimum value */
    }
  }

         /* unwrap phase values into local array */
  local_pha_arr = (double *)(calloc((*p_number_points),sizeof(double)));
  added_value = prev_phase = 0.0;
  unwrapped_flag = 0;
  for(i=0; i<(*p_number_points); i++)
  {      /* for each phase value; unwrap if necessary */
    old_pha = (*phase_ptr)[i];
    new_pha = unwrap_phase(old_pha,prev_phase,360.0,&added_value);
    if(added_value == 0.0)             /* if phase value not unwrapped */
      local_pha_arr[i] = old_pha;      /* then copy original value */
    else
    {    /* phase value was unwrapped */
      local_pha_arr[i] = new_pha;      /* enter new phase value */
      unwrapped_flag = 1;              /* indicate unwrapping */
    }
    prev_phase = new_pha;
  }

         /* interpolate phase values */
  retstr = evr_spline(*p_number_points,*frequency_ptr,local_pha_arr,
                                     tension,1.0,req_freq_arr,req_num_freqs,
                                                 &retvals_arr,&num_retvals, log);
  free(local_pha_arr);
  if(retstr != NULL)
  {
    evalresp_log(log, ERROR, 0,"Error interpolating phases:  %s",retstr);
    /*XXX error_exit(IMPROP_DATA_TYPE,"Error interpolating phases:  %s",retstr); */
    return; /*TODO this should return something */
  }
  if(num_retvals != req_num_freqs)
  {      /* # of generated values != # requested (shouldn't happen) */
    evalresp_log(log, ERROR, 0,"Error interpolating phases:  %s",
                                "Bad # of values");
    /*XXX error_exit(IMPROP_DATA_TYPE,"Error interpolating phases:  %s",
                                "Bad # of values"); */
    return; /*TODO this should return something */
  }

  if(unwrapped_flag)
  { /* phase values were previously unwrapped; wrap interpolated values */
    added_value = 0.0;
    new_pha = retvals_arr[0];          /* check first phase value */
    if(new_pha > 180.0)
    {    /* first phase value is too high */
      do           /* set offset to put values in range */
        added_value -= 360.0;
      while(new_pha + added_value > 180.0);
    }
    else if(new_pha < -180.0)
    {    /* first phase value is too low */
      do           /* set offset to put values in range */
        added_value += 360.0;
      while(new_pha + added_value < -180.0);
    }
    for(i=0; i<num_retvals; i++)
    {    /* for each phase value; wrap if out of range */
      new_pha = wrap_phase(retvals_arr[i],360.0,&added_value);
      if(added_value != 0.0)             /* if phase value wrapped then */
        retvals_arr[i] = new_pha;        /* enter new phase value */
    }
  }

         /* free arrays passed into function */
  free(*frequency_ptr);
  free(*amplitude_ptr);
  free(*phase_ptr);

         /* enter generated arrays and # of points */
  *frequency_ptr = req_freq_arr;
  *amplitude_ptr = retamps_arr;
  *phase_ptr = retvals_arr;
  *p_number_points = num_retvals;
}
