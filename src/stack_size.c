#include "mcc/stack_size.h"

#include "mcc/asm.h"
#include "mcc/ir.h"
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

struct mcc_annotated_ir *mcc_new_annotated_ir(struct mcc_ir_row *row, int stack_size)
{
	assert(row);
	struct mcc_annotated_ir *ir = malloc(sizeof(*ir));
	if (!ir)
		return NULL;
	// Hold stack size of current IR line. If line is func label, holds stack size of that function
	ir->stack_size = stack_size;
	ir->row = row;
	ir->next = NULL;
	ir->prev = NULL;
	return ir;
}

void mcc_delete_annotated_ir(struct mcc_annotated_ir *head)
{
	if (!head)
		return;

	mcc_delete_annotated_ir(head->next);
	free(head);
}

// --------------------------------------------------------------------------------------- Forward declarations

static int get_array_type_size(struct mcc_ir_row *ir);
static struct mcc_ir_row *first_line_of_function(struct mcc_ir_row *ir);
static int get_temporary_size(struct mcc_ir_row *ir);
static struct mcc_ir_row *last_line_of_function(struct mcc_ir_row *ir);
static int argument_size(struct mcc_ir_arg *arg, struct mcc_ir_row *ir);

// --------------------------------------------------------------------------------------- Calc stack size and position

// DONE
static bool assignment_is_first_occurence(struct mcc_ir_row *first, struct mcc_ir_row *ir)
{
	assert(first);
	assert(ir);
	assert(ir->instr == MCC_IR_INSTR_ASSIGN);

	// Arrays are allocated when they're declared
	if (ir->arg1->type == MCC_IR_TYPE_ARR_ELEM) {
		return false;
	}

	char *id_name = ir->arg1->ident->identifier_name;
	struct mcc_ir_row *head = first;
	while (head != ir) {
		if (head->instr != MCC_IR_INSTR_ASSIGN) {
			head = head->next_row;
			continue;
		}
		if (strcmp(head->arg1->ident->identifier_name, id_name) == 0) {
			return false;
		}
		head = head->next_row;
	}
	return true;
}

// Finds first occurence of variable (not array element) in a function
static struct mcc_ir_row *find_first_occurence(char *identifier, struct mcc_ir_row *ir)
{
	struct mcc_ir_row *first = first_line_of_function(ir);
	ir = first;
	struct mcc_ir_row *last = last_line_of_function(ir);
	while (ir && (ir != last)) {
		if (ir->instr == MCC_IR_INSTR_ASSIGN || ir->instr == MCC_IR_INSTR_ARRAY_BOOL ||
		    ir->instr == MCC_IR_INSTR_ARRAY_FLOAT || ir->instr == MCC_IR_INSTR_ARRAY_INT ||
		    ir->instr == MCC_IR_INSTR_ARRAY_STRING) {
			if (strcmp(identifier, ir->arg1->ident->identifier_name) == 0) {
				return ir;
			}
		}
		ir = ir->next_row;
	}

	return NULL;
}

// This function returns the size of an argument in the IR line.
// Since semantic consistency is guaranteed, we can infer the required size of an IR line
// by knowing the size of one of its arguments
static int argument_size(struct mcc_ir_arg *arg, struct mcc_ir_row *ir)
{
	assert(arg);
	struct mcc_ir_row *ref = NULL;

	switch (arg->type) {
	// Not on the stack
	case MCC_IR_TYPE_LIT_STRING:
		return STACK_SIZE_STRING;
	case MCC_IR_TYPE_LIT_INT:
		return STACK_SIZE_INT;
	case MCC_IR_TYPE_LIT_FLOAT:
		return STACK_SIZE_FLOAT;
	case MCC_IR_TYPE_LIT_BOOL:
		return STACK_SIZE_BOOL;
	case MCC_IR_TYPE_IDENTIFIER:
		ref = find_first_occurence(arg->ident->identifier_name, ir);
		if (!ref)
			return 0;
		return argument_size(ir->arg2, ir);
	// TODO: Find out size of the array element
	case MCC_IR_TYPE_ARR_ELEM:
		ref = find_first_occurence(arg->arr_ident->identifier_name, ir);
		if (!ref)
			return 0;
		return get_array_type_size(ref);
	case MCC_IR_TYPE_ROW:
		return get_temporary_size(arg->row);
	default:
		return 0;
	}
}

