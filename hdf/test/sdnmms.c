#ifdef RCSID
static char RcsId[] = "@(#)$Revision$";
#endif
/*
$Header$

$Log$
Revision 1.2  1993/04/19 23:04:02  koziol
General Code Cleanup to reduce/remove compilation warnings on PC

 * Revision 1.1  1993/04/15  20:00:33  koziol
 * Re-named the new tests for MS-DOS compatibility
 *
 * Revision 1.2  1993/01/27  22:41:31  briand
 * Fixed problem with compiling on RS6000.
 *
 * Revision 1.1  1993/01/27  22:04:35  briand
 * Converted test files to work with master test program: testhdf
 *
 * Revision 1.4  1992/07/08  22:05:20  sxu
 * Changed DFSDgetmaxmin() to DFSDgetrange().
 * Changed DFSDsetmaxmin() to DFSDsetrange().
 *
 * Revision 1.3  1992/05/31  15:27:30  mfolk
 * Changed rank and dims[2] to int32 to satisfy Convex.
 *
 * Revision 1.2  1992/04/28  18:26:38  dilg
 * Changed absolute path to relative path for include files.
 *
 * Revision 1.1  1992/04/27  17:31:08  sxu
 * Initial revision
 *
 * Revision 1.1  1992/02/26  20:08:43  mfolk
 * Initial revision
 *
*/
#include "tproto.h"

extern int Verbocity;
extern int num_errs;

static  float64 f64[10][10], tf64[10][10];
static  float64 f64scale[10], tf64scale[10];
static  float64 f64max = (float64)40.0, f64min = (float64)0.0;
static  float64 tf64max, tf64min;

static  float32 f32[10][10], tf32[10][10];
static  float32 f32scale[10], tf32scale[10];
static  float32 f32max = (float32)40.0, f32min = (float32)0.0;
static  float32 tf32max, tf32min;

static  int8 i8[10][10], ti8[10][10];
static  int8 i8scale[10], ti8scale[10];
static  int8 i8max = 127, i8min = -128;
static  int8 ti8max, ti8min;

static  uint8 ui8[10][10], tui8[10][10];
static  uint8 ui8scale[10], tui8scale[10];
static  uint8 ui8max = 255, ui8min = 0;
static  uint8 tui8max, tui8min;

static  int16 i16[10][10], ti16[10][10];
static  int16 i16scale[10], ti16scale[10];
static  int16 i16max = 1200, i16min = -1200;
static  int16 ti16max, ti16min;

static  uint16 ui16[10][10], tui16[10][10];
static  uint16 ui16scale[10], tui16scale[10];
static  uint16 ui16max = 20000, ui16min = 0;
static  uint16 tui16max, tui16min;

static  int32 i32[10][10], ti32[10][10];
static  int32 i32scale[10], ti32scale[10];
static  int32 i32max = 99999999, i32min = -999999999;
static  int32 ti32max, ti32min;

static  uint32 ui32[10][10], tui32[10][10];
static  uint32 ui32scale[10], tui32scale[10];
static  uint32 ui32max = 999999999, ui32min = 0;
static  uint32 tui32max, tui32min;

