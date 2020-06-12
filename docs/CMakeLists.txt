# --------------------------------------------------------------------------------
#                         Documentation (no change needed).
# --------------------------------------------------------------------------------
# Add a make target 'doc' to generate API documentation with Doxygen.
# You should set options to your liking in the file 'Doxyfile.in'.
find_package(Doxygen REQUIRED)

# Find all the public headers
get_target_property(DISTILLERY_PUBLIC_HEADER_DIR distillery INTERFACE_INCLUDE_DIRECTORIES)
file(GLOB_RECURSE DISTILLERY_PUBLIC_HEADERS ${DISTILLERY_PUBLIC_HEADER_DIR}/*.hh)

set(DOXYGEN_INPUT_DIR ${PROJECT_SOURCE_DIR}/)
set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/doxygen)
set(DOXYGEN_INDEX_FILE ${DOXYGEN_OUTPUT_DIR}/html/index.html)
set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
set(DOXYFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

#Replace variables inside @@ with the current values
configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)

file(MAKE_DIRECTORY ${DOXYGEN_OUTPUT_DIR}) #Doxygen won't create this for us
add_custom_command(
  OUTPUT ${DOXYGEN_INDEX_FILE}
  DEPENDS ${DISTILLERY_PUBLIC_HEADERS}
  COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}
  MAIN_DEPENDENCY ${DOXYFILE_OUT} ${DOXYFILE_IN}
  COMMENT "Generating Doxygen XML")

add_custom_target(Doxygen ALL DEPENDS ${DOXYGEN_INDEX_FILE})

find_package(Sphinx REQUIRED)
set(SPHINX_SOURCE ${CMAKE_CURRENT_SOURCE_DIR})
set(SPHINX_BUILD ${CMAKE_CURRENT_BINARY_DIR}/sphinx)
set(SPHINX_INDEX_FILE ${SPHINX_BUILD}/index.html)

add_custom_command(
  OUTPUT ${SPHINX_INDEX_FILE}
  COMMAND
  ${SPHINX_EXECUTABLE} -b html
  # Tell Breathe where to find the Doxygen output
  -Dbreathe_projects.Distillery=${DOXYGEN_OUTPUT_DIR}/xml
  ${SPHINX_SOURCE} ${SPHINX_BUILD}
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/*.rst
  MAIN_DEPENDENCY ${SPHINX_SOURCE}/conf.py
  COMMENT "${Cyan}Generating documentation with Sphinx.${ColourReset}" VERBATIM)

add_custom_target(Sphinx ALL DEPENDS ${SPHINX_INDEX_FILE})
