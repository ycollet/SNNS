#ifndef _STARTPOP_H
#define _STARTPOP_H


extern int   startPop_init   (ModuleTableEntry *self, int msgc, char *msg[] );
extern int   startPop_work   (PopID *parent,PopID *offsprings,PopID *reference);
extern char *startPop_errMsg (int err_code);

#endif
