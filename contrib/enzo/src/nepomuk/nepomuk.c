/*
 * File:     (@(#)nepomuk.c	1.2    8/23/95) 
 * Purpose:  `Nepomuk' adds some features to SNNS for managing populations of       
 *            Neural Networks; it is used mainly together with `knete', but may      
 *            also be useful as standalone-library. definition of main functions
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


#include <string.h>
#include <stdlib.h>

#include "glob_typ.h"
#include "kr_typ.h"	 /*  Kernel Types and Constants  */
#include "kernel.h"
#include "kr_const.h"	 /*  Constant Declarators for SNNS-Kernel  */
#include "kr_def.h"	 /*  Default Values  */
#include "kr_mac.h"	 /*  Kernel Macros  */
#include "enzo_mem_typ.h"
#include "kr_shell.h"


#include "nepomuk.h"


/* --------------------------------------------------------------- macros --- */
/*                                                                            */

#define LAST_NET         (netMember+max_netPop_size)

#define VALID_NET(id)   ((id)&&(id>=netMember)&&(id<LAST_NET))
#define CHECK_NET(id)   if(!VALID_NET(id)) return( KPM_INVALID_NETID );

/* make sure you don't get/put the current network twice ! */
#define GET_CURRENT_NET if(currentNet) ksh_getNet( &currentNet->snnsNet )
#define PUT_CURRENT_NET if(currentNet) ksh_putNet( &currentNet->snnsNet )


#define LAST_PAT        (patMember+max_patPop_size)
#define VALID_PAT(id)   ((id)&&(id>=patMember)&&(id<LAST_PAT))
#define CHECK_PAT(id)   if(!VALID_PAT(id)) return( KPM_INVALID_PATID );

/* make sure yo-u don't get/put the current pattern twice ! */
#define GET_CURRENT_PAT if(currentPat) ksh_getPattern( &currentPat->snnsPat )
#define PUT_CURRENT_PAT if(currentPat) ksh_putPattern( &currentPat->snnsPat )


#define VALID_POP(id)   (((id)>0)&&(id<popIDcnt))
#define CHECK_POP(id)   if(!VALID_POP(id)) return( KPM_INVALID_POPID );

#define SWAP_NETID(x,y) {NetID tmp; tmp = (x); (x) = (y); (y) = tmp;}

/*                                                                            */
/* -------------------------------------------------------------------------- */



/* ----------------------------------------------------------- local vars --- */
/*                                                                            */

static NetID   currentNet = NULL;    /* just remember the good times          */
static PatID   currentPat = NULL;    /* and don't forget about old pat!       */

static int     max_netPop_size;      /* Maximum number of nets to store.      */
static int     max_patPop_size;      /* Maximum number of pats to store.      */

static PopNet *netMember;            /* The nets live in here.                */
static PopPat *patMember;            /* The pats live in here.                */

static PopNet *firstFreeNet = NULL;  /* Marks the first free net.             */
static PopNet *firstUsedNet = NULL;  /* Marks the first used net.             */

static PopPat *firstFreePat = NULL;  /* Marks the first free pat.             */
static PopPat *firstUsedPat = NULL;  /* Marks the first used pat.             */


static PopID popIDcnt = 1;           /* PopID counter                         */

/*                                                                            */
/* -------------------------------------------------------------------------- */


/* ---------------------------------------------------------- local funcs --- */
/*                                                                            */

/* --- ulf_functions -------------------------------------------------------- */
/*                                                                            */
/*   These functions are used for managing the free- and used-list            */
/*                                                                            */

static NetID ulf_net_alloc( void )
{
    NetID id;
    
    if( !VALID_NET(firstFreeNet) ) return( NULL );

    id = firstFreeNet;
    firstFreeNet = id->freeSucc;

    id->used = TRUE;
    id->usedSucc = firstUsedNet;

    firstUsedNet = id;

    return id;
}

