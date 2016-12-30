/*
 * File:     (%W%    %G%)
 * Purpose:  definition of main functions
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
 * Implementation:   1.0
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

#include <signal.h>

#include "enzo.h"

/* ------------------------------------------------------------ constants --- */
/*                                                                            */

#define MAX_LTABLE_ENTRIES   10    /* max. number of localTable-entries       */

#define MAX_CMD_LINE        128    /* max. length of a line in command-file   */
#define MAX_MSGV             64    /* max. number of tokens per command-line  */
#define DELIMITERS        " \t"    /* delimiters for cmd-line tokens          */

#define RESET_LTABLE(t)   { int i;		\
    for(i=0;i<MAX_LTABLE_ENTRIES;i++)		\
      {						\
	t[i].initFct = NULL;			\
	t[i].workFct = NULL;			\
	t[i].errFct = NULL;			\
	t[i].type=t[i].flags=0;			\
      } }

/*                                                                            */
/* -------------------------------------------------------------------------- */

/* ---------------------------------------------------------- global vars --- */
/*                                                                            */

int signal_evolution = FALSE;

/*                                                                            */
/* -------------------------------------------------------------------------- */

/* ----------------------------------------------------------- local vars --- */
/*                                                                            */

static int stop_evolution = DO_EVOLUTION;  /* evolution is still in progress  */

/*                                                                            */
/*  The following tables are local and will contain copies from the global    */
/*  module-table, that are marked `active', so there will be no loss in       */
/*  performance.                                                              */
/*                                                                            */

static ModuleTableEntry preTable       [MAX_LTABLE_ENTRIES],
  stopTable      [MAX_LTABLE_ENTRIES],
  selectionTable [MAX_LTABLE_ENTRIES],
  crossoverTable [MAX_LTABLE_ENTRIES],
  mutationTable  [MAX_LTABLE_ENTRIES],
  optTable       [MAX_LTABLE_ENTRIES],
  evalTable      [MAX_LTABLE_ENTRIES],
  historyTable   [MAX_LTABLE_ENTRIES],
  survivalTable  [MAX_LTABLE_ENTRIES],
  postTable      [MAX_LTABLE_ENTRIES];

/* poulations for the GA to work with                                         */

static PopID parents    = NULL,
  offsprings = NULL,
  reference  = NULL;  /* contains the max. network                 */

static FILE *lfp;      /* log-file-pointer */

/*                                                                            */
/* -------------------------------------------------------------------------- */

/* ----------------------------------------------------- local utilities  --- */
/*                                                                            */

static char *timestr( void ) {
  long clock;
  clock = time( NULL );
  return( ctime( (time_t *) &clock ) );
}

static char *trimStr( char *string ) {
  char *c, *s = string;

  /* Both '%' and '#' are provided as comment characters;                  */
  /* the use of '#' is not recommanded as it interferes with cpp.          */
  if( (c = strstr( s, "#"  )) )  *c = '\0';      /* delete comments        */
  if( (c = strstr( s, "%"  )) )  *c = '\0';      /* delete comments        */

  if( (c = strstr( s, "\n" )) )  *c = '\0';      /* delete newlines        */

  while( *s  && (*s == ' '  || *s == '\t') ) s++; /* delete leading blanks  */

  return( s );
}

static void show_usage( char *prg_name ) {
  printf( "usage: %s command_file [ logfile [seed] ]\n", prg_name );
}

static void reset_localTables( void ) {
  RESET_LTABLE( preTable );
  RESET_LTABLE( stopTable );
  RESET_LTABLE( selectionTable );
  RESET_LTABLE( crossoverTable );
  RESET_LTABLE( mutationTable );
  RESET_LTABLE( optTable );
  RESET_LTABLE( evalTable );
  RESET_LTABLE( historyTable );
  RESET_LTABLE( survivalTable );
  RESET_LTABLE( postTable );
}

static int splitLine( char *line, char *argv[] ) {
  int argc = 0;

  for( argv[  argc]  = strtok( line, DELIMITERS );
       argv[  argc] != NULL;
       argv[++argc]  = strtok( NULL, DELIMITERS ) );

  return( argc );
}

