# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/test/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/test/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_BINARY_DIR}/googletest-src
                 ${CMAKE_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

# The gtest/gtest_main targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
  include_directories("${gtest_SOURCE_DIR}/include")
endif()
if (CMAKE_VERSION VERSION_LESS 2.8.11)
  include_directories(BEFORE SYSTEM
    "${gtest_SOURCE_DIR}/include" "${gmock_SOURCE_DIR}/include")
else()
  target_include_directories(gmock_main SYSTEM BEFORE INTERFACE
    "${gtest_SOURCE_DIR}/include" "${gmock_SOURCE_DIR}/include")
endif()

function(add_stream_test test_name)
    add_executable(${test_name} "${test_name}.cpp")
    #add_dependencies(${test_name} googletest)
    target_link_libraries(${test_name} gtest_main gmock_main)
    add_test(${test_name} ${test_name})
endfunction(add_stream_test)

# TODO(wanatpj): fix tests
# Stream generators
add_stream_test(EmptyTest)
add_stream_test(SingletonTest)
add_stream_test(RepeatTest)
# add_stream_test(CycleTest)
add_stream_test(CounterTest)
add_stream_test(RecurrenceTest)
add_stream_test(RangeTest)
add_stream_test(FromTest)

# Stream operators
add_stream_test(AlgebraTest)
# add_stream_test(FilterTest)
# add_stream_test(WhileTest)
add_stream_test(SliceTest)
add_stream_test(PeekTest)
# add_stream_test(MapTest)
add_stream_test(FlatMapTest)
add_stream_test(AdjacentDistinctTest)
add_stream_test(AdjacentDifferenceTest)
add_stream_test(PartialSumTest)
add_stream_test(ConcatTest)
add_stream_test(GroupTest)
add_stream_test(OverlapTest)
add_stream_test(ZipTest)
add_stream_test(SetOperationsTest)
add_stream_test(StatefulTest)

# Stream terminators
add_stream_test(AccessTest)
add_stream_test(NumericReductionTest)
# add_stream_test(QuantifierTest)
# add_stream_test(ConversionTest)
# add_stream_test(ReduceTest)
add_stream_test(SaveTest)
add_stream_test(SampleTest)
add_stream_test(ForEachTest)
