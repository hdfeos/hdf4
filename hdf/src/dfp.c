/****************************************************************************
 * NCSA HDF                                                                 *
 * Software Development Group                                               *
 * National Center for Supercomputing Applications                          *
 * University of Illinois at Urbana-Champaign                               *
 * 605 E. Springfield, Champaign IL 61820                                   *
 *                                                                          *
 * For conditions of distribution and use, see the accompanying             *
 * hdf/COPYING file.                                                        *
 *                                                                          *
 ****************************************************************************/

#ifdef RCSID
static char RcsId[] = "@(#)$Revision$";
#endif

/* $Id$ */

/*-----------------------------------------------------------------------------
 * File:     dfp.c
 * Purpose:  read and write palettes
 * Invokes:  df.c
 * Contents:
 *  DFPgetpal    : retrieve next palette
 *  DFPputpal    : write palette to file
 *  DFPaddpal    : add palette to file
 *  DFPnpals     : number of palettes in HDF file
 *  DFPreadref   : get palette with this reference number next
 *  DFPwriteref  : put palette with this reference number next
 *  DFPrestart   : forget info about last file accessed - restart from beginning
 *  DFPlastref   : return reference number of last element read or written
 *  DFPIopen     : open/reopen file
 *---------------------------------------------------------------------------*/

#include "hdf.h"
#include "hfile.h"

PRIVATE uint16 Readref = 0;
PRIVATE uint16 Writeref = 0;
PRIVATE uint16 Refset = 0;	/* Ref of palette to get next */
PRIVATE uint16 Lastref = 0;	/* Last ref read/written */

PRIVATE char Lastfile[DF_MAXFNLEN] = "";	/* last file opened */

PRIVATE int32 DFPIopen
            (const char _HUGE * filename, intn acc_mode);

/*--------------------------------------------------------------------------
 NAME
    DFPgetpal -- get next palette from file
 USAGE
    intn DFPgetpal(filename,palette)
        char *filename;         IN: name of HDF file
        VOIDP palette;          OUT: ptr to the buffer to store the palette in
 RETURNS
    SUCCEED on success, FAIL on failure.
 DESCRIPTION
    Gets the next palette from the file specified.
 GLOBAL VARIABLES
    Lastref, Refset
 COMMENTS, BUGS, ASSUMPTIONS
 EXAMPLES
 REVISION LOG
--------------------------------------------------------------------------*/
intn
DFPgetpal(const char *filename, VOIDP palette)
{
    CONSTR(FUNC, "DFPgetpal");
    int32       file_id;
    int32       aid;
    int32       length;

    HEclear();

    if (!palette)
	HRETURN_ERROR(DFE_ARGS, FAIL);

    if ((file_id = DFPIopen(filename, DFACC_READ)) == FAIL)
	HRETURN_ERROR(DFE_BADOPEN, FAIL);

    if (Refset)
      {
	  aid = Hstartread(file_id, DFTAG_IP8, Refset);
	  if (aid == FAIL)
	      aid = Hstartread(file_id, DFTAG_LUT, Refset);
      }		/* end if */
    else if (Readref)
      {
	  aid = Hstartread(file_id, DFTAG_IP8, Readref);
	  if (aid == FAIL)
	      aid = Hstartread(file_id, DFTAG_LUT, Readref);
	  if (aid != FAIL &&
	    (Hnextread(aid, DFTAG_IP8, DFREF_WILDCARD, DF_CURRENT) == FAIL))
	    {
		if (Hnextread(aid, DFTAG_LUT, DFREF_WILDCARD, DF_CURRENT) == FAIL)
		  {
		      Hendaccess(aid);
		      aid = FAIL;
		  }	/* end if */
	    }	/* end if */
      }		/* end if */
    else
      {
	  aid = Hstartread(file_id, DFTAG_IP8, DFREF_WILDCARD);
	  if (aid == FAIL)
	      aid = Hstartread(file_id, DFTAG_LUT, DFREF_WILDCARD);
      }		/* end else */

    Refset = 0;
    if (aid == FAIL)
	return (HDerr(file_id));

    /* on error, close file and return -1 */

    if (Hinquire(aid, (int32 *) NULL, (uint16 *) NULL, &Readref, &length,
    (int32 *) NULL, (int32 *) NULL, (int16 *) NULL, (int16 *) NULL) == FAIL)
      {
	  Hendaccess(aid);
	  return HDerr(file_id);
      }		/* end if */

    /* read palette */
    if (Hread(aid, length, (uint8 *) palette) == FAIL)
      {
	  Hendaccess(aid);
	  return (HDerr(file_id));
      }		/* end if */

    Hendaccess(aid);

    Lastref = Readref;

    return (Hclose(file_id));
}	/* end DFPgetpal() */

