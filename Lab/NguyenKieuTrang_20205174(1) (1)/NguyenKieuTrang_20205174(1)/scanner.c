/* Scanner
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "reader.h"
#include "charcode.h"
#include "token.h"
#include "error.h"

#include <limits.h>
#include "reader.c"
#include "charcode.c"
#include "token.c"
#include "error.c"

extern int lineNo;
extern int colNo;
extern int currentChar;

extern CharCode charCodes[];

/***************************************************************/

void skipBlank()
{
  //bỏ qua khoảng trắng
  while (currentChar != EOF && charCodes[currentChar] == CHAR_SPACE)
  {
    readChar();
  }
}

void skipComment()
{
  // TODO
  // Note: currentChar = '*'
  //bỏ qua cmt
  int state = 0;
  while ((currentChar != EOF) && (state < 2))
  {
    switch (charCodes[currentChar])
    {
    case CHAR_TIMES:
      state = 1;
      break;
    case CHAR_RPAR:
      if (state == 1)
        state = 2;
      else
        state = 0;
      break;
    default:
      state = 0;
    }
    readChar();
  }
  if (state != 2)
    error(ERR_ENDOFCOMMENT, lineNo, colNo);
}

Token *readIdentKeyword(void)
{
  // TODO
  // Read token and store it
  // Hàm này đọc một từ (identifier hoặc keyword) từ mã nguồn và trả về một con trỏ tới một Token mới tạo.
  Token *token = makeToken(TK_IDENT, lineNo, colNo);
  int i = 0;
  while (charCodes[currentChar] == CHAR_LETTER || charCodes[currentChar] == CHAR_DIGIT)
  {
    token->string[i] = currentChar;
    readChar();
    i++;
  }
  token->string[i] = '\0';
  // Check the length of the token and check wether token is keyword or not
  if (i >= MAX_IDENT_LEN)
  {
    error(ERR_IDENTTOOLONG, lineNo, colNo);
  }
  else
  {
    TokenType type = checkKeyword(token->string);
    if (type != TK_NONE)
    {
      token->tokenType = type;
    }
  }
  return token;
}

Token *readNumber(void)
{
  // TODO
  // NOTE: currentChar is a number
  // Hàm này đọc một từ (identifier hoặc keyword) từ mã nguồn và trả về một con trỏ tới một Token mới tạo.
  // NOTE: currentChar is a number
  Token *token = makeToken(TK_NUMBER, lineNo, colNo);

  int numDot = 0;
  int i = 0;
  while (charCodes[currentChar] == CHAR_DIGIT || (charCodes[currentChar] == CHAR_PERIOD && numDot == 0))
  {
    if (charCodes[currentChar] == CHAR_PERIOD)
    {
      numDot++;
    }
    token->string[i] = currentChar;
    readChar();
    i++;
  }
  if (charCodes[token->string[i - 1]] != CHAR_DIGIT)
  {
    error(ERR_INVALIDNUMBER, lineNo, colNo);
  }

  token->string[i] = '\0';
  token->value = numDot == 0 ? atoi(token->string) : atof(token->string);

  if (numDot == 1)
  {
    token->tokenType = TK_FLOAT;
  }
  return token;
}

Token *readConstChar(void)
{
  // TODO
  // Note: currentChar = '
  // Hàm này đọc một kí tự hằng từ mã nguồn và trả về một con trỏ tới một Token mới tạo.
  

  // Read the next char
  Token *token = makeToken(TK_CHAR, lineNo, colNo);

  // Read the next char
  // If the next char is EOF -> ERROR: ERR_INVALIDCHARCONSTANT
  readChar();
  if (currentChar == EOF)
  {
    token->tokenType = TK_NONE;
    error(ERR_INVALIDCHARCONSTANT, token->lineNo, token->colNo);
    return token;
  }
  // Else -> store the char
  token->string[0] = currentChar;
  token->string[1] = '\0';

  // Read the next char
  // Check wether it is ' or not
  readChar();
  if (charCodes[currentChar] == CHAR_SINGLEQUOTE)
  {
    readChar();
  }
  else
  {
    token->tokenType = TK_NONE;
    error(ERR_INVALIDCHARCONSTANT, token->lineNo, token->colNo);
  }
  return token;
}

