%{
  #ifdef WIN32
  #define YY_NO_UNISTD_H
  int isatty(int i) { return 0;}
  #endif
  #include "scanner.h"
  #include "parser.h"
  #define YY_DECL int yylex(void* yyval)
  
  unsigned int flag = 0;

  #define EOF_NUMBER 0
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
number    [0-9]+
float     {number}\.{number}?


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


{IF}        {return IF;} //return IF;
{ELSE}      {return ELSE;}
{WHILE}     {return WHILE;}
{FOR}       {return FOR;}
{FUNCTION}  {return FUNCTION;}
{RETURN}    {return RETURN;}
{BREAK}     {return BREAK;}
{CONTINUE}  {return CONTINUE;}
{AND}       {return AND;}
{NOT}       {return NOT;}
{OR}        {return OR;}
{LOCAL}     {return LOCAL;}
{TRUE}      {return TRUE;}
{FALSE}     {return FALSE;}
{NIL}       {return NIL;}
{CURL_O}    {return CURL_O;}
{CURL_C}    {return CURL_C;}
{BRAC_O}    {return BRAC_O;}
{BRAC_C}    {return BRAC_C;}
{ANGL_O}    {return ANGL_O;}
{ANGL_C}    {return ANGL_C;}
{SEMI}      {return SEMI;}
{COMMA}     {return COMMA;}
{COLON}     {return COLON;}
{DCOLON}    {return DCOLON;}
{DOT}       {return DOT;}
{DOTDOT}    {return DOTDOT;}
{ASSIGN}    {return ASSIGN;}
{PLUS}      {return PLUS;}
{MINUS}     {return MINUS;}
{MUL}       {return MUL;}
{SLASH}     {return SLASH;}
{PERC}      {return PERC;}
{EQUALS}    {return EQUALS;}
{NEQUALS}   {return NEQUALS;}
{INCR}      {return INCR;}
{DECR}      {return DECR;}
{GREATER}   {return GREATER;}
{LESS}      {return LESS;}
{GREATER_E} {return GREATER_E;}
{LESS_E}    {return LESS_E;}


{id}  {
  alpha_yylval.stringValue = strdup(yytext);
  return ID;
}

{string} {
  char* buffer = (char*)malloc(sizeof(char)*1026);
  int filled = 0;
  int reallocs = 1;
  char c;
  while ((c = input()) != '"') {
    if (c == 0) {
      fprintf(stderr, "EOF during string\n");
      exit(EXIT_FAILURE);
    }
    if (filled == reallocs*1024) {
        buffer = (char*)realloc(buffer, reallocs*1024 + 1026);
        reallocs++;
      }
    if (c != '\\') {
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
  alpha_yylval.stringValue = strdup(buffer);
  free(buffer);
  return STRING;
}

{multiline_comment} {
  char c, *buffer, temp[128];
  int balanced = 1, i ;
  Stack *stack = Stack_init();
  Stack *reverse = Stack_init();
  Stack_Node *node;
  comment_node *comment_n;
  comment_n = (comment_node *) malloc(sizeof(comment_node));
  comment_n->line_open = yylineno;
  comment_n->line_close = -1;
  comment_n->isBlock = 1;
  Stack_append(stack, (void *)comment_n);
  while (1) {
    c = input();
    if (c == EOF_NUMBER) {
      break;
    }
    if (c == '/') {
      c = input();
      if (c != '*') {
        unput(c);
        continue;
      }
      balanced++;
      comment_n = (comment_node *) malloc(sizeof(comment_node));
      comment_n->line_open = yylineno;
      comment_n->line_close = -1;
      comment_n->isBlock = 1;
      Stack_append(stack, (void *)comment_n);
    } else if (c == '*') {
      c = input();
      if (c != '/') {
        unput(c);
        continue;
      }
      node = stack->top;
      for (i=0; i<stack->size; i++) {
        comment_n = (comment_node *)(node->content);
        if (comment_n->isBlock == 1 && comment_n->line_close == -1){
          comment_n->line_close = yylineno;
          break;
        }
        node = node->next;
      }
      balanced--;
      if (balanced == 0) break;
    }
  }
  while (Stack_isEmpty(stack) == 0)Stack_append(reverse, Stack_pop(stack));
  while (Stack_isEmpty(reverse) == 0) {
    comment_n = Stack_pop(reverse);
    if (comment_n->isBlock == 0) {
      sprintf(temp , "%d" ,comment_n->line_open);
      //new_token((Queue *) alpha_yylval, comment_n->line_open, temp, comment, LINECOMM);
      continue;
    }
    if (comment_n->line_close == -1){
      sprintf(temp , "%d-EOF" , comment_n->line_open);
      flag=1;
    } else {
      sprintf(temp , "%d-%d" , comment_n->line_open, comment_n->line_close);
    }
    //new_token((Queue *) alpha_yylval, comment_n->line_open, temp, comment, BLOCKCOMM);
    //free(comment_n);
  }
  //free(yytext);
  Stack_destroy(stack);
  Stack_destroy(reverse);
}

{comment} {
}

{number} {
  alpha_yylval.intValue = atoi(yytext);
  return INTNUM;
}

{float} {
  alpha_yylval.floatValue = atof(yytext);
  return REALNUM;
}

{delimiter} {
}

. {
  fprintf(stderr, "Unexpected identifier \"%s\" in line %d.\n", yytext, yylineno);
  exit(EXIT_FAILURE);
}

<<EOF>> {
  return 0;
}
%%