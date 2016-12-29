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

add_executable(vistra ${VISTRA_SRCS})
#set_target_properties(vistra PROPERTIES COMPILE_DEFINITIONS XFWF_FILE_SELECTION)
target_link_libraries(vistra Xaw Xt Xmu Xext X11 m) # fwf Dir

install(TARGETS vistra
        DESTINATION bin)
