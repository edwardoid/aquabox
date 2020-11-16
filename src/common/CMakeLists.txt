project(aquabox-common)

file(GLOB AQUABOX_COMMON_SOURCES "*.cpp")

add_library(${PROJECT_NAME} SHARED ${AQUABOX_COMMON_SOURCES})
target_link_libraries(${PROJECT_NAME} ${tgbot_LIBRARIES})
set_property(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE 1)
