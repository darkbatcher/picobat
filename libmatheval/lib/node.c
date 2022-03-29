/*
 * Copyright (C) 1999, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2011,
 * 2012, 2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU libmatheval
 *
 * GNU libmatheval is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * GNU libmatheval is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU libmatheval.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <math.h>
#include <assert.h>
#include <stdarg.h>
#include "common.h"
#include "node.h"

extern _Thread_local int floats;
extern double(*get_var)(const char*);
extern double(*set_var)(const char*, double);

Node           *
node_create(char type, ...)
{
	Node           *node;	/* New node.  */
	va_list         ap;	/* Variable argument list.  */
	char            *p, *end; /* Pointers to convert to numbers */

	/* Allocate memory for node and initialize its type. */
	node = XMALLOC(Node, 1);
	node->type = type;

	/* According to node type, initialize rest of the node from
	 * variable argument list. */
	va_start(ap, type);
	switch (node->type) {
	case 'n':
		/* Initialize number value. */
		p =  va_arg(ap, char*);

		/* try getting an integer first */
		node->data.number = strtol(p, &end, 0);
        if (*end) {

            /* use float instead */
            floats = 1;
            node->data.number = atof(p);

        }
		break;

	case 'c':
		/* Remember pointer to symbol table record describing
		 * constant. */
        floats = 1;
		node->data.constant = va_arg(ap, Record *);
		break;

	case 'v':
		/* Remember pointer to symbol table record describing
		 * variable. */
		node->data.variable = va_arg(ap, Record *);
		break;

	case 'f':
		/* Remember pointer to symbol table record describing
		 * function and initialize function argument. */
        floats = 1;
		node->data.function.record = va_arg(ap, Record *);
		node->data.function.child = va_arg(ap, Node *);
		break;

	case 'u':
		/* Initialize operation type and operand. */
		node->data.un_op.operation = (char) va_arg(ap, int);

		node->data.un_op.child = va_arg(ap, Node *);
		break;

	case 'b':
		/* Initialize operation type and operands. */
		node->data.un_op.operation = (char) va_arg(ap, int);

		node->data.bin_op.left = va_arg(ap, Node *);
		node->data.bin_op.right = va_arg(ap, Node *);
		break;

	default:
		assert(0);
	}
	va_end(ap);

	return node;
}

void
node_destroy(Node * node)
{
	/* Skip if node already null (this may occur during
	 * simplification). */
	if (!node)
		return;

	/* If necessary, destroy subtree rooted at node. */
	switch (node->type) {
	case 'n':
	case 'c':
	case 'v':
		break;

	case 'f':
		node_destroy(node->data.function.child);
		break;

	case 'u':
		node_destroy(node->data.un_op.child);
		break;

	case 'b':
		node_destroy(node->data.bin_op.left);
		node_destroy(node->data.bin_op.right);
		break;
	}

	/* Deallocate memory used by node. */
	XFREE(node);
}

Node           *
node_copy(Node * node)
{
	/* According to node type, create (deep) copy of subtree rooted at
	 * node. */
	switch (node->type) {
	case 'n':
		return node_create('n', node->data.number);

	case 'c':
		return node_create('c', node->data.constant);

	case 'v':
		return node_create('v', node->data.variable);

	case 'f':
		return node_create('f', node->data.function.record,
				   node_copy(node->data.function.child));

	case 'u':
		return node_create('u', node->data.un_op.operation,
				   node_copy(node->data.un_op.child));

	case 'b':
		return node_create('b', node->data.bin_op.operation,
				   node_copy(node->data.bin_op.left),
				   node_copy(node->data.bin_op.right));
	}
}

