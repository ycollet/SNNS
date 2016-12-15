/*
 * File:     (@(#)nepomuk.h	1.2    8/23/95) 
 * Purpose:  declaration of population managment functions
 *
 *    
 *           #######     #     #     #######      #####  
 *           #           ##    #          #      #     #
 *           #           # #   #         #       #     #
 *           ######      #  #  #        #        #     #
 *           #           #   # #       #         #     #
 *           #           #    ##      #          #     #
 *           #######     #     #     #######      #####
 *
 *             ( Evolutionaerer NetZwerk Optimierer )
 *
 *               Implementation:   knete 1.0  
 *               adapted to:       SNNSv4.0    
 *
 *                      Copyright (c) 1994 - 1995
 *      Institut fuer Logik, Komplexitaet und Deduktionssysteme
 *                        Universitaet Karlsruhe 
 *
 * Authors: Johannes Schaefer, Matthias Schubert, Thomas Ragg
 * Release: 1.0, August 1995 
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose is hereby granted without fee, provided
 * that the above copyright notice, author statement and this permission
 * notice appear in all copies of this software and related documentation.
 *
 * THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND, EXPRESS,
 * IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
 * MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * IN NO EVENT SHALL THE INSTITUTE OF LOGIC, COMPLEXITY AND DEDUCTION SYSTEMS OR
 * THE UNIVERSITY OF KARSLRUHE BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT ADVISED OF THE POSSIBILITY OF
 * DAMAGE, AND ON ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH
 * THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 *
 *      date        | author          | description                          
 *    --------------+-----------------+------------------------------------  
 *      dd. mon. yy | name of author  | Short description of changes made.   
 *                  | (initials)      | Mark changed parts with initials.    
 *                  |                 |                                      
 *                                                                           
 */

#ifndef _NEPOMUK_H
#define _NEPOMUK_H
/* ---------------------------------------------------------------- const --- */
/*                                                                            */

/* --- Error codes --------------- */

#define KPM_NO_ERROR        0
#define KPM_INVALID_NETID   1
#define KPM_MEM_EXCESS      2
#define KPM_NETIO           3
#define KPM_INVALID_POPID   4
#define KPM_INVALID_PATID   5
#define KPM_NO_UNITS        6
#define KPM_NO_WEIGHTS      7

/* -- this PopID is surly not correct --------- */

#define KPM_INVALID_POP_ID  0


/*                                                                            */
/* -------------------------------------------------------------------------- */



/* ---------------------------------------------------------------- types --- */
/*                                                                            */

/* structure used for maintaining the nets */
/* nets are generally stored in a fixed sized array; */
/* free and used nets additionally are linked via a simple list */

typedef struct _PopNet
{
    int used;
    int subPop;

    struct _PopNet *freeSucc;
    struct _PopNet *usedSucc;
    
    memNet snnsNet;   /* this is the structure returned from krm_getNet() */

    void *userData;
    
} PopNet;

/* the patterns are organized exactly like the nets */

typedef struct _PopPat
{
    int used;
    int subPop;

    struct _PopPat *freeSucc;
    struct _PopPat *usedSucc;
    
    memPat snnsPat;  /* this is the structure returned from krm_getPat() */

    char *name;
    
    void *userData;
    
} PopPat;


typedef PopNet *NetID;    /* the user should only care about NetID's ... */
typedef PopPat *PatID;    /* ... and PatID's  */

typedef int     PopID;    /* ... and PopID's  */

typedef int     kpm_err;  /* ... and worry about kpm_errors !    */

typedef int (*CmpFct)( NetID id_1, NetID id_2 );   /* cf. strcmp() */


/* the following structures are mainly all informations you can get about     */
/* a net from SNNS; be careful when using them, they mai be stripped someday  */
     
typedef struct
{
    /* everything worth knowing about an unit */

    int number;
    
    char *name,
         *outFuncName,
         *actFuncName,
         *FTypeName;
    
    FlintType activation,
              initAct,
              output,
              bias;
    
    int  subnetNo,
         layerNo;
    
    struct PosType    position;

    int TType,
        frozen,
        inputType;
    
} UnitDescr;


