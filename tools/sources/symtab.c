/*****************************************************************************
  FILE           : $Source: /projects/higgs1/SNNS/CVS/SNNS/tools/sources/symtab.c,v $
  SHORTNAME      : symtab
  SNNS VERSION   : 4.2

  PURPOSE        : Symbol table (ST) for SNNS batch interpreter
  NOTES          :

  AUTHOR         : Jens Wieland
  DATE           :

  CHANGED BY     :
  RCS VERSION    : $Revision: 1.13 $
  LAST CHANGE    : $Date: 1998/04/20 11:48:15 $

    Copyright (c) 1990-1995  SNNS Group, IPVR, Univ. Stuttgart, FRG
    Copyright (c) 1996-1998  SNNS Group, WSI, Univ. Tuebingen, FRG

******************************************************************************/
#include <config.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>             /* for MAXFLOAT */

#include "symtab.ph"
#include "arglist.h"
#include "special_jacket.h"   /* for print, xit, execute */
#include "snns_intfce.h"      /* for SNNS-kernel call jacket fcts. */
#include "glob_typ.h"         /* SNNS-Kernel: Global Datatypes and Constants */
#include "error.h"
#include "memory.h"           /* for re_malloc */

#ifndef MAXFLOAT
#include <float.h>
#define MAXFLOAT FLT_MAX
#endif

/*****************************************************************************
  FUNCTION : st_hashval

  PURPOSE  : compute a hash value for a symbol name (djb2)
  RETURNS  : the hash value
  NOTES    : internal helper for the name->index hash index

  UPDATE   :
******************************************************************************/
static unsigned long st_hashval(const char *name) {
    unsigned long h = 5381UL;
    unsigned char c;

    while ((c = (unsigned char) *name++) != 0)
        h = ((h << 5) + h) + c;         /* h * 33 + c */

    return h;
}


/*****************************************************************************
  FUNCTION : st_hash_put

  PURPOSE  : insert the st[] entry at index idx into the hash index
  RETURNS  :
  NOTES    : open addressing with linear probing; the name field of st[idx]
             must already be set

  UPDATE   :
******************************************************************************/
static void st_hash_put(St_ptr_type idx) {
    size_t mask = st_hash_size - 1;
    size_t h = (size_t) (st_hashval(st[idx].name) & mask);

    while (st_hash[h] != ST_NULL)
        h = (h + 1) & mask;

    st_hash[h] = idx;
}


/*****************************************************************************
  FUNCTION : st_hash_build

  PURPOSE  : (re)allocates the hash index at new_size slots and re-inserts
             all existing symbol table entries
  RETURNS  :
  NOTES    : new_size must be a power of two; frees the old hash index

  UPDATE   :
******************************************************************************/
static void st_hash_build(size_t new_size) {
    size_t i;
    St_ptr_type idx;

    free(st_hash);                      /* free(NULL) is safe */
    st_hash = malloc(new_size * sizeof (St_ptr_type));
    if (st_hash == NULL) err_prt(ERR_MEM);

    st_hash_size = new_size;
    for (i = 0; i < st_hash_size; i++)
        st_hash[i] = ST_NULL;

    for (idx = 0; idx < st_pos; idx++)
        st_hash_put(idx);
}