static void ulf_net_free( NetID id )
{
    NetID pred;
    
    if( VALID_NET(id) && id->used )
    {
	id->used = FALSE;
	id->freeSucc = firstFreeNet;
	firstFreeNet = id;

	if( firstUsedNet == id ) firstUsedNet = id->usedSucc;
	else
	{
	    for( pred = firstUsedNet; pred->usedSucc != id;
		 pred = pred->usedSucc );

	    pred->usedSucc = id->usedSucc;
	    id->usedSucc = NULL;
	}
    }
}

static void print_net_list( void )
{
    /* for debugging only */
    NetID p;
    
    for( p = firstUsedNet; p != NULL; p = p->usedSucc )
    {
	printf( "%p->", p );
    }
    printf( "NULL\n" );
}


static PatID ulf_pat_alloc( void )
{
    PatID id;
    
    if( !VALID_PAT(firstFreePat) ) return( NULL );

    id = firstFreePat;
    firstFreePat = id->freeSucc;

    id->used = TRUE;
    id->usedSucc = firstUsedPat;

    firstUsedPat = id;

    return id;
}

static void ulf_pat_free( PatID id )
{
    PatID pred;
    
    if( VALID_PAT(id) && id->used )
    {
	id->used = FALSE;
	id->freeSucc = firstFreePat;
	firstFreePat = id;

	if( firstUsedPat == id ) firstUsedPat = id->usedSucc;
	else
	{
	    for( pred = firstUsedPat; pred->usedSucc != id;
		 pred = pred->usedSucc );

	    pred->usedSucc = id->usedSucc;
	    id->usedSucc = NULL;
	}
    }
}


/* --- utility-functions ---------------------------------------------------- */
/*                                                                            */

static char *my_strdup( char *s )  /* strdup() doesn't check for NULL         */
{
    if( s == NULL ) return( NULL );

    return( strdup( s ) );
}

/* --- kpm_putNetDescr ------------------------------------------------------ */
/*                                                                            */
/* This function is NOT to become public;                                     */
/* network manipulation should be done exclusivly via SNNS!                   */
/* Its only purpose is to create a copy of a net.                             */
/*                                                                            */

static NetID kpm_putNetDescr( NetDescr *n )
{
    int unit_no, u_no, w_no;

    NetID id;
    
    UnitDescr   *u = n->units;
    WeightDescr *w = n->weights;


    GET_CURRENT_NET;

    ksh_setUnitDefaults( n->act, n->bias, n->io_type,
                          n->subnet_no, n->layer_no,
                          n->act_func, n->out_func         );

    for( u_no = 0, w_no = 0; u_no < n->no_of_units; u_no++ )
    {
        unit_no = ksh_createDefaultUnit();
        if( u[u_no].name )      ksh_setUnitName( unit_no, u[u_no].name );
        if( u[u_no].FTypeName ) ksh_setUnitFType( unit_no, u[u_no].FTypeName );
        
        ksh_setUnitActFunc( unit_no, u[u_no].actFuncName  );
        ksh_setUnitOutFunc( unit_no, u[u_no].outFuncName  );
        
        ksh_setUnitActivation       ( unit_no, u[u_no].activation   );
        ksh_setUnitInitialActivation( unit_no, u[u_no].initAct      );
        ksh_setUnitOutput           ( unit_no, u[u_no].output       );
        ksh_setUnitBias             ( unit_no, u[u_no].bias         );
        
        if( unit_no != u_no+1 )  /* ERROR !!?  vgl. krio_readUnitDefnitions  */;

        if( u[u_no].name != NULL ) ksh_setUnitName( unit_no, u[u_no].name );

        ksh_setUnitSubnetNo( unit_no, u[u_no].subnetNo );
        ksh_setUnitLayerNo( unit_no, u[u_no].layerNo );

        ksh_setUnitTType( unit_no, u[u_no].TType );
        if( u[u_no].frozen ) ksh_freezeUnit( unit_no );

        ksh_setUnitPosition( unit_no, &u[u_no].position );
    }

    for( w_no=0; w_no < n->no_of_links; /* nix */ )
    {
        unit_no = w[w_no].target;
        ksh_setCurrentUnit( unit_no );
        while( w_no < n->no_of_links  && w[w_no].target == unit_no )
        {
            ksh_createLink( w[w_no].source, w[w_no].strength );
            w_no++;
        }
    }

    NetInitialize = TRUE;  /* net needs new initialization! */
    NetModified   = TRUE;  /* this is a new net ...         */
    
    if( (id = ulf_net_alloc()) == NULL )  return( NULL );
    
    ksh_getNet( &id->snnsNet );
    PUT_CURRENT_NET;
    return id;
}




