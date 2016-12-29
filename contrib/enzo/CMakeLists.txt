include_directories(${CMAKE_SOURCE_DIR}/kernel/sources
                    ${CMAKE_SOURCE_DIR}/contrib/enzo/src
		    ${CMAKE_SOURCE_DIR}/contrib/enzo/src/nepomuk
                    ${CMAKE_BINARY_DIR})

set(ENZO_CROSSOVER_SRCS src/crossover/implant.c
                        src/crossover/linCross.c)

add_library(crossover STATIC ${ENZO_CROSSOVER_SRCS})

set(ENZO_EVAL_SRCS src/eval/tssEval.c
                   src/eval/topoRating.c
                   src/eval/learnRating.c
                   src/eval/bestGuessHigh.c
                   src/eval/bestGuessLow.c
                   src/eval/classes.c
                   src/eval/distClass.c
                   src/eval/topoEval.c)

add_library(eval STATIC ${ENZO_EVAL_SRCS})

set(ENZO_EXAMPLES_SRCS src/examples/my_module.c)

add_library(examples STATIC ${ENZO_EXAMPLES_SRCS})

set(ENZO_HISTORY_SRCS src/history/histUtils.c
                      src/history/histSimple.c
                      src/history/histWeights.c
                      src/history/histFitness.c
                      src/history/histCross.c
                      src/history/histInputs.c
                      src/history/ancestry.c
                      src/history/saveThem.c
                      src/history/Xgraf/graf.c
                      src/history/Xhist.c)

add_library(history STATIC ${ENZO_HISTORY_SRCS})

set(ENZO_MUTATION_SRCS src/mutation/simpleMut.c
                       src/mutation/mutUnits.c
                       src/mutation/mutInputs.c
                       src/mutation/mutLinks.c)

add_library(mutation STATIC ${ENZO_MUTATION_SRCS})

set(ENZO_NEPOMUK_SRCS src/nepomuk/nepomuk.c
                      src/nepomuk/kr_shell.c)

add_library(nepomuk STATIC ${ENZO_NEPOMUK_SRCS})
set_target_properties(nepomuk PROPERTIES COMPILE_DEFINITIONS __ENZO__)

set(ENZO_OPT_SRCS src/opt/prune.c
                  src/opt/relearn.c
                  src/opt/cleanup.c
                  src/opt/nullWeg.c
                  src/opt/adapPrune.c
                  src/opt/jogWeights.c
                  src/opt/learnSNNS.c
                  src/opt/learnCV.c
                  src/opt/learnINIT.c)

add_library(opt STATIC ${ENZO_OPT_SRCS})

set(ENZO_POST_SRCS src/post/saveAll.c)

add_library(post STATIC ${ENZO_POST_SRCS})

set(ENZO_PRE_SRCS src/pre/genpopNepomuk.c
                  src/pre/initPop.c
                  src/pre/loadPop.c
                  src/pre/optInitPop.c
                  src/pre/loadSNNSPat.c
                  src/pre/hiddenInitPop.c
                  src/pre/inputInit.c
                  src/pre/weightInit.c
                  src/pre/startPop.c
                  src/pre/initTrain.c)

add_library(pre STATIC ${ENZO_PRE_SRCS})

set(ENZO_SELECTION_SRCS src/selection/uniformSel.c
			src/selection/preferSel.c)

add_library(selection STATIC ${ENZO_SELECTION_SRCS})

set(ENZO_STOP_SRCS src/stop/stopIt.c
		   src/stop/stopErr.c)

add_library(stop STATIC ${ENZO_STOP_SRCS})

set(ENZO_SURVIVAL_SRCS src/survival/fittest.c)

add_library(survival STATIC ${ENZO_SURVIVAL_SRCS})

set(ENZO_SRCS src/enzo.c
              src/m_table.c
              src/networkData.c
              src/subUtils.c)

add_executable(enzo ${ENZO_SRCS})
target_link_libraries(enzo kernel func pre kernel func nepomuk kernel func stop selection mutation crossover opt eval history survival post m)

install(TARGETS enzo
        DESTINATION bin)
