#include "mcc/ast.h"

#include <assert.h>
#include <stdlib.h>

// included for debugging:

#include <stdio.h>


// ---------------------------------------------------------------- Expressions

struct mcc_ast_expression *mcc_ast_new_expression_literal(struct mcc_ast_literal *literal)
{
	assert(literal);

	struct mcc_ast_expression *expr = malloc(sizeof(*expr));
	if (!expr) {
		return NULL;
	}

	expr->type = MCC_AST_EXPRESSION_TYPE_LITERAL;
	expr->literal = literal;
	return expr;
}

struct mcc_ast_expression *mcc_ast_new_expression_binary_op(enum mcc_ast_binary_op op,
                                                            struct mcc_ast_expression *lhs,
                                                            struct mcc_ast_expression *rhs)
{
	assert(lhs);
	assert(rhs);

	struct mcc_ast_expression *expr = malloc(sizeof(*expr));
	if (!expr) {
		return NULL;
	}

	expr->type = MCC_AST_EXPRESSION_TYPE_BINARY_OP;
	expr->op = op;
	expr->lhs = lhs;
	expr->rhs = rhs;
	return expr;
}

struct mcc_ast_expression *mcc_ast_new_expression_parenth(struct mcc_ast_expression *expression)
{
	assert(expression);

	struct mcc_ast_expression *expr = malloc(sizeof(*expr));
	if (!expr) {
		return NULL;
	}

	expr->type = MCC_AST_EXPRESSION_TYPE_PARENTH;
	expr->expression = expression;
	return expr;
}

struct mcc_ast_expression *mcc_ast_new_expression_unary_op(enum mcc_ast_unary_op u_op, struct mcc_ast_expression *child)
{
	assert(child);

	struct mcc_ast_expression *expr = malloc(sizeof(*expr));
	if(!expr) {
		return NULL;
	}

	expr->type = MCC_AST_EXPRESSION_TYPE_UNARY_OP;
	expr->u_op = u_op;
	expr->child = child;
	return expr;
}

struct mcc_ast_expression *mcc_ast_new_expression_variable(char *identifier){

	assert(identifier);

	struct mcc_ast_expression *expr = malloc(sizeof(*expr));
	if (!expr) {
		return NULL;
	}

	struct mcc_ast_identifier *id = mcc_ast_new_identifier(identifier);

	expr->type = MCC_AST_EXPRESSION_TYPE_VARIABLE;
	expr->identifier = id;

	return expr;


}

struct mcc_ast_expression *mcc_ast_new_expression_array_element(char* identifier, struct mcc_ast_expression *index){

	assert(identifier);
	assert(index);

	struct mcc_ast_expression *expr = malloc(sizeof(*expr));
	if (!expr) {
		return NULL;
	}

	struct mcc_ast_identifier *id = mcc_ast_new_identifier(identifier);

	expr->type = MCC_AST_EXPRESSION_TYPE_ARRAY_ELEMENT;
	expr->array_identifier = id;
	expr->index = index;

	return expr;


}

void mcc_ast_delete_expression(struct mcc_ast_expression *expression)
{
	assert(expression);

	switch (expression->type) {
	case MCC_AST_EXPRESSION_TYPE_LITERAL:
		mcc_ast_delete_literal(expression->literal);
		break;

	case MCC_AST_EXPRESSION_TYPE_BINARY_OP:
		mcc_ast_delete_expression(expression->lhs);
		mcc_ast_delete_expression(expression->rhs);
		break;

	case MCC_AST_EXPRESSION_TYPE_PARENTH:
		mcc_ast_delete_expression(expression->expression);
		break;

    case MCC_AST_EXPRESSION_TYPE_UNARY_OP:
        mcc_ast_delete_expression(expression->child);
        break;

	case MCC_AST_EXPRESSION_TYPE_VARIABLE:
		mcc_ast_delete_identifier(expression->identifier);
		break;

	case MCC_AST_EXPRESSION_TYPE_ARRAY_ELEMENT:
		mcc_ast_delete_identifier(expression->array_identifier);
		mcc_ast_delete_expression(expression->index);
		break;

	}

	free(expression);
}

// ------------------------------------------------------------------ Types

void mcc_ast_delete_type(struct mcc_ast_type *type){
    assert(type);
    assert(type->node);
    free(type->node),
    free(type);
}

struct mcc_ast_type *mcc_ast_new_type(enum mcc_ast_types type){
	struct mcc_ast_type *newtype = malloc (sizeof(*newtype));
	if(!newtype){
		return NULL;
	}
	newtype->type_value = type;
	return newtype;
}

// ------------------------------------------------------------------ Declarations

struct mcc_ast_variable_declaration *mcc_ast_new_variable_declaration(enum mcc_ast_types type, char* identifier){

	assert(identifier);

	struct mcc_ast_variable_declaration *decl = malloc (sizeof(*decl));
	if (!decl) {
		return NULL;
	}

	struct mcc_ast_identifier *id = mcc_ast_new_identifier(identifier);

	struct mcc_ast_type *newtype = mcc_ast_new_type(type);

	decl->type = newtype;
	decl->identifier = id;

	return decl;
}

void mcc_ast_delete_variable_declaration(struct mcc_ast_variable_declaration* decl){
    assert(decl);
    mcc_ast_delete_identifier(decl->identifier);
    mcc_ast_delete_type(decl->type);
    free(decl);
}

struct mcc_ast_array_declaration *mcc_ast_new_array_declaration(enum mcc_ast_types type, struct mcc_ast_literal* size, char* identifier){

	assert(identifier);
	assert(size);

	struct mcc_ast_array_declaration *array_decl = malloc (sizeof(*array_decl));
	if (!array_decl) {
		return NULL;
	}