/*--------------------------------------------------------------------------
 NAME
    DFPputpal -- Write palette to file
 USAGE
    intn DFPputpal(filename,palette,overwrite,filemode)
        char *filename;         IN: name of HDF file
        VOIDP palette;          IN: ptr to the buffer retrieve the palette from
        intn overwrite;         IN: whether to (1) overwrite last palette written,
                                    or (0) write it as a fresh palette
        char *filemode;         IN: if "a" append palette to file, "w" create
                                    new file
 RETURNS
    SUCCEED on success, FAIL on failure.
 DESCRIPTION
    Stores a palette in an HDF file, with options for creating new file or appending,
    and overwriting last palette written.
 GLOBAL VARIABLES
 COMMENTS, BUGS, ASSUMPTIONS
    To overwrite, the filename must be the same as for the previous call.
 EXAMPLES
 REVISION LOG
--------------------------------------------------------------------------*/
intn
DFPputpal(const char *filename, VOIDP palette, intn overwrite, const char *filemode)
{
    CONSTR(FUNC, "DFPputpal");
    int32       file_id;

    HEclear();

    if (!palette)
	HRETURN_ERROR(DFE_ARGS, FAIL);

    if (overwrite && HDstrcmp(filename, Lastfile))
	HRETURN_ERROR(DFE_BADCALL, FAIL);

    file_id = DFPIopen(filename, (*filemode == 'w') ? DFACC_CREATE : DFACC_WRITE);
    if (file_id == FAIL)
	HRETURN_ERROR(DFE_BADOPEN, FAIL);

    /* if we want to overwrite, Lastref is the ref to write.  If not, if
       Writeref is set, we use that ref.  If not we get a fresh ref. The
       ref to write is placed in Lastref */
    if (!overwrite)
	Lastref = (uint16) (Writeref ? Writeref : Hnewref(file_id));
    if (Lastref == 0)
	HRETURN_ERROR(DFE_NOREF, FAIL);

    Writeref = 0;	/* don't know ref to write after this */

    /* write out palette */
    if (Hputelement(file_id, DFTAG_IP8, Lastref, (uint8 *) palette, (int32) 768) < 0)
	return (HDerr(file_id));

    Hdupdd(file_id, DFTAG_LUT, Lastref, DFTAG_IP8, Lastref);

    return (Hclose(file_id));
}	/* end DFPputpal() */

/*--------------------------------------------------------------------------
 NAME
    DFPaddpal -- Append palette to file
 USAGE
    intn DFPaddpal(filename,palette)
        char *filename;         IN: name of HDF file
        VOIDP palette;          IN: ptr to the buffer retrieve the palette from
 RETURNS
    SUCCEED on success, FAIL on failure.
 DESCRIPTION
    Appends a palette in an HDF file.
 GLOBAL VARIABLES
 COMMENTS, BUGS, ASSUMPTIONS
 EXAMPLES
 REVISION LOG
--------------------------------------------------------------------------*/
intn
DFPaddpal(const char *filename, VOIDP palette)
{
    return (DFPputpal(filename, palette, 0, "a"));
}	/* end DFPaddpal() */