Token *getToken(void)
{
  // Hàm này đọc một Token tiếp theo từ mã nguồn và trả về một con trỏ tới Token đó.
  Token *token;
  int ln, cn;

  if (currentChar == EOF)
    return makeToken(TK_EOF, lineNo, colNo);

  switch (charCodes[currentChar])
  {
  case CHAR_SPACE:
    skipBlank();
    return getToken();
  case CHAR_LETTER:
    return readIdentKeyword();
  case CHAR_DIGIT:
    return readNumber();
  case CHAR_SINGLEQUOTE:
    return readConstChar();
  // case CHAR_PLUS:
  //   token = makeToken(SB_PLUS, lineNo, colNo);
  //   readChar();
  //   return token;
    // ....
    // TODO
  case CHAR_PLUS: // "+" or "+="
    token = makeToken(SB_PLUS, lineNo, colNo);
    readChar();
    if (charCodes[currentChar] == CHAR_EQ)
    {
      // "+="
      token->tokenType = SB_ASSIGN_PLUS;
      readChar();
    }
    return token;
  case CHAR_MINUS: // "-" or "-="
    token = makeToken(SB_MINUS, lineNo, colNo);
    readChar();
    if (charCodes[currentChar] == CHAR_EQ)
    {
      // "-="
      token->tokenType = SB_ASSIGN_SUBTRACT;
      readChar();
    }
    return token;
  case CHAR_TIMES: // "*" or "*="
    token = makeToken(SB_TIMES, lineNo, colNo);
    readChar();
    if (charCodes[currentChar] == CHAR_EQ)
    {
      // "*="
      token->tokenType = SB_ASSIGN_TIMES;
      readChar();
    }
    return token;
  case CHAR_SLASH: // "/" or "/="
    token = makeToken(SB_SLASH, lineNo, colNo);
    readChar();
    if (charCodes[currentChar] == CHAR_EQ)
    {
      // "/="
      token->tokenType = SB_ASSIGN_DIVIDE;
      readChar();
    }
    return token;
  case CHAR_LT: // "<" or "<="
    token = makeToken(SB_LT, lineNo, colNo);
    readChar();
    if (charCodes[currentChar] == CHAR_EQ)
    {
      // "<="
      token->tokenType = SB_LE;
      readChar();
    }
    return token;
  case CHAR_GT: // ">" or ">="
    token = makeToken(SB_GT, lineNo, colNo);
    readChar();
    if (charCodes[currentChar] == CHAR_EQ)
    {
      // ">="
      token->tokenType = SB_GE;
      readChar();
    }
    return token;
  case CHAR_PERIOD: // "." or ".)"
    token = makeToken(SB_PERIOD, lineNo, colNo);
    readChar();
    if (charCodes[currentChar] == CHAR_RPAR)
    {
      // ".)"
      token->tokenType = SB_RSEL;
      readChar();
    }
    return token;
  case CHAR_COLON: // ":" or ":="
    token = makeToken(SB_COLON, lineNo, colNo);
    readChar();
    if (charCodes[currentChar] == CHAR_EQ)
    {
      // ":="
      token->tokenType = SB_ASSIGN;
      readChar();
    }
    return token;
  case CHAR_EXCLAIMATION: // "!" or "!="
    token = makeToken(TK_NONE, lineNo, colNo);
    readChar();
    if (charCodes[currentChar] == CHAR_EQ)
    {
      // "!="
      token->tokenType = SB_NEQ;
      readChar();
    }
    else
    {
      // invalid "!"
      error(ERR_INVALIDSYMBOL, token->lineNo, token->colNo);
    }
    return token;
  case CHAR_LPAR: // "(" or "(." or "(*"
    token = makeToken(SB_LPAR, lineNo, colNo);
    readChar();
    switch (charCodes[currentChar])
    {
    case CHAR_PERIOD: // "(."
      token->tokenType = SB_LSEL;
      break;
    case CHAR_TIMES: // "(*"
      skipComment();
      return getToken();
    default: // "("
      return token;
    }
    break;

  case CHAR_EQ: // "="
    token = makeToken(SB_EQ, lineNo, colNo);
    break;
  case CHAR_COMMA: // ","
    token = makeToken(SB_COMMA, lineNo, colNo);
    break;
  case CHAR_SEMICOLON: // ";"
    token = makeToken(SB_SEMICOLON, lineNo, colNo);
    break;
  case CHAR_RPAR: // ")"
    token = makeToken(SB_RPAR, lineNo, colNo);
    break;
  case CHAR_LBRACKET:
    token = makeToken(SB_LBRACKET, lineNo, colNo);
    break;
  case CHAR_RBRACKET:
    token = makeToken(SB_RBRACKET, lineNo, colNo);
    break;
  case CHAR_PERCENT:
    token = makeToken(SB_MOD, lineNo, colNo);
    break;

    // ....
  default:
    token = makeToken(TK_NONE, lineNo, colNo);
    error(ERR_INVALIDSYMBOL, lineNo, colNo);
    // readChar();
    // return token;
    break;
  }
  readChar();
  return token;
}

/******************************************************************/

