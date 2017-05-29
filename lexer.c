#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"
#include "AST_TREE.h"
#define CHUNK_SIZE 1000
#define SPACES 4

int insert_lexeme(lexeme_list** head, lexeme_list** tail, lexeme_type type, char* content, int start, int end)
{
	int count;
	char* q;
	lexeme_list* p;
	p = (lexeme_list*)malloc(sizeof(lexeme_list));
	if (NULL == p)
	{
		return 0;
	}
	count = end - start;
	q = (char*)malloc(count + 1);
	if (NULL == q)
	{
		free(p);
		return 0;
	}
	memmove(q, content + start, count);
	q[count] = '\0';
	p->type = type;
	p->value = q;
	p->previous = *tail;
	p->next = NULL;
	if (NULL != (*tail))
	{
		(*tail)->next = p;
	}
	if (NULL == (*head))
	{
		*head = p;
	}
	*tail = p;
	return 1;
}

int read_file(char* file_name, char** content, int* file_length)
{
	int allocated = 0;
	char* q = NULL;
	char* p = NULL;
	int numread = 0;
	FILE *f;
	if (0 != fopen_s(&f, file_name, "r"))
	{
		return 0;
	}
	*content = NULL;
	*file_length = 0;
	do
	{
		q = realloc(p, allocated + CHUNK_SIZE);
		if (NULL == q)
		{
			fclose(f);
			free(p);
			return 0;
		}
		p = q;
		allocated += CHUNK_SIZE;
		numread = fread(p + *file_length, 1, CHUNK_SIZE, f);
		*file_length += numread;
	} while (numread == CHUNK_SIZE);
	fclose(f);
	*content = (char*)malloc(*file_length);
	if (NULL == *content)
	{
		free(p);
		return 0;
	}
	memmove((*content), p, *file_length);
	free(p);
	return 1;
}

int lexeme_analyser(char* content, lexeme_list** head, lexeme_list** tail) 
{
	int position;
	int error = 0;
	*head = NULL;
	*tail = NULL;
	int start;
	position = 0;
	while (content[position] != '\0') {
		if (isdigit(content[position])) 
		{ // обработка числа
			start = position;
			while (isdigit(content[position]))// пока позиция нормец
				position++;
			if (!insert_lexeme(head, tail, NUMBER, content, start, position))
				return 0;
		}
		else if ((content[position] == '+') || (content[position] == '-')) 
		{
			start = position;
			position++;
			if (!insert_lexeme(head, tail, OPERATION, content, start, position))
				return 0;
		}
		else if (content[position] == '(') 
		{
			start = position;
			position++;
			if (!insert_lexeme(head, tail, LEFT_PAR, content, start, position))
				return 0;
		}
		else if (content[position] == ')') 
		{
			start = position;
			position++;
			if (!insert_lexeme(head, tail, RIGHT_PAR, content, start, position))
				return 0;
		}
		else 
		{
			error = 1;
			break;
		}
	}
	if (error)
		return 0;
	return 1;
}

int print_XML(char* file_name, ast* root) 
{
	//Open file.
	FILE* f;
	//If we can not open the file -> return an error.
	if (0 != fopen_s(&f, file_name, "w"))
	{
		perror("fopen");
		return 0;
	}
	fprintf(f, "<?xml version =\"1.0\"?>\n");
	print_tree(f, root, 0);
	fclose(f);
	return 1;
}

void print_tree(FILE* f, ast* root, int count) 
{
	if (NULL != root) {
		if (root->value != NULL) {
			fprintf(f, "%*c<node value=\"%s\" type=\"", count, ' ', root->value);
		}
		else
		{
			fprintf(f, "%*c<node value=\"\" type = \"", count, ' ');
		}
		if (A_LEFR_PAR == (root->type)) {
			fprintf(f, "AST_LEFT_PAR\">\n");
		}

		if (A_NUMBER == (root->type)) {
			fprintf(f, "AST_NUMBER\">\n");
		}
		if (A_RIGHT_PAR == (root->type)) {
			fprintf(f, "AST_RIGHT_PAR\">\n");
		}

		if (A_OPERATION == (root->type)) {
			fprintf(f, "AST_OPERATION\">\n");
		}

		if (A_SUM == (root->type)) {
			fprintf(f, "AST_SUM\">\n");
		}

		if (A_OTHER == (root->type)) {
			fprintf(f, "AST_REST\">\n");
		}

		print_tree(f, root->p1, count + SPACES);
		print_tree(f, root->left, count + SPACES);
		print_tree(f, root->p2, count + SPACES);
		print_tree(f, root->right, count + SPACES);
		fprintf(f, "%*c</node>\n", count, ' ');
	}
}

