include_directories(${CMAKE_SOURCE_DIR}/kernel/sources
                    ${CMAKE_BINARY_DIR}
                    ${CMAKE_SOURCE_DIR}
		    sources)

set(TOOLS_SOURCES_C sources/analyze.c
		    sources/pat_sel_simple.c
                    sources/mkhead.c
                    sources/mkout.c
                    sources/mkpat.c)

set(KRTOOLS_SOURCES_C sources/snns2c.c
                      sources/snns2clib.c
                      sources/pat_sel.c
                      sources/netlearn.c
                      sources/td_bignet.c
                      sources/ff_bignet.c
                      sources/netperf.c
                      sources/snnsbat.c
                      sources/feedback-gennet.c
                      sources/convert2snns.c
                      sources/isnns.c
                      sources/linknets.c)

set(BATCH_SOURCES_YACC sources/gram1.y)
set(BATCH_SOURCES1_LEX sources/scan1.l)
set(BATCH_SOURCES2_LEX sources/scan2.l)

set(BATCH_SOURCES_C sources/batchman.c
                    sources/icopjmp.c
                    sources/ictab.c
                    sources/symtab.c
                    sources/backpatch.c
                    sources/memory.c
                    sources/ic_snns.c
                    sources/error.c
                    sources/arglist.c
                    sources/snns_intfce.c
                    sources/special_jacket.c)

set(SNNS2C_SRCS sources/snns2c.c
                sources/snns2clib.c)

set(PAT_SEL_SRCS sources/pat_sel.c)
set(NETLEARN_SRCS sources/netlearn.c)
set(TD_BIGNET_SRCS sources/td_bignet.c)
set(FF_BIGNET_SRCS sources/ff_bignet.c)
set(NETPERF_SRCS sources/netperf.c)
set(SNNSBAT_SRCS sources/snnsbat.c)
set(FEEDBACK_GENNET_SRCS sources/feedback-gennet.c)
set(CONVERT2SNNS_SRCS sources/convert2snns.c)
set(ISNNS_SRCS sources/isnns.c)
set(LINKNETS_SRCS sources/linknets.c
                  sources/snns2clib.c)

set(BATCHMAN_SRCS sources/batchman.c
                  sources/gram1.tab.c
		  sources/icopjmp.c
		  sources/ictab.c
		  sources/symtab.c
		  sources/backpatch.c
		  sources/memory.c
		  sources/ic_snns.c
		  sources/error.c
		  sources/arglist.c
		  sources/snns_intfce.c
		  sources/special_jacket.c)

find_package(BISON REQUIRED)
find_package(FLEX  REQUIRED)

bison_target(gram1 ${BATCH_SOURCES_YACC} ${CMAKE_BINARY_DIR}/gram1.tab.c
             COMPILE_FLAGS "-t -y -p yyz")
flex_target(scan1  ${BATCH_SOURCES1_LEX} ${CMAKE_BINARY_DIR}/lex.yyz.c
             COMPILE_FLAGS "-P yyz")
flex_target(scan2  ${BATCH_SOURCES2_LEX} ${CMAKE_BINARY_DIR}/lex.yyy.c
             COMPILE_FLAGS "-P yyy")
add_flex_bison_dependency(scan1 gram1)

add_executable(snns2c ${SNNS2C_SRCS})
target_link_libraries(snns2c kernel func m)
add_executable(pat_sel ${PAT_SEL_SRCS})
target_link_libraries(pat_sel kernel func m)
add_executable(netlearn ${NETLEARN_SRCS})
target_link_libraries(netlearn kernel func m)
add_executable(td_bignet ${TD_BIGNET_SRCS})
target_link_libraries(td_bignet kernel func m)
add_executable(ff_bignet ${FF_BIGNET_SRCS})
target_link_libraries(ff_bignet kernel func m)
add_executable(netperf ${NETPERF_SRCS})
target_link_libraries(netperf kernel func m)
add_executable(snnsbat ${SNNSBAT_SRCS})
target_link_libraries(snnsbat kernel func m)
add_executable(feedback-gennet ${FEEDBACK_GENNET_SRCS})
target_link_libraries(feedback-gennet kernel func m)
add_executable(convert2snns ${CONVERT2SNNS_SRCS})
target_link_libraries(convert2snns kernel func m)
add_executable(isnns ${ISNNS_SRCS})
target_link_libraries(isnns kernel func m)
add_executable(linknets ${LINKNETS_SRCS})
target_link_libraries(linknets kernel func m)
add_executable(batchman ${BATCHMAN_SRCS}
                        ${CMAKE_BINARY_DIR}/gram1.tab.c
			${CMAKE_BINARY_DIR}/lex.yyz.c
			${CMAKE_BINARY_DIR}/lex.yyy.c)

target_link_libraries(batchman kernel func m)

add_executable(analyze sources/analyze.c)
target_link_libraries(analyze m)
add_executable(pat_sel_simple sources/pat_sel_simple.c)
target_link_libraries(pat_sel_simple m)
add_executable(mkhead sources/mkhead.c)
target_link_libraries(mkhead m)
add_executable(mkout sources/mkout.c)
target_link_libraries(mkout m)
add_executable(mkpat sources/mkpat.c)
target_link_libraries(mkpat m)
