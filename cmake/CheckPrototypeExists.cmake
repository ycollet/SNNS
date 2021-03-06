# AWI, downloaded from KDE repository since has not yet been transferred
# to cmake repository as of 2006-07-31.
# http://websvn.kde.org/trunk/KDE/kdelibs/cmake/modules/CheckPrototypeExists.cmake?rev=505849&view=markup
#
# - Check if the prototype for a function exists.
# CHECK_PROTOTYPE_EXISTS (FUNCTION HEADER VARIABLE)
#
#  FUNCTION - the name of the function you are looking for
#  HEADER - the header(s) where the prototype should be declared
#  VARIABLE - variable to store the result
#

include(CheckCXXSourceCompiles)

macro(CHECK_PROTOTYPE_EXISTS _SYMBOL _HEADER _RESULT)
  set(_INCLUDE_FILES)
  foreach(it ${_HEADER})
    set(_INCLUDE_FILES "${_INCLUDE_FILES}#include <${it}>\n")
  endforeach(it)
  
  set(_CHECK_PROTO_EXISTS_SOURCE_CODE "
${_INCLUDE_FILES}
void cmakeRequireSymbol(int dummy,...){(void)dummy;}
int main()
{
#ifndef ${_SYMBOL}
#ifndef _MSC_VER
  cmakeRequireSymbol(0,&${_SYMBOL});
#else
  char i = sizeof(&${_SYMBOL});
#endif
#endif
  return 0;
}
")
  check_cxx_source_compiles("${_CHECK_PROTO_EXISTS_SOURCE_CODE}" ${_RESULT})
endmacro(CHECK_PROTOTYPE_EXISTS _SYMBOL _HEADER _RESULT)