/*****************************************************************************
  FUNCTION : st_init

  PURPOSE  : allocates memory for the symbol table
             inserts names and initialisation values of built-in functions,
             variables and constants in the symbol table
  RETURNS  :
  NOTES    :

  UPDATE   :
******************************************************************************/
void st_init(void) {
    St_ptr_type tmp;
    Val_type val;

    st = malloc(st_size * sizeof (St_type));
    if (st == NULL) err_prt(ERR_MEM);

    /* start with an empty table and a fresh hash index: */
    st_pos = 0;
    st_hash_build((size_t) ST_HASH_INIT);

    /* insert constants, built-in variables and functions here:

       1. insert name with st_insert()
       2. set val to symbol's value
       3. insert value and type with st_set_val_type()
       4. if necessary, set read-only flag with st_set_ro()
    */

    /* boolean constants: */
    tmp = st_insert("FALSE");
    val.bool_val = FALSE;
    st_set_val_type(tmp, BOOL, val);
    st_set_ro(tmp);

    tmp = st_insert("TRUE");
    val.bool_val = TRUE;
    st_set_val_type(tmp, BOOL, val);
    st_set_ro(tmp);

    /* a constant for negation ... */
    tmp = st_insert("%ZERO");
    val.int_val = 0;
    st_set_val_type(tmp, INT, val);
    st_set_ro(tmp);

    /* ... and for increment by 1: */
    tmp = st_insert("%ONE");
    val.int_val = 1;
    st_set_val_type(tmp, INT, val);
    st_set_ro(tmp);

    /* non-SNNS-functions:
    */
    tmp = st_insert("print");
    val.fct_val = print;
    st_set_val_type(tmp, FCT, val);
    st_set_ro(tmp);

    tmp = st_insert("execute");
    val.fct_val = execute;
    st_set_val_type(tmp, FCT, val);
    st_set_ro(tmp);

    tmp = st_insert("exit");
    val.fct_val = xit;
    st_set_val_type(tmp, FCT, val);
    st_set_ro(tmp);

    tmp = st_insert("st_out");
    val.fct_val = (void(*)(arglist_type *)) st_out;
    /* debug function pointer with different declaration */
    st_set_val_type(tmp, FCT, val);
    st_set_ro(tmp);

    /* SNNS functions:
    */
    tmp = st_insert("setInitFunc");
    val.fct_val = int_setInitFunc;
    st_set_val_type(tmp, FCT, val);
    st_set_ro(tmp);

    tmp = st_insert("setLearnFunc");
    val.fct_val = int_setLearnFunc;
    st_set_val_type(tmp, FCT, val);
    st_set_ro(tmp);

    tmp = st_insert("setUpdateFunc");
    val.fct_val = int_setUpdateFunc;
    st_set_val_type(tmp, FCT, val);
    st_set_ro(tmp);

    tmp = st_insert("setPruningFunc");
    val.fct_val = int_setPruningFunc;
    st_set_val_type(tmp, FCT, val);
    st_set_ro(tmp);

    tmp = st_insert("setRemapFunc");
    val.fct_val = int_setRemapFunc;
    st_set_val_type(tmp, FCT, val);
    st_set_ro(tmp);

    tmp = st_insert("setSubPattern");
    val.fct_val = int_setSubPattern;
    st_set_val_type(tmp, FCT, val);
    st_set_ro(tmp);

    tmp = st_insert("setShuffle");
    val.fct_val = int_setShuffle;
    st_set_val_type(tmp, FCT, val);
    st_set_ro(tmp);

    tmp = st_insert("setSubShuffle");
    val.fct_val = int_setSubShuffle;
    st_set_val_type(tmp, FCT, val);
    st_set_ro(tmp);

    tmp = st_insert("setClassDistrib");
    val.fct_val = int_setClassDistrib;
    st_set_val_type(tmp, FCT, val);
    st_set_ro(tmp);

    tmp = st_insert("setParallelMode");
    val.fct_val = int_setParallelMode;
    st_set_val_type(tmp, FCT, val);
    st_set_ro(tmp);

    tmp = st_insert("setCascadeParams");
    val.fct_val = int_setCascadeParams;
    st_set_val_type(tmp, FCT, val);
    st_set_ro(tmp);

    tmp = st_insert("initNet");
    val.fct_val = int_initNet;
    st_set_val_type(tmp, FCT, val);
    st_set_ro(tmp);

    tmp = st_insert("resetNet");
    val.fct_val = int_resetNet;
    st_set_val_type(tmp, FCT, val);
    st_set_ro(tmp);

    tmp = st_insert("loadNet");
    val.fct_val = int_loadNet;
    st_set_val_type(tmp, FCT, val);
    st_set_ro(tmp);

    tmp = st_insert("saveNet");
    val.fct_val = int_saveNet;
    st_set_val_type(tmp, FCT, val);
    st_set_ro(tmp);

    tmp = st_insert("saveResult");
    val.fct_val = int_saveResult;
    st_set_val_type(tmp, FCT, val);
    st_set_ro(tmp);

    tmp = st_insert("trainNet");
    val.fct_val = int_trainNet;
    st_set_val_type(tmp, FCT, val);
    st_set_ro(tmp);

    tmp = st_insert("testNet");
    val.fct_val = int_testNet;
    st_set_val_type(tmp, FCT, val);
    st_set_ro(tmp);

    tmp = st_insert("pruneNet");
    val.fct_val = int_pruneNet;
    st_set_val_type(tmp, FCT, val);
    st_set_ro(tmp);

    tmp = st_insert("pruneTrainNet");
    val.fct_val = int_pruneTrainNet;
    st_set_val_type(tmp, FCT, val);
    st_set_ro(tmp);

    tmp = st_insert("pruneNetNow");
    val.fct_val = int_pruneNetNow;
    st_set_val_type(tmp, FCT, val);
    st_set_ro(tmp);

    tmp = st_insert("setActFunc");
    val.fct_val = int_setActFunc;
    st_set_val_type(tmp, FCT, val);
    st_set_ro(tmp);

    tmp = st_insert("delCandUnits");
    val.fct_val = int_delCandUnits;
    st_set_val_type(tmp, FCT, val);
    st_set_ro(tmp);

    tmp = st_insert("loadPattern");
    val.fct_val = int_loadPattern;
    st_set_val_type(tmp, FCT, val);
    st_set_ro(tmp);

    tmp = st_insert("setPattern");
    val.fct_val = int_setPattern;
    st_set_val_type(tmp, FCT, val);
    st_set_ro(tmp);

    tmp = st_insert("delPattern");
    val.fct_val = int_delPattern;
    st_set_val_type(tmp, FCT, val);
    st_set_ro(tmp);

    tmp = st_insert("setSeed");
    val.fct_val = int_setSeed;
    st_set_val_type(tmp, FCT, val);
    st_set_ro(tmp);

    tmp = st_insert("jogWeights");
    val.fct_val = int_jogWeights;
    st_set_val_type(tmp, FCT, val);
    st_set_ro(tmp);

    tmp = st_insert("jogCorrWeights");
    val.fct_val = int_jogCorrWeights;
    st_set_val_type(tmp, FCT, val);
    st_set_ro(tmp);


    /* built-in variables:
    */
    tmp = st_insert("SSE");
    val.real_val = MAXFLOAT;
    st_set_val_type(tmp, REAL, val);
    st_set_ro(tmp);

    tmp = st_insert("MSE");
    val.real_val = MAXFLOAT;
    st_set_val_type(tmp, REAL, val);
    st_set_ro(tmp);

    tmp = st_insert("SSEPU");
    val.real_val = MAXFLOAT;
    st_set_val_type(tmp, REAL, val);
    st_set_ro(tmp);

    tmp = st_insert("PAT");
    val.int_val = 0;
    st_set_val_type(tmp, INT, val);
    st_set_ro(tmp);

    tmp = st_insert("CYCLES");
    val.int_val = 0;
    st_set_val_type(tmp, INT, val);
    st_set_ro(tmp);

    tmp = st_insert("SIGNAL");
    val.int_val = 0;
    st_set_val_type(tmp, INT, val);
    st_set_ro(tmp);

    tmp = st_insert("EXIT_CODE");
    val.int_val = 0;
    st_set_val_type(tmp, INT, val);
    st_set_ro(tmp);
}


