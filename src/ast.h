#ifndef AST_H
#define AST_H

#include "lexer.h"

#define AST_EXIT_FAILURE 2 

typedef struct Expr Expr;

typedef enum {
    EXPR_LITERAL,
    EXPR_UNARY,
    EXPR_BINARY,
    EXPR_GROUPING,
} ExprType;

typedef struct {
    Token op;        // "-" or "!"
    Expr* right;
} UnaryExpr;

typedef struct {
    Expr* left;
    Token op;        // "+", "-", "*", "/", etc.
    Expr* right;
} BinaryExpr;

typedef struct {
    Expr* expression;
} GroupingExpr;

typedef union {
    Token literal;
    UnaryExpr unary;
    BinaryExpr binary;
    GroupingExpr grouping;
} ExprValue;

struct Expr {
    ExprType type;
    ExprValue value;
};

typedef struct ExprVisitor {
    void* (*visit_literal)(struct ExprVisitor*, Token*);
    void* (*visit_unary)(struct ExprVisitor*, UnaryExpr*);
    void* (*visit_binary)(struct ExprVisitor*, BinaryExpr*);
    void* (*visit_grouping)(struct ExprVisitor*, GroupingExpr*);
} ExprVisitor;

void* expr_accept(Expr* expr, ExprVisitor* visitor);

extern const ExprVisitor AstPrinter;

#endif  // AST_H
