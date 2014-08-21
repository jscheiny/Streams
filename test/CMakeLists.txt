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

add_stream_test(EmptyTest)
add_stream_test(RepeatTest)
add_stream_test(CycleTest)
add_stream_test(CounterTest)