Node           *
node_simplify(Node * node)
{

    #if 0
	/* According to node type, apply further simplifications.
	 * Constants are not simplified, in order to eventually appear
	 * unchanged in derivatives. */
	switch (node->type) {
	case 'n':
	case 'c':
	case 'v':
		return node;

	case 'f':
		/* Simplify function argument and if number evaluate
		 * function and replace function node with number node. */
		node->data.function.child =
		    node_simplify(node->data.function.child);
		if (node->data.function.child->type == 'n') {
			double          value = node_evaluate(node, 1);

			node_destroy(node);
			return node_create('n', value);
		} else
			return node;

	case 'u':
		/* Simplify unary operation operand and if number apply
		 * operation and replace operation node with number node. */
		node->data.un_op.child =
		    node_simplify(node->data.un_op.child);
		if (node->data.un_op.operation == '-'
		    && node->data.un_op.child->type == 'n') {
			double          value = node_evaluate(node, 1);

			node_destroy(node);
			return node_create('n', value);
		} else
			return node;

	case 'b':
		/* Simplify binary operation operands. */
		node->data.bin_op.left =
		    node_simplify(node->data.bin_op.left);
		node->data.bin_op.right =
		    node_simplify(node->data.bin_op.right);

		/* If operands numbers apply operation and replace
		 * operation node with number node. */
		if (node->data.bin_op.left->type == 'n'
		    && node->data.bin_op.right->type == 'n') {
			double          value = node_evaluate(node);

			node_destroy(node);
			return node_create('n', value);
		}
		/* Eliminate 0 as neutral addition operand. */
		else if (node->data.bin_op.operation == '+')
			if (node->data.bin_op.left->type == 'n'
			    && node->data.bin_op.left->data.number == 0) {
				Node           *right;

				right = node->data.bin_op.right;
				node->data.bin_op.right = NULL;
				node_destroy(node);
				return right;
			} else if (node->data.bin_op.right->type == 'n'
				   && node->data.bin_op.right->data.
				   number == 0) {
				Node           *left;

				left = node->data.bin_op.left;
				node->data.bin_op.left = NULL;
				node_destroy(node);
				return left;
			} else
				return node;
		/* Eliminate 0 as neutral subtraction right operand. */
		else if (node->data.bin_op.operation == '-')
			if (node->data.bin_op.right->type == 'n'
			    && node->data.bin_op.right->data.number == 0) {
				Node           *left;

				left = node->data.bin_op.left;
				node->data.bin_op.left = NULL;
				node_destroy(node);
				return left;
			} else
				return node;
		/* Eliminate 1 as neutral multiplication operand. */
		else if (node->data.bin_op.operation == '*')
			if (node->data.bin_op.left->type == 'n'
			    && node->data.bin_op.left->data.number == 1) {
				Node           *right;

				right = node->data.bin_op.right;
				node->data.bin_op.right = NULL;
				node_destroy(node);
				return right;
			} else if (node->data.bin_op.right->type == 'n'
				   && node->data.bin_op.right->data.
				   number == 1) {
				Node           *left;

				left = node->data.bin_op.left;
				node->data.bin_op.left = NULL;
				node_destroy(node);
				return left;
			} else
				return node;
		/* Eliminate 1 as neutral division right operand. */
		else if (node->data.bin_op.operation == '/')
			if (node->data.bin_op.right->type == 'n'
			    && node->data.bin_op.right->data.number == 1) {
				Node           *left;

				left = node->data.bin_op.left;
				node->data.bin_op.left = NULL;
				node_destroy(node);
				return left;
			} else
				return node;
		else
			return node;
	}
	#endif // 0

	return node;
}

double
node_evaluate(Node * node, int fmode)
{
	/* According to node type, evaluate subtree rooted at node. */
	switch (node->type) {
	case 'n':
		return node->data.number;

	case 'c':
		/* Constant values are used from symbol table. */
		return node->data.constant->data.value;

	case 'v':
		/* Variable values are used from symbol table. */
		return get_var(node->data.variable->name);

	case 'f':
		/* Functions are evaluated through symbol table. */
		return (*node->data.function.record->data.
			function) (node_evaluate(node->data.function.
						 child, fmode));

	case 'u':
		/* Unary operation node is evaluated according to
		 * operation type. */
		switch (node->data.un_op.operation) {
		case '-':
			return -node_evaluate(node->data.un_op.child, fmode);

        case '!':
            return (double)(!((int)node_evaluate(node->data.un_op.child, fmode)));

        case '~':
            return (double)(~((int)node_evaluate(node->data.un_op.child, fmode)));

		}

#define OPERATE_INT(op1, op, op2) \
    (double)(((int)op1) op ((int)(op2)));

#define OPERATE(op1, op, op2, mode) \
    if (!mode) \
        return OPERATE_INT(op1, op, op2) \
    else \
        return op1 op op2;

	case 'b':
		/* Binary operation node is evaluated according to
		 * operation type. */
		switch (node->data.un_op.operation) {
		case '+':
			return node_evaluate(node->data.bin_op.left, fmode)
                    + node_evaluate(node->data.bin_op.right, fmode);


		case '-':
			return node_evaluate(node->data.bin_op.left, fmode)
                    - node_evaluate(node->data.bin_op.right, fmode);

		case '*':
			return node_evaluate(node->data.bin_op.left, fmode)
                    * node_evaluate(node->data.bin_op.right, fmode);

		case '/':
		    /* This is the critical step when dealing with integers.
               indeed division is the only way you can get floating
               point numbers using integer-only expression. */
            if (fmode)
                return node_evaluate(node->data.bin_op.left, fmode)
                        / node_evaluate(node->data.bin_op.right, fmode);
            else
                return (double)((int)(node_evaluate(node->data.bin_op.left, fmode)
                        / node_evaluate(node->data.bin_op.right, fmode)));

        case '%':
            return fmod(node_evaluate(node->data.bin_op.left, 1),
                            node_evaluate(node->data.bin_op.right, 1));

        case '&':
            return OPERATE_INT(node_evaluate(node->data.bin_op.left, fmode),
                      &,
                    node_evaluate(node->data.bin_op.right, fmode));

        case 'a':
            return OPERATE_INT(node_evaluate(node->data.bin_op.left, fmode),
                      &&,
                    node_evaluate(node->data.bin_op.right, fmode));

        case '|':
            return OPERATE_INT(node_evaluate(node->data.bin_op.left, fmode),
                      |,
                    node_evaluate(node->data.bin_op.right, fmode));

        case 'o':
            return OPERATE_INT(node_evaluate(node->data.bin_op.left, fmode),
                      ||,
                    node_evaluate(node->data.bin_op.right, fmode));

		case '^':
			return OPERATE_INT(node_evaluate(node->data.bin_op.left, fmode),
                      ^,
                    node_evaluate(node->data.bin_op.right, fmode));

        case '>':
            return OPERATE_INT(node_evaluate(node->data.bin_op.left, fmode),
                      >>,
                    node_evaluate(node->data.bin_op.right, fmode));

        case '<':
            return OPERATE_INT(node_evaluate(node->data.bin_op.left, fmode),
                      <<,
                    node_evaluate(node->data.bin_op.right, fmode));

        case '=':
            if (node->data.bin_op.left->type != 'v')
                return node_evaluate(node->data.bin_op.right, fmode);

            return set_var(node->data.bin_op.left->data.variable->name,
                                node_evaluate(node->data.bin_op.right,fmode));
		}
	}

	return 0;
}