	struct mcc_ast_identifier *id = mcc_ast_new_identifier(identifier);

	struct mcc_ast_type *newtype = mcc_ast_new_type(type);

	array_decl->type = newtype;
	array_decl->identifier = id;
	array_decl->size = size;

	return array_decl;
}

void mcc_ast_delete_array_declaration(struct mcc_ast_array_declaration* array_decl){
	assert(array_decl);
	mcc_ast_delete_identifier(array_decl->identifier);
	mcc_ast_delete_type(array_decl->type);
	mcc_ast_delete_literal(array_decl->size);
	free(array_decl);
}

// ------------------------------------------------------------------ Assignments


struct mcc_ast_variable_assignment *mcc_ast_new_variable_assignment (char *identifier, struct mcc_ast_expression *assigned_value){
	assert(identifier);
	assert(assigned_value);
	struct mcc_ast_variable_assignment *variable_assignment = malloc(sizeof(*variable_assignment));
	if(variable_assignment == NULL){
		return NULL;
	}
	variable_assignment->identifier = mcc_ast_new_identifier(identifier);
	variable_assignment->assigned_value = assigned_value;
	return variable_assignment;
}


struct mcc_ast_array_assignment *mcc_ast_new_array_assignment (char *identifier, struct mcc_ast_expression *index, struct mcc_ast_expression *assigned_value){

	assert(identifier);
	assert(assigned_value);
	assert(index);
	struct mcc_ast_array_assignment *array_assignment = malloc(sizeof(sizeof(*array_assignment)));
	if(array_assignment == NULL){
		return NULL;
	}
	array_assignment->identifier = mcc_ast_new_identifier(identifier);
	array_assignment->assigned_value = assigned_value;
	array_assignment->index = index;
	return array_assignment;
}

// ------------------------------------------------------------------ Identifier


struct mcc_ast_identifier *mcc_ast_new_identifier(char *identifier){
	assert(identifier);

	struct mcc_ast_identifier *expr = malloc(sizeof(*expr));
	if (!expr) {
		return NULL;
	}

	expr->identifier_name = identifier;

	return expr;
}

void mcc_ast_delete_identifier(struct mcc_ast_identifier *identifier){
	assert(identifier);
	assert(identifier->identifier_name);
	free(identifier->identifier_name);
	free(identifier);

}

// ------------------------------------------------------------------- Statements

struct mcc_ast_statement *mcc_ast_new_statement_if_stmt( struct mcc_ast_expression *condition,
													    struct mcc_ast_statement *on_true)
{
	assert(condition);
	assert(on_true);

	struct mcc_ast_statement *statement = malloc(sizeof(*statement));
	if(!statement) {
		return NULL;
	}

	statement->type = MCC_AST_STATEMENT_TYPE_IF_STMT;
	statement->if_condition = condition;
	statement->if_on_true = on_true;

	return statement;
}

struct mcc_ast_statement *mcc_ast_new_statement_if_else_stmt( struct mcc_ast_expression *condition,
										   struct mcc_ast_statement *on_true,
										   struct mcc_ast_statement *on_false)
{
	assert(condition);
	assert(on_true);
	assert(on_false);

	struct mcc_ast_statement *statement = malloc(sizeof(*statement));
	if(!statement) {
		return NULL;
	}

	statement->type = MCC_AST_STATEMENT_TYPE_IF_ELSE_STMT;
	statement->if_else_condition = condition;
	statement->if_else_on_true = on_true;
	statement->if_else_on_false = on_false;

	return statement;
}

struct mcc_ast_statement *mcc_ast_new_statement_expression( struct mcc_ast_expression *expression)
{
	assert(expression);

	struct mcc_ast_statement *statement = malloc(sizeof(*statement));
	if(!statement) {
		return NULL;
	}

	statement->type = MCC_AST_STATEMENT_TYPE_EXPRESSION;
	statement->stmt_expression = expression;

	return statement;
}

//TODO mcc_ast_delete_statement()

// ------------------------------------------------------------------- Literals

struct mcc_ast_literal *mcc_ast_new_literal_int(long value)
{
	struct mcc_ast_literal *lit = malloc(sizeof(*lit));
	if (!lit) {
		return NULL;
	}

	lit->type = MCC_AST_LITERAL_TYPE_INT;
	lit->i_value = value;
	return lit;
}

struct mcc_ast_literal *mcc_ast_new_literal_float(double value)
{
	struct mcc_ast_literal *lit = malloc(sizeof(*lit));
	if (!lit) {
		return NULL;
	}

	lit->type = MCC_AST_LITERAL_TYPE_FLOAT;
	lit->f_value = value;
	return lit;
}

struct mcc_ast_literal *mcc_ast_new_literal_string(char* value)
{
	struct mcc_ast_literal *lit = malloc(sizeof(*lit));
	if (!lit) {
		return NULL;
	}

	lit->type = MCC_AST_LITERAL_TYPE_STRING;
	lit->string_value = mcc_remove_quotes_from_string(value);


	return lit;
}

char* mcc_remove_quotes_from_string(char* string){

	assert(string);
	char* intermediate = (char*) malloc (strlen(string)*sizeof(char));
	strncpy (intermediate, string+1, strlen(string)-2);
	return intermediate;

}


struct mcc_ast_literal *mcc_ast_new_literal_bool(bool value)
{
	struct mcc_ast_literal *lit = malloc(sizeof(*lit));
	if (!lit) {
		return NULL;
	}

	lit->type = MCC_AST_LITERAL_TYPE_BOOL;
	lit->bool_value = value;
	return lit;
}

void mcc_ast_delete_literal(struct mcc_ast_literal *literal)
{
	assert(literal);
	if(literal->type == MCC_AST_LITERAL_TYPE_STRING){
		free(literal->string_value);
	}
	free(literal);
}