static int handleError( int err_code, char *err_msg ) {
  if( err_code == MODULE_NO_ERROR ) return( MODULE_NO_ERROR );

  fprintf( lfp, "%s: %s\n", err_code > 0 ? "ERROR" : "WARNING", err_msg );
  if( err_code > 0 ) return( ERR_EVOLUTION );
  else               return( WARN_EVOLUTION );
}

static void broadcast_message( char *msg, int ignore_active, void *data ) {
  ModuleTableEntry *module;
  char *msgv[2];
  int   msgc = 0, err_code = INIT_NOT_USED;

  msgv[0] = msg;
  msgc++;
  msgv[1] = (char *) data;
  msgc++; /* one more supplementary data          */

  for( module = ModuleTable;
       (err_code == INIT_USED || err_code == INIT_NOT_USED )
	 && module->initFct; module++ ) {
    if( ignore_active || MODULE_ACTIVE(module) )
      if( (err_code = module->initFct( module, msgc, msgv ) != INIT_USED ) )
	stop_evolution = handleError( err_code, module->errFct( err_code ) );
  }
}

void catch_signal( void ) {
  enzo_logprint( "\n\ncought a signal, stopping evolution!\n\n" );
  signal_evolution = TRUE;
  return;
}

/*                                                                            */
/* -------------------------------------------------------------------------- */

/* ---------------------------------------------------- localTable-module --- */
/*                                                                            */
/*    I'm sorry, but this module has to be in here, because it needs access   */
/*    to the local-function-tables.                                           */
/*                                                                            */
/*  this is no longer a module by itself; use kpm_actModule()                 */

#define LT_PRE_EXCESS         2
#define LT_STOP_EXCESS        3
#define LT_SELECTION_EXCESS   4
#define LT_CROSSOVER_EXCESS   5
#define LT_MUTATION_EXCESS    6
#define LT_OPT_EXCESS         7
#define LT_EVAL_EXCESS        8
#define LT_HIST_EXCESS        9
#define LT_SURVIVAL_EXCESS   10
#define LT_POST_EXCESS       11
#define LT_UNKNOWN_TYPE      12

static void insertTableEntry( ModuleTableEntry t[], int cnt,
                              ModuleTableEntry e,   int prio ) {
  cnt--;  /* cnt-1 is the last entry made yet */
  while( cnt >= 0 && t[cnt].flags > prio ) {
    t[cnt+1] = t[cnt];
    cnt--;
  }
  t[cnt+1] = e;
}