/*--------------------------------------------------------------------------
 NAME
    DFPnpals -- determine # of palettes in a file
 USAGE
    intn DFPnpals(filename)
        char *filename;         IN: name of HDF file
 RETURNS
    SUCCEED on success, FAIL on failure.
 DESCRIPTION
    Determines the number of unique palettes in a file.
 GLOBAL VARIABLES
 COMMENTS, BUGS, ASSUMPTIONS
 EXAMPLES
 REVISION LOG
--------------------------------------------------------------------------*/
intn
DFPnpals(const char *filename)
{
    CONSTR(FUNC, "DFPnpals");
    int32       file_id;
    intn        curr_pal;	/* current palette count */
    int32       nip8, nlut;	/* number of IP8s & number of LUTs */
    intn        npals;		/* total number of palettes */
    uint16      find_tag, find_ref;	/* storage for tag/ref pairs found */
    int32       find_off, find_len;	/* storage for offset/lengths of tag/refs found */
    int32      *pal_off;	/* storage for an array of palette offsets */
    intn        i, j;		/* local counting variable */

    HEclear();

    /* should use reopen if same file as last time - more efficient */
    if ((file_id = DFPIopen(filename, DFACC_READ)) == FAIL)
	HRETURN_ERROR(DFE_BADOPEN, FAIL);

    /* count number of IPs */
    if ((nip8 = Hnumber(file_id, DFTAG_IP8)) == FAIL)
	return (HDerr(file_id));
    /* count number of LUTs */
    if ((nlut = Hnumber(file_id, DFTAG_LUT)) == FAIL)
	return (HDerr(file_id));
    npals = (intn) (nip8 + nlut);

    /* if no palettes just return zero and get out */
    if (npals == 0)
      {
	  if (Hclose(file_id) == FAIL)
	      return FAIL;

	  return (npals);
      }

    /* Get space to store the palette offsets */
    if ((pal_off = (int32 *) HDgetspace(npals * sizeof(int32))) == NULL)
	            HRETURN_ERROR(DFE_NOSPACE, FAIL);

    /* go through the IP8s */
    curr_pal = 0;
    find_tag = find_ref = 0;
    while (Hfind(file_id, DFTAG_IP8, DFREF_WILDCARD, &find_tag, &find_ref, &find_off, &find_len, DF_FORWARD) == SUCCEED)
      {
	  pal_off[curr_pal] = find_off;		/* store offset */
	  curr_pal++;
      }		/* end while */

    /* go through the LUTs */
    find_tag = find_ref = 0;
    while (Hfind(file_id, DFTAG_LUT, DFREF_WILDCARD, &find_tag, &find_ref, &find_off, &find_len, DF_FORWARD) == SUCCEED)
      {
	  pal_off[curr_pal] = find_off;		/* store offset */
	  curr_pal++;
      }		/* end while */

    npals = curr_pal;	/* reset the number of palettes we really have */
    for (i = 1; i < curr_pal; i++)
      {		/* go through the palettes looking for duplicates */
	  for (j = 0; j < i; j++)
	    {
		if (pal_off[i] == pal_off[j])
		    npals--;	/* if duplicate found, decrement the number of palettes */
	    }	/* end for */
      }		/* end for */

    HDfreespace((VOIDP) pal_off);	/* free offsets */

    if (Hclose(file_id) == FAIL)
	HRETURN_ERROR(DFE_CANTCLOSE, FAIL);

    return (npals);
}	/* end DFPnpals() */

/*--------------------------------------------------------------------------
 NAME
    DFPreadref -- set ref # of palette to read next
 USAGE
    intn DFPreadref(filename,ref)
        char *filename;         IN: name of HDF file
        uint16 ref;             IN: ref # of palette to read next
 RETURNS
    SUCCEED on success, FAIL on failure.
 DESCRIPTION
    Sets the ref # of the next palette to read from a file
 GLOBAL VARIABLES
    Refset
 COMMENTS, BUGS, ASSUMPTIONS
 EXAMPLES
 REVISION LOG
--------------------------------------------------------------------------*/
intn
DFPreadref(const char *filename, uint16 ref)
{
    CONSTR(FUNC, "DFPreadref");
    int32       file_id;
    int32       aid;

    HEclear();

    if ((file_id = DFPIopen(filename, DFACC_READ)) == FAIL)
	HRETURN_ERROR(DFE_BADOPEN, FAIL);

    aid = Hstartread(file_id, DFTAG_IP8, ref);
    if (aid == FAIL)
      {
	  aid = Hstartread(file_id, DFTAG_LUT, ref);
	  if (aid == FAIL)
	      return (HDerr(file_id));
      }		/* end if */

    Hendaccess(aid);
    Refset = ref;

    return (Hclose(file_id));
}	/* end DFPreadref() */

