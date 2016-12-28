set(KERNELSOURCES_C    sources/kernel.c
                       sources/kr_io.c
                       sources/kr_mem.c
                       sources/kr_ui.c
                       sources/kr_inversion.c
                       sources/art_ui.c
                       sources/kr_art.c
                       sources/kr_art1.c
                       sources/kr_art2.c
                       sources/kr_amap.c
                       sources/kr_newpattern.c
                       sources/kr_JordElm.c
		       sources/kr_rand.c)

set(USR_FUNC_SOURCES_C sources/func_tbl.c
                       sources/init_f.c
                       sources/kr_td.c
                       sources/kr_funcs.c
		       sources/learn_f.c
		       sources/prun_f.c
		       sources/tbl_func.c
                       sources/trans_f.c
                       sources/update_f.c
                       sources/remap_f.c
                       sources/matrix.c
                       sources/arttr_f.c
                       sources/cc_learn.c
                       sources/cc_prune.c
                       sources/cc_modify.c
                       sources/cc_glob.c
                       sources/cc_display.c
                       sources/tacoma_learn.c
                       sources/dlvq_learn.c
                       sources/stochastic_learn_f.c
		       sources/scaled_conj_grad.c)

set(UTILITY_SOURCES_C  sources/make_tbl.c
                       sources/calc_err.c)

set(KERNELSOURCES_YACC sources/kr_pat_parse_bison.y)
set(KERNELSOURCES_LEX  sources/kr_pat_scan.l)

find_package(BISON REQUIRED)
find_package(FLEX  REQUIRED)

bison_target(kr_pat_parse ${KERNELSOURCES_YACC} ${CMAKE_BINARY_DIR}/kr_pat_scan.c
             COMPILE_FLAGS "-d -t -y -p pp"
	     DEFINES_FILE ${CMAKE_BINARY_DIR}/y.tab.h)
flex_target(kr_pat_scan ${KERNELSOURCES_LEX}    ${CMAKE_BINARY_DIR}/kr_pat_parse.c)
add_flex_bison_dependency(kr_pat_scan kr_pat_parse)

include_directories(${CMAKE_BINARY_DIR}
                    ${CMAKE_SOURCE_DIR}
                    sources)

add_library(kernel STATIC ${KERNELSOURCES_C}
                          ${BISON_kr_par_parse_OUTPUTS}
			  ${CMAKE_BINARY_DIR}/kr_pat_scan.c
			  ${CMAKE_BINARY_DIR}/kr_pat_parse.c)

if (SNNS_ENABLE_ENZO)
  set_target_properties(kernel PROPERTIES COMPILE_DEFINITIONS "__ENZO__")
endif ()

add_library(func STATIC ${USR_FUNC_SOURCES_C}
                        ${UTILITY_SOURCES_C})