static int copyModuleEntry( ModuleTableEntry entry, int prio ) {
  static int preTable_cnt       = 0, stopTable_cnt      = 0,
    selectionTable_cnt = 0, crossoverTable_cnt = 0,
    mutationTable_cnt  = 0, optTable_cnt       = 0,
    evalTable_cnt      = 0, historyTable_cnt   = 0,
    survivalTable_cnt  = 0, postTable_cnt      = 0;

  ModuleTableEntry *localTable;
  int localCnt;

  switch( entry.type ) {
  case MTYPE_PRE: {
    if( preTable_cnt >= MAX_LTABLE_ENTRIES )
      return( LT_PRE_EXCESS );
    localTable = preTable;
    localCnt = preTable_cnt++;
    break;
  }
  case MTYPE_STOP: {
    if( stopTable_cnt >= MAX_LTABLE_ENTRIES )
      return( LT_STOP_EXCESS );
    localTable = stopTable;
    localCnt = stopTable_cnt++;
    break;
  }
  case MTYPE_SELECTION: {
    if( selectionTable_cnt >= MAX_LTABLE_ENTRIES )
      return( LT_SELECTION_EXCESS );
    localTable = selectionTable;
    localCnt = selectionTable_cnt++;
    break;
  }
  case MTYPE_CROSSOVER: {
    if( crossoverTable_cnt >= MAX_LTABLE_ENTRIES )
      return( LT_CROSSOVER_EXCESS );
    localTable = crossoverTable;
    localCnt = crossoverTable_cnt++;
    break;
  }
  case MTYPE_MUTATION: {
    if( mutationTable_cnt >= MAX_LTABLE_ENTRIES )
      return( LT_MUTATION_EXCESS );
    localTable = mutationTable;
    localCnt = mutationTable_cnt++;
    break;
  }
  case MTYPE_OPT: {
    if( optTable_cnt >= MAX_LTABLE_ENTRIES )
      return( LT_OPT_EXCESS );
    localTable = optTable;
    localCnt = optTable_cnt++;
    break;
  }
  case MTYPE_EVAL: {
    if( evalTable_cnt >= MAX_LTABLE_ENTRIES )
      return( LT_EVAL_EXCESS );
    localTable = evalTable;
    localCnt = evalTable_cnt++;
    break;
  }
  case MTYPE_HISTORY: {
    if( evalTable_cnt >= MAX_LTABLE_ENTRIES )
      return( LT_HIST_EXCESS );
    localTable = historyTable;
    localCnt = historyTable_cnt++;
    break;
  }
  case MTYPE_SURVIVAL: {
    if( survivalTable_cnt >= MAX_LTABLE_ENTRIES )
      return( LT_SURVIVAL_EXCESS );
    localTable = survivalTable;
    localCnt = survivalTable_cnt++;
    break;
  }
  case MTYPE_POST: {
    if( postTable_cnt >= MAX_LTABLE_ENTRIES )
      return( LT_POST_EXCESS );
    localTable = postTable;
    localCnt = postTable_cnt++;
    break;
  }
  default:
    return( LT_UNKNOWN_TYPE );
  }

  insertTableEntry( localTable, localCnt, entry, prio );

  return( MODULE_NO_ERROR );
}

static char *localTables_errMsg( int err_code ) {
  static  int err_cnt = 13;      /* number of passible error-codes */

  static char *erray[] = {
    "no error",  "unknown",  "pre", "stop", "selection",
    "crossover", "mutation", "opt", "eval", "history",
    "survival",  "post",     "unknown type for module"
  };

  static char err_msg_fmt[] =
    "Capacity of %s-buffer exceeded. Enlarge MAX_LTABLE_SIZE.";
  static char err_msg[128];

  if( err_code > MODULE_UNKNOWN_ERR )
    sprintf( err_msg, err_msg_fmt,
	     err_code < err_cnt ? erray[err_code] : erray[MODULE_UNKNOWN_ERR] );
  else strcpy( err_msg, erray[ err_code > err_cnt ? 2 : err_code ] );

  return( err_msg );
}

/*                                                                            */
/* -------------------------------------------------------------------------- */

/* ----------------------------------------------------- public functions --- */
/*                                                                            */

void enzo_actModule( ModuleTableEntry *module, int prio ) {
  int err_code;

  if( (err_code = copyModuleEntry( *module, prio ) != MODULE_NO_ERROR) ) {
    handleError( err_code, localTables_errMsg( err_code ) );
  }
  module->flags = TRUE;
}

void enzo_logprint( char *fmt, ... ) {
  va_list  args;

  va_start( args, fmt );

  (void ) vfprintf( lfp, fmt, args );

  va_end( args );
  fflush( lfp );
}

/*                                                                            */
/* -------------------------------------------------------------------------- */

/* ------------------------------------------------------------ init/exit --- */
/*                                                                            */

