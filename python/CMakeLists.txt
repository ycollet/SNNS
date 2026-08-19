# The Python.h ABI helpers used below (Development.Module component,
# Python3_add_library WITH_SOABI) need a newer CMake than the rest of
# the project; only enforced when this optional interface is enabled.
cmake_minimum_required(VERSION 3.18)

find_package(Python3 COMPONENTS Interpreter Development.Module REQUIRED)

Python3_add_library(krui MODULE WITH_SOABI snns/krui.c)

target_include_directories(krui PRIVATE ${CMAKE_SOURCE_DIR}/kernel/sources)
target_link_libraries(krui PRIVATE kernel func)

# Build straight into snns/, next to the package's __init__.py/util.py,
# so "from snns import krui" works without a separate install step -
# matching what "setup.py build_ext --inplace" produces.
set_target_properties(krui PROPERTIES
                       LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/snns
                       RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/snns)

# Install into the found interpreter's own site-packages so "import snns"
# works right away, with no PYTHONPATH needed - unlike the rest of the
# project, which installs everything self-contained under
# CMAKE_INSTALL_PREFIX (Python3_SITEARCH is an absolute path outside it).
install(TARGETS krui DESTINATION ${Python3_SITEARCH}/snns)
install(FILES snns/__init__.py snns/util.py DESTINATION ${Python3_SITEARCH}/snns)

install(DIRECTORY examples/
        DESTINATION ${CMAKE_INSTALL_DATADIR}/${PACKAGE_NAME}/python/examples)
