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

#include "hdf.h"
#include "hfile.h"

#ifdef MAC
/* this isn't worth putting in hdfi.h */
/* otherwise, need to include stdlib.h, which */
/* in turn requires other includes - very messy, trust me */ 
void qsort(void *base, size_t nmemb, size_t size, 
        int (*compar) (const void*, const void *));
#endif

#ifdef PC
#define MAXBUFF 2048
#else
#define MAXBUFF 8192
#endif

dd_t desc[MAXBUFF];

intn
    debug    = FALSE,        /* Debugging is off by default */
    sort     = TRUE,         /* Sorting is on by default */
    longout  = FALSE,        /* short output by default */
    labels   = FALSE,        /* no label info by default */
    special  = FALSE;        /* no special info by default */
uint16  only_tag = DFTAG_NULL;  /* by default print info about all tags */

char * file_name;    /* name of current file being listed */

int compare
  (const VOID *, const VOID *);
int main
  (int, char **);
int lprint
  (int32, dd_t *, int);

int compare(const VOID * aa, const VOID * bb)
{
    const dd_t *a = (const dd_t *) aa;
    const dd_t *b = (const dd_t *) bb;
    
    if(a->tag > b->tag) return(1);
    if(a->tag < b->tag) return(-1);
    if(a->ref > b->ref) return(1);
    if(a->ref < b->ref) return(-1);
    return(0);
}

int main(int argc, char *argv[])
{
    int32 fid, aid;
    int i=1, j, n, status;
    
    while((i < argc) && (argv[i][0]=='-')){
        switch(argv[i][1]) {
        case 'o':                 /* give non ordered output */
            sort = FALSE;
            break;
        case 'd':                 /* go into debugging mode */
            debug = TRUE;
            break;
        case 'v':                 /* print labels for elements */
            labels = TRUE;
            longout = TRUE;
            break;
        case 'l':                 /* give long output */
            longout = TRUE;
            break;
        case 's':                 /* give info on special elements */
            special = TRUE;
            longout = TRUE;
            break;
        case 't' :
            if(argv[i][2] != '\0') 
                only_tag = (uint16)atoi(&(argv[i][2]));
            else 
                only_tag = (uint16)atoi(&(argv[++i][0]));
            break;
        default:    
            printf("Unknown option : -%c\n", argv[1][1]);
            break;
        }
        i++;
    }
    
    /*
     * If a file name has not been supplied print the usage message
     */
    if(i == argc) {
        printf("%s,  version: 1.2   date: June 8, 1992\n",argv[0]);
        printf("%s [-o] [-l] [-d] [-v] [-t #] fn ....\n", argv[0]);
        printf("        This program displays information about the");
        printf(" data elements in\n");
        printf("        HDF file.\n");
        printf("    -d: offset & length info of each element in the file\n");
        printf("    -o: Ordered - display in reference number order\n");
        printf("    -l: Long format - display more information\n");
        printf("    -v: Verbose format - display text of annotations and labels.\n");
        printf("        (Verbose format automatically puts you in Long format).\n");
        printf("    -t #: List only information about a specific type of tag.\n");
        printf("          For example '%s -t 700 foo.hdf' \n", argv[0]);
        printf("          will list information only about Scientific Data\n");
        printf("          Groups.\n");
        printf("    -s: Give detailed descriptions of \"special elements\"\n");
        exit (1);
    }
    
    while(i < argc) {
        file_name = argv[i];
        fid = Hopen(file_name, DFACC_READ, -1);
        printf( "%s:  ", argv[i]);
        if(fid == FAIL) {
            if(HEvalue(1) == DFE_NOTDFFILE) {
                printf("\n\tNot an HDF file.\n");
                i++;
                continue;
            } else {
                printf("\n");
                fflush(stdout);
                HEprint(stderr, 0);
            }
        }
        
	aid = Hstartread(fid, DFTAG_WILDCARD, DFREF_WILDCARD);
	if(aid == FAIL) {
            HEprint(stderr, 0);
            i++;
            continue;	  
	}
        
	status = SUCCEED;
	for(n = 0; (n < MAXBUFF) && (status != FAIL); n++) {
            Hinquire(aid, NULL, &desc[n].tag, &desc[n].ref, &desc[n].length,
                     &desc[n].offset, NULL, NULL, NULL);
            status = Hnextread(aid, DFTAG_WILDCARD, DFREF_WILDCARD, DF_CURRENT);
	}
        
	if(n == MAXBUFF) {
            fprintf(stderr, 
                    "Warning:  File may have more DD's than hdfls can display\n");
	}
        
	if(debug) {
            printf("\n");
            for (j=0; j<n; j++) {
                printf("%6d) tag %6d ref %6d ", j, desc[j].tag, desc[j].ref);
                printf(" offset %10ld length %10ld\n", desc[j].offset, desc[j].length);
            }
	}
	
	if (sort) qsort( (char *)desc, n, sizeof(dd_t), compare);
	
	lprint(fid, desc, n);
        
	if(Hendaccess(aid) == FAIL) {
            HEprint(stderr, 0);
	}
        
        if (Hclose(fid) == FAIL) {
            HEprint(stderr, 0);
	}
        
        i++;
        printf("\n");
    }
    return(0);
}