void printToken(Token *token)
{
  // Hàm này in thông tin của một Token ra màn hình.
  printf("%d-%d:", token->lineNo, token->colNo);

  switch (token->tokenType)
  {
  case TK_NONE:
    printf("TK_NONE\n");
    break;
  case TK_IDENT:
    printf("TK_IDENT(%s)\n", token->string);
    break;
  case TK_NUMBER:
    printf("TK_NUMBER(%s)\n", token->string);
    break;
  case TK_CHAR:
    printf("TK_CHAR(\'%s\')\n", token->string);
    break;
  case TK_EOF:
    printf("TK_EOF\n");
    break;
  case TK_FLOAT:
    printf("TK_FLOAT(\'%s\')\n", token->string);
    break;

  case KW_PROGRAM:
    printf("KW_PROGRAM\n");
    break;
  case KW_CONST:
    printf("KW_CONST\n");
    break;
  case KW_TYPE:
    printf("KW_TYPE\n");
    break;
  case KW_VAR:
    printf("KW_VAR\n");
    break;
  case KW_INTEGER:
    printf("KW_INTEGER\n");
    break;
  case KW_CHAR:
    printf("KW_CHAR\n");
    break;
  case KW_ARRAY:
    printf("KW_ARRAY\n");
    break;
  case KW_OF:
    printf("KW_OF\n");
    break;
  case KW_FUNCTION:
    printf("KW_FUNCTION\n");
    break;
  case KW_PROCEDURE:
    printf("KW_PROCEDURE\n");
    break;
  case KW_BEGIN:
    printf("KW_BEGIN\n");
    break;
  case KW_END:
    printf("KW_END\n");
    break;
  case KW_CALL:
    printf("KW_CALL\n");
    break;
  case KW_IF:
    printf("KW_IF\n");
    break;
  case KW_THEN:
    printf("KW_THEN\n");
    break;
  case KW_ELSE:
    printf("KW_ELSE\n");
    break;
  case KW_WHILE:
    printf("KW_WHILE\n");
    break;
  case KW_DO:
    printf("KW_DO\n");
    break;
  case KW_FOR:
    printf("KW_FOR\n");
    break;
  case KW_TO:
    printf("KW_TO\n");
    break;
  case KW_FLOAT:
    printf("KW_FLOAT\n");
    break;

  case SB_SEMICOLON:
    printf("SB_SEMICOLON\n");
    break;
  case SB_COLON:
    printf("SB_COLON\n");
    break;
  case SB_PERIOD:
    printf("SB_PERIOD\n");
    break;
  case SB_COMMA:
    printf("SB_COMMA\n");
    break;
  case SB_ASSIGN:
    printf("SB_ASSIGN\n");
    break;
  case SB_EQ:
    printf("SB_EQ\n");
    break;
  case SB_NEQ:
    printf("SB_NEQ\n");
    break;
  case SB_LT:
    printf("SB_LT\n");
    break;
  case SB_LE:
    printf("SB_LE\n");
    break;
  case SB_GT:
    printf("SB_GT\n");
    break;
  case SB_GE:
    printf("SB_GE\n");
    break;
  case SB_PLUS:
    printf("SB_PLUS\n");
    break;
  case SB_MINUS:
    printf("SB_MINUS\n");
    break;
  case SB_TIMES:
    printf("SB_TIMES\n");
    break;
  case SB_SLASH:
    printf("SB_SLASH\n");
    break;
  case SB_LPAR:
    printf("SB_LPAR\n");
    break;
  case SB_RPAR:
    printf("SB_RPAR\n");
    break;
  case SB_LSEL:
    printf("SB_LSEL\n");
    break;
  case SB_RSEL:
    printf("SB_RSEL\n");
    break;

  case SB_LBRACKET:
    printf("SB_LBRACKET\n");
    break;
  case SB_RBRACKET:
    printf("SB_RBRACKET\n");
    break;
  case SB_MOD:
    printf("SB_MOD\n");
    break;
  case SB_ASSIGN_PLUS:
    printf("SB_ASSIGN_PLUS\n");
    break;
  case SB_ASSIGN_SUBTRACT:
    printf("SB_ASSIGN_SUBTRACT\n");
    break;
  case SB_ASSIGN_DIVIDE:
    printf("SB_ASSIGN_DIVIDE\n");
    break;
  case SB_ASSIGN_TIMES:
    printf("SB_ASSIGN_TIMES\n");
    break;
  }
}

int scan(char *fileName)
{
  Token *token;

  if (openInputStream(fileName) == IO_ERROR)
    return IO_ERROR;

  token = getToken();
  while (token->tokenType != TK_EOF)
  {
    printToken(token);
    free(token);
    token = getToken();
  }

  free(token);
  closeInputStream();
  return IO_SUCCESS;
}

/******************************************************************/

int main(int argc, char *argv[]) {
  if (argc <= 1) {
    printf("scanner: no input file.\n");
    return -1;
  }

  if (scan(argv[1]) == IO_ERROR) {
    printf("Can\'t read input file!\n");
    return -1;
  }

  return 0;
}

// int main()
// {

//   if (scan("example11.kpl") == IO_ERROR)
//   {
//     printf("Can\'t read input file!\n");
//   }
//   return 0;
// }