int parse_sum(lexeme_list** a, ast** node, int* syntax_error, int* count) {
	int result = 0;

	if (!cr_node(node, A_SUM, NULL)) {
		*syntax_error = 0;
		return 0;
	}

	if (!((*a)->type == NUMBER)) {
		*syntax_error = 1;
		return 0;
	}

	if ((*a)->type == NUMBER) {
		result = cr_node((&((*node)->left)), A_NUMBER, (*a)->value);
	}

	else {
		*syntax_error = 0;
		return 0;
	}
	if (!result) {
		*syntax_error = 0;
		return 0;
	}

	*a = (*a)->next;

	if (*a == NULL) {
		result = parse_other_empty(a, &((*node)->right), syntax_error, count);
	}
	else if (((*a)->type == RIGHT_PAR) && ((*count) > 0)) {
		result = parse_other_empty(a, &((*node)->right), syntax_error, count);
	}
	else if (((*a)->type == RIGHT_PAR) && ((*count) <= 0)) {
		*syntax_error = 1;
		return 0;
	}
	else if ((*a)->type == OPERATION) {
		result = parse_other_operation(a, &((*node)->right), syntax_error, count);
	}
	else {
		*syntax_error = 1;
		return 0;
	}
	return result;
}

int parse_sum_p(lexeme_list** a, ast** node, int* syntax_error, int* count) {
	int result;
	if (!cr_node(node, A_SUM, NULL)) {
		*syntax_error = 0;
		return 0;
	}
	if ((!((*a)->type == LEFT_PAR)) || ((*count) < 0)) {
		*syntax_error = 1;
		return 0;
	}
	if ((*a)->type == LEFT_PAR) {
		(*count)++;
		result = cr_node((&((*node)->p1)), A_LEFR_PAR, (*a)->value);
	}
	else {
		*syntax_error = 1;
		return 0;
	}

	if (!result) {
		*syntax_error = 0;
		return 0;
	}

	*a = (*a)->next;

	if (*a == NULL) {
		*syntax_error = 1;
		return 0;
	}
	if (((*a)->type == NUMBER) || ((*a)->type == LEFT_PAR)) {
		result = sum_choice(a, &((*node)->left), syntax_error, count);
	}
	else {
		*syntax_error = 1;
		return 0;
	}

	if (!result) {
		return 0;
	}

	if ((*a)->type == RIGHT_PAR) {
		(*count)--;
		result = cr_node((&((*node)->p2)), A_RIGHT_PAR, (*a)->value);
	}
	else {
		*syntax_error = 1;
		return 0;
	}

	if (!result) {
		*syntax_error = 0;
		return 0;
	}

	*a = (*a)->next;

	if (*a == NULL) {
		result = parse_other_empty(a, &((*node)->right), syntax_error, count);
	}
	else if (((*a)->type == RIGHT_PAR) && ((*count) > 0)) {
		result = parse_other_empty(a, &((*node)->right), syntax_error, count);
	}
	else if (((*a)->type == RIGHT_PAR) && ((*count) <= 0)) {
		*syntax_error = 1;
		return 0;
	}
	else if ((*a)->type == OPERATION) {
		result = parse_other_operation(a, &((*node)->right), syntax_error, count);
	}
	else {
		*syntax_error = 1;
		return 0;
	}

	return result;
}

int sum_choice(lexeme_list** a, ast** node, int* syntax_error, int* count) {
	if ((*a)->type == NUMBER)
		return parse_sum(a, node, syntax_error, count);
	if ((*a)->type == LEFT_PAR)
		return parse_sum_p(a, node, syntax_error, count);
	else
		return 0;
}

int parse_other_operation(lexeme_list** a, ast** node, int* syntax_error, int* count) {
	int result;
	if (!cr_node(node, A_OTHER, NULL)) {
		*syntax_error = 0;
		return 0;
	}
	if (!((*a)->type == OPERATION)) {
		*syntax_error = 1;
		return 0;
	}
	if ((*a)->type == OPERATION) {
		result = cr_node((&((*node)->left)), A_OPERATION, (*a)->value);
	}
	else {
		*syntax_error = 0;
		return 0;
	}
	if (!result) {
		*syntax_error = 0;
		return 0;
	}
	*a = (*a)->next;
	if (*a == NULL) {
		result = parse_other_empty(a, &((*node)->left), syntax_error, count);
	}
	if ((*a)->type == NUMBER || (*a)->type == LEFT_PAR) {
		result = sum_choice(a, &((*node)->right), syntax_error, count);
	}
	else {
		*syntax_error = 1;
		return 0;
	}
	return result;
}

int parse_other_empty(lexeme_list** a, ast** node, int* syntax_error, int* count) {
	return 1;
}