/*  */
/* ------------------------------------------------------------ init/exit --- */
/*                                                                            */

kpm_err kpm_initialize( int max_nets, int max_pats )
{
    PopNet *n;
    PopPat *p;

    max_netPop_size = max_nets;
    max_patPop_size = max_pats;
    
    if( !(netMember = (PopNet*) malloc( max_netPop_size*sizeof(PopNet) )) )
        return( KPM_MEM_EXCESS );

    if( !(patMember = (PopPat*) malloc( max_patPop_size*sizeof(PopPat) )) )
        return( KPM_MEM_EXCESS );


    firstFreeNet = netMember;
    for( n=netMember; n<LAST_NET; n++ )
    {
        n->used     = FALSE;
        n->subPop   = 0;
        n->freeSucc = n+1;
        n->usedSucc = NULL;
	n->userData = NULL;
    }

    firstFreePat = patMember;
    for( p=patMember; p<LAST_PAT; p++ )
    {
        p->used     = FALSE;
        p->subPop   = 0;
        p->freeSucc = p+1;
        p->usedSucc = NULL;
	p->name     = NULL;
	p->userData = NULL;
    }
    
    return( KPM_NO_ERROR );
}

kpm_err kpm_exit( void )
{
    PopNet *p;
    
    for( p=netMember; p<netMember+max_netPop_size; p++ )
    {
        if( p->used )
        {
            ksh_putNet( &p->snnsNet );
	    kpm_deleteNet( p );
        }
    }
    firstFreeNet = firstUsedNet = NULL;
    return( KPM_NO_ERROR );
}

/*                                                                            */
/* -------------------------------------------------------------------------- */


/*  */
/* --------------------------------------------------- network management --- */


/* --- kpm_setCurrentNet ---------------------------------------------------- */
/*                                                                            */
/*   tries to put the net given by id into snns                               */
/*                                                                            */
/*                                                                            */

kpm_err kpm_setCurrentNet( NetID id )  
{
    if( VALID_NET(id) && id->used )
    {
        GET_CURRENT_NET;
        currentNet = id;
        PUT_CURRENT_NET;
        ksh_readNetinfo();
        
        return( KPM_NO_ERROR );
    }
    else return( KPM_INVALID_NETID );
}

/*                                                                            */
/* -------------------------------------------------------------------------- */


/* --- kpm_setCurrPNet ------------------------------------------------------ */
/*                                                                            */
/*   tries to put the net given by id into snns                               */
/*   generates a dummy patetrn for this net additionally                      */
/*                                                                            */

kpm_err kpm_setCurrPNet( NetID id )  
{
    if( VALID_NET(id) && id->used )
    {
        GET_CURRENT_NET;
	GET_CURRENT_PAT;
        currentNet = id;
        PUT_CURRENT_NET;
        ksh_readNetinfo();
	(void) ksh_newPattern();
	currentPat = NULL; /* there is only the dummy-pattern current */
        
        return( KPM_NO_ERROR );
    }
    else return( KPM_INVALID_NETID );
}

/*                                                                            */
/* -------------------------------------------------------------------------- */


/* --- kpm_getCurrentNet ---------------------------------------------------- */
/*                                                                            */
/*   returns the NetID of the currently active net                            */
/*                                                                            */

NetID kpm_getCurrentNet( void )  
{
    return currentNet;
}

/*                                                                            */
/* -------------------------------------------------------------------------- */


/* --- kpm_loadNet ---------------------------------------------------------- */
/*                                                                            */
/*   loads the net given by filename via snns-loading and makes it current    */
/*                                                                            */

