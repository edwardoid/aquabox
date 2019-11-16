project(aquabox-cloud-service)

file(GLOB SOURCES "*.cpp")

add_executable(${PROJECT_NAME} ${SOURCES})
target_compile_definitions(${PROJECT_NAME} PUBLIC AB_VERSION_MAJOR=${AB_V})
target_compile_definitions(${PROJECT_NAME} PUBLIC AB_VERSION_MINOR=${AB_v})
target_compile_definitions(${PROJECT_NAME} PUBLIC AB_VERSION=\"${AB_V}.${AB_v}\")
target_compile_definitions(${PROJECT_NAME} PUBLIC AB_GIT_HASH=\"${GIT_COMMIT_HASH}\")


if(OPENSSL_FOUND)
    target_compile_definitions(${PROJECT_NAME} INTERFACE HAVE_OPENSSL)
    target_link_libraries(${PROJECT_NAME} ${OPENSSL_LIBRARIES})
    include_directories(${PROJECT_NAME} ${OPENSSL_INCLUDE_DIR})
endif()

target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${OPENSSL_CRYPTO_LIBRARY})
target_link_libraries(${PROJECT_NAME} ${PNG_LIBRARY})
target_link_libraries(${PROJECT_NAME} ${ZLIB_LIBRARIES})
target_link_libraries(${PROJECT_NAME} Boost::system)
target_link_libraries(${PROJECT_NAME} OpenSSL::SSL)
target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(${PROJECT_NAME} ${LIBMONGOCXX_LIBRARIES})
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