static int get_var_size(struct mcc_ir_row *ir)
{
	assert(ir);
	assert(ir->instr == MCC_IR_INSTR_ASSIGN);

	struct mcc_ir_row *first = first_line_of_function(ir);

	if (!assignment_is_first_occurence(first, ir)) {
		return 0;
	}
	// "a = x" -> find size of x
	return argument_size(ir->arg2, ir);
}

static struct mcc_ir_row *first_line_of_function(struct mcc_ir_row *ir)
{
	assert(ir);
	if (ir->instr == MCC_IR_INSTR_FUNC_LABEL)
		return ir;

	struct mcc_ir_row *prev = ir->prev_row;
	while (prev) {
		if (prev->instr == MCC_IR_INSTR_FUNC_LABEL) {
			return prev;
		}
		prev = prev->prev_row;
	}
	return NULL;
}

static struct mcc_ir_row *last_line_of_function(struct mcc_ir_row *ir)
{
	assert(ir->instr == MCC_IR_INSTR_FUNC_LABEL);
	assert(ir);
	struct mcc_ir_row *next = ir->next_row;
	while (next) {
		if (next->instr == MCC_IR_INSTR_FUNC_LABEL) {
			return ir;
		}
		ir = ir->next_row;
		next = next->next_row;
	}
	return ir;
}

// Done
static int get_temporary_size(struct mcc_ir_row *ir)
{
	assert(ir);
	assert(ir->arg1);
	return argument_size(ir->arg1, ir);
}

static int get_array_type_size(struct mcc_ir_row *ir)
{
	assert(ir);
	assert(ir->instr == MCC_IR_INSTR_ARRAY_BOOL || MCC_ASM_DECLARATION_TYPE_ARRAY_FLOAT ||
	       MCC_ASM_DECLARATION_TYPE_ARRAY_INT || MCC_ASM_DECLARATION_TYPE_ARRAY_STRING);
	assert(ir->arg2->type == MCC_IR_TYPE_LIT_INT);

	switch (ir->instr) {
	case MCC_IR_INSTR_ARRAY_BOOL:
		return STACK_SIZE_BOOL * (ir->arg2->lit_int);
	case MCC_IR_INSTR_ARRAY_FLOAT:
		return STACK_SIZE_FLOAT * (ir->arg2->lit_int);
	case MCC_IR_INSTR_ARRAY_INT:
		return STACK_SIZE_INT * (ir->arg2->lit_int);
	case MCC_IR_INSTR_ARRAY_STRING:
		return STACK_SIZE_STRING;
	// Unreached, as per assertion
	default:
		return 0;
	}
}

static int get_array_size(struct mcc_ir_row *ir)
{
	assert(ir);
	assert(ir->instr == MCC_IR_INSTR_ARRAY_BOOL || MCC_ASM_DECLARATION_TYPE_ARRAY_FLOAT ||
	       MCC_ASM_DECLARATION_TYPE_ARRAY_INT || MCC_ASM_DECLARATION_TYPE_ARRAY_STRING);
	assert(ir->arg2->type == MCC_IR_TYPE_LIT_INT);

	switch (ir->instr) {
	case MCC_IR_INSTR_ARRAY_BOOL:
		return STACK_SIZE_BOOL * (ir->arg2->lit_int);
	case MCC_IR_INSTR_ARRAY_FLOAT:
		return STACK_SIZE_FLOAT * (ir->arg2->lit_int);
	case MCC_IR_INSTR_ARRAY_INT:
		return STACK_SIZE_INT * (ir->arg2->lit_int);
	case MCC_IR_INSTR_ARRAY_STRING:
		return STACK_SIZE_STRING;
	// Unreached, as per assertion
	default:
		return 0;
	}
}

