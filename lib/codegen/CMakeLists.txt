set(codegen
    codegen.cc
    conditional_codegen.cc
    loop_codegen.cc
    var_expr_codegen.cc
    function_codegen.cc
)

add_library(${CODEGEN_LIBRARY_NAME} ${codegen})

target_include_directories(${CODEGEN_LIBRARY_NAME} PRIVATE
    ${PROJECT_INCLUDE_DIR}
    ${LLVM_INCLUDE_DIRS}
)

target_link_libraries(${CODEGEN_LIBRARY_NAME} PRIVATE 
    ${AST_LIBRARY_NAME}
    ${LLVM}
)
