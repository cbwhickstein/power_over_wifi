set(requires "")
idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    list(APPEND requires esp_stubs esp-tls esp_http_server protocol_examples_common nvs_flash)
endif()

set(HTML_FILE_PATH "${CMAKE_SOURCE_DIR}/pages/homewebpage.html")
set(HEADER_FILE_PATH "${CMAKE_BINARY_DIR}/html_content.h")
set(HEADER_FILE_PATH_RENAMED "${CMAKE_BINARY_DIR}/pages.h")

# Extract the file name (without extension) and replace slashes with underscores
get_filename_component(HTML_FILE_NAME "${HTML_FILE_PATH}" NAME_WE)
message(STATUS "Got file name: ${HTML_FILE_NAME}")
string(REPLACE "/" "_" HTML_VAR_NAME "${HTML_FILE_PATH}")
string(REPLACE "." "_" HTML_VAR_NAME "${HTML_VAR_NAME}")
string(REPLACE "_build" "" HTML_VAR_NAME "${HTML_VAR_NAME}")

# Print the paths and variable name for debugging
message(STATUS "HTML file path: ${HTML_FILE_PATH}")
message(STATUS "Generated variable name: ${HTML_VAR_NAME}")
message(STATUS "Generated file name: ${HTML_FILE_NAME}")

# Run 'xxd' to convert the HTML file into a C header file
execute_process(
    COMMAND rm ${HEADER_FILE_PATH}
    COMMAND xxd -i ${HTML_FILE_PATH} ${HEADER_FILE_PATH}
    COMMAND /bin/bash -c "sed \"s/${HTML_VAR_NAME}/${HTML_FILE_NAME}/g\" ${HEADER_FILE_PATH} > ${HEADER_FILE_PATH_RENAMED}"
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)


idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS "." "../build"
                    REQUIRES ${requires})
