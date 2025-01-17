/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright by The HDF Group.                                               *
 * Copyright by the Board of Trustees of the University of Illinois.         *
 * All rights reserved.                                                      *
 *                                                                           *
 * This file is part of HDF.  The full HDF copyright notice, including       *
 * terms governing use, modification, and redistribution, is contained in    *
 * the COPYING file, which can be found at the root of the source code       *
 * distribution tree, or in https://support.hdfgroup.org/ftp/HDF/releases/.  *
 * If you do not have access to either file, you may request a copy from     *
 * help@hdfgroup.org.                                                        *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef MFH4_MFHDF_H
#define MFH4_MFHDF_H

/* Formerly used to divide up the HDF and netCDF code. The symbol is retained
 * since it's technically in the public API.
 */
#define HDF 1

#include "hdf.h"

#define SD_UNLIMITED        0L    /* Same as NC_UNLIMITED, use this as marker for unlimited dimension */
#define SD_NOFILL           0x100 /* Same as NC_NOFILL */
#define SD_FILL             0     /* Same as NC_FILL */
#define SD_DIMVAL_BW_COMP   1
#define SD_DIMVAL_BW_INCOMP 0
#define SD_RAGGED           -1 /* Marker for ragged dimension */

/* Fill values
 *
 * These values are stuffed into newly allocated space as appropriate.
 * The hope is that one might use these to notice that a particular datum
 * has not been set.
 */

#define FILL_BYTE  ((char)-127) /* Largest Negative value */
#define FILL_CHAR  ((char)0)
#define FILL_SHORT ((short)-32767)
#define FILL_LONG  ((long)-2147483647)

#define FILL_FLOAT  9.9692099683868690e+36F /* near 15 * 2^119 */
#define FILL_DOUBLE 9.9692099683868690e+36

/* enumerated type used to specify whether a variable is an SDS, coordinate
 * variable, or its type is unknown because it was created before HDF4.2r2
 *
 * NOTE: Formerly in mfhdfi.h
 */
typedef enum {
    IS_SDSVAR = 0, /* variable is an actual SDS */
    IS_CRDVAR = 1, /* variable is a coordinate variable */
    UNKNOWN   = 2  /* variable is created before HDF4.2r2, unknown type */
} hdf_vartype_t;

/* used to indicate the type of the variable at an index */
typedef struct hdf_varlist {
    int32         var_index; /* index of the current variable */
    hdf_vartype_t var_type;  /* type of a variable (IS_SDSVAR, IS_CRDVAR, or UNKNOWN */
} hdf_varlist_t;

/* enumerated types for various types of ids in SD interface */
typedef enum {
    NOT_SDAPI_ID = -1, /* not an SD API id */
    SD_ID        = 0,  /* SD id */
    SDS_ID,            /* SDS id */
    DIM_ID             /* Dimension id */
} hdf_idtype_t;

