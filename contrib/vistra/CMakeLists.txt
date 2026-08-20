set(VISTRA_SRCS format.c
                patterns.c
                globals.c
                coll.c
                eh.c
                symtab.c
                vector.c
                veccoll.c
                vistra.c
                misc.c
                batch.c)

find_package(X11 REQUIRED)
if (NOT X11_Xaw_FOUND)
  message(FATAL_ERROR "X11_Xaw not found. Package required")
endif ()
if (NOT X11_Xt_FOUND)
  message(FATAL_ERROR "X11_Xt not found. Package required")
endif ()
if (NOT X11_Xmu_FOUND)
  message(FATAL_ERROR "X11_Xmu not found. Package required")
endif ()
if (NOT X11_Xext_FOUND)
  message(FATAL_ERROR "X11_Xext not found. Package required")
endif ()
if (NOT X11_FOUND)
  message(FATAL_ERROR "X11_X11 not found. Package required")
endif ()

add_executable(vistra ${VISTRA_SRCS})
#set_target_properties(vistra PROPERTIES COMPILE_DEFINITIONS XFWF_FILE_SELECTION)
target_include_directories(vistra PRIVATE ${X11_INCLUDE_DIR})
target_link_libraries(vistra ${X11_Xaw_LIB} ${X11_Xt_LIB} ${X11_Xmu_LIB} ${X11_Xext_LIB} ${X11_X11_LIB} m) # fwf Dir

install(TARGETS vistra
        DESTINATION ${CMAKE_INSTALL_BINDIR})
