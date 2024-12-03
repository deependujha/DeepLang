set(codegen
    codegen.cc
)

add_library(${CODEGEN_LIBRARY_NAME} ${codegen})

target_include_directories(${CODEGEN_LIBRARY_NAME} PRIVATE
    ${PROJECT_INCLUDE_DIR}/codegen
    ${PROJECT_INCLUDE_DIR}/ast
    ${LLVM_INCLUDE_DIRS}
)

target_link_libraries(${CODEGEN_LIBRARY_NAME} PRIVATE 
    ${AST_LIBRARY_NAME}
    ${LLVM}
)
