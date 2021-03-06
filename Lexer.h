#pragma once

typedef enum type
{
	A_NUMBER,
	A_OPERATION,
	A_LEFR_PAR,
	A_RIGHT_PAR,
	A_SUM,
	A_OTHER
}node_type;

typedef struct tree
{
	node_type type;
	char* value;
	struct tree* p1;
	struct tree* p2;
	struct tree* left;
	struct tree* right;
} ast;

typedef enum t
{
	NUMBER,
	OPERATION,
	LEFT_PAR,
	RIGHT_PAR
}lexeme_type;

typedef struct list
{
	lexeme_type type;
	char* value;
	struct list* previous;
	struct list* next;
}lexeme_list;

int print_XML(char* file_name, ast* root);
void print_tree(FILE* f, ast* root, int count);
int lexeme_analyser(char* content, lexeme_list** head, lexeme_list** tail);
int read_file(char* file_name, char** content, int* file_length);
int parse_other_empty(lexeme_list** a, ast** node, int* syntax_error, int* count);
int parse_other_operation(lexeme_list** a, ast** node, int* syntax_error, int* count);
int sum_choice(lexeme_list** a, ast** node, int* syntax_error, int* count);
int parse_sum_p(lexeme_list** a, ast** node, int* syntax_error, int* count);
int parse_sum(lexeme_list** a, ast** node, int* syntax_error, int* count);
int insert_lexeme(lexeme_list** head, lexeme_list** tail, lexeme_type type, char* content, int start, int end);