void test_sdnmms()
{
    int i, j, err, err1, err2, ret;
    intn rank;
    int32 dims[2];

    rank = 2;
    dims[0] = 10;
    dims[1] = 10;

    MESSAGE(5,printf("Creating arrays...\n"););

    for (i=0; i<10; i++) {
        for (j=0; j<10; j++) {

	    f64[i][j] = (i * 40) + j;	/* range: 0 ~ 4-billion */

	    f32[i][j] = (i * 40) + j;	/* range: 0 ~ 4-billion */

        i8[i][j] = (int8)((i * 10) + j);    /* range: 0 ~ 100 */
        ui8[i][j] = (uint8)((i * 20) + j);  /* range: 0 ~ 200 */

        i16[i][j] = (int16)((i * 3000) + j);    /* range: 0 ~ 30000 */
        ui16[i][j] = (uint16)((i * 6000) + j);  /* range: 0 ~ 60000 */

        i32[i][j] = (int32)((i * 20) + j);      /* range: 0 ~ 2-billion */
        ui32[i][j] = (uint32)((i * 40) + j);    /* range: 0 ~ 4-billion */
	}

    f64scale[i] = (i * 40) + j;             /* range: 0 ~ 4-billion */

    f32scale[i] = (float32)((i * 40) + j);  /* range: 0 ~ 4-billion */

    i8scale[i] = (int8)((i * 10) + j);      /* range: 0 ~ 100 */
    ui8scale[i] = (uint8)((i * 20) + j);    /* range: 0 ~ 200 */

    i16scale[i] = (int16)((i * 3000) + j);  /* range: 0 ~ 30000 */
    ui16scale[i] = (uint16)((i * 6000) + j);/* range: 0 ~ 60000 */

    i32scale[i] = (int32)((i * 20) + j);    /* range: 0 ~ 2-billion */
    ui32scale[i] = (uint32)((i * 40) + j);  /* range: 0 ~ 4-billion */
    }

    ret = DFSDsetdims(rank, dims);
    RESULT("DFSDsetdims");

    MESSAGE(5,printf("Writing arrays to single file...\n"););

    ret =DFSDsetNT(DFNT_NFLOAT64);
    RESULT("DFSDsetNT");
    ret=DFSDsetdimscale(1, (int32)10, (VOIDP)f64scale);
    RESULT("DFSDsetdimscale");
    ret=DFSDsetrange(&f64max, &f64min);
    RESULT("DFSDsetrange");
    ret = DFSDadddata("nntcheck.hdf", rank, dims, f64);
    RESULT("DFSDadddata");

    ret=DFSDsetNT(DFNT_NFLOAT32);
    RESULT("DFSDsetNT");
    ret=DFSDsetdimscale(1, (int32)10, (VOIDP)f32scale);
    RESULT("DFSDsetdimscale");
    ret=DFSDsetrange(&f32max, &f32min);
    RESULT("DFSDsetrange");
    ret = DFSDadddata("nntcheck.hdf", rank, dims, f32);
    RESULT("DFSDadddata");

    ret=DFSDsetNT(DFNT_NINT8);
    RESULT("DFSDsetNT");
    ret=DFSDsetdimscale(1, (int32)10, (VOIDP)i8scale);
    RESULT("DFSDsetdimscale");
    ret=DFSDsetrange(&i8max, &i8min);
    RESULT("DFSDsetrange");
    ret = DFSDadddata("nntcheck.hdf", rank, dims, i8);
    RESULT("DFSDadddata");

    ret=DFSDsetNT(DFNT_NUINT8);
    RESULT("DFSDsetNT");
    ret=DFSDsetdimscale(1, (int32)10, (VOIDP)ui8scale);
    RESULT("DFSDsetdimscale");
    ret=DFSDsetrange(&ui8max, &ui8min);
    RESULT("DFSDsetrange");
    ret = DFSDadddata("nntcheck.hdf", rank, dims, ui8);
    RESULT("DFSDadddata");

    ret=DFSDsetNT(DFNT_NINT16);
    RESULT("DFSDsetNT");
    ret=DFSDsetdimscale(1, (int32)10, (VOIDP)i16scale);
    RESULT("DFSDsetdimscale");
    ret=DFSDsetrange(&i16max, &i16min);
    RESULT("DFSDsetrange");
    ret= DFSDadddata("nntcheck.hdf", rank, dims, i16);
    RESULT("DFSDadddata");

    ret=DFSDsetNT(DFNT_NUINT16);
    RESULT("DFSDsetNT");
    ret=DFSDsetdimscale(1, (int32)10, (VOIDP)ui16scale);
    RESULT("DFSDsetdimscale");
    ret=DFSDsetrange(&ui16max, &ui16min);
    RESULT("DFSDsetrange");
    ret= DFSDadddata("nntcheck.hdf", rank, dims, ui16);
    RESULT("DFSDadddata");

    ret=DFSDsetNT(DFNT_NINT32);
    RESULT("DFSDsetNT");
    ret=DFSDsetdimscale(1, (int32)10, (VOIDP)i32scale);
    RESULT("DFSDsetdimscale");
    ret=DFSDsetrange(&i32max, &i32min);
    RESULT("DFSDsetrange");
    ret= DFSDadddata("nntcheck.hdf", rank, dims, i32);
    RESULT("DFSDadddata");

    ret=DFSDsetNT(DFNT_NUINT32);
    RESULT("DFSDsetNT");
    ret= DFSDsetdimscale(1, (int32)10, (VOIDP)ui32scale);
    RESULT("DFSDsetdimscale");
    ret=DFSDsetrange(&ui32max, &ui32min);
    RESULT("DFSDsetrange");
    ret= DFSDadddata("nntcheck.hdf", rank, dims, ui32);
    RESULT("DFSDadddata");

    MESSAGE(5,printf("Reading arrays from single file...\n"););

    ret= DFSDgetdata("nntcheck.hdf", rank, dims, tf64);
    RESULT("DFSDgetdata");
    ret= DFSDgetdimscale(1, (int32)10, (VOIDP)tf64scale);
    RESULT("DFSDgetdimscale");
    ret= DFSDgetrange(&tf64max, &tf64min);
    RESULT("DFSDgetrange");

    ret= DFSDgetdata("nntcheck.hdf", rank, dims, tf32);
    RESULT("DFSDgetdata");
    ret= DFSDgetdimscale(1, (int32)10, (VOIDP)tf32scale);
    RESULT("DFSDgetdimscale");
    ret= DFSDgetrange(&tf32max, &tf32min);
    RESULT("DFSDgetrange");

    ret= DFSDgetdata("nntcheck.hdf", rank, dims, ti8);
    RESULT("DFSDgetdata");
    ret= DFSDgetdimscale(1, (int32)10, (VOIDP)ti8scale);
    RESULT("DFSDgetdimscale");
    ret= DFSDgetrange(&ti8max, &ti8min);
    RESULT("DFSDgetrange");

    ret= DFSDgetdata("nntcheck.hdf", rank, dims, tui8);
    RESULT("DFSDgetdata");
    ret= DFSDgetdimscale(1, (int32)10, (VOIDP)tui8scale);
    RESULT("DFSDgetdimscale");
    ret= DFSDgetrange(&tui8max, &tui8min);
    RESULT("DFSDgetrange");

    ret= DFSDgetdata("nntcheck.hdf", rank, dims, ti16);
    RESULT("DFSDgetdata");
    ret= DFSDgetdimscale(1, (int32)10, (VOIDP)ti16scale);
    RESULT("DFSDgetdimscale");
    ret= DFSDgetrange(&ti16max, &ti16min);
    RESULT("DFSDgetrange");

    ret= DFSDgetdata("nntcheck.hdf", rank, dims, tui16);
    RESULT("DFSDgetdata");
    ret= DFSDgetdimscale(1, (int32)10, (VOIDP)tui16scale);
    RESULT("DFSDgetdimscale");
    ret= DFSDgetrange(&tui16max, &tui16min);
    RESULT("DFSDgetrange");

    ret= DFSDgetdata("nntcheck.hdf", rank, dims, ti32);
    RESULT("DFSDgetdata");
    ret= DFSDgetdimscale(1, (int32)10, (VOIDP)ti32scale);
    RESULT("DFSDgetdimscale");
    ret= DFSDgetrange(&ti32max, &ti32min);
    RESULT("DFSDgetrange");

    ret= DFSDgetdata("nntcheck.hdf", rank, dims, tui32);
    RESULT("DFSDgetdata");
    ret= DFSDgetdimscale(1, (int32)10, (VOIDP)tui32scale);
    RESULT("DFSDgetdimscale");
    ret= DFSDgetrange(&tui32max, &tui32min);
    RESULT("DFSDgetrange");

    MESSAGE(5,printf("Checking arrays from single file...\n\n"););

    err = 0;
    err1 = 0;
    err2 = 0;
    for (i=0; i<10; i++) {
	for (j=0; j<10; j++)
	   if (f64[i][j] != tf64[i][j])
	       err = 1;
	if (f64scale[i] != tf64scale[i])
	    err2 = 1;
    }
    if ((f64max != tf64max) || (f64min != tf64min)) 
        err1 = 1;

    num_errs += err + err1 + err2;
    MESSAGE(5,if (err == 1) 
	printf(">>> Test failed for float64 array.\n");
    else 
	printf("Test passed for float64 array.\n"););
    MESSAGE(5,if (err2 == 1) 
	printf(">>> Test failed for float64 scales.\n");
    else 
	printf("Test passed for float64 scales.\n"););
    MESSAGE(5,if (err1 == 1)
	printf(">>> Test failed for float64 max/min.\n");
    else 
	printf("Test passed for float64 max/min.\n"););

    err = 0;
    err1 = 0;
    err2 = 0;
    for (i=0; i<10; i++) {
	for (j=0; j<10; j++)
	   if (f32[i][j] != tf32[i][j])
	       err = 1;
	if (f32scale[i] != tf32scale[i])
	    err2 = 1;
    }
    if ((f32max != tf32max) || (f32min != tf32min)) 
        err1 = 1;

    num_errs += err + err1 + err2;
    MESSAGE(5,if (err == 1) 
	printf(">>> Test failed for float32 array.\n");
    else 
	printf("Test passed for float32 array.\n"););
    MESSAGE(5,if (err2 == 1) 
	printf(">>> Test failed for float32 scales.\n");
    else 
	printf("Test passed for float32 scales.\n"););
    MESSAGE(5,if (err1 == 1)
	printf(">>> Test failed for float32 max/min.\n");
    else 
	printf("Test passed for float32 max/min.\n"););

    err = 0;
    err1 = 0;
    err2 = 0;
    for (i=0; i<10; i++) {
	for (j=0; j<10; j++)
	   if (i8[i][j] != ti8[i][j])
	       err = 1;
	if (i8scale[i] != ti8scale[i])
	    err2 = 1;
    }
    if ((i8max != ti8max) || (i8min != ti8min)) 
        err1 = 1;

    num_errs += err + err1 + err2;
    MESSAGE(5,if (err == 1)
	printf(">>> Test failed for int8 array.\n");
    else
	printf("Test passed for int8 array.\n"););
    MESSAGE(5,if (err2 == 1)
	printf(">>> Test failed for int8 scales.\n");
    else
	printf("Test passed for int8 scales.\n"););
    MESSAGE(5,if (err1 == 1)
	printf(">>> Test failed for int8 max/min.\n");
    else
	printf("Test passed for int8 max/min.\n"););

    err = 0;
    err1 = 0;
    err2 = 0;
    for (i=0; i<10; i++) {
	for (j=0; j<10; j++)
	   if (ui8[i][j] != tui8[i][j])
	       err = 1;
	if (ui8scale[i] != tui8scale[i])
	    err2 = 1;
    }
    if ((ui8max != tui8max) || (ui8min != tui8min))
        err1 = 1;

    num_errs += err + err1 + err2;
    MESSAGE(5,if (err == 1)
	printf(">>> Test failed for uint8 array.\n");
    else
	printf("Test passed for uint8 array.\n"););
    MESSAGE(5,if (err2 == 1)
	printf(">>> Test failed for uint8 scales.\n");
    else
	printf("Test passed for uint8 scales.\n"););
    MESSAGE(5,if  (err1 == 1)
	printf(">>> Test failed for uint8 max/min.\n");
    else
	printf("Test passed for uint8 max/min.\n"););

    err = 0;
    err1 = 0;
    err2 = 0;
    for (i=0; i<10; i++) {
	for (j=0; j<10; j++)
	   if (i16[i][j] != ti16[i][j])
	       err = 1;
	if (i16scale[i] != ti16scale[i])
	    err2 = 1;
    }
    if ((i16max != ti16max) || (i16min != ti16min))
        err1 = 1;

    num_errs += err + err1 + err2;
    MESSAGE(5,if (err == 1)
	printf(">>> Test failed for int16 array.\n");
    else
	printf("Test passed for int16 array.\n"););
    MESSAGE(5,if (err2 == 1)
	printf(">>> Test failed for int16 scales.\n");
    else
	printf("Test passed for int16 scales.\n"););
    MESSAGE(5,if (err1 == 1)
	printf(">>> Test failed for int16 max/min.\n");
    else
	printf("Test passed for int16 max/min.\n"););

    err = 0;
    err1 = 0;
    err2 = 0;
    for (i=0; i<10; i++) {
	for (j=0; j<10; j++)
	   if (ui16[i][j] != tui16[i][j])
	       err = 1;
	if (ui16scale[i] != tui16scale[i])
	    err2 = 1;
    }
    if ((ui16max != tui16max) || (ui16min != tui16min))
        err1 = 1;

    num_errs += err + err1 + err2;
    MESSAGE(5,if (err == 1)
	printf(">>> Test failed for uint16 array.\n");
    else
	printf("Test passed for uint16 array.\n"););
    MESSAGE(5,if (err2 == 1)
	printf(">>> Test failed for uint16 scales.\n");
    else
	printf("Test passed for uint16 scales.\n"););
    MESSAGE(5,if (err1 == 1)
	printf(">>> Test failed for uint16 max/min.\n");
    else
	printf("Test passed for uint16 max/min.\n"););

    err = 0;
    err1 = 0;
    err2 = 0;
    for (i=0; i<10; i++) {
	for (j=0; j<10; j++)
	   if (i32[i][j] != ti32[i][j])
	       err = 1;
	if (i32scale[i] != ti32scale[i])
	    err2 = 1;
    }
    if ((i32max != ti32max) || (i32min != ti32min))
        err1 = 1;

    num_errs += err + err1 + err2;
    MESSAGE(5,if (err == 1)
	printf(">>> Test failed for int32 array.\n");
    else
	printf("Test passed for int32 array.\n"););
    MESSAGE(5,if (err2 == 1)
	printf(">>> Test failed for int32 scales.\n");
    else
	printf("Test passed for int32 scales.\n"););
    MESSAGE(5,if (err1 == 1)
	printf(">>> Test failed for int32 max/min.\n");
    else
	printf("Test passed for int32 max/min.\n"););

    err = 0;
    err1 = 0;
    err2 = 0;
    for (i=0; i<10; i++) {
	for (j=0; j<10; j++)
	   if (ui32[i][j] != tui32[i][j])
	       err = 1;
	if (ui32scale[i] != tui32scale[i])
	    err2 = 1;
    }
    if ((ui32max != tui32max) || (ui32min != tui32min))
        err1 = 1;

    num_errs += err + err1 + err2;
    MESSAGE(5,if (err == 1)
	printf(">>> Test failed for uint32 array.\n");
    else
	printf("Test passed for uint32 array.\n"););
    MESSAGE(5,if (err2 == 1)
	printf(">>> Test failed for uint32 scales.\n");
    else
	printf("Test passed for uint32 scales.\n"););
    MESSAGE(5,if (err1 == 1)
	printf(">>> Test failed for uint32 max/min.\n");
    else
	printf("Test passed for uint32 max/min.\n"););

}