int lprint(int32 fid, dd_t *desc, int num)
{
    sp_info_block_t info;
    
    intn j = 0, empty = 0, status;
    uint16 prev = 0;
    int32 len;
    char *name, *label_str;
    
    while (j <num) {
        if (desc[j].tag == DFTAG_NULL) {
            empty++;
            j++;
            continue;               /* don't print anything now */
        }
        
        /* 
         * skip this tag if the user only wants to see some tags and
         *  this is not one of them 
         */
        if(only_tag != DFTAG_NULL && only_tag != desc[j].tag) {
            j++;
            continue;
        }
        
        /*
         ** Find and print text description of this tag
         */
        name = (char *) HDgettagname(desc[j].tag);
        if(!name) name = "Unknown Tag";
        printf("\n%-30s: (tag %d)\n", name, desc[j].tag);
        
        /*
         ** Print out reference number information
         */
        prev = desc[j].tag;
        if(longout) {
            while (desc[j].tag == prev && j < num) {
                printf("\tRef no %6d\t%8ld bytes\n", desc[j].ref, desc[j].length);
                
                /* print out labels and annotations if desired */
                if(labels) {
                    /* read in all of the labels */
                    len = DFANgetlablen(file_name, prev, desc[j].ref);
                    if(len != FAIL) {
                        label_str = (char *) HDgetspace((uint32) len + 1);
                        status = DFANgetlabel(file_name, prev, desc[j].ref, label_str, len + 1);
                        label_str[len] = '\0';
                        if(status == FAIL) 
                            printf("\t  Unable to read label\n");
                        else
                            printf("\t  Label: %s\n", label_str);
                        HDfreespace(label_str);
                    }
                    
                    /* read in all of the annotations */
                    len = DFANgetdesclen(file_name, prev, desc[j].ref);
                    if(len != FAIL) {
                        label_str = (char *) HDgetspace((uint32) len + 1);
                        status = DFANgetdesc(file_name, prev, desc[j].ref, label_str, len + 1);
                        label_str[len] = '\0';
                        if(status == FAIL) 
                            printf("\t  Unable to read description\n");
                        else
                            printf("\t  Description: %s\n", label_str);
                        HDfreespace(label_str);
                    }
                }
                /* print out special info if desired */
                if((special) && (SPECIALTAG(prev))) {
                    int32 aid, ret; 

                    aid = Hstartread(fid, prev, desc[j].ref);
                    ret = HDget_special_info(aid, &info);
                    if((ret == FAIL) || (info.key == FAIL))
                        continue;
                    
                    switch(info.key) {
                    case SPECIAL_LINKED:
                        printf("\tLinked Block: first %d  standard %d  per unit %d\n",
                               info.first_len, info.block_len, info.nblocks);
                        break;
                    case SPECIAL_EXT:
                        printf("\tExternal File: path %s  offset %d\n", 
                               info.path, info.offset);
                        break;
                    default:
                        printf("\tDo not understand special element type %d\n", info.key);
                        break;
                    }
                    Hendaccess(aid);
                }
                j++;
            }
        } else {
            printf("\tRef nos:");
            while (desc[j].tag == (uint16)prev && j < num) {
                printf(" %d",desc[j].ref);
                j++;
            }
        }
    }
    
    if(empty) printf("\nEmpty (tag %d): %d slots\n", DFTAG_NULL, empty);
    return 0;
    
}
