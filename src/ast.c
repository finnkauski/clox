#include "ast.h"
#include "lexer.h"
#include "utils.h"
#include <stdarg.h>
#include <stdio.h>

Expr *expression(Parser *parser);
Expr *equality(Parser *parser);
Expr *comparison(Parser *parser);
Expr *term(Parser *parser);
Expr *factor(Parser *parser);
Expr *unary(Parser *parser);
Expr *primary(Parser *parser);

#define arrput_getp(arr, value)                                                \
  ((arrput((arr), (value))), &((arr)[arrlen(arr) - 1]))

void ast_error(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  fprintf(stderr, ERROR ": ");
  vfprintf(stderr, fmt, args);
  fprintf(stderr, "\n");
  va_end(args);
}

// AstPrinter
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
    printf("%s", literal->value.as.bool_value ? "true"
                                              : "<err: false found when true>");
    break;
  case TOKEN_FALSE:
    printf("%s", literal->value.as.bool_value ? "false"
                                              : "<err: true found when false>");
    break;
  default:
    printf("%s", TOKEN_REPRESENTATIONS[literal->type].name);
    break;
  }
  return NULL;
};

void *print_unary(ExprVisitor *visitor, UnaryExpr *expr) {
  printf("( ");
  printf("%s ", TOKEN_REPRESENTATIONS[expr->op.type].symbol);
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

// Parser
static Token previous(Parser *parser) {
  return parser->tokens[parser->index - 1];
}
static Token peek(Parser *parser) { return parser->tokens[parser->index]; }
static bool finished(Parser *parser) { return peek(parser).type == TOKEN_EOF; }
static bool check(Parser *parser, TokenType expected) {
  if (finished(parser))
    return false;
  return (parser->current->type == expected);
}

static Token advance(Parser *parser) {
  debug("Advanced parser");
  ASSERT(!finished(parser) || !parser->finished,
         "Tried advancing a finished parser.");

  Token token;
  if (finished(parser)) {
    parser->finished = true;
    debug("Trying to advance a finished parser");
    token = peek(parser);
  } else {
    parser->index++;
    parser->current = &parser->tokens[parser->index];
    token = previous(parser);
  }

  return token;
};

static bool match(Parser *parser, uint32_t count, ...) {
  debug("Matching [%d] values", count);
  va_list args;
  va_start(args, count);
  for (uint32_t i = 0; i < count; i++) {
    TokenType expected = va_arg(args, TokenType);
    debug("-- Trying to match: %s", TOKEN_REPRESENTATIONS[expected].name);
    if (check(parser, expected)) {
      debug("Match SUCCESS");
      advance(parser);
      va_end(args);
      return true;
    }
  }

  va_end(args);
  return false;
}

Expr *expression(Parser *parser) {
  debug("Parsing [EXPRESSION]");
  return equality(parser);
}

Expr *equality(Parser *parser) {
  Expr *expr = comparison(parser);
  debug("Parsing [EQUALITY]");
  while (match(parser, 2, TOKEN_BANG_EQUAL, TOKEN_EQUAL_EQUAL)) {
    Token operator = previous(parser);
    debug("Operator");
    debug_token(&operator);
    Expr *right = comparison(parser);
    expr = arrput_getp(
        parser->expressions,
        ((Expr){
            .type = EXPR_BINARY,
            .value = {.binary = {.left = expr, .op = operator, .right = right}}

        }));
  }
  return expr;
}

Expr *comparison(Parser *parser) {
  debug("Parsing [COMPARISON]");
  Expr *expr = term(parser);
  while (match(parser, 2, TOKEN_GREATER, TOKEN_GREATER_EQUAL, TOKEN_LESS,
               TOKEN_LESS_EQUAL)) {
    Token operator = previous(parser);
    debug("Operator");
    debug_token(&operator);
    Expr *right = term(parser);
    expr = arrput_getp(
        parser->expressions,
        ((Expr){
            .type = EXPR_BINARY,
            .value = {.binary = {.left = expr, .op = operator, .right = right}}

        }));
  }
  return expr;
}

Expr *term(Parser *parser) {
  Expr *expr = factor(parser);
  debug("Parsing [TERM]");
  while (match(parser, 2, TOKEN_MINUS, TOKEN_PLUS)) {
    Token operator = previous(parser);
    debug("Operator");
    debug_token(&operator);
    Expr *right = factor(parser);
    expr = arrput_getp(
        parser->expressions,
        ((Expr){
            .type = EXPR_BINARY,
            .value = {.binary = {.left = expr, .op = operator, .right = right}}

        }));
  }
  return expr;
}

Expr *factor(Parser *parser) {
  Expr *expr = unary(parser);
  debug("Parsing [FACTOR]");
  while (match(parser, 2, TOKEN_SLASH, TOKEN_STAR)) {
    Token operator = previous(parser);
    debug("Operator");
    debug_token(&operator);
    Expr *right = unary(parser);
    expr = arrput_getp(
        parser->expressions,
        ((Expr){
            .type = EXPR_BINARY,
            .value = {.binary = {.left = expr, .op = operator, .right = right}}

        }));
  }
  return expr;
}

Expr *unary(Parser *parser) {
  debug("Parsing [UNARY]");
  if (match(parser, 2, TOKEN_BANG, TOKEN_MINUS)) {
    Token operator = previous(parser);
    debug("Operator");
    debug_token(&operator);
    Expr *right = unary(parser);
    return arrput_getp(
        parser->expressions,
        ((Expr){.type = EXPR_UNARY,
                .value = {.unary = {.op = operator, .right = right}}}));
  }
  return primary(parser);
}

Expr *primary(Parser *parser) {
  debug("Parsing [PRIMARY]");
  if (match(parser, 5, TOKEN_FALSE, TOKEN_TRUE, TOKEN_NIL, TOKEN_STRING,
            TOKEN_NUMBER))
    return arrput_getp(
        parser->expressions,
        ((Expr){.type = EXPR_LITERAL,
                .value = {.literal = previous(
                              parser)}})); // previous as match advances

  if (match(parser, 1, TOKEN_LEFT_PAREN)) {
    Expr *right = expression(parser);
    if (advance(parser).type != TOKEN_RIGHT_PAREN) {
      ast_error("Expected ')' after expression.");
      exit(AST_EXIT_FAILURE);
    };
    return arrput_getp(parser->expressions,
                       ((Expr){.type = EXPR_GROUPING,
                               .value = {.grouping = {.expression = right}}}));
  } else {
    ast_error(
        "Unreachable parsing state while parsing primary. Failed on token: ");
    Token token = peek(parser);
    debug_token(&token);
    exit(AST_EXIT_FAILURE);
  }
}

Parser parse(Lexer *lexer) {
  Parser parser = {.current = &lexer->tokens[0],
                   .n_tokens = arrlenu(&lexer->tokens),
                   .source_filename = lexer->source_filename,

                   .tokens = lexer->tokens,
                   .expressions = NULL,

                   .finished = false,
                   .had_error = false,

                   .root = NULL};

  parser.root = expression(&parser);

  return parser;
}

void free_parser(Parser *parser) { arrfree(parser->expressions); }