/*****************************************************************************
  FUNCTION : st_insert

  PURPOSE  : looks up name in the ST and inserts it, if it is not already
             there
  RETURNS  : the ST position of name
  NOTES    :

  UPDATE   :
******************************************************************************/
St_ptr_type st_insert(const char *name) {
    size_t new_st_size;

    /* if name is found, return its position: */
    St_ptr_type tmp = st_lookup(name);
    if (tmp != ST_NULL) return tmp;

    D( printf("st entry: %d\n",st_pos); )

    /* allocate new memory if necessary: */
    if(st_pos == st_size) {
        /* increase the size of st multiplicatively (double) so that the total
           copying cost across all growths stays amortized O(1) per insert: */
        new_st_size = st_size * 2;
        /* reallocate memory with self-made realloc-function: */
        st = re_malloc(st,
                       (size_t) (st_size * sizeof (St_type)),
                       (size_t) (new_st_size * sizeof (St_type)));

        /* store the new st size for the next re_malloc:*/
        st_size = new_st_size;

        D( printf("st_insert: re_malloc newsize = %d\n", st_size); st_out();)
    }

    /* insert name in the ST name field: */
    st[st_pos].name = malloc(strlen(name) + 1);
    if (st[st_pos].name == NULL) err_prt(ERR_MEM);

    strcpy(st[st_pos].name, name);

    /* preset the other ST fields: */
    st[st_pos].type = UNKNOWN;
    /* let it be read-write: */
    st[st_pos].read_only = FALSE;

    /* keep the hash index load factor below 3/4; double it (rehashing the
       existing entries) before it gets too full: */
    if ((size_t) (st_pos + 1) * 4 >= st_hash_size * 3)
        st_hash_build(st_hash_size * 2);

    /* register the new entry in the hash index: */
    st_hash_put(st_pos);

    return st_pos ++;
}