NetID kpm_loadNet( char *filename, void *usr_data )  
{
    NetID  id = NULL;
    char  *netname;
    int err;
    
    GET_CURRENT_NET;
    GET_CURRENT_PAT;

    err = ksh_loadNet( filename, &netname );

    if( err == KRERR_CHANGED_I_UNITS || err == KRERR_CHANGED_O_UNITS )
    {
	printf( "warning: no of input/output units changed\n" );
	err = KRERR_NO_ERROR;
    }

    if( err == KRERR_NO_ERROR && (id = ulf_net_alloc()) )
    {
        ksh_getNet( &id->snnsNet );
        id->userData = usr_data;
        currentNet = id;
    }
    PUT_CURRENT_PAT;
    PUT_CURRENT_NET;
    ksh_readNetinfo();

    return( id );
}

/*                                                                            */
/* -------------------------------------------------------------------------- */



/* --- kpm_saveNet ---------------------------------------------------------- */
/*                                                                            */

kpm_err kpm_saveNet( NetID id, char *filename, char *netname )
{
    kpm_err p_err;
    
    CHECK_NET(id);

    GET_CURRENT_NET;
    ksh_putNet( &id->snnsNet );

    p_err = ksh_saveNet( filename, netname );

    ksh_getNet( &id->snnsNet );
    PUT_CURRENT_NET;

    if( p_err == KRERR_NO_ERROR ) return KPM_NO_ERROR;
    else                          return KPM_NETIO;
}

/*                                                                            */
/* -------------------------------------------------------------------------- */



/* --- kpm_sortNets --------------------------------------------------------- */
/*                                                                            */

void kpm_sortNets( CmpFct netcmp )
{
    PopNet *s, *sorted, *last, *net, *l;

    if( netcmp == NULL || firstUsedNet == NULL ) return;
    
    sorted = last = firstUsedNet;
    
    firstUsedNet = firstUsedNet->usedSucc;
    sorted->usedSucc = NULL; /* Ende der sort. Liste */

    while( firstUsedNet != NULL )
    {
	net = firstUsedNet;
	firstUsedNet = firstUsedNet->usedSucc;
	
	s = sorted; last->usedSucc = net; l = NULL;

	while( netcmp( net, s ) > 0 )	{  l = s;  s = s->usedSucc;  }

	if( s == net ) 	{  s->usedSucc = NULL;  last = s;  }
	else
	{
	    last->usedSucc = NULL;
	    net->usedSucc = s;
	    
	    if( l != NULL ) l->usedSucc = net;
	    else            sorted      = net;
	}
    }

    firstUsedNet = sorted;
}

/*                                                                            */
/* -------------------------------------------------------------------------- */








/* --- kpm_deleteNet -------------------------------------------------------- */
/*                                                                            */

kpm_err kpm_deleteNet( NetID id )
{
    CHECK_NET(id);
    
    GET_CURRENT_NET;
    GET_CURRENT_PAT;           /* save patterns from SNNS */
    ksh_putNet( &id->snnsNet );
    ksh_deleteNet();  
    free( id->userData );  id->userData = NULL;
    ulf_net_free( id );
    PUT_CURRENT_PAT;
    PUT_CURRENT_NET;
    
    return( KPM_NO_ERROR );
}

/*                                                                            */
/* -------------------------------------------------------------------------- */

/* --- kpm_freeNetDescr( NetDescr *n ) -------------------------------------- */
/* L"oscht den Speicher nach dem Benutzen des Descriptors (weights, units)    */
/*                                                           masch 12.02.94  */

void kpm_freeNetDescr( NetDescr *n )
{
    /* --- js, 16.2.94 --- */
    /* should be done properly */
    int i;
    UnitDescr   *u = n->units;
    
    free( n->act_func );
    free( n->out_func );

    for( i=0; i<n->no_of_units; i++)
    {
	free( u[i].name );
	free( u[i].actFuncName );
	free( u[i].outFuncName );
	free( u[i].FTypeName   );
    }
    /* --- js --- */

    free( n->units   );
    free( n->weights );
}


