%{
  #ifdef WIN32
  #define YY_NO_UNISTD_H
  int isatty(int i) { return 0;}
  #endif

  #include <stdlib.h>
  #include <string.h>
  #define YY_DECL int yylex(void* yyval)


  typedef enum alpha_token_type_t {
    id, string, comment
  }alpha_token_type_t;

  typedef struct alpha_token_t {
    alpha_token_type_t type;
    char *value;
  }alpha_token_t;

  void fill_token(alpha_token_t *token, char *yytext, alpha_token_type_t type) {
    if (token == NULL) {
      printf("Token memory not allocated");
      exit(-1);
    }
    /* token->value = (char*) malloc(sizeof(yytext)+1) ; //to be freed */
    printf("%s\n", strlen(yytext));
    /* strncpy(token->value, yytext, strlen(yytext)+1); */
    token->value = yytext;
    token->type = type;
  }
%}

%option PREFIX="alpha_yy"
%option noyywrap
%option yylineno

/* Flex macros */

id  [a-zA-Z][a-zA-Z_0-9]*
string  \"[^\n"]*\"
comment "//".*
delimiter [\r\n \t\v]*

%%
{id}  {
  fprintf(stderr, "Recognized id with value: %s\n", yytext);
  fill_token((alpha_token_t*) yyval, yytext, id);
  return 1;
}
{string}  {
  fprintf(stderr, "Recognized string with value: %s\n", yytext);
  fill_token((alpha_token_t*) yyval, yytext, string);
  return 1;
}
{comment} {
  fprintf(stderr, "Recognized comment with value: %s\n", yytext);
  fill_token((alpha_token_t*) yyval, yytext, comment);
  return 1;
}
{delimiter} {
  return 1;
}
. {
  fprintf(stderr, "Cannot match character %s with any rule\n", yytext);
  return 1;
}
<<EOF>> {
  return 0;
}
%%

int main (int argc, char** argv) {
    if (argc > 1) {
      if (!(yyin = fopen(argv[1], "r"))) {
        fprintf(stderr, "Cannot read file: %s\n",argv[1]);
        return 1;
      }
    }
    else yyin= stdin;


    alpha_token_t *t = malloc(sizeof(alpha_token_t));
    int more;
    do {
      more = yylex(t);
      printf(t->value);
      free(t->value);
    } while (more);
    free(t);
    return 0;
}