typedef struct
{
    /* everything worth knowing about a weight */

    FlintType strength;
    int source, target;
    
} WeightDescr;


typedef struct
{
    /* Default values for units */
    int       no_of_units,
              no_of_links;
    FlintType act, bias;
    int       io_type,
              subnet_no,
              layer_no;
    char     *act_func,
             *out_func;

    /* net description vectors */

    /* the units are numbered from 0 to no_of_units-1 */
    /* this is NOT the SNNS-unit-number; you'll have to subtract one */
    UnitDescr   *units;
    
    WeightDescr *weights;
    
} NetDescr;

/*                                                                            */
/* -------------------------------------------------------------------------- */




/* ------------------------------------------------------------ init/exit --- */
/*                                                                            */
/*    Use these two functions to init/exit nepomuk.                           */
/*    Initialization must be done before using nepomuk!                       */
/*    (But it isn't checked for, yet;                                         */
/*    so there will be some surprises if you forget about it.)                */
/*                                                                            */
/*    Use the exit-function to be sure that the memory is freed correctly.    */
/*                                                                            */
/*    Parameters:                                                             */
/*       max_nets -- max number of nets you want nepomuk to take care of      */
/*       max_pats -- the same for the patterns                                */
/*                                                                            */
/*                                                                            */

extern kpm_err kpm_initialize( int max_nets, int max_pats );
extern kpm_err kpm_exit      ( void );

/*                                                                            */
/* -------------------------------------------------------------------------- */



/* --------------------------------------------------- network management --- */
/*                                                                            */


/* --- kpm_setCurrPNet ------------------------------------------------------ */
/*                                                                            */
/*   Tries to put the net given by id into snns and makes it current;         */
/*   the current patterns are saved away and a dummy pattern for this         */
/*   net is generated. If you don't have any patterns and changing            */
/*   topologies this function's made for you.                                 */
/*                                                                            */
/*                                                                            */

extern kpm_err kpm_setCurrPNet( NetID id );

/*                                                                            */
/* -------------------------------------------------------------------------- */


/* --- kpm_setCurrentNet ---------------------------------------------------- */
/*                                                                            */
/*   tries to put the net given by id into snns and makes it current          */
/*                                                                            */
/*                                                                            */

extern kpm_err kpm_setCurrentNet( NetID id );

/*                                                                            */
/* -------------------------------------------------------------------------- */


/* --- kpm_getCurrentNet ---------------------------------------------------- */
/*                                                                            */
/*   returns the NetID of the currently active net                            */
/*                                                                            */

extern NetID kpm_getCurrentNet( void );

/*                                                                            */
/* -------------------------------------------------------------------------- */


/* --- kpm_loadNet ---------------------------------------------------------- */
/*                                                                            */
/*   loads the net given by filename via snns-loading and makes it current    */
/*                                                                            */

extern NetID kpm_loadNet( char *filename, void *usr_data );

/*                                                                            */
/* -------------------------------------------------------------------------- */



/* --- kpm_saveNet ---------------------------------------------------------- */
/*                                                                            */
/*   saves the net given by id to filename; snns-name is netname              */
/*                                                                            */

extern kpm_err kpm_saveNet( NetID id, char *filename, char *netname );

/*                                                                            */
/* -------------------------------------------------------------------------- */



/* --- kpm_sortNets --------------------------------------------------------- */
/*                                                                            */
/*   Sorts all used nets stored in nepomuk according to netcmp;               */
/*   netcmp works the same as strcmp, despite the fact, that it gets NetID's  */
/*   to compare.                                                              */
/*   The nets are sorted in increasing order.                                 */
/*                                                                            */

extern void kpm_sortNets( CmpFct netcmp );

/*                                                                            */
/* -------------------------------------------------------------------------- */



/* --- kpm_deleteNet -------------------------------------------------------- */
/*                                                                            */
/*   Deletes the net given by id from nepomuk                                 */
/*                                                                            */

