# DocTest
#--------------------------------------
add_library(doctest INTERFACE)
target_sources(doctest INTERFACE doctest/doctest.h)
target_include_directories(doctest INTERFACE
  "${CMAKE_CURRENT_SOURCE_DIR}/doctest"
)
#target_compile_features(doctest INTERFACE cxx_std_17)

include(${CMAKE_CURRENT_SOURCE_DIR}/doctest/scripts/cmake/doctest.cmake)

# Tester
#--------------------------------------
add_executable(tester
    tester.cpp
)
target_link_libraries(tester PRIVATE
    doctest
    TVector
)

# Enable Testing
#--------------------------------------
enable_testing()
#add_test(NAME TVectorTests COMMAND tester)
doctest_discover_tests(tester)
