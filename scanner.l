%{
  #ifdef WIN32
  #define YY_NO_UNISTD_H
  int isatty(int i) { return 0;}
  #endif

  #include "scanner.h"
  #define YY_DECL int yylex(void* yyval)
  
  unsigned int comm_count;
%}

%option PREFIX="alpha_yy"
%option noyywrap
%option yylineno

/* Flex macros */

id        [a-zA-Z][a-zA-Z_0-9]*
string    \"
multiline_comment   "/*"
comment   "//".*
delimiter [\r\n \t\v]*
new_line  "\n"
number    [0-9]+
duble     {number}\.{number}?


/* KEYWORDS */
IF        "if"
ELSE      "else"
WHILE     "while"
FOR       "for"
FUNCTION  "function"
RETURN    "return"
BREAK     "break"
CONTINUE  "continue"
AND       "and"
NOT       "not"
OR        "or"
LOCAL     "local"
TRUE      "true"
FALSE     "false"
NIL       "nil"

/* PUNCTUATIONS */
CURL_O "\{"
CURL_C "\}"
BRAC_O "\["
BRAC_C "\]"
ANGL_O "\("
ANGL_C "\)"
SEMI   "\;"
COMMA  "\,"
COLON  "\:"
DCOLON "\:\:"
DOT    "\."
DOTDOT "\.\."

/* OPERATIONS */
ASSIGN    "\="
PLUS      "\+"
MINUS     "\-"
MUL       "\*"
SLASH     "\/"
PERC      "\%"
EQUALS    "\=\="
NEQUALS   "\!\="
INCR      "\+\+"
DECR      "\-\-"
GREATER   "\>"
LESS      "\<"
GREATER_E "\>\="
LESS_E    "\<\="

%%


{IF}        {new_token((t_queue *) yyval, yylineno, yytext, keyword);}
{ELSE}      {new_token((t_queue *) yyval, yylineno, yytext, keyword);}
{WHILE}     {new_token((t_queue *) yyval, yylineno, yytext, keyword);}
{FOR}       {new_token((t_queue *) yyval, yylineno, yytext, keyword);}
{FUNCTION}  {new_token((t_queue *) yyval, yylineno, yytext, keyword);}
{RETURN}    {new_token((t_queue *) yyval, yylineno, yytext, keyword);}
{BREAK}     {new_token((t_queue *) yyval, yylineno, yytext, keyword);}
{CONTINUE}  {new_token((t_queue *) yyval, yylineno, yytext, keyword);}
{AND}       {new_token((t_queue *) yyval, yylineno, yytext, keyword);}
{NOT}       {new_token((t_queue *) yyval, yylineno, yytext, keyword);}
{OR}        {new_token((t_queue *) yyval, yylineno, yytext, keyword);}
{LOCAL}     {new_token((t_queue *) yyval, yylineno, yytext, keyword);}
{TRUE}      {new_token((t_queue *) yyval, yylineno, yytext, keyword);}
{FALSE}     {new_token((t_queue *) yyval, yylineno, yytext, keyword);}
{NIL}       {new_token((t_queue *) yyval, yylineno, yytext, keyword);}
{CURL_O}    {new_token((t_queue *) yyval, yylineno, yytext, punct);}
{CURL_C}    {new_token((t_queue *) yyval, yylineno, yytext, punct);}
{BRAC_O}    {new_token((t_queue *) yyval, yylineno, yytext, punct);}
{BRAC_C}    {new_token((t_queue *) yyval, yylineno, yytext, punct);}
{ANGL_O}    {new_token((t_queue *) yyval, yylineno, yytext, punct);}
{ANGL_C}    {new_token((t_queue *) yyval, yylineno, yytext, punct);}
{SEMI}      {new_token((t_queue *) yyval, yylineno, yytext, punct);}
{COMMA}     {new_token((t_queue *) yyval, yylineno, yytext, punct);}
{COLON}     {new_token((t_queue *) yyval, yylineno, yytext, punct);}
{DCOLON}    {new_token((t_queue *) yyval, yylineno, yytext, punct);}
{DOT}       {new_token((t_queue *) yyval, yylineno, yytext, punct);}
{DOTDOT}    {new_token((t_queue *) yyval, yylineno, yytext, punct);}
{ASSIGN}    {new_token((t_queue *) yyval, yylineno, yytext, oper);}
{PLUS}      {new_token((t_queue *) yyval, yylineno, yytext, oper);}
{MINUS}     {new_token((t_queue *) yyval, yylineno, yytext, oper);}
{MUL}       {new_token((t_queue *) yyval, yylineno, yytext, oper);}
{SLASH}     {new_token((t_queue *) yyval, yylineno, yytext, oper);}
{PERC}      {new_token((t_queue *) yyval, yylineno, yytext, oper);}
{EQUALS}    {new_token((t_queue *) yyval, yylineno, yytext, oper);}
{NEQUALS}   {new_token((t_queue *) yyval, yylineno, yytext, oper);}
{INCR}      {new_token((t_queue *) yyval, yylineno, yytext, oper);}
{DECR}      {new_token((t_queue *) yyval, yylineno, yytext, oper);}
{GREATER}   {new_token((t_queue *) yyval, yylineno, yytext, oper);}
{LESS}      {new_token((t_queue *) yyval, yylineno, yytext, oper);}
{GREATER_E} {new_token((t_queue *) yyval, yylineno, yytext, oper);}
{LESS_E}    {new_token((t_queue *) yyval, yylineno, yytext, oper);}


