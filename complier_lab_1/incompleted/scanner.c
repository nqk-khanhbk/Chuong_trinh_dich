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


extern int lineNo;
extern int colNo;
extern int currentChar;

extern CharCode charCodes[];

/***************************************************************/

// Skip blanks bo khoang trong ky tu
void skipBlank() {
  while (currentChar != EOF && charCodes[currentChar] == CHAR_SPACE) {
    readChar();
  }
}

void skipComment() {
  readChar();

  while (currentChar != EOF) {
    if (currentChar == '*') {
      readChar();
      if (currentChar == '/') {
        // consume the closing '/' and return to scanning
        readChar();
        return;
      }
      /* else continue scanning inside comment */
    } else {
      readChar();
    }
  }

  // If we reach here, EOF arrived before the end of comment
  error(ERR_ENDOFCOMMENT, lineNo, colNo);
}

/* Skip comment that starts with '(*' and ends with '*)' */
void skipCommentParen() {
  /* assume caller consumed '(' and currentChar is '*' */
  /* consume the '*' */
  readChar();

  while (currentChar != EOF) {
    if (currentChar == '*') {
      readChar();
      if (currentChar == ')') {
        readChar();
        return;
      }
    } else {
      readChar();
    }
  }
  /* EOF before end of comment */
  error(ERR_ENDOFCOMMENT, lineNo, colNo);
}

Token* readIdentKeyword(void) {
  Token *token;
  int ln = lineNo, cn = colNo;
  char str[MAX_IDENT_LEN + 1];
  int len = 0;

  /* read letters and digits */
  while (currentChar != EOF && (charCodes[currentChar] == CHAR_LETTER || charCodes[currentChar] == CHAR_DIGIT)) {
    if (len < MAX_IDENT_LEN) {
      str[len++] = (char)currentChar;
    } else {
      /* identifier too long */
      error(ERR_IDENTTOOLONG, lineNo, colNo);
    }
    readChar();
  }
  str[len] = '\0';

  /* check if it's a keyword */
  TokenType kt = checkKeyword(str);
  if (kt != TK_NONE) {
    token = makeToken(kt, ln, cn);
  } else {
    token = makeToken(TK_IDENT, ln, cn);
    strcpy(token->string, str);
  }
  return token;
}

Token* readNumber(void) {
  Token *token;
  int ln = lineNo, cn = colNo;
  char str[MAX_IDENT_LEN + 1];
  int len = 0;

  while (currentChar != EOF && charCodes[currentChar] == CHAR_DIGIT) {
    if (len < MAX_IDENT_LEN) {
      str[len++] = (char)currentChar;
    } else {
      /* truncate extra digits */
      /* simply keep reading but do not store beyond buffer */
    }
    readChar();
  }
  str[len] = '\0';

  token = makeToken(TK_NUMBER, ln, cn);
  strcpy(token->string, str);
  token->value = atoi(str);
  return token;
}

Token* readConstChar(void) {
  Token *token;
  int ln = lineNo, cn = colNo;
  char str[2];

  /* currentChar is the opening single quote '\'' */
  readChar(); /* move to the character content */

  if (currentChar == EOF || currentChar == '\n') {
    error(ERR_INVALIDCHARCONSTANT, lineNo, colNo);
  }

  /* take one character as the char constant */
  str[0] = (char)currentChar;
  str[1] = '\0';

  readChar(); /* consume the character */

  if (currentChar != '\'') {
    /* missing closing quote or multi-char constant */
    error(ERR_INVALIDCHARCONSTANT, lineNo, colNo);
  }

  /* consume closing quote */
  readChar();

  token = makeToken(TK_CHAR, ln, cn);
  strcpy(token->string, str);
  return token;
}

Token* getToken(void) {
  Token *token;
  int ln, cn;

  if (currentChar == EOF) 
    return makeToken(TK_EOF, lineNo, colNo);

  ln = lineNo; cn = colNo;

  switch (charCodes[currentChar]) {
  case CHAR_SPACE:
    skipBlank();
    return getToken();
  case CHAR_LETTER:
    return readIdentKeyword();
  case CHAR_DIGIT:
    return readNumber();
  case CHAR_PLUS:
    token = makeToken(SB_PLUS, ln, cn);
    readChar();
    return token;
  case CHAR_MINUS:
    token = makeToken(SB_MINUS, ln, cn);
    readChar();
    return token;
  case CHAR_TIMES:
    token = makeToken(SB_TIMES, ln, cn);
    readChar();
    return token;
  case CHAR_SLASH:
    /* possible comment start */
    readChar();
    if (currentChar == '*') {
      /* inside comment: consume and skip */
      skipComment();
      return getToken();
    } else {
      /* it was just a slash */
      token = makeToken(SB_SLASH, ln, cn);
      return token;
    }
  case CHAR_LPAR:
    /* check for comment start '(*' */
    readChar();
    if (currentChar == '*') {
      skipCommentParen();
      return getToken();
    } else {
      token = makeToken(SB_LPAR, ln, cn);
      return token;
    }
  case CHAR_RPAR:
    token = makeToken(SB_RPAR, ln, cn);
    readChar();
    return token;
  case CHAR_SINGLEQUOTE:
    return readConstChar();
  case CHAR_COMMA:
    token = makeToken(SB_COMMA, ln, cn);
    readChar();
    return token;
  case CHAR_SEMICOLON:
    token = makeToken(SB_SEMICOLON, ln, cn);
    readChar();
    return token;
  case CHAR_COLON:
    /* check for ':=' */
    readChar();
    if (currentChar == '=') {
      token = makeToken(SB_ASSIGN, ln, cn);
      readChar();
      return token;
    } else {
      token = makeToken(SB_COLON, ln, cn);
      return token;
    }
  case CHAR_PERIOD:
    token = makeToken(SB_PERIOD, ln, cn);
    readChar();
    return token;
  case CHAR_EQ:
    token = makeToken(SB_EQ, ln, cn);
    readChar();
    return token;
  case CHAR_LT:
    /* <, <=, <> */
    readChar();
    if (currentChar == '=') {
      token = makeToken(SB_LE, ln, cn);
      readChar();
      return token;
    } else if (currentChar == '>') {
      token = makeToken(SB_NEQ, ln, cn);
      readChar();
      return token;
    } else {
      token = makeToken(SB_LT, ln, cn);
      return token;
    }
  case CHAR_GT:
    /* >, >= */
    readChar();
    if (currentChar == '=') {
      token = makeToken(SB_GE, ln, cn);
      readChar();
      return token;
    } else {
      token = makeToken(SB_GT, ln, cn);
      return token;
    }
  case CHAR_EXCLAIMATION:
    /* maybe '!' followed by '=' for '!=' */
    readChar();
    if (currentChar == '=') {
      token = makeToken(SB_NEQ, ln, cn);
      readChar();
      return token;
    } else {
      token = makeToken(TK_NONE, ln, cn);
      error(ERR_INVALIDSYMBOL, ln, cn);
      return token;
    }
  default:
    /* handle some characters not in charCodes mapping (like '[' and ']') */
    if (currentChar == '[') {
      token = makeToken(SB_LSEL, ln, cn);
      readChar();
      return token;
    } else if (currentChar == ']') {
      token = makeToken(SB_RSEL, ln, cn);
      readChar();
      return token;
    } else if (currentChar == '\'') {
      return readConstChar();
    } else {
      token = makeToken(TK_NONE, ln, cn);
      error(ERR_INVALIDSYMBOL, ln, cn);
      readChar();
      return token;
    }
  }
}


