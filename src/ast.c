#include "ast.h"
#include "lexer.h"
#include "utils.h"
#include <stdarg.h>
#include <stdio.h>

void ast_error(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  fprintf(stderr, ERROR ": ");
  vfprintf(stderr, fmt, args);
  fprintf(stderr, "\n");
  va_end(args);
}

void *print_literal(ExprVisitor *_, Token *literal) {
  (void)_; // unused

  switch (literal->type) {
  case TOKEN_NUMBER:
    printf("%f", literal->value.as.number_value);
    break;
  case TOKEN_STRING:
    printf("\"");
    PRINT_AS_STRING(string, literal);
    printf("\"");
    break;
  case TOKEN_IDENTIFIER:
    printf("[");
    PRINT_AS_STRING(identifier, literal);
    printf("]");
    break;
  case TOKEN_TRUE:
    printf("%s",  literal->value.as.bool_value ? "true" : "<err: false found when true>");
    break;
  case TOKEN_FALSE:
    printf("%s", literal->value.as.bool_value  ? "false" : "<err: true found when false>");
    break;
  default:
    printf("%s", TOKEN_REPRESENTATIONS[literal->type].name);
    break;
  }
  return NULL;
};

void *print_unary(ExprVisitor *visitor, UnaryExpr *expr) {
  printf("( ");
  printf("- ");
  expr_accept(expr->right, visitor);
  printf(" )");
  return NULL;
};

void *print_binary(ExprVisitor *visitor, BinaryExpr *expr) {
  printf("( ");
  printf("%s", TOKEN_REPRESENTATIONS[expr->op.type].symbol);
  printf(" ");
  expr_accept(expr->left, visitor);
  printf(" ");
  expr_accept(expr->right, visitor);
  printf(" )");
  return NULL;
}
void *print_grouping(ExprVisitor *visitor, GroupingExpr *expr) {
  printf("( group ");
  expr_accept(expr->expression, visitor);
  printf(" )");
  return NULL;
}

const ExprVisitor AstPrinter = {.visit_literal = print_literal,
                                .visit_unary = print_unary,
                                .visit_binary = print_binary,
                                .visit_grouping = print_grouping};

void *expr_accept(Expr *expr, ExprVisitor *visitor) {
  switch (expr->type) {
  case EXPR_LITERAL:
    return visitor->visit_literal(visitor, &expr->value.literal);
  case EXPR_UNARY:
    return visitor->visit_unary(visitor, &expr->value.unary);
  case EXPR_BINARY:
    return visitor->visit_binary(visitor, &expr->value.binary);
  case EXPR_GROUPING:
    return visitor->visit_grouping(visitor, &expr->value.grouping);
  }
  return NULL;
}
