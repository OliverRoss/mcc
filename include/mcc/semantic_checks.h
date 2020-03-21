//
// Created by oliver on 20.03.20.
//

#ifndef PROJECT_SEMANTIC_CHECKS_H
#define PROJECT_SEMANTIC_CHECKS_H

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "mcc/ast.h"
#include "mcc/symbol_table.h"

enum mcc_semantic_check_status{
    MCC_SEMANTIC_CHECK_OK,
    MCC_SEMANTIC_CHECK_FAIL,
};

// ------------------------------------------------------------ Data structure: A suite of semantic checks

struct mcc_semantic_check_all_checks{
   enum mcc_semantic_check_status status;
   struct mcc_semantic_check *type_check;
   struct mcc_semantic_check *nonvoid_check;
   struct mcc_semantic_check *main_function;
   struct mcc_semantic_check *unknown_function_call;
   struct mcc_semantic_check *multiple_function_definitions;
   struct mcc_semantic_check *multiple_variable_declarations;
   struct mcc_semantic_check *use_undeclared_variable;
};

// ------------------------------------------------------------ Data structure: A single semantic check

enum mcc_semantic_check_type{
    MCC_SEMANTIC_CHECK_TYPE_CHECK,
    MCC_SEMANTIC_CHECK_NONVOID_CHECK,
    MCC_SEMANTIC_CHECK_MAIN_FUNCTION,
    MCC_SEMANTIC_CHECK_UNKNOWN_FUNCTION_CALL,
    MCC_SEMANTIC_CHECK_MULTIPLE_FUNCTION_DEFINITIONS,
    MCC_SEMANTIC_CHECK_MULTIPLE_VARIABLE_DECLARATIONS,
    MCC_SEMANTIC_CHECK_USE_UNDECLARED_VARIABLE,

};

struct mcc_semantic_check {
    enum mcc_semantic_check_status status;
    enum mcc_semantic_check_type type;
    union{
        struct mcc_semantic_check_type_check *type_check;
        struct mcc_semantic_check_nonvoid_check *nonvoid_check;
        struct mcc_semantic_check_main_function *main_function;
        struct mcc_semantic_check_unknown_function_call *unknown_function_call;
        struct mcc_semantic_check_multiple_function_definitions *multiple_function_definitions;
        struct mcc_semantic_check_multiple_variable_declarations *multiple_variable_declarations;
        struct mcc_semantic_check_use_undeclared_variable *use_undeclared_variable;
    };
};

// ------------------------------------------------------------- Functions: Running all semantic checks

// Run all semantic checks
struct mcc_semantic_check_all_checks* mcc_semantic_check_run_all(struct mcc_ast_program* ast,
        struct mcc_symbol_table* symbol_table);

// ------------------------------------------------------------- Functions: Running single semantic checks

// Types of used variables
struct mcc_semantic_check* mcc_semantic_check_run_type_check(struct mcc_ast_program* ast,
                                                                struct mcc_symbol_table* symbol_table);

// Each execution path of non-void function returns a value
struct mcc_semantic_check* mcc_semantic_check_run_nonvoid_check(struct mcc_ast_program* ast,
                                                                struct mcc_symbol_table* symbol_table);

// Main function exists and has correct signature
struct mcc_semantic_check* mcc_semantic_check_run_main_function(struct mcc_ast_program* ast,
                                                                struct mcc_symbol_table* symbol_table);

// No Calls to unknown functions
struct mcc_semantic_check* mcc_semantic_check_run_unknown_function_call(struct mcc_ast_program* ast,
                                                                struct mcc_symbol_table* symbol_table);

// No multiple definitions of the same function
struct mcc_semantic_check* mcc_semantic_check_run_multiple_function_definitions(struct mcc_ast_program* ast,
                                                                struct mcc_symbol_table* symbol_table);

// No multiple declarations of a variable in the same scope
struct mcc_semantic_check* mcc_semantic_check_run_multiple_variable_declarations(struct mcc_ast_program* ast,
                                                                struct mcc_symbol_table* symbol_table);

// No use of undeclared variables
struct mcc_semantic_check* mcc_semantic_check_run_use_undeclared_variable(struct mcc_ast_program* ast,
                                                                struct mcc_symbol_table* symbol_table);

// ------------------------------------------------------------- Functions: Cleanup

// Delete all checks
void mcc_semantic_check_delete_all_checks(struct mcc_semantic_check_all_checks *checks);

// Delete single checks
void mcc_semantic_check_delete_single_check(struct mcc_semantic_check *check);


#endif //PROJECT_SEMANTIC_CHECKS_H