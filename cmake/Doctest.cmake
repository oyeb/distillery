if(ENABLE_DOCTESTS)
    add_definitions(-DENABLE_DOCTEST_IN_LIBRARY)
endif()

CPMAddPackage(
  NAME doctest
  GITHUB_REPOSITORY onqtam/doctest
  GIT_TAG 2.3.8
)

if (doctest_ADDED)
  target_include_directories(doctest INTERFACE "${doctest_SOURCE_DIR}/doctest")
endif()
