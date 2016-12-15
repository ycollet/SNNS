/* -------------------------------------------------------------------------- */
/*                                                                            */
/*    my_module.c                                                             */
/*                                                                            */
/*    Try to start off with this file in implementing a new module.           */
/*    Replace `my_module' with the name of your module and                    */
/*    fill in the body as you like!                                           */
/*                                                                            */
/*                                                     --- author, date ---   */
/*                                                       --- js, 8.2.94 ---   */
/*                                                                            */
/* -------------------------------------------------------------------------- */


#include "enzo.h"
#include "my_module.h"


#define MY_MODULE_KEY  "my_module"
#define MY_PARAM       "myParam"



int my_param = 42;  /* use a meaningful value as default */



int my_module_init( ModuleTableEntry *self, int msgc, char *msgv[] )
{
    MODULE_KEY( MY_MODULE_KEY );

    SEL_MSG( msgv[0] )

    MSG_CASE( GENERAL_INIT    ) { /* nothing to do */ }
    MSG_CASE( GENERAL_EXIT    ) { /* nothing to do */ }
    MSG_CASE( MY_PARAM        ) { if( msgc > 1 )
				      my_param = atoi( msgv[1] );
			        }
    END_MSG;

    return( INIT_USED );
}


int my_module_work( PopID *parents, PopID *offsprings, PopID *ref )
{
    NetID net;
    
    /* do whatever you want to do in here */
    /* return MODULE_NO_ERROR, if everything's OK, an error_code else */

    FOR_ALL_PARENTS( net )
    {
	/* doing something with the parents */
    }

    FOR_ALL_OFFSPRINGS( net )
    {
	/* doing something with the offsprings */
    }

    return( MODULE_NO_ERROR );
}


char *my_module_errMsg( int err_code )
{
    /* supply the caller with some information about an error */

    static int   err_cnt   = 2;   /* number of recognized errors */
    static char *err_msg[] =
    { "no error (my_module)", "unknown error (my_module)",
      "specific error message -- not used"};

    return( err_msg[ err_code < err_cnt ? err_code : MODULE_UNKNOWN_ERR ] );
}
