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
