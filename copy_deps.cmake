
function(copy_all_conan_deps libs_list target_path)
    set (as_list "${libs_list}")
    string (REPLACE " " ";" as_list "${as_list}")
    foreach(SO_DIR IN LISTS as_list)
        message("Copying shared libs from ${SO_DIR} to ${target_path}")
        file(GLOB so_files "${SO_DIR}/*.so*")
        #file(COPY ${so_files} DESTINATION ${target_path})
        file(INSTALL ${so_files} DESTINATION ${target_path})
    endforeach()
endfunction()

copy_all_conan_deps(${CONAN_LIBS_LIST} ${SHARED_LIBS_TARGET_PATH})