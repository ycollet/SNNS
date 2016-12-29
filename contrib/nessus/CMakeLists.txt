include_directories(${CMAKE_SOURCE_DIR}/contrib/nessus/compiler/sources
                    ${CMAKE_BINARY_DIR})
                    
add_definitions(-DYYTOKENTYPE)

set(COMPOBJECTS compiler/sources/co_main.c
	        compiler/sources/co_symTab.c
	        compiler/sources/co_error.c
	        compiler/sources/co_output.c
		compiler/sources/co_cFuncs.c
		compiler/sources/co_conns.c
		compiler/sources/co_strings.c
		compiler/sources/co_mem.c
		compiler/sources/co_pFuncs.c
		compiler/sources/co_expr.c
		compiler/sources/co_stmt.c
		compiler/sources/co_struct.c
		compiler/sources/co_sites.c
		compiler/sources/co_io.c
		compiler/sources/co_logical.c
		compiler/sources/co_params.c
		compiler/sources/co_loop.c)

find_package(BISON REQUIRED)
find_package(FLEX  REQUIRED)

bison_target(nessus_parse compiler/sources/Syntax.y ${CMAKE_BINARY_DIR}/co_parser.c
             COMPILE_FLAGS "-d -t -y -p pp"
	     DEFINES_FILE ${CMAKE_BINARY_DIR}/y.tab.h)
flex_target(nessus_scan compiler/sources/Tokens.l ${CMAKE_BINARY_DIR}/co_scanner.c)
add_flex_bison_dependency(nessus_scan nessus_parse)

add_executable(nessus ${COMPOBJECTS}
                      ${CMAKE_BINARY_DIR}/co_parser.c
                      ${CMAKE_BINARY_DIR}/co_scanner.c)

#file(STRINGS ${CMAKE_BINARY_DIR}/co_parser.c TEMP1 REGEX "# define YYMAXDEPTH")
#file(STRINGS ${CMAKE_BINARY_DIR}/co_parser.c TEMP2 REGEX "# define YLMAX")
file(COPY compiler/sources/co_pfBody.h
     DESTINATION ${CMAKE_BINARY_DIR})
file(RENAME ${CMAKE_BINARY_DIR}/co_pfBody.h ${CMAKE_BINARY_DIR}/co_pFuncs.h)
#file(APPEND ${CMAKE_BINARY_DIR}/co_pFuncs.h "${TEMP1}\n")
file(APPEND ${CMAKE_BINARY_DIR}/co_pFuncs.h "#define YYMAXDEPTH 256\n")
file(APPEND ${CMAKE_BINARY_DIR}/co_pFuncs.h "#define YYLMAX 2048\n")

message(STATUS "TEMP1 = ${TEMP1}")
message(STATUS "TEMP2 = ${TEMP2}")

#co_scanner.c: Tokens.l co_scanner.h
#		lex Tokens.l 
#		mv lex.yy.c co_scanner.c