static void initialize_all( int argc, char *argv[] ) {
  ModuleTableEntry *module;

  char  cmd_line[ MAX_CMD_LINE ];
  int   msgc;
  char *msg, *msgv[ MAX_MSGV ];

  char *cmd_file = NULL,
    *log_file = NULL;

  FILE *cfp;
  int   used, rand_seed;

  /* --- command line handling -------------------------------------------- */

  if( argc < 2 ) {
    show_usage(  argv[0] );
    exit( 0 );
  } else           {
    cmd_file  =  argv[1];
  }

  if( argc > 2 ) {
    log_file  =  argv[2];
  }

  if( argc > 3 ) {
    rand_seed =  atoi( argv[3] );
  } else           {
    rand_seed =  (int) time( NULL );
  }

  if( (cfp = fopen( cmd_file, "r" )) == NULL ) {
    fprintf( stderr, "ERROR: Can't open command-file!\n" );
    exit( 1 );
  }

  if( log_file ) {
    if( (lfp = fopen( log_file, "w" )) == NULL ) {
      fprintf( stderr, "ERROR: Can't open log-file!\n" );
      exit( 1 );
    }
  } else  lfp = stderr;

  enzo_logprint( "init start:        %s", timestr() );
  enzo_logprint( "seed == %ld\n", rand_seed );
  ksh_setSeedNo( rand_seed );
  srand48       ( rand_seed );

  /* --- setting local vars ----------------------------------------------- */

  reset_localTables();

  /* --- init modules ----------------------------------------------------- */

  broadcast_message( GENERAL_INIT, TRUE, NULL );

  /* --- handle command-file ---------------------------------------------- */

  while( fgets( cmd_line, MAX_CMD_LINE, cfp ) != NULL ) {
    if( *(msg = trimStr(cmd_line)) != '\0' ) {
      msgc = splitLine( msg, msgv );

      used = FALSE;
      for( module = ModuleTable; module->initFct; module++ ) {
	used += module->initFct( module, msgc, msgv );
      }

      if( !used ) { /* seems to be a couch-potatoe */
	fprintf( stderr, "Something's wrong with cmd-line: %s\n", msg );
	exit( 1 );
      }
    }
  }

  fclose( cfp );

  signal( SIGTERM, catch_signal );
  signal( SIGHUP, catch_signal );
  signal( SIGINT, catch_signal );
}

static void exit_all( void ) {
  /* --- exit modules ----------------------------------------------------- */

  broadcast_message( GENERAL_EXIT, TRUE, NULL );

  /* we arrived at the end of the universe ... dinner's ready.   */
}

/*                                                                            */
/* -------------------------------------------------------------------------- */

/* ------------------------------------------------------------- genetics --- */
/*                                                                            */

static void pre_evolution( void ) {
  int i, err_code;

  if( stop_evolution > 0 ) return;

  for( i = 0;
       i < MAX_LTABLE_ENTRIES && preTable[i].workFct
	 && stop_evolution == DO_EVOLUTION;              i++ ) {
    if( (err_code =
	 preTable[i].workFct( &parents, &offsprings, &reference )) ) {
      stop_evolution = handleError( err_code,
				    preTable[i].errFct(err_code) );
    }
  }
  if( !stop_evolution )
    broadcast_message( EVOLUTION_INIT, FALSE, (void *) &reference );

  else {
    /* TR: break with error message */
    fprintf(stderr,"An ERROR occurred during pre-evolution. Evolution stopped !\n");
    exit(1);
  }
}

static void test_evolution( void ) {
  int i;

  if( stop_evolution > 0 ) return;

  for( i = 0; i < MAX_LTABLE_ENTRIES && stopTable[i].workFct
	 && stop_evolution == DO_EVOLUTION;              i++ ) {
    if( stopTable[i].workFct( &parents, &offsprings, &reference ) ) {
      stop_evolution = STOP_EVOLUTION;
    }
  }
}

static void selection( void ) {
  int i, err_code;

  if( stop_evolution > 0 ) return;

  for( i = 0; i < MAX_LTABLE_ENTRIES && selectionTable[i].workFct
	 && stop_evolution == DO_EVOLUTION;              i++ ) {
    if( (err_code = selectionTable[i].workFct( &parents, &offsprings,
					       &reference )) ) {
      stop_evolution = handleError( err_code,
				    selectionTable[i].errFct(err_code) );
    }
  }
}

