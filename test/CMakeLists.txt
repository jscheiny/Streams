include(ExternalProject)

externalproject_add(googlemock
  PREFIX "${CMAKE_BINARY_DIR}/gtest-1.7.0"
  URL "http://googlemock.googlecode.com/files/gmock-1.7.0.zip"
  URL_HASH SHA1=f9d9dd882a25f4069ed9ee48e70aff1b53e3c5a5
  INSTALL_COMMAND "")
externalproject_get_property(googlemock source_dir binary_dir)

include_directories("${source_dir}/include")
include_directories("${source_dir}/gtest/include")
link_directories("${binary_dir}")

function(add_stream_test test_name)
    add_executable(${test_name} "${test_name}.cpp")
    add_dependencies(${test_name} googlemock)
    target_link_libraries(${test_name} gmock gmock_main)
    add_test(${test_name} ${test_name})
endfunction(add_stream_test)

# Stream generators
add_stream_test(EmptyTest)
add_stream_test(SingletonTest)
add_stream_test(RepeatTest)
add_stream_test(CycleTest)
add_stream_test(CounterTest)
add_stream_test(RecurrenceTest)
add_stream_test(RangeTest)
add_stream_test(FromTest)

# Stream operators
add_stream_test(AlgebraTest)
add_stream_test(FilterTest)
add_stream_test(WhileTest)
add_stream_test(SliceTest)
add_stream_test(PeekTest)
add_stream_test(MapTest)
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
add_stream_test(QuantifierTest)
add_stream_test(ConversionTest)
add_stream_test(ReduceTest)
add_stream_test(SaveTest)
add_stream_test(SampleTest)
add_stream_test(ForEachTest)
