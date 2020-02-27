
set(AB_V 1)
set(AB_v 0)
set(AQUABOX_COMMON_PATH ${CMAKE_CURRENT_LIST_DIR}/common)

file(GLOB TEST_SOURCES
  "*.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/../src/aquabox/time_details.cpp"
)
include_directories(${CMAKE_CURRENT_LIST_DIR}/../src)

add_executable(tests ${TEST_SOURCES})