/******************************************************************/

void printToken(Token *token) {

  printf("%d-%d:", token->lineNo, token->colNo);

  switch (token->tokenType) {
  case TK_NONE: printf("TK_NONE\n"); break;
  case TK_IDENT: printf("TK_IDENT(%s)\n", token->string); break;
  case TK_NUMBER: printf("TK_NUMBER(%s)\n", token->string); break;
  case TK_CHAR: printf("TK_CHAR(\'%s\')\n", token->string); break;
  case TK_EOF: printf("TK_EOF\n"); break;

  case KW_PROGRAM: printf("KW_PROGRAM\n"); break;
  case KW_CONST: printf("KW_CONST\n"); break;
  case KW_TYPE: printf("KW_TYPE\n"); break;
  case KW_VAR: printf("KW_VAR\n"); break;
  case KW_INTEGER: printf("KW_INTEGER\n"); break;
  case KW_CHAR: printf("KW_CHAR\n"); break;
  case KW_ARRAY: printf("KW_ARRAY\n"); break;
  case KW_OF: printf("KW_OF\n"); break;
  case KW_FUNCTION: printf("KW_FUNCTION\n"); break;
  case KW_PROCEDURE: printf("KW_PROCEDURE\n"); break;
  case KW_BEGIN: printf("KW_BEGIN\n"); break;
  case KW_END: printf("KW_END\n"); break;
  case KW_CALL: printf("KW_CALL\n"); break;
  case KW_IF: printf("KW_IF\n"); break;
  case KW_THEN: printf("KW_THEN\n"); break;
  case KW_ELSE: printf("KW_ELSE\n"); break;
  case KW_WHILE: printf("KW_WHILE\n"); break;
  case KW_DO: printf("KW_DO\n"); break;
  case KW_FOR: printf("KW_FOR\n"); break;
  case KW_TO: printf("KW_TO\n"); break;

  case SB_SEMICOLON: printf("SB_SEMICOLON\n"); break;
  case SB_COLON: printf("SB_COLON\n"); break;
  case SB_PERIOD: printf("SB_PERIOD\n"); break;
  case SB_COMMA: printf("SB_COMMA\n"); break;
  case SB_ASSIGN: printf("SB_ASSIGN\n"); break;
  case SB_EQ: printf("SB_EQ\n"); break;
  case SB_NEQ: printf("SB_NEQ\n"); break;
  case SB_LT: printf("SB_LT\n"); break;
  case SB_LE: printf("SB_LE\n"); break;
  case SB_GT: printf("SB_GT\n"); break;
  case SB_GE: printf("SB_GE\n"); break;
  case SB_PLUS: printf("SB_PLUS\n"); break;
  case SB_MINUS: printf("SB_MINUS\n"); break;
  case SB_TIMES: printf("SB_TIMES\n"); break;
  case SB_SLASH: printf("SB_SLASH\n"); break;
  case SB_LPAR: printf("SB_LPAR\n"); break;
  case SB_RPAR: printf("SB_RPAR\n"); break;
  case SB_LSEL: printf("SB_LSEL\n"); break;
  case SB_RSEL: printf("SB_RSEL\n"); break;
  }
}

int scan(char *fileName) {
  Token *token;

  if (openInputStream(fileName) == IO_ERROR)
    return IO_ERROR;

  token = getToken();
  while (token->tokenType != TK_EOF) {
    printToken(token);
    free(token);
    token = getToken();
  }

  free(token);
  closeInputStream();
  return IO_SUCCESS;
}

/******************************************************************/

// int main(int argc, char *argv[]) {
//   if (argc <= 1) {
//     printf("scanner: no input file.\n");
//     return -1;
//   }

//   if (scan(argv[1]) == IO_ERROR) {
//     printf("Can\'t read input file!\n");
//     return -1;
//   }
    
//   return 0;
// }



