/*	Copyright (c) 1987, 1988 by Software Research Associates, Inc.	*/

#ifndef lint
# define FEP_STAT \
    "$Header$ (SRA)"
#endif /* lint */

extern long stat_obyte;
extern long stat_ibyte;
extern long stat_rerror;
extern long stat_werror;
extern long stat_nselect;

struct statistics {
    char *info_name;
    long *info_valp;
};