#ifdef __cplusplus
extern "C" {
#endif

HDFLIBAPI int32 SDstart(const char *name, int32 accs);

HDFLIBAPI int SDend(int32 fid);

HDFLIBAPI int SDfileinfo(int32 fid, int32 *datasets, int32 *attrs);

HDFLIBAPI int32 SDselect(int32 fid, int32 idx);

HDFLIBAPI int SDgetinfo(int32 sdsid, char *name, int32 *rank, int32 *dimsizes, int32 *nt, int32 *nattr);

HDFLIBAPI int SDreaddata(int32 sdsid, int32 *start, int32 *stride, int32 *end, void *data);

HDFLIBAPI uint16 SDgerefnumber(int32 sdsid);

HDFLIBAPI int32 SDnametoindex(int32 fid, const char *name);

HDFLIBAPI int SDnametoindices(int32 fid, const char *name, hdf_varlist_t *var_list);

HDFLIBAPI int SDgetnumvars_byname(int32 fid, const char *name, int32 *n_vars);

HDFLIBAPI int SDgetrange(int32 sdsid, void *pmax, void *pmin);

HDFLIBAPI int32 SDcreate(int32 fid, const char *name, int32 nt, int32 rank, int32 *dimsizes);

HDFLIBAPI int32 SDgetdimid(int32 sdsid, int number);

HDFLIBAPI int SDsetdimname(int32 id, const char *name);

HDFLIBAPI int SDendaccess(int32 id);

HDFLIBAPI int SDsetrange(int32 sdsid, void *pmax, void *pmin);

HDFLIBAPI int SDsetattr(int32 id, const char *name, int32 nt, int32 count, const void *data);

HDFLIBAPI int SDattrinfo(int32 id, int32 idx, char *name, int32 *nt, int32 *count);

HDFLIBAPI int SDreadattr(int32 id, int32 idx, void *buf);

HDFLIBAPI int SDwritedata(int32 sdsid, int32 *start, int32 *stride, int32 *end, void *data);

HDFLIBAPI int SDsetdatastrs(int32 sdsid, const char *l, const char *u, const char *f, const char *c);

HDFLIBAPI int SDsetcal(int32 sdsid, float64 cal, float64 cale, float64 ioff, float64 ioffe, int32 nt);

HDFLIBAPI int SDsetfillvalue(int32 sdsid, void *val);

HDFLIBAPI int SDgetfillvalue(int32 sdsid, void *val);

HDFLIBAPI int SDsetfillmode(int32 id, int fillmode);

HDFLIBAPI int SDgetdatastrs(int32 sdsid, char *l, char *u, char *f, char *c, int len);

HDFLIBAPI int SDgetcal(int32 sdsid, float64 *cal, float64 *cale, float64 *ioff, float64 *ioffe, int32 *nt);

HDFLIBAPI int SDsetdimstrs(int32 id, const char *l, const char *u, const char *f);

HDFLIBAPI int SDsetdimscale(int32 id, int32 count, int32 nt, void *data);

HDFLIBAPI int SDgetdimscale(int32 id, void *data);

HDFLIBAPI int SDdiminfo(int32 id, char *name, int32 *size, int32 *nt, int32 *nattr);

HDFLIBAPI int SDgetdimstrs(int32 id, char *l, char *u, char *f, int len);

HDFLIBAPI int SDgetexternalfile(int32 id, int buf_size, char *ext_filename, int32 *offset);

HDFLIBAPI int SDgetexternalinfo(int32 id, unsigned buf_size, char *ext_filename, int32 *offset,
                                int32 *length);

HDFLIBAPI int SDsetexternalfile(int32 id, const char *filename, int32 offset);

HDFLIBAPI int SDsetnbitdataset(int32 id, int start_bit, int bit_len, int sign_ext, int fill_one);

HDFLIBAPI int SDsetcompress(int32 id, comp_coder_t type, comp_info *c_info);

#ifndef H4_NO_DEPRECATED_SYMBOLS
HDFLIBAPI int SDgetcompress(int32 id, comp_coder_t *type, comp_info *c_info);
#endif

HDFLIBAPI int SDgetcompinfo(int32 id, comp_coder_t *type, comp_info *c_info);

HDFLIBAPI int SDgetcomptype(int32 id, comp_coder_t *type);

HDFLIBAPI int32 SDfindattr(int32 id, const char *attrname);

HDFLIBAPI int32 SDidtoref(int32 id);

HDFLIBAPI int32 SDreftoindex(int32 fid, int32 ref);

HDFLIBAPI int32 SDisrecord(int32 id);

HDFLIBAPI int SDiscoordvar(int32 id);

HDFLIBAPI int SDsetaccesstype(int32 id, unsigned accesstype);

HDFLIBAPI int SDsetblocksize(int32 sdsid, int32 block_size);

HDFLIBAPI int SDgetblocksize(int32 sdsid, int32 *block_size);

HDFLIBAPI int SDsetdimval_comp(int32 dimid, int compt_mode);

HDFLIBAPI int SDisdimval_bwcomp(int32 dimid);

HDFLIBAPI int32 SDcheckempty(int32 sdsid, int *emptySDS);

HDFLIBAPI hdf_idtype_t SDidtype(int32 an_id);

HDFLIBAPI int SDreset_maxopenfiles(int req_max);

HDFLIBAPI int SDget_maxopenfiles(int *curr_max, int *sys_limit);

HDFLIBAPI int SDget_numopenfiles(void);

HDFLIBAPI int SDgetdatasize(int32 sdsid, int32 *comp_size, int32 *uncomp_size);

HDFLIBAPI int SDgetfilename(int32 fid, char *filename);

HDFLIBAPI int SDgetnamelen(int32 sdsid, uint16 *name_len);

/*====================== Chunking Routines ================================*/

/* For definition of HDF_CHUNK_DEF union see hproto.h since
   this definition is also used by GRs. */

/******************************************************************************
 NAME
      SDsetchunk   -- make SDS a chunked SDS

 DESCRIPTION
      This routine makes the SDS a chunked SDS according to the chunk
      definition passed in.

      The dataset currently cannot be special already.  i.e. NBIT,
      COMPRESSED, or EXTERNAL. This is an Error.

      The definition of the HDF_CHUNK_DEF union with relevant fields is:

      typedef union hdf_chunk_def_u
      {
         int32   chunk_lengths[H4_MAX_VAR_DIMS];  Chunk lengths along each dimension

         struct
          {
            int32     chunk_lengths[H4_MAX_VAR_DIMS]; Chunk lengths along each dimension
            int32     comp_type;                   Compression type
            comp_info cinfo;                       Compression info struct
          }comp;

      } HDF_CHUNK_DEF

      The simplest is the 'chunk_lengths' array specifying chunk
      lengths for each dimension where the 'flags' argument set to
      'HDF_CHUNK';

      COMPRESSION is set by using the 'HDF_CHUNK_DEF' structure to set the
      appropriate compression information along with the required chunk lengths
      for each dimension. The compression information is the same as
      that set in 'SDsetcompress()'. The bit-or'd'flags' argument' is set to
      'HDF_CHUNK | HDF_COMP'.

      See the example in pseudo-C below for further usage.

      The maximum number of Chunks in an HDF file is 65,535.

      The dataset currently cannot have an UNLIMITED dimension.

      The performance of the SDxxx interface with chunking is greatly
      affected by the users access pattern over the dataset and by
      the maximum number of chunks set in the chunk cache. The cache contains
      the Least Recently Used(LRU cache replacement policy) chunks. See the
      routine SDsetchunkcache() for further info on the chunk cache and how
      to set the maximum number of chunks in the chunk cache. A default chunk
      cache is always created.

      The following example shows the organization of chunks for a 2D array.
      e.g. 4x4 array with 2x2 chunks. The array shows the layout of
           chunks in the chunk array.

            4 ---------------------
              |         |         |
        Y     |  (0,1)  |  (1,1)  |
        ^     |         |         |
        |   2 ---------------------
        |     |         |         |
        |     |  (0,0)  |  (1,0)  |
        |     |         |         |
        |     ---------------------
        |     0         2         4
        ---------------> X

        --Without compression--:
        {
        HDF_CHUNK_DEF chunk_def;

        .......
        -- Set chunk lengths --
        chunk_def.chunk_lengths[0]= 2;
        chunk_def.chunk_lengths[1]= 2;

        -- Set Chunking --
        SDsetchunk(sdsid, chunk_def, HDF_CHUNK);
         ......
        }

        --With compression--:
        {
        HDF_CHUNK_DEF chunk_def;

        .......
        -- Set chunk lengths first --
        chunk_def.chunk_lengths[0]= 2;
        chunk_def.chunk_lengths[1]= 2;

        -- Set compression --
        chunk_def.comp.cinfo.deflate.level = 9;
        chunk_def.comp.comp_type = COMP_CODE_DEFLATE;

        -- Set Chunking with Compression --
        SDsetchunk(sdsid, chunk_def, HDF_CHUNK | HDF_COMP);
         ......
        }

 RETURNS
        SUCCEED/FAIL
******************************************************************************/
HDFLIBAPI int SDsetchunk(int32         sdsid,     /* IN: sds access id */
                         HDF_CHUNK_DEF chunk_def, /* IN: chunk definition */
                         int32         flags /* IN: flags */);

/******************************************************************************
 NAME
     SDgetchunkinfo -- get Info on SDS

 DESCRIPTION
     This routine gets any special information on the SDS. If its chunked,
     chunked and compressed or just a regular SDS. Currently it will only
     fill the array of chunk lengths for each dimension as specified in
     the 'HDF_CHUNK_DEF' union. It does not tell you the type of compression
     used or the compression parameters. You can pass in a NULL for 'chunk_def'
     if don't want the chunk lengths for each dimension.
     Additionally if successful it will return a bit-or'd value in 'flags'
     indicating if the SDS is:

     Chunked                  -> flags = HDF_CHUNK
     Chunked and compressed   -> flags = HDF_CHUNK | HDF_COMP
     Non-chunked              -> flags = HDF_NONE

     e.g. 4x4 array - Pseudo-C
     {
     int32   rcdims[3];
     HDF_CHUNK_DEF rchunk_def;
     int32   cflags;
     ...
     rchunk_def.chunk_lengths = rcdims;
     SDgetchunkinfo(sdsid, &rchunk_def, &cflags);
     ...
     }

 RETURNS
        SUCCEED/FAIL
******************************************************************************/
HDFLIBAPI int SDgetchunkinfo(int32          sdsid,     /* IN: sds access id */
                             HDF_CHUNK_DEF *chunk_def, /* IN/OUT: chunk definition */
                             int32         *flags /* IN/OUT: flags */);

/******************************************************************************
 NAME
     SDwritechunk  -- write the specified chunk to the SDS

 DESCRIPTION
     This routine writes a whole chunk of data to the chunked SDS
     specified by chunk 'origin' for the given SDS and can be used
     instead of SDwritedata() when this information is known. This
     routine has less overhead and is much faster than using SDwritedata().

     Origin specifies the coordinates of the chunk according to the chunk
     position in the overall chunk array.

     'datap' must point to a whole chunk of data.

     See SDsetchunk() for a description of the organization of chunks in an SDS.

 RETURNS
        SUCCEED/FAIL
******************************************************************************/
HDFLIBAPI int SDwritechunk(int32       sdsid,  /* IN: sds access id */
                           int32      *origin, /* IN: origin of chunk to write */
                           const void *datap /* IN: buffer for data */);

/******************************************************************************
 NAME
     SDreadchunk   -- read the specified chunk to the SDS

 DESCRIPTION
     This routine reads a whole chunk of data from the chunked SDS
     specified by chunk 'origin' for the given SDS and can be used
     instead of SDreaddata() when this information is known. This
     routine has less overhead and is much faster than using SDreaddata().

     Origin specifies the coordinates of the chunk according to the chunk
     position in the overall chunk array.

     'datap' must point to a whole chunk of data.

     See SDsetchunk() for a description of the organization of chunks in an SDS.

 RETURNS
        SUCCEED/FAIL
******************************************************************************/
HDFLIBAPI int SDreadchunk(int32  sdsid,  /* IN: sds access id */
                          int32 *origin, /* IN: origin of chunk to read */
                          void  *datap /* IN/OUT: buffer for data */);

/******************************************************************************
NAME
     SDsetchunkcache -- maximum number of chunks to cache

DESCRIPTION
     Set the maximum number of chunks to cache.

     The cache contains the Least Recently Used(LRU cache replacement policy)
     chunks. This routine allows the setting of maximum number of chunks that
     can be cached, 'maxcache'.

     The performance of the SDxxx interface with chunking is greatly
     affected by the users access pattern over the dataset and by
     the maximum number of chunks set in the chunk cache. The number chunks
     that can be set in the cache is process memory limited. It is a good
     idea to always set the maximum number of chunks in the cache as the
     default heuristic does not take into account the memory available for
     the application.

     By default when the SDS is promoted to a chunked element the
     maximum number of chunks in the cache 'maxcache' is set to the number of
     chunks along the last dimension.

     The values set here affects the current object's caching behaviour.

     If the chunk cache is full and 'maxcache' is greater then the
     current 'maxcache' value, then the chunk cache is reset to the new
     'maxcache' value, else the chunk cache remains at the current
     'maxcache' value.

     If the chunk cache is not full, then the chunk cache is set to the
     new 'maxcache' value only if the new 'maxcache' value is greater than the
     current number of chunks in the cache.

     Use flags argument of 'HDF_CACHEALL' if the whole object is to be cached
     in memory, otherwise pass in zero(0). Currently you can only
     pass in zero.

    See SDsetchunk() for a description of the organization of chunks in an SDS.

RETURNS
     Returns the 'maxcache' value for the chunk cache if successful
     and FAIL otherwise
******************************************************************************/
HDFLIBAPI int SDsetchunkcache(int32 sdsid,    /* IN: sds access id */
                              int32 maxcache, /* IN: max number of chunks to cache */
                              int32 flags /* IN: flags = 0, HDF_CACHEALL */);

/*
 ** Public functions for getting raw data information - from mfdatainfo.c
 */

HDFLIBAPI int SDgetdatainfo(int32 sdsid, int32 *chk_coord, unsigned start_block, unsigned info_count,
                            int32 *offsetarray, int32 *lengtharray);

HDFLIBAPI int SDgetattdatainfo(int32 id, int32 attrindex, int32 *offset, int32 *length);

HDFLIBAPI int SDgetoldattdatainfo(int32 dimid, int32 sdsid, char *attr_name, int32 *offset, int32 *length);

HDFLIBAPI int SDgetanndatainfo(int32 sdsid, ann_type annot_type, unsigned size, int32 *offsetarray,
                               int32 *lengtharray);

#ifdef __cplusplus
}
#endif

#endif /* MFH4_MFHDF_H */
