// AST Visitor Infrastructure
//
// This module defines a depth-first visitor utility for traversing the AST.
//
// Instantiate the `mcc_ast_visitor` struct with the desired configuration and callbacks.
// Use this instance with the functions declared below. Each callback is optional, just set it to NULL.

#ifndef MCC_AST_VISIT_H
#define MCC_AST_VISIT_H

#include "mcc/ast.h"

enum mcc_ast_visit_order {
	MCC_AST_VISIT_PRE_ORDER,
	MCC_AST_VISIT_POST_ORDER,
};

// Callbacks
typedef void (*mcc_ast_visit_expression_cb)(struct mcc_ast_expression *, void *userdata);
typedef void (*mcc_ast_visit_statement_cb)(struct mcc_ast_statement *, void *userdata);
typedef void (*mcc_ast_visit_literal_cb)(struct mcc_ast_literal *, void *userdata);
typedef void (*mcc_ast_visit_assignment_cb)(struct mcc_ast_assignment *, void *userdata);
typedef void (*mcc_ast_visit_declaration_cb)(struct mcc_ast_declaration *, void *userdata);
typedef void (*mcc_ast_visit_type_cb)(struct mcc_ast_type *, void *userdata);
typedef void (*mcc_ast_visit_identifier_cb)(struct mcc_ast_identifier *, void *userdata);
typedef void (*mcc_ast_visit_compound_statement_cb)(struct mcc_ast_compound_statement *, void *userdata);
typedef void (*mcc_ast_visit_program_cb)(struct mcc_ast_program *, void *userdata);
typedef void (*mcc_ast_visit_parameters_cb)(struct mcc_ast_parameters *, void *userdata);
typedef void (*mcc_ast_visit_arguments_cb)(struct mcc_ast_arguments *, void *userdata);
typedef void (*mcc_ast_visit_function_definition_cb)(struct mcc_ast_function_definition *, void *userdata);

struct mcc_ast_visitor {
	enum mcc_ast_visit_order order;

	// This will be passed to every callback along with the corresponding AST
	// node. Use it to share data while traversing the tree.
	void *userdata;

	mcc_ast_visit_expression_cb expression;
	mcc_ast_visit_expression_cb expression_literal;
	mcc_ast_visit_expression_cb expression_binary_op;
	mcc_ast_visit_expression_cb expression_parenth;
	mcc_ast_visit_expression_cb expression_unary_op;
	mcc_ast_visit_expression_cb expression_variable;
	mcc_ast_visit_expression_cb expression_array_element;
	mcc_ast_visit_expression_cb expression_function_call;

	mcc_ast_visit_literal_cb literal;
	mcc_ast_visit_literal_cb literal_int;
	mcc_ast_visit_literal_cb literal_float;
	mcc_ast_visit_literal_cb literal_bool;
	mcc_ast_visit_literal_cb literal_string;

	mcc_ast_visit_statement_cb statement;
	mcc_ast_visit_statement_cb statement_if_stmt;
	mcc_ast_visit_statement_cb statement_if_else_stmt;
	mcc_ast_visit_statement_cb statement_expression_stmt;
	mcc_ast_visit_statement_cb statement_while;
	mcc_ast_visit_statement_cb statement_declaration;
	mcc_ast_visit_statement_cb statement_assignment;
	mcc_ast_visit_statement_cb statement_return;
	mcc_ast_visit_statement_cb statement_compound_stmt;

	mcc_ast_visit_compound_statement_cb compound_statement;
	mcc_ast_visit_program_cb program;
	mcc_ast_visit_function_definition_cb function_definition;
	mcc_ast_visit_parameters_cb parameters;
	mcc_ast_visit_arguments_cb arguments;

	mcc_ast_visit_assignment_cb assignment;
	mcc_ast_visit_assignment_cb variable_assignment;
	mcc_ast_visit_assignment_cb array_assignment;
	mcc_ast_visit_declaration_cb declaration;
	mcc_ast_visit_declaration_cb variable_declaration;
	mcc_ast_visit_declaration_cb array_declaration;

	mcc_ast_visit_type_cb type;
	mcc_ast_visit_identifier_cb identifier;
};

void mcc_ast_visit_expression(struct mcc_ast_expression *expression, struct mcc_ast_visitor *visitor);

void mcc_ast_visit_statement(struct mcc_ast_statement *statement, struct mcc_ast_visitor *visitor);

void mcc_ast_visit_compound_statement(struct mcc_ast_compound_statement *compound_statement,
                                      struct mcc_ast_visitor *visitor);

void mcc_ast_visit_literal(struct mcc_ast_literal *literal, struct mcc_ast_visitor *visitor);

void mcc_ast_visit_declaration(struct mcc_ast_declaration *declaration, struct mcc_ast_visitor *visitor);

void mcc_ast_visit_assignment(struct mcc_ast_assignment *assignment, struct mcc_ast_visitor *visitor);

void mcc_ast_visit_type(struct mcc_ast_type *type, struct mcc_ast_visitor *visitor);

void mcc_ast_visit_identifier(struct mcc_ast_identifier *identifier, struct mcc_ast_visitor *visitor);

void mcc_ast_visit_function_definition(struct mcc_ast_function_definition *function_definition,
                                       struct mcc_ast_visitor *visitor);

void mcc_ast_visit_parameters(struct mcc_ast_parameters *parameters, struct mcc_ast_visitor *visitor);

void mcc_ast_visit_arguments(struct mcc_ast_arguments *arguments, struct mcc_ast_visitor *visitor);

void mcc_ast_visit_program(struct mcc_ast_program *program, struct mcc_ast_visitor *visitor);

// clang-format off

#define mcc_ast_visit(x, visitor) _Generic((x), \
		struct mcc_ast_expression *: 		  mcc_ast_visit_expression, \
		struct mcc_ast_statement *: 		  mcc_ast_visit_statement, \
		struct mcc_ast_literal *:    		  mcc_ast_visit_literal, \
		struct mcc_ast_declaration *:		  mcc_ast_visit_declaration, \
		struct mcc_ast_assignment *: 		  mcc_ast_visit_assignment, \
		struct mcc_ast_type *: 				  mcc_ast_visit_type, \
		struct mcc_ast_identifier *: 		  mcc_ast_visit_identifier, \
		struct mcc_ast_compound_statement *:  mcc_ast_visit_compound_statement, \
		struct mcc_ast_program *: 			  mcc_ast_visit_program, \
		struct mcc_ast_function_definition *: mcc_ast_visit_function_definition, \
		struct mcc_ast_parameters *: 		  mcc_ast_visit_parameters, \
		struct mcc_ast_arguments *: 		  mcc_ast_visit_arguments \
	)(x, visitor)

// clang-format on

#endif // MCC_AST_VISIT_H