{id}  {
  new_token((t_queue *) yyval, yylineno, yytext, id);
}
{string} {
  char* buffer = (char*)malloc(sizeof(char)*1025);
  int filled = 0;
  char c;
  while ((c = input()) != '"') {
    if (c == 0) {
      fprintf(stderr, "EOF during string\n");
      exit(EXIT_FAILURE);
    } else if (c == '\\') {
      // just dont continue, to check next char
    } else {
      if (filled == 1023) {
        fprintf(stderr, "String is bigger than 1024 characters, line %d\n", yylineno);
        exit(EXIT_FAILURE);
      }
      buffer[filled++] = c;
      continue;
    }
    c = input();
    if (c == 0) {
      fprintf(stderr, "EOF during escaping in string\n");
      exit(EXIT_FAILURE);
    } else if (c == '\\'){
      buffer[filled] = '\\';
    } else if (c == 'n') {
      buffer[filled] = '\n';
    } else if (c == 't') {
      buffer[filled] = '\t';
    } else if (c == '"') {
      buffer[filled] = '\"';
    } else {
      fprintf(stderr, "Unknown escaped character in string, line %d\n", yylineno );
      exit(EXIT_FAILURE);
    }
      filled++;
  }
  buffer[filled] = '\0';
  new_token((t_queue *) yyval, yylineno, buffer, string);
}

{multiline_comment} {
  char c;
  int stack = 1;
  while (1) {
    c = input();
    if (c == 0) {
      break;
    }
    if (c == '/') {
      c = input();
      if (c == '/') {
        new_token((t_queue *) yyval, yylineno, "comment", comment);
        continue;
      } else if (c != '*') {
        unput(c);
        continue;
      } 
      stack++;
      // nested comment here
    } else if (c == '*') {
      c = input();
      if (c != '/') {
        unput(c);
        continue;
      } 
      new_token((t_queue *) yyval, yylineno, "multiline_comment", comment);
      stack--;
      if (stack == 0) break;
    }
  }
  printf("\nStack ended with %d\n",stack);
}
{comment} {
    new_token((t_queue *) yyval, yylineno, yytext, comment);
}
{number} {
  new_token((t_queue *) yyval, yylineno, yytext, intconst);
}
{duble} {
  new_token((t_queue *) yyval, yylineno, yytext, doubleconst);
}

{delimiter} {
}
. {
  fprintf(stderr, "Unexpected identifier \"%s\" in line %d.\n", yytext, yylineno);
  print_queue((t_queue *) yyval);
  exit(EXIT_FAILURE);
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
    t_queue *q;
    q = queue_init();

    yylex((void *)q);
    print_queue(q);
    if(comm_count!=0)fprintf(stderr,"Warning: some comments did'nt close correctly!\n");
    return 0;
}