/* --- kpm_getNetDescr ------------------------------------------------------ */
/*                                                                            */

kpm_err kpm_getNetDescr( NetID id, NetDescr *n )
{
    int  no_of_units, no_of_links, no_of_sites;
    int  unit_no, src_unit, u_no, w_no;
    int  no_of_STable_entries, no_of_FTable_entries;
    FlintType strength;
    
    UnitDescr   *u;
    WeightDescr *w;

    
    GET_CURRENT_NET;
    ksh_putNet( &id->snnsNet );

    /*   kr_forceGC();   what does happen to the net in here? */
    
    no_of_units = ksh_getNoOfUnits();
    ksh_getNetInfo( &no_of_sites, &no_of_links,
                     &no_of_STable_entries, &no_of_FTable_entries );

/*    if( no_of_links == 0 )  return( KPM_NO_WEIGHTS );  ??? */
/*    if( no_of_units == 0 )  return( KPM_NO_UNITS   );    ??? */

    n->units   = u = (UnitDescr   *) malloc(no_of_units*sizeof(UnitDescr  ));
    n->weights = w = (WeightDescr *) malloc(no_of_links*sizeof(WeightDescr));

    if( !w || !u )    return( KPM_MEM_EXCESS );  /* free u, w */

    n->no_of_units = no_of_units;
    n->no_of_links = no_of_links;

    ksh_getUnitDefaults( &n->act,       &n->bias,
                          &n->io_type,
                          &n->subnet_no, &n->layer_no,
                          &n->act_func,  &n->out_func );

    n->act_func = my_strdup(n->act_func); /* the pointers aren't sure   */
    n->out_func = my_strdup(n->out_func); /* kernel knows about them    */
    
    for( unit_no = ksh_getFirstUnit(), u_no = 0, w_no = 0;
         unit_no > 0;
         unit_no = ksh_getNextUnit(),  u_no++    )
    {
        u[u_no].number      = u_no+1;

        u[u_no].name        = my_strdup( ksh_getUnitName( unit_no ) );
        
        u[u_no].actFuncName = my_strdup( ksh_getUnitActFuncName( unit_no ) );
        u[u_no].outFuncName = my_strdup( ksh_getUnitOutFuncName( unit_no ) );
        u[u_no].FTypeName   = my_strdup( ksh_getUnitFTypeName( unit_no ) );

        u[u_no].activation  = ksh_getUnitActivation( unit_no );
        u[u_no].initAct     = ksh_getUnitInitialActivation( unit_no );
        u[u_no].output      = ksh_getUnitOutput( unit_no );
        u[u_no].bias        = ksh_getUnitBias( unit_no );

        u[u_no].subnetNo    = ksh_getUnitSubnetNo( unit_no );
        u[u_no].layerNo     = ksh_getUnitLayerNo( unit_no );

        u[u_no].TType       = ksh_getUnitTType( unit_no );
        u[u_no].inputType   = ksh_getUnitInputType( unit_no );
        u[u_no].frozen      = ksh_isUnitFrozen( unit_no );

        ksh_getUnitPosition( unit_no, &u[u_no].position );

        switch( u[u_no].inputType )
        {
          case DIRECT_LINKS: /* scan though all the links */
            for( src_unit = ksh_getFirstPredUnit( &strength );
                 src_unit > 0;
                 src_unit = ksh_getNextPredUnit ( &strength ),  w_no++ )
            {
                w[w_no].strength = strength;
                w[w_no].source   = src_unit;
                w[w_no].target   = unit_no;
            }
                 
            break;
          //case SITES:
            /* sites aren't supported (yet) */
        }
    }

    ksh_getNet( &id->snnsNet );
    PUT_CURRENT_NET;
    return ( KPM_NO_ERROR );
}

/*                                                                            */
/* -------------------------------------------------------------------------- */




/* --- kpm_copyNet ---------------------------------------------------------- */
/*                                                                            */

