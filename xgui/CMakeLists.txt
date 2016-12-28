include_directories(sources
                    iconsXgui
                    ${CMAKE_SOURCE_DIR}/kernel/sources
                    ${CMAKE_SOURCE_DIR}/tools/sources
                    ${CMAKE_BINARY_DIR}
		    ${CMAKE_SOURCE_DIR})

set(BN3DSOURCES_C sources/net_analyzer.c
                  sources/d3_xUtils.c
                  sources/d3_global.c
                  sources/d3_anageo.c
                  sources/d3_shade.c
                  sources/d3_lists.c
                  sources/d3_point.c
                  sources/d3_dither.c
                  sources/d3_zgraph.c
                  sources/d3_fonts.c
                  sources/d3_graph.c
                  sources/d3_draw.c
                  sources/d3_disp.c
                  sources/d3_move.c
                  sources/d3_zValue.c
                  sources/d3_setup.c
                  sources/d3_units.c
                  sources/d3_links.c
                  sources/d3_model.c
                  sources/d3_project.c
                  sources/d3_light.c
                  sources/d3_pannels.c
                  sources/d3_main.c
                  sources/bn_bignet.c
                  sources/bn_basics.c
                  sources/bn_art1.c
                  sources/bn_art2.c
                  sources/bn_artmap.c
                  sources/bn_TD_bignet.c
                  sources/bn_kohonen.c
                  sources/bn_assoz.c
                  sources/o_graph.c
                  sources/bn_JordElm.c
                  sources/ui_class.c
                  sources/bn_bighop.c)

set(XGUISOURCES1_C sources/ui_xWidgets.c
                   sources/ui_confirmer.c
                   sources/ui_mainP.c
                   sources/ui_status.c
                   sources/ui_info.c
                   sources/ui_infoP.c
                   sources/ui_maspar.c
                   sources/ui_control.c
                   sources/ui_controlP.c
                   sources/ui_file.c
                   sources/ui_fileP.c
                   sources/ui_setup.c
                   sources/ui_setupP.c
                   sources/ui_xGraphic.c
                   sources/ui_netGraph.c
                   sources/ui_utilP.c
                   sources/ui_netUpdate.c)

set(XGUISOURCES2_C sources/ui_event.c
                   sources/ui_selection.c
                   sources/ui_action.c
                   sources/ui_key.c
                   sources/ui_textP.c
                   sources/ui_lists.c
                   sources/ui_layer.c
                   sources/ui_layerP.c
                   sources/ui_display.c
                   sources/ui_edit.c
                   sources/ui_config.c
                   sources/ui_color.c
                   sources/ui_inversion.c
                   sources/ui_funcdispl.c
                   sources/ui_displmap.c
                   sources/ui_displwght.c
                   sources/ui_print.c
                   sources/ui_colEdit.c
                   sources/ui_result.c
                   sources/cc_main.c
                   sources/pr_main.c
                   sources/ui_kohonen.c)

add_library(xgui1 STATIC ${XGUISOURCES1_C})
add_library(xgui2 STATIC ${XGUISOURCES2_C})
add_library(3d STATIC ${BN3DSOURCES_C})

find_library(XAW3D_LIB NAME Xaw3d)
if (NOT "${XAW3D_LIB}" MATCHES "NOTFOUND")
  message(FATAL_ERROR "Xaw3D not found. Package required")
endif ()

find_package(X11 REQUIRED)
if (NOT X11_SM_FOUND)
  message(FATAL_ERROR "X11_SM not found. Package required")
endif ()
if (NOT X11_ICE_FOUND)
  message(FATAL_ERROR "X11_ICE not found. Package required")
endif ()
if (NOT X11_Xmu_FOUND)
  message(FATAL_ERROR "X11_Xmu not found. Package required")
endif ()
if (NOT X11_Xt_FOUND)
  message(FATAL_ERROR "X11_Xt not found. Package required")
endif ()
if (NOT X11_Xext_FOUND)
  message(FATAL_ERROR "X11_Xext not found. Package required")
endif ()
if (NOT X11_FOUND)
  message(FATAL_ERROR "X11_X11 not found. Package required")
endif ()

add_executable(xgui sources/ui_main.c)
target_link_libraries(xgui xgui1 xgui2 3d kernel func m)
target_link_libraries(xgui SM ICE Xmu Xt Xext X11)
target_link_libraries(xgui Xaw3d)