static int get_stack_frame_size(struct mcc_ir_row *ir)
{
	assert(ir);

	switch (ir->instr) {

	// Labels: Size 0
	case MCC_IR_INSTR_LABEL:
	case MCC_IR_INSTR_JUMPFALSE:
	case MCC_IR_INSTR_JUMP:
	case MCC_IR_INSTR_FUNC_LABEL:
		return 0;

	// Assignment of variables to immediate value or temporary:
	case MCC_IR_INSTR_ASSIGN:
		return get_var_size(ir);

	// Assignment of temporary: Int or Float
	case MCC_IR_INSTR_PLUS:
	case MCC_IR_INSTR_DIVIDE:
	case MCC_IR_INSTR_MINUS:
	case MCC_IR_INSTR_MULTIPLY:
	case MCC_IR_INSTR_NEGATIV:
		return get_temporary_size(ir);

	// Assignment of temporary: Bool (TODO: 4 bytes for bool?)
	case MCC_IR_INSTR_AND:
	case MCC_IR_INSTR_OR:
	case MCC_IR_INSTR_EQUALS:
	case MCC_IR_INSTR_NOTEQUALS:
	case MCC_IR_INSTR_GREATER:
	case MCC_IR_INSTR_GREATEREQ:
	case MCC_IR_INSTR_NOT:
	case MCC_IR_INSTR_SMALLER:
	case MCC_IR_INSTR_SMALLEREQ:
		return STACK_SIZE_BOOL;

	// TODO
	case MCC_IR_INSTR_POP:
	case MCC_IR_INSTR_PUSH:
		break;

	// TODO
	case MCC_IR_INSTR_CALL:
	case MCC_IR_INSTR_RETURN:
		return 0;

	// Arrays are located on the stack, and have special declaration IR instructions
	case MCC_IR_INSTR_ARRAY_BOOL:
	case MCC_IR_INSTR_ARRAY_INT:
	case MCC_IR_INSTR_ARRAY_FLOAT:
	case MCC_IR_INSTR_ARRAY_STRING:
		return get_array_size(ir);

	case MCC_IR_INSTR_UNKNOWN:
		return 0;
	}
	return 0;
}

static struct mcc_annotated_ir *add_stack_sizes(struct mcc_ir_row *ir)
{
	assert(ir);
	assert(ir->instr == MCC_IR_INSTR_FUNC_LABEL);

	struct mcc_annotated_ir *head = mcc_new_annotated_ir(ir, 0);
	struct mcc_annotated_ir *first = head;
	struct mcc_annotated_ir *new;

	while (ir) {
		int size = get_stack_frame_size(ir);
		new = mcc_new_annotated_ir(ir, size);
		if (!new) {
			mcc_delete_annotated_ir(first);
			return NULL;
		}
		// If size == 0, we basically copy the previous line's stack_position to correctly reference later
		// variables
		new->prev = head;
		head->next = new;
		head = new;

		ir = ir->next_row;
	}
	return first;
}

static int get_frame_size_of_function(struct mcc_annotated_ir *head)
{
	assert(head);
	assert(head->row->instr == MCC_IR_INSTR_FUNC_LABEL);

	int frame_size;
	struct mcc_ir_row *last = last_line_of_function(head->row);
	while (head->row != last) {
		frame_size = frame_size + head->stack_size;
		head = head->next;
	}
	return frame_size;
}

static void add_stack_positions(struct mcc_annotated_ir *head)
{
	assert(head);
	assert(head->row->instr == MCC_IR_INSTR_FUNC_LABEL);

	head->stack_size = get_frame_size_of_function(head);
	int current_position = 0;

	while (head) {
		if (head->row->instr == MCC_IR_INSTR_FUNC_LABEL) {
			head->stack_size = get_frame_size_of_function(head);
			current_position = 0;
			head = head->next;
			continue;
		}
		current_position = current_position - head->stack_size;
		head->stack_position = current_position;
		head = head->next;
	}
}

struct mcc_annotated_ir *mcc_annotate_ir(struct mcc_ir_row *ir)
{
	assert(ir);
	// First IR line should be function label
	assert(ir->instr == MCC_IR_INSTR_FUNC_LABEL);

	struct mcc_annotated_ir *an_head = add_stack_sizes(ir);
	if (!an_head)
		return NULL;
	add_stack_positions(an_head);
	return an_head;
}
