#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"
//#include "AST_TREE.h"
#define CHUNK_SIZE 1000
#define SPACES 4

int lexeme_analyser(char* content, lexeme_list** head, lexeme_list** tail) {
	int position;
	int error = 0;
	*head = NULL;
	*tail = NULL;
	int start;
	position = 0;
	while (content[position] != '\0') {
		if (isdigit(content[position])) { // обработка числа
			start = position;
			while (isdigit(content[position])) { // пока позиция нормец
				position++;
			}
			if (!insert_lexeme(head, tail, NUMBER, content, start, position)) {
				return 0;
			}
		}
		else if ((content[position] == '+') || (content[position] == '-')) {
			start = position;
			position++;
			if (!insert_lexeme(head, tail, OPERATION, content, start, position)) {
				return 0;
			}
		}
		else if (content[position] == '(') {
			start = position;
			position++;
			if (!insert_lexeme(head, tail, LEFT_PAR, content, start, position)) {
				return 0;
			}
		}
		else if (content[position] == ')') {
			start = position;
			position++;
			if (!insert_lexeme(head, tail, RIGHT_PAR, content, start, position)) {
				return 0;
			}
		}
		else {
			error = 1;
			break;
		}

	}
	if (error) {
		return 0;
	}
	return 1;
}

int lexeme_analyser(char* content, lexeme_list** head, lexeme_list** tail) {
	int position;
	int error = 0;
	*head = NULL;
	*tail = NULL;
	int start;
	position = 0;
	while (content[position] != '\0') {
		if (isdigit(content[position])) { // обработка числа
			start = position;
			while (isdigit(content[position])) { // пока позиция нормец
				position++;
			}
			if (!insert_lexeme(head, tail, NUMBER, content, start, position)) {
				return 0;
			}
		}
		else if ((content[position] == '+') || (content[position] == '-')) {
			start = position;
			position++;
			if (!insert_lexeme(head, tail, OPERATION, content, start, position)) {
				return 0;
			}
		}
		else if (content[position] == '(') {
			start = position;
			position++;
			if (!insert_lexeme(head, tail, LEFT_PAR, content, start, position)) {
				return 0;
			}
		}
		else if (content[position] == ')') {
			start = position;
			position++;
			if (!insert_lexeme(head, tail, RIGHT_PAR, content, start, position)) {
				return 0;
			}
		}
		else {
			error = 1;
			break;
		}

	}
	if (error) {
		return 0;
	}
	return 1;
}

int print_XML(char* file_name, ast* root) {
	FILE* f = fopen(file_name, "w");
	if (NULL == f) {
		perror("fopen");
		return 0;
	}
	fprintf(f, "<?xml version =\"1.0\"?>\n");
	print_tree(f, root, 0);
	fclose(f);
	return 1;
}

void print_tree(FILE* f, ast* root, int count) {
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

		print_tree(f, root->p1, count + Spaces);
		print_tree(f, root->left, count + Spaces);
		print_tree(f, root->p2, count + Spaces);
		print_tree(f, root->right, count + Spaces);
		fprintf(f, "%*c</node>\n", count, ' ');
	}
}
