message(STATUS "init.cmake")

# General Path Helpers
get_filename_component(PROJECT_ROOT "${PROJECT_TARGET_DIR}/../.." ABSOLUTE)
set(PROJECT_ROOT_SRC ${PROJECT_ROOT}/src)
set(PROJECT_ROOT_MODULE ${PROJECT_ROOT}/module) 
set(PROJECT_ROOT_LIB ${PROJECT_ROOT}/lib)



## Build Types
if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    message(STATUS "Release: Maximum optimization for size")
    add_compile_options(-Os)
else ()
    add_definitions(-DDEBUG)
    message(STATUS "Debug: Minimal optimization, debug info included")
    add_compile_options(-Og -g)
    #add_compile_options(-O0 -g)
endif ()


set(LINKER_SCRIPTNAME "app")


set(LINKER_SCRIPT ${PROJECT_TARGET_DIR}/${LINKER_SCRIPTNAME}.ld)
message(STATUS "LinkerScript: ${PROJECT_TARGET_DIR}/${LINKER_SCRIPTNAME}.ld")

#####################################################################

# Use newlib-nano (newlib is a libc replacement, nano variant is for embedded devices)
add_link_options(-specs=nano.specs -specs=nosys.specs)

#####################################################################