static void crossover( void ) {
  int i, err_code;

  if( stop_evolution > 0 ) return;

  for( i = 0; i < MAX_LTABLE_ENTRIES && crossoverTable[i].workFct
	 && stop_evolution == DO_EVOLUTION;              i++ ) {
    if( (err_code = crossoverTable[i].workFct( &parents, &offsprings,
					       &reference  )) ) {
      stop_evolution = handleError( err_code,
				    crossoverTable[i].errFct(err_code) );
    }
  }
}

static void mutation( void ) {
  int i, err_code;

  if( stop_evolution > 0 ) return;

  for( i = 0; i < MAX_LTABLE_ENTRIES && mutationTable[i].workFct
	 && stop_evolution == DO_EVOLUTION;              i++ ) {
    if( (err_code = mutationTable[i].workFct( &parents, &offsprings,
					      &reference  )) ) {
      stop_evolution = handleError( err_code,
				    mutationTable[i].errFct(err_code) );
    }
  }
}

static void optimization( void ) {
  int i, err_code;

  if( stop_evolution > 0 ) return;

  for( i = 0; i < MAX_LTABLE_ENTRIES && optTable[i].workFct
	 && stop_evolution == DO_EVOLUTION;              i++ ) {
    if( (err_code =
	 optTable[i].workFct( &parents, &offsprings, &reference  )) ) {
      stop_evolution = handleError( err_code,
				    optTable[i].errFct(err_code) );
    }
  }
}

static void evaluation( void ) {
  int i, err_code;

  if( stop_evolution > 0 ) return;

  for( i = 0; i < MAX_LTABLE_ENTRIES && evalTable[i].workFct
	 && stop_evolution == DO_EVOLUTION;              i++ ) {
    if( (err_code = evalTable[i].workFct( &parents, &offsprings,
					  &reference  )) ) {
      stop_evolution = handleError( err_code,
				    evalTable[i].errFct(err_code) );
    }
  }
}

static void history( void ) {
  int i, err_code;

  if( stop_evolution > 0 ) return;

  for( i = 0; i < MAX_LTABLE_ENTRIES && historyTable[i].workFct
	 && stop_evolution == DO_EVOLUTION;              i++ ) {
    if( (err_code = historyTable[i].workFct( &parents, &offsprings,
					     &reference  )) ) {
      stop_evolution = handleError( err_code,
				    historyTable[i].errFct(err_code) );
    }
  }
}

static void survival( void ) {
  int i, err_code;

  if( stop_evolution > 0 ) return;

  for( i = 0; i < MAX_LTABLE_ENTRIES && survivalTable[i].workFct
	 && stop_evolution == DO_EVOLUTION;              i++ ) {
    if( (err_code = survivalTable[i].workFct( &parents, &offsprings,
					      &reference  )) ) {
      stop_evolution = handleError( err_code,
				    survivalTable[i].errFct(err_code) );
    }
  }
}

static void post_evolution( void ) {
  int i, err_code;

  for( i = 0; i < MAX_LTABLE_ENTRIES && postTable[i].workFct; i++ ) {
    if( (err_code = postTable[i].workFct( &parents, &offsprings,
					  &reference  )) ) {
      stop_evolution = handleError( err_code,
				    postTable[i].errFct(err_code) );
      break;
    }
  }
}

/*                                                                            */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------- not much, eh? --- */
/*                                                                            */

int main( int argc, char *argv[] ) {
  initialize_all( argc, argv );

  enzo_logprint( "preevolution start: %s", timestr() );

  pre_evolution();
  /*    evaluation();  */
  /*    history();     */         /* masch: mich stoeren diese Infos */

  optimization();
  evaluation();
  history();
  survival();

  enzo_logprint( "genetics start:     %s", timestr() );

  while( !stop_evolution && !signal_evolution) {
    test_evolution();
    selection();
    crossover();
    mutation();
    optimization();
    evaluation();
    history();
    survival();
  }

  /* history for the last generation */
  stop_evolution = DO_EVOLUTION;
  history();

  enzo_logprint( "\ngenetics end:       %s", timestr() );

  post_evolution();

  exit_all();
  return( 0 );
}

/*                                                                            */
/* -------------------------------------------------------------------------- */
