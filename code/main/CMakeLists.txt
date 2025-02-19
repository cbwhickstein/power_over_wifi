file(GLOB_RECURSE SRC_FILES "src/*.c")
idf_component_register(SRCS "code.c"
                            ${SRC_FILES}
                    INCLUDE_DIRS "include")
