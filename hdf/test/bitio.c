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

/*
 FILE
       bitio.c
       Test HDF bit-level I/O routines

 REMARKS

 DESIGN

 BUGS/LIMITATIONS

 EXPORTED ROUTINES

 AUTHOR
       Quincey Koziol

 MODIFICATION HISTORY
   10/19/93 - Started coding.
*/

/* $Id$ */

#ifdef RCSID
static char RcsId[] = "@(#)$Revision$";
#endif

#include "tproto.h"

#define TESTFILE_NAME "tbitio.hdf"
#define DATAFILE_NAME "bitio.dat"

#ifndef RAND_MAX
#define RAND_MAX (UINT_MAX)
#endif

/* define aliases for random number generation */
#define RAND rand
#define SEED(a) srand(a)

#define BUFSIZE     4096
#define DATASIZE    4096

#define DATA_TAG_1      1000
#define DATA_REF_1      1000

#define BITIO_TAG_1     1500
#define BITIO_REF_1     1500
#define BITIO_TAG_2     2500
#define BITIO_REF_2     2500

#ifdef TEST_PC
#define FAR far
#else
#define FAR /* */
#endif

static uint8 FAR outbuf[BUFSIZE],
    FAR inbuf[DATASIZE];

static uint32 FAR outbuf2[BUFSIZE],
    FAR inbuf2[BUFSIZE];

static uint32 FAR maskbuf[]={
    0x00000000,
    0x00000001,0x00000003,0x00000007,0x0000000f,
    0x0000001f,0x0000003f,0x0000007f,0x000000ff,
    0x000001ff,0x000003ff,0x000007ff,0x00000fff,
    0x00001fff,0x00003fff,0x00007fff,0x0000ffff,
    0x0001ffff,0x0003ffff,0x0007ffff,0x000fffff,
    0x001fffff,0x003fffff,0x007fffff,0x00ffffff,
    0x01ffffff,0x03ffffff,0x07ffffff,0x0fffffff,
    0x1fffffff,0x3fffffff,0x7fffffff,0xffffffff};

extern int num_errs;
extern int Verbocity;

void test_bitio_write()
{
    int32 fid;
    int32 bitid1,bitid2;
    int32 ret;
    intn i;

    MESSAGE(6,printf("Testing bitio write routines\n"););
    SEED((int)time(NULL));
    for (i=0; i<BUFSIZE; i++) {
        outbuf[i]=((RAND()>>4)%32)+1;       /* number of bits to output */
        outbuf2[i]=RAND() & maskbuf[outbuf[i]];     /* actual bits to output */
      } /* end for */

    fid=Hopen(TESTFILE_NAME,DFACC_CREATE,0);
    CHECK(fid,FAIL,"Hopen");
    bitid1=Hstartbitwrite(fid,BITIO_TAG_1,BITIO_REF_1,16);
    CHECK(bitid1,FAIL,"Hstartbitwrite");
    ret=Hbitappendable(bitid1);
    RESULT("Hbitappendable");

    for(i=0; i<BUFSIZE; i++) {
        ret=Hbitwrite(bitid1,outbuf[i],(uint32)outbuf2[i]);
        VERIFY(ret,outbuf[i],"Hbitwrite");
      } /* end for */

    ret=Hendbitaccess(bitid1);
    RESULT("Hbitendaccess");

    bitid1=Hstartbitread(fid,BITIO_TAG_1,BITIO_REF_1);
    CHECK(bitid1,FAIL,"Hstartbitread");

    for(i=0; i<BUFSIZE; i++) {
        ret=Hbitread(bitid1,outbuf[i],&inbuf2[i]);
        VERIFY(ret,outbuf[i],"Hbitread");
      } /* end for */
    if(HDmemcmp(outbuf2,inbuf2,sizeof(int32)*BUFSIZE)) {
        printf("Error in writing/reading bit I/O data\n");
        HEprint(stdout,0);
        num_errs++;
      }	/* end for */

    ret=Hendbitaccess(bitid1);
    RESULT("Hbitendaccess");
    ret=Hclose(fid);
    RESULT("Hclose");
}

void test_bitio_read()
{
    int32 fid;
    int32 bitid1;
    int32 ret;
    intn inbits;
    int32 tempbuf;
    intn i;

    SEED((int)time(NULL));

    MESSAGE(6,printf("Testing bitio read routines\n"););

    fid=Hopen(DATAFILE_NAME,DFACC_READ,0);
    CHECK(fid,FAIL,"Hopen");
    ret=Hgetelement(fid,DATA_TAG_1,DATA_REF_1,inbuf);
    RESULT("Hgetelement");
    ret=Hclose(fid);
    RESULT("Hclose");

    MESSAGE(8,printf("Reading 8 bits at a time\n"););
    fid=Hopen(DATAFILE_NAME,DFACC_READ,0);
    CHECK(fid,FAIL,"Hopen");
    bitid1=Hstartbitread(fid,DATA_TAG_1,DATA_REF_1);
    CHECK(bitid1,FAIL,"Hstartbitread");

    for(i=0; i<DATASIZE; i++) {
        ret=Hbitread(bitid1,8,&inbuf2[i]);
        VERIFY(ret,8,"Hbitread");
      } /* end for */
    ret=Hendbitaccess(bitid1);
    RESULT("Hbitendaccess");

    /* check the data */
    for(i=0; i<DATASIZE; i++) {
        if(inbuf[i]!=(uint8)inbuf2[i]) {
    	    printf("Error in reading bit I/O data at position %d\n",i);
    	    num_errs++;
          }	/* end for */
      } /* end for */

    MESSAGE(8,printf("Read random # of bits at a time\n"););
    bitid1=Hstartbitread(fid,DATA_TAG_1,DATA_REF_1);
    CHECK(bitid1,FAIL,"Hstartbitread");

    /* read in random #'s of bits */
    for(i=0; i<DATASIZE/4; i++) {
        inbits=((RAND()>>4)%32)+1;       /* number of bits to input */
        ret=Hbitread(bitid1,inbits,&inbuf2[i]);
        VERIFY(ret,inbits,"Hbitread");
        if(inbits<32) {     /* if we've already grabbed 32-bit don't try for more */
            inbits=32-inbits;
            ret=Hbitread(bitid1,inbits,&tempbuf);
            VERIFY(ret,inbits,"Hbitread");
            inbuf2[i]<<=inbits;
            inbuf2[i]|=tempbuf;
          } /* end if */
      } /* end for */
    ret=Hendbitaccess(bitid1);
    RESULT("Hbitendaccess");

    /* check the data */
    if(HDmemcmp(inbuf,inbuf2,sizeof(uint8)*DATASIZE)) {
        printf("Error in reading bit I/O data\n");
        HEprint(stdout,0);
        num_errs++;
      }	/* end for */

    ret=Hclose(fid);
    RESULT("Hclose");
}

void test_bitio()
{
    test_bitio_read();
    test_bitio_write();
}