NetID kpm_copyNet( NetID id, void *usr_data )  
{
    NetDescr  my_net;
    NetID     new_id;
    
    kpm_getNetDescr( id, &my_net );
    new_id = kpm_putNetDescr( &my_net );
    kpm_freeNetDescr( &my_net );

    if( new_id != NULL )
    {
	new_id->subPop = id->subPop;   
	new_id->userData = usr_data;
    }

    return new_id;
}


/*                                                                            */
/* -------------------------------------------------------------------------- */

/* --- kpm_copyNetInfo ------------------------------------------------------ */
/*                                                                            */

int kpm_copyNetInfo( NetID id)  
{
  CHECK_NET(id);
  ksh_getNet(&id->snnsNet);
  return KPM_NO_ERROR;

}


/*                                                                            */
/* -------------------------------------------------------------------------- */


/* --- kpm_newNet ----------------------------------------------------------- */
/*                                                                            */
/*   creates a new net and returns its NetID                                  */
/*                                                                            */

NetID kpm_newNet ( void *usr_data )  
{
    NetID id;
    
    GET_CURRENT_NET;
    if( !(id = ulf_net_alloc()) )  return( NULL );
    
    kr_forceUnitGC();               /* do GarbageCollection now!           */
    ksh_getNet( &id->snnsNet );     /* this sould be a lot of NULL-stuff   */
    id->userData = usr_data;
    
    PUT_CURRENT_NET;
}

/*                                                                            */
/* -------------------------------------------------------------------------- */



/* --- kpm_getNetData ------------------------------------------------------- */
/*                                                                            */
/*   returns the stored userData in NetID, NULL if id's not valid             */
/*                                                                            */

void *kpm_getNetData( NetID id )
{
    if( VALID_NET(id) && id->used )   return( id->userData );
    else                              return( NULL );
}

/*                                                                            */
/* -------------------------------------------------------------------------- */


/*  */
/* ---------------------------------------------- managing pop-membership --- */
/*                                                                            */


/* --- kpm_newPopID --------------------------------------------------------- */
/*                                                                            */

PopID kpm_newPopID( void )
{
    return( popIDcnt++ );
}

/*                                                                            */
/* -------------------------------------------------------------------------- */


/* --- kpm_validPopID ------------------------------------------------------- */
/*                                                                            */

kpm_err kpm_validPopID( PopID id )
{
    CHECK_POP( id );
    return( KPM_NO_ERROR );
}

/*                                                                            */
/* -------------------------------------------------------------------------- */


/* --- kpm_setPopMember ----------------------------------------------------- */
/*                                                                            */

kpm_err kpm_setPopMember( NetID n_id, PopID p_id )  
{
    CHECK_NET( n_id );
    CHECK_POP( p_id );
    
    if( n_id->used )
    {
        n_id->subPop = p_id;
        return( KPM_NO_ERROR );
    }
    else return( KPM_INVALID_NETID );
}

/*                                                                            */
/* -------------------------------------------------------------------------- */


/* --- kpm_getPopID --------------------------------------------------------- */
/*                                                                            */

PopID kpm_getPopID( NetID id )  
{
    if( VALID_NET(id) && id->used ) return( id->subPop );
    else                            return( 0 );
}

/*                                                                            */
/* -------------------------------------------------------------------------- */


/* --- kpm_popFirstMember --------------------------------------------------- */
/*                                                                            */

NetID kpm_popFirstMember( PopID p_id )  
{
    NetID n_id;
    
    if( VALID_POP( p_id ) )
    {
        for( n_id = firstUsedNet;
             n_id != NULL && n_id->subPop != p_id;
             n_id = n_id->usedSucc ) /* just search */;

	kpm_setCurrentNet( n_id );
        return( n_id );   /* which is NULL in case of not existance */
    }
    else return( NULL );
}

/*                                                                            */
/* -------------------------------------------------------------------------- */


/* --- kpm_popNextMember ---------------------------------------------------- */
/*                                                                            */

