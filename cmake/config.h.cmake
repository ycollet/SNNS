/* Define if using alloca.c.  */
/* #undef C_ALLOCA */

/* Define if you have alloca, as a function or macro.  */
#cmakedefine HAVE_ALLOCA

/* Define if you have <alloca.h> and it should be used (not on Ultrix).  */
#cmakedefine HAVE_ALLOCA_H

/* Define if you have <sys/wait.h> that is POSIX.1 compatible.  */
#cmakedefine HAVE_SYS_WAIT_H

/* Define if you need to in order for stat and other things to work.  */
/* #undef _POSIX_SOURCE */

/* Define as the return type of signal handlers (int or void).  */
#define RETSIGTYPE @RETSIGTYPE@

/* Define to `unsigned' if <sys/types.h> doesn't define.  */
#cmakedefine size_t

/* If using the C implementation of alloca, define if you know the
   direction of stack growth for your system; otherwise it will be
   automatically deduced at run-time.
	STACK_DIRECTION > 0 => grows toward higher addresses
	STACK_DIRECTION < 0 => grows toward lower addresses
	STACK_DIRECTION = 0 => direction of growth unknown
 */
/* #undef STACK_DIRECTION */

/* Define if you have the ANSI C header files.  */
#cmakedefine STDC_HEADERS

/* Define if you have xgrabsc installed */
#cmakedefine HAVE_XGRABSC

/* Define if you have the getcwd function.  */
#cmakedefine HAVE_GETCWD

/* Define if you have the lrand48 function.  */
#cmakedefine HAVE_LRAND48

/* Define if you have the strdup function.  */
#cmakedefine HAVE_STRDUP

/* Define if you have the <dirent.h> header file.  */
#cmakedefine HAVE_DIRENT_H

/* Define if you have the <fcntl.h> header file.  */
#cmakedefine HAVE_FCNTL_H

/* Define if you have the <limits.h> header file.  */
#cmakedefine HAVE_LIMITS_H

/* Define if you have the <ndir.h> header file.  */
#cmakedefine HAVE_NDIR_H

/* Define if you have the <sys/dir.h> header file.  */
#cmakedefine HAVE_SYS_DIR_H

/* Define if you have the <sys/file.h> header file.  */
#cmakedefine HAVE_SYS_FILE_H

/* Define if you have the <sys/ndir.h> header file.  */
#cmakedefine HAVE_SYS_NDIR_H

/* Define if you have the <sys/time.h> header file.  */
#cmakedefine HAVE_SYS_TIME_H

/* Define if you have the <unistd.h> header file.  */
#cmakedefine HAVE_UNISTD_H

/* Define if you have the <values.h> header file.  */
#cmakedefine HAVE_VALUES_H

/* AIX requires this to be the first thing in the file.  */
/* don't change the indentation !!!! */
#ifdef __GNUC__
# define alloca __builtin_alloca
#else
# if HAVE_ALLOCA_H
#  include <alloca.h>
# else
#  ifdef _AIX
 #pragma alloca
#  else
#   ifndef alloca /* predefined by HP cc +Olibcalls */
char *alloca (int size);
#   endif
#  endif
# endif
#endif

#include "kernel/sources/alloca.h"
#include "kernel/sources/getcwd.h"
#include "kernel/sources/strdup.h"
#include "kernel/sources/lrand48.h"