/*--------------------------------------------------------------------------
 NAME
    DFPwriteref -- set ref # of palette to write next
 USAGE
    intn DFPwriteref(filename,ref)
        char *filename;         IN: name of HDF file
        uint16 ref;             IN: ref # of palette to write next
 RETURNS
    SUCCEED on success, FAIL on failure.
 DESCRIPTION
    Sets the ref # of the next palette to write to a file.  The filename is
    ignored -- this is probably a bug.  No matter what file the next palette
    is written to, it will have the reference number ref.
 GLOBAL VARIABLES
    Writeref

--------------------------------------------------------------------------*/
intn
DFPwriteref(const char *filename, uint16 ref)
{
    /* shut compiler up */
    filename = filename;

    Writeref = ref;
    return (SUCCEED);
}	/* end DFPwriteref() */

/*--------------------------------------------------------------------------
 NAME
    DFPrestart -- restart reading/writing palettes from the start of a file
 USAGE
    intn DFPrestart(void)
 RETURNS
    SUCCEED on success, FAIL on failure.
 DESCRIPTION
    Restart reading/writing palettes to a file.
 GLOBAL VARIABLES
    Lastfile
 COMMENTS, BUGS, ASSUMPTIONS
 EXAMPLES
 REVISION LOG
--------------------------------------------------------------------------*/
intn
DFPrestart(void)
{
    Lastfile[0] = '\0';
    return (SUCCEED);
}	/* end DFPrestart() */

/*--------------------------------------------------------------------------
 NAME
    DFPlastref -- returns last ref # read/written
 USAGE
    uint16 DFPlastref(void)
 RETURNS
    ref # on on success, FAIL on failure.
 DESCRIPTION
    Return the last ref # read/written from a file.
 GLOBAL VARIABLES
    Lastref
 COMMENTS, BUGS, ASSUMPTIONS
 EXAMPLES
 REVISION LOG
--------------------------------------------------------------------------*/
uint16
DFPlastref(void)
{
    return (Lastref);
}	/* end DFPlastref() */

/**************************************************************************/
/*----------------------- Internal routines ------------------------------*/
/**************************************************************************/

/*-----------------------------------------------------------------------------
 * Name:    DFPIopen
 * Purpose: open or reopen a file
 * Inputs:  filename: name of file to open
 *          acc_mode : access mode
 * Returns: file pointer on success, NULL on failure with DFerror set
 * Users:   HDF systems programmers, other DFP routines
 * Invokes: DFopen
 * Remarks: This is a hook for someday providing more efficient ways to
 *          reopen a file, to avoid re-reading all the headers
 *---------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 NAME
    DFPIopen -- open/reopen file for palette interface
 USAGE
    int32 DFPIopen(filename,acc_mode)
        char *filename;         IN: name of HDF file
        intn acc_mode;            IN: type of access to open file with
 RETURNS
    HDF file handle on success, FAIL on failure.
 DESCRIPTION
    Open/reopen a file for the DFP interface to work with.
 GLOBAL VARIABLES
    Refset, Readref, Lastfile
 COMMENTS, BUGS, ASSUMPTIONS
    This is a hook for someday providing more efficient ways to
    reopen a file, to avoid re-reading all the headers
 EXAMPLES
 REVISION LOG
--------------------------------------------------------------------------*/
PRIVATE int32
DFPIopen(const char *filename, intn acc_mode)
{
    CONSTR(FUNC, "DFPIopen");
    int32       file_id;

    /* use reopen if same file as last time - more efficient */
    if (HDstrncmp(Lastfile, filename, DF_MAXFNLEN) || (acc_mode == DFACC_CREATE))
      {
	  /* treat create as different file */
	  if ((file_id = Hopen(filename, acc_mode, 0)) == FAIL)
	      HRETURN_ERROR(DFE_BADOPEN, FAIL);
	  Refset = 0;	/* no ref to get set for this file */
	  Readref = 0;
      }		/* end if */
    else if ((file_id = Hopen(filename, acc_mode, 0)) == FAIL)
	HRETURN_ERROR(DFE_BADOPEN, FAIL);

    /* remember filename, so reopen may be used next time if same file */
    HDstrncpy(Lastfile, filename, DF_MAXFNLEN);

    return (file_id);
}	/* end DFPIopen() */