/*****************************************************************************
  FUNCTION : st_lookup

  PURPOSE  : searches the symbol table for name
  RETURNS  : name's position if found or ST_NULL if not
  NOTES    :

  UPDATE   :
******************************************************************************/
St_ptr_type st_lookup(const char *name) {
    size_t mask, h;
    St_ptr_type idx;

    if (st_hash == NULL) return ST_NULL;   /* table not initialised yet */

    mask = st_hash_size - 1;
    h = (size_t) (st_hashval(name) & mask);

    /* probe linearly until the name or an empty slot is found: */
    while ((idx = st_hash[h]) != ST_NULL) {
        if (strcmp(name, st[idx].name) == 0) return idx;
        h = (h + 1) & mask;
    }

    return ST_NULL;
}


/*****************************************************************************
  FUNCTION : st_get_val_type

  PURPOSE  : get value and type of the ST entry pointed to by pos
  RETURNS  : alters the type and val arguments
  NOTES    :

  UPDATE   :
******************************************************************************/
void st_get_val_type(St_ptr_type pos, Data_type *type, Val_type *val) {
    *type = st[pos].type;
    *val = st[pos].val;
}


/*****************************************************************************
  FUNCTION : st_set_val_type

  PURPOSE  : set value and type of the ST entry pointed to by pos
  RETURNS  :
  NOTES    :

  UPDATE   :
******************************************************************************/
void st_set_val_type(St_ptr_type pos, Data_type type, Val_type val) {
    st[pos].type = type;
    st[pos].val = val;
}


/*****************************************************************************
  FUNCTION : st_set_ro

  PURPOSE  : sets the read-only field of the ST entry pointed to by pos
  RETURNS  :
  NOTES    :

  UPDATE   :
******************************************************************************/
void st_set_ro(St_ptr_type pos) {
    st[pos].read_only = TRUE;
}


/*****************************************************************************
  FUNCTION : st_get_ro

  PURPOSE  : reads the read-only field of the ST entry pointed to by pos
  RETURNS  : TRUE for read-only, FALSE for read-write
  NOTES    :

  UPDATE   :
******************************************************************************/
Bool_type st_get_ro(St_ptr_type pos) {
    return st[pos].read_only;
}


/*****************************************************************************
  FUNCTION : newtmp

  PURPOSE  : creates a new unique name for a temporary variable
             name begins with an integer followed by TMP_STR
  RETURNS  : a pointer to the new name
  NOTES    : subsequent calls to newtmp overwrite the created names
             -> returned name should be used immediately
  UPDATE   :
******************************************************************************/
char *newtmp(void) {
    static int num = 0;
    static char tmp[20];

    sprintf(tmp, "%d%s", num, TMP_STR);
    num ++;
    return tmp;
}


/*****************************************************************************
  FUNCTION : st_out

  PURPOSE  : prints out the entire symbol table
  RETURNS  :
  NOTES    : debugging use only

  UPDATE   :
******************************************************************************/
void st_out(void) {
    St_ptr_type a;

    printf("\nsymbol table:\n");
    printf("entry name status type value\n");

    for(a = 0; a < st_pos; a++) {
        printf(" %d  %s  %s  ",
               a, st[a].name, (st[a].read_only==1)?"rd-only":"rd-write" );

        switch(st[a].type) {
        case UNKNOWN:
            printf("UNKNOWN\n");
            break;
        case INT:
            printf("INT %d\n", st[a].val.int_val);
            break;
        case REAL:
            printf("REAL %g\n", st[a].val.real_val);
            break;
        case BOOL:
            printf("BOOL %d\n", st[a].val.bool_val);
            break;
        case FCT:
            printf("FCT\n");
            break;
        case STRING:
            printf("STRING %s\n", st[a].val.string_val);
            break;
        default:
            printf("*** Unallowed type in ST!\n");
        }
    }
}