extern kpm_err kpm_deleteNet( NetID id );

/*                                                                            */
/* -------------------------------------------------------------------------- */



/* --- kpm_getNetDescr ------------------------------------------------------ */
/*                                                                            */
/*  Sets n to a network description of NetID.                                 */
/*                                                                            */

extern kpm_err kpm_getNetDescr( NetID id, NetDescr *n );

/*                                                                            */
/* -------------------------------------------------------------------------- */


/* --- kpm_freeNetDescr ----------------------------------------------------- */
/*                                                                            */
/*   Frees the memory allocated for a net descriptor.                         */
/*                                                                            */

extern void kpm_freeNetDescr( NetDescr *n );

/*                                                                            */
/* --- kpm_freeNetDescr ----------------------------------------------------- */


/* --- kpm_copyNet ---------------------------------------------------------- */
/*                                                                            */
/*   Creates a copy of the net given by id, appends usr_data to it and        */
/*   returns its NetID.                                                       */
/*                                                                            */

extern NetID kpm_copyNet( NetID id, void *usr_data );

/*                                                                            */
/* -------------------------------------------------------------------------- */

/* --- kpm_copyNetInfo   ------------------------------------------------------ */
/*   Copies the data of the currrent net to  the snnsNet field id       */
/*   returns an error code.                                                   */
/*                                                                            */

extern int kpm_copyNetInfo( NetID id);

/*                                                                            */
/* -------------------------------------------------------------------------- */


/* --- kpm_newNet ----------------------------------------------------------- */
/*                                                                            */
/*   Creates a new net and returns its NetID.                                 */
/*                                                                            */

extern NetID kpm_newNet ( void *usr_data );

/*                                                                            */
/* -------------------------------------------------------------------------- */



/* --- kpm_getNetData ------------------------------------------------------- */
/*                                                                            */
/*   Returns the stored userData in NetID, NULL if id's not valid             */
/*                                                                            */

extern void *kpm_getNetData( NetID id );

/*                                                                            */
/* -------------------------------------------------------------------------- */



/* ---------------------------------------------- managing pop-membership --- */
/*                                                                            */

extern PopID kpm_newPopID( void );

extern kpm_err kpm_validPopID( PopID id );

extern kpm_err kpm_setPopMember( NetID n_id, PopID p_id );

extern PopID kpm_getPopID( NetID id );

extern NetID kpm_popFirstMember( PopID p_id );
extern NetID kpm_popNextMember ( PopID p_id, NetID n_id );

/*                                                                            */
/* -------------------------------------------------------------------------- */




/* --------------------------------------------------- pattern management --- */
/*                                                                            */

extern kpm_err kpm_setCurrentPattern( PatID id );
extern PatID   kpm_loadPat( char *filename, void *usr_data );
extern PatID   kpm_getCurrentPattern( void );


/* --- kpm_getPatData ------------------------------------------------------- */
/*                                                                            */
/*   returns the stored userData in NetID, NULL if it's not valid             */
/*                                                                            */

extern void *kpm_getPatData( PatID id );

/*                                                                            */
/* -------------------------------------------------------------------------- */



/* --- kpm_getFirst/NextPat ------------------------------------------------- */
/*                                                                            */
/* return NULL if no more Pat available                                       */
/* does not make the net current!                                             */
/*                                                                            */

extern PatID kpm_getFirstPat( void );
extern PatID kpm_getNextPat( PatID pat );
     
/*                                                                            */
/* -------------------------------------------------------------------------- */


/* --- kpm_set/getPatName --------------------------------------------------- */
/*                                                                            */
/*   sets/returns the associated name to pattern id                           */
/*   pats are initialized with NULL name                                      */
/*   set creates a copy of name                                               */
/*   get returns a pointer to the stored name                                 */
/*                                                                            */

extern char *kpm_getPatName( PatID id );
extern kpm_err kpm_setPatName( PatID id, char *name );

/*                                                                            */
/* -------------------------------------------------------------------------- */
#endif