void
node_flag_variables(Node * node)
{
	/* According to node type, flag variable in symbol table or
	 * proceed with calling function recursively on node children. */
	switch (node->type) {
	case 'v':
		node->data.variable->flag = TRUE;
		break;

	case 'f':
		node_flag_variables(node->data.function.child);
		break;

	case 'u':
		node_flag_variables(node->data.un_op.child);
		break;

	case 'b':
		node_flag_variables(node->data.bin_op.left);
		node_flag_variables(node->data.bin_op.right);
		break;
	}
}

int
node_get_length(Node * node)
{
	FILE           *file;	/* Temporary file. */
	int             count;	/* Count of bytes written to above file. */
	int             length;	/* Length of above string. */

	/* According to node type, calculate length of string representing
	 * subtree rooted at node. */
	switch (node->type) {
	case 'n':
		length = 0;
		if (node->data.number < 0)
			length += 1;

		file = tmpfile();
		if (file) {
			if ((count =
			     fprintf(file, "%g", node->data.number)) >= 0)
				length += count;
			fclose(file);
		}

		if (node->data.number < 0)
			length += 1;
		return length;

	case 'c':
		return strlen(node->data.constant->name);

	case 'v':
		return strlen(node->data.variable->name);

	case 'f':
		return strlen(node->data.function.record->name) + 1 +
		    node_get_length(node->data.function.child) + 1;
		break;

	case 'u':
		return 1 + 1 + node_get_length(node->data.un_op.child) + 1;

	case 'b':
		return 1 + node_get_length(node->data.bin_op.left) + 1 +
		    node_get_length(node->data.bin_op.right) + 1;
	}

	return 0;
}

void
node_write(Node * node, char *string)
{
	/* According to node type, write subtree rooted at node to node
	 * string variable.  Always use parenthesis to resolve operation
	 * precedence. */
	switch (node->type) {
	case 'n':
		if (node->data.number < 0) {
			sprintf(string, "%c", '(');
			string += strlen(string);
		}
		sprintf(string, "%g", node->data.number);
		string += strlen(string);
		if (node->data.number < 0)
			sprintf(string, "%c", ')');
		break;

	case 'c':
		sprintf(string, "%s", node->data.constant->name);
		break;

	case 'v':
		sprintf(string, "%s", node->data.variable->name);
		break;

	case 'f':
		sprintf(string, "%s%c", node->data.function.record->name,
			'(');
		string += strlen(string);
		node_write(node->data.function.child, string);
		string += strlen(string);
		sprintf(string, "%c", ')');
		break;

	case 'u':
		sprintf(string, "%c", '(');
		string += strlen(string);
		sprintf(string, "%c", node->data.un_op.operation);
		string += strlen(string);
		node_write(node->data.un_op.child, string);
		string += strlen(string);
		sprintf(string, "%c", ')');
		break;

	case 'b':
		sprintf(string, "%c", '(');
		string += strlen(string);
		node_write(node->data.bin_op.left, string);
		string += strlen(string);
		sprintf(string, "%c", node->data.bin_op.operation);
		string += strlen(string);
		node_write(node->data.bin_op.right, string);
		string += strlen(string);
		sprintf(string, "%c", ')');
		break;
	}
}
