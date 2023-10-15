project(sandbox LANGUAGES C CXX)

set(ASSETS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/assets)

add_executable(${PROJECT_NAME} main.cpp)

#target_precompile_headers(${PROJECT_NAME} PRIVATE app.hpp)

target_link_libraries(${PROJECT_NAME} PRIVATE magnetar-core tweeny)
                      
# Copy assets to the output directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
  # COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different ${ASSETS_DIR}/resources $<TARGET_FILE_DIR:${PROJECT_NAME}>
  COMMAND tar -C ${ASSETS_DIR} -cvf - . | lz4 > $<TARGET_FILE_DIR:${PROJECT_NAME}>/data
)

# decompress example:
# lz4 -d folderABC.tar.lz4 -c | tar xvf -