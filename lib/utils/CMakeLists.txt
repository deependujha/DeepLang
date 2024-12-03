set(utils
    check_type.cc
)

add_library(${UTILS_LIBRARY_NAME} ${utils})
target_include_directories(${UTILS_LIBRARY_NAME} PRIVATE ${PROJECT_INCLUDE_DIR}/utils)
