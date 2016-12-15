/* -------------------------------------------------------------------------- */
/*                                                                            */
/*    my_module.h                                                             */
/*                                                                            */
/*                                                                            */
/*    Try to start off with this file in implementing a new module.           */
/*    Replace `my_module' with the name of your module and                    */
/*    fill in the body as you like!                                           */
/*                                                                            */
/*    You shoudn't have to change a lot in this h-file.                       */
/*    And don't forget to include it in `m_table.c' ...                       */
/*                                                                            */
/*                                                     --- author, date ---   */
/*                                                       --- js, 8.2.94 ---   */
/*                                                                            */
/* -------------------------------------------------------------------------- */


#ifndef __MY_MODULE__
#define __MY_MODULE__


int   my_module_init( ModuleTableEntry *self, int msgc, char *msgv[] );
int   my_module_work( PopID *parents, PopID *offsprings, PopID *ref );
char *my_module_errMsg( int err_code );


#endif
