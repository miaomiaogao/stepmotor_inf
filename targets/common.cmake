message(STATUS "common.cmake")


file(GLOB_RECURSE SOURCES_CUBEMX "${PROJECT_TARGET_DIR}/Core/*.*" "${PROJECT_TARGET_DIR}/Drivers/*.*")

file(GLOB_RECURSE SOURCES_PROJECT "${PROJECT_ROOT_SRC}/app/*.*" "${PROJECT_ROOT_SRC}/common/*.*")


set(HEADERS ${PROJECT_ROOT_SRC}  ${PROJECT_ROOT_MODULE})
set(SOURCES ${SOURCES_CUBEMX} ${SOURCES_PROJECT} ${SUBMODULE_PROJECT})


########################################################################################################################
# SOURCES/HEADERS will be processed by outer CMakeLists
