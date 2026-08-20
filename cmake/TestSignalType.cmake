# cmake/TestSignalType.cmake
#
# Sets RETSIGTYPE, the return type of signal handlers passed to signal().
#
# This used to run a try_compile() probe (see git history) for an
# ambiguity between pre-POSIX Unix variants where some declared signal
# handlers as returning int and others void. POSIX settled on void
# decades ago, and every target this project actually builds for
# (glibc/Linux, macOS/BSD libc) uses it unconditionally. The old probe
# relied on re-declaring signal() with an obsolescent K&R-style empty
# parameter list, which strict C23 compilers now reject outright as a
# conflicting declaration - always falling through to the wrong "int"
# answer instead of reporting the real, unambiguous value.

set(RETSIGTYPE void CACHE INTERNAL "Signal return type")
