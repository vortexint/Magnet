function(magnetar_package)
  if(NOT DEFINED ASSETS_DIR)
    message(FATAL_ERROR "Please define ASSETS_DIR it before calling magnetar_package()")
  endif()
  
  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    # COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different ${ASSETS_DIR}/resources $<TARGET_FILE_DIR:${PROJECT_NAME}>
    COMMAND rm -f $<TARGET_FILE_DIR:${PROJECT_NAME}>/data
    COMMAND tar -C ${ASSETS_DIR} -cvf - . | lz4 > $<TARGET_FILE_DIR:${PROJECT_NAME}>/data
  )

  # decompress example:
  # lz4 -d folderABC.tar.lz4 -c | tar xvf -
endfunction()

# Project list

add_subdirectory(sandbox)  # test app

# Add your project here