NetID kpm_popNextMember( PopID p_id, NetID n_id )  
{
    if( VALID_POP( p_id ) && VALID_NET( n_id ) && n_id->used )
    {
        for( n_id = n_id->usedSucc;
             n_id != NULL && n_id->subPop != p_id;
             n_id = n_id->usedSucc ) /* just search */;
	kpm_setCurrentNet( n_id );
        return( n_id );   /* which is NULL in case of not existance */
    }
    else return( NULL );
}

/*                                                                            */
/* -------------------------------------------------------------------------- */



/*  */
/* --------------------------------------------------- pattern management --- */
/*                                                                            */


/* --- kpm_setCurrentPattern ------------------------------------------------ */
/*                                                                            */

kpm_err  kpm_setCurrentPattern( PatID id )  
{
    if( VALID_PAT(id) && id->used )
    {
        GET_CURRENT_PAT;
        currentPat = id;
        PUT_CURRENT_PAT;
        
        return( KPM_NO_ERROR );
    }
    else return( KPM_INVALID_PATID );
}

/*                                                                            */
/* -------------------------------------------------------------------------- */



/* --- kpm_loadPat ---------------------------------------------------------- */
/*                                                                            */
/*  load a pattern file via snns and makes it current                         */
/*                                                                            */

PatID kpm_loadPat( char *filename, void *usr_data )  
{
    PatID  id = NULL;
    int err;

    int number;
    
    GET_CURRENT_PAT;

    /* -- function changed     -- masch 11.08.95    */ 
    err = ksh_loadNewPatterns( filename,&number );  

    if( err == KRERR_INPUT_PATTERNS || err == KRERR_OUTPUT_PATTERNS )
    {
	printf( "warning: no of input/output units changed\n" );
	err = KRERR_NO_ERROR;
    }
    
    if( (err == KRERR_NO_ERROR)  && (id = ulf_pat_alloc()) )
    {
        ksh_getPattern( &id->snnsPat );

        /* -- included changed, just needed      -- masch 11.08.95    */
        id->snnsPat.number   = number;

        id->userData          = usr_data;
	id->name              = NULL;
        if( currentNet ) id->subPop   = currentNet->subPop;
        currentPat = id;
    }    PUT_CURRENT_PAT;

    return( id );
}


/*                                                                            */
/* -------------------------------------------------------------------------- */


/* --- kpm_getCurrentPattern ------------------------------------------------ */
/*                                                                            */

PatID kpm_getCurrentPattern( void )  
{
    return( currentPat );
}

/*                                                                            */
/* -------------------------------------------------------------------------- */


/* --- kpm_getPatData ------------------------------------------------------- */
/*                                                                            */
/*   returns the stored userData in NetID, NULL if it's not valid             */
/*                                                                            */

void *kpm_getPatData( PatID id )
{
    return( id->userData );
}

/*                                                                            */
/* -------------------------------------------------------------------------- */




/* --- kpm_getFirst/NextPat ------------------------------------------------- */
/*                                                                            */
/* return NULL if no more Pat available                                       */
/* does not make the net current!                                             */
/*                                                                            */

PatID kpm_getFirstPat( void )
{
    return( firstUsedPat );
}

PatID kpm_getNextPat( PatID id )
{
    PatID p_id;

    if( !VALID_PAT( id ) ) return( NULL );

    return( id->usedSucc );
}

/*                                                                            */
/* -------------------------------------------------------------------------- */


/* --- kpm_set/getPatName --------------------------------------------------- */
/*                                                                            */
/*   sets/returns the associated name to pattern id                           */
/*   pats are initialized with NULL name                                      */
/*   set creates a copy of name                                               */
/*   get returns a pointer to the stored name                                 */
/*                                                                            */

char *kpm_getPatName( PatID id )
{
    if( VALID_PAT(id) )    return( id->name );
    else                   return( NULL );
}

kpm_err kpm_setPatName( PatID id, char *name )
{
    CHECK_PAT( id );

    free( id->name );
    
    id->name = name ? strdup( name ) : NULL;
    return( KPM_NO_ERROR );
}

/*                                                                            */
/* -------------------------------------------------------------------------- */
