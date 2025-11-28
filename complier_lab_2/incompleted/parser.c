/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdlib.h>

#include "reader.h"
#include "scanner.h"
#include "parser.h"
#include "error.h"

Token *currentToken; // token hien tai
Token *lookAhead; // token kế tiếp

// ham khoi dong trinh phan tich
void scan(void) {
  Token* tmp = currentToken;
  currentToken = lookAhead;
  lookAhead = getValidToken();
  free(tmp);
}

// kiem tra va tieu thu mot token
void eat(TokenType tokenType) {
  // kiem tra token hien tai co phai la tokenType khong
  if (lookAhead->tokenType == tokenType) {
    printToken(lookAhead); // in token
    scan(); // di chuyen den token ke tiep
  } 
  else missingToken(tokenType, lookAhead->lineNo, lookAhead->colNo); // xu ly loi
}

// chuyen doi chuong trinh
void compileProgram(void) {
  // bat dau phan tich
  assert("Parsing a Program ....");
  eat(KW_PROGRAM); // tieu thu tu khoa program
  eat(TK_IDENT);  // tieu thu ten chuong trinh
  eat(SB_SEMICOLON); // tieu thu dau cham phay
  compileBlock(); // phan tich khoi
  eat(SB_PERIOD);
  assert("Program parsed!");
}

// phan tich mot khoi
void compileBlock(void) {
  assert("Parsing a Block ....");
  // phan tich khai bao hang so
  if (lookAhead->tokenType == KW_CONST) {
    eat(KW_CONST);
    compileConstDecl();
    compileConstDecls();
    compileBlock2();
  } 
  // khong co khai bao hang so
  else compileBlock2();
  assert("Block parsed!");
}

// phan tich khoi tiep theo
void compileBlock2(void) {
  // phan tich khai bao kieu
  if (lookAhead->tokenType == KW_TYPE) {
    eat(KW_TYPE);
    compileTypeDecl();
    compileTypeDecls();
    compileBlock3();
  } 
  else compileBlock3();
}

// phan tich khoi tiep theo
void compileBlock3(void) {
  // phan tich khai bao bien
  if (lookAhead->tokenType == KW_VAR) {
    eat(KW_VAR);
    compileVarDecl(); // phan tich khai bao bien dau tien
    compileVarDecls(); // phan tich cac khai bao bien tiep theo
    compileBlock4(); // phan tich khoi tiep theo
  } 
  else compileBlock4();
}

// phan tich khoi tiep theo
void compileBlock4(void) {
  compileSubDecls(); // phan tich cac khai bao thu tuc/ ham
  compileBlock5();
}

// phan tich khoi cuoi cung
void compileBlock5(void) {
  eat(KW_BEGIN);
  compileStatements(); // phan tich cac cau lenh
  eat(KW_END);
}

// phan tich cac khai bao hang so
void compileConstDecls(void) {
  // neu con khai bao hang so
  if (lookAhead->tokenType == TK_IDENT) {
    compileConstDecl();
    compileConstDecls();
  }
}

// phan tich mot khai bao hang so
void compileConstDecl(void) {
  eat(TK_IDENT);
  eat(SB_EQ);
  compileConstant();
  eat(SB_SEMICOLON);
}

// phan tich cac khai bao kieu
void compileTypeDecls(void) {
  if (lookAhead->tokenType == TK_IDENT) {
    compileTypeDecl();
    compileTypeDecls();
  }
}

// phan tich mot khai bao kieu
void compileTypeDecl(void) {
  eat(TK_IDENT);
  eat(SB_EQ);
  compileType();
  eat(SB_SEMICOLON);
}

// phan tich cac khai bao bien
void compileVarDecls(void) {
  if (lookAhead->tokenType == TK_IDENT) {
    compileVarDecl();
    compileVarDecls();
  }
}

// phan tich mot khai bao bien
void compileVarDecl(void) {
  eat(TK_IDENT);
  eat(SB_COLON);
  compileType();
  eat(SB_SEMICOLON);
}

// phan tich cac khai bao thu tuc/ ham
void compileSubDecls(void) {
  assert("Parsing subtoutines ....");
  // neu con khai bao ham/ thu tuc
  if (lookAhead->tokenType == KW_FUNCTION){
    compileFuncDecl();
    compileSubDecls();
  } 
  else if(lookAhead->tokenType == KW_PROCEDURE){
    compileProcDecl();
    compileSubDecls();
  }
  assert("Subtoutines parsed ....");
}

// phan tich mot khai bao ham
void compileFuncDecl(void) {
  assert("Parsing a function ....");
  eat(KW_FUNCTION);
  eat(TK_IDENT);
  compileParams();
  eat(SB_COLON);
  compileBasicType();
  eat(SB_SEMICOLON);
  compileBlock();
  eat(SB_SEMICOLON);
  assert("Function parsed ....");
}

// phan tich mot khai bao thu tuc
void compileProcDecl(void) {
  assert("Parsing a procedure ....");
  eat(KW_PROCEDURE);
  eat(TK_IDENT);
  compileParams();
  eat(SB_SEMICOLON);
  compileBlock();
  eat(SB_SEMICOLON);
  assert("Procedure parsed ....");
}

// phan tich mot hang so khong dau
void compileUnsignedConstant(void) {
  if (lookAhead->tokenType == TK_NUMBER) {
    eat(TK_NUMBER);
  } else if (lookAhead->tokenType == TK_CHAR) {
    eat(TK_CHAR);
  } else if (lookAhead->tokenType == TK_IDENT) {
    eat(TK_IDENT);
  }
}

// phan tich mot hang so
void compileConstant(void) {
  if (lookAhead->tokenType == SB_MINUS || lookAhead->tokenType == SB_PLUS) {
    eat(lookAhead->tokenType);
    compileConstant2();
  } else if (lookAhead->tokenType == TK_CHAR) {
    eat(TK_CHAR);
  } else {
    compileConstant2();
  }
}

// phan tich phan con lai cua hang so
void compileConstant2(void) {
  if (lookAhead->tokenType == TK_IDENT || lookAhead->tokenType == TK_NUMBER) {
    eat(lookAhead->tokenType);
  }
}

//
void compileType(void) {
  if (lookAhead->tokenType == KW_INTEGER || lookAhead->tokenType == KW_CHAR || lookAhead->tokenType == TK_IDENT) {
    eat(lookAhead->tokenType);
  } else if (lookAhead->tokenType == KW_ARRAY) {
    eat(KW_ARRAY);
    eat(SB_LSEL);
    eat(TK_NUMBER);
    eat(SB_RSEL);
    eat(KW_OF);
    compileType();
  }
}

void compileBasicType(void) {
  if (lookAhead->tokenType == KW_INTEGER || lookAhead->tokenType == KW_CHAR) {
    eat(lookAhead->tokenType);
  }
}

void compileParams(void) {
  if (lookAhead->tokenType == SB_LPAR) {
    eat(SB_LPAR);
    compileParam();
    compileParams2();
    eat(SB_RPAR);
  }
}

void compileParams2(void) {
  if (lookAhead->tokenType == SB_SEMICOLON) {
    eat(SB_SEMICOLON);
    compileParam();
    compileParams2();
  }
}

void compileParam(void) {
  if (lookAhead->tokenType == TK_IDENT) {
    eat(TK_IDENT);
    eat(SB_COLON);
    compileBasicType();
  } else if (lookAhead->tokenType == KW_VAR) {
    eat(KW_VAR);
    eat(TK_IDENT);
    eat(SB_COLON);
    compileBasicType();
  }
}

void compileStatements(void) {
  compileStatement();
  compileStatements2();
}

void compileStatements2(void) {
  if (lookAhead->tokenType == SB_SEMICOLON) {
    eat(SB_SEMICOLON);
    compileStatement();
    compileStatements2();
  } else if (lookAhead->tokenType == TK_IDENT || 
             lookAhead->tokenType == KW_CALL ||
             lookAhead->tokenType == KW_IF ||
             lookAhead->tokenType == KW_WHILE ||
             lookAhead->tokenType == KW_FOR ||
             lookAhead->tokenType == KW_BEGIN) {
    missingToken(SB_SEMICOLON, lookAhead->lineNo, lookAhead->colNo);
  }
}

void compileStatement(void) {
  if(lookAhead->tokenType == TK_IDENT) {
    compileAssignSt();
  } else if(lookAhead->tokenType == KW_CALL) {
    compileCallSt();
  } else if(lookAhead->tokenType == KW_IF) {
    compileIfSt();
  } else if(lookAhead->tokenType == KW_WHILE) {
    compileWhileSt();
  } else if(lookAhead->tokenType == KW_FOR) {
    compileForSt();
  } else if(lookAhead->tokenType == KW_BEGIN) {
    compileGroupSt();
  }
}

void compileAssignSt(void) {
  assert("Parsing an assign statement ....");
  eat(TK_IDENT);
  compileIndexes();
  eat(SB_ASSIGN);
  compileExpression();
  assert("Assign statement parsed ....");
}

void compileCallSt(void) {
  assert("Parsing a call statement ....");
  eat(KW_CALL);
  eat(TK_IDENT);
  compileArguments();
  assert("Call statement parsed ....");
}

void compileGroupSt(void) {
  assert("Parsing a group statement ....");
  eat(KW_BEGIN);
  compileStatements();
  eat(KW_END);
  assert("Group statement parsed ....");
}

void compileIfSt(void) {
  assert("Parsing an if statement ....");
  eat(KW_IF);
  compileCondition();
  eat(KW_THEN);
  compileStatement();
  if (lookAhead->tokenType == KW_ELSE) 
    compileElseSt();
  assert("If statement parsed ....");
}

void compileElseSt(void) {
  eat(KW_ELSE);
  compileStatement();
}

void compileWhileSt(void) {
  assert("Parsing a while statement ....");
  eat(KW_WHILE);
  compileCondition();
  eat(KW_DO);
  compileStatement();
  assert("While statement parsed ....");
}

void compileForSt(void) {
  assert("Parsing a for statement ....");
  eat(KW_FOR);
  eat(TK_IDENT);
  eat(SB_ASSIGN);
  compileExpression();
  eat(KW_TO);
  compileExpression();
  eat(KW_DO);
  compileStatement();
  assert("For statement parsed ....");
}

void compileArguments(void) {
  if (lookAhead->tokenType == SB_LPAR) {
    eat(SB_LPAR);
    compileExpression();
    compileArguments2();
    eat(SB_RPAR);
  }
}

void compileArguments2(void) {
  while (lookAhead->tokenType == SB_COMMA) {
    eat(SB_COMMA);
    compileExpression();
  }
}

void compileCondition(void) {
  compileExpression();
  compileCondition2();
}

void compileCondition2(void) {
  switch (lookAhead->tokenType) {
  case SB_EQ:
    eat(SB_EQ);
    compileExpression();
    break;
  case SB_NEQ:
    eat(SB_NEQ);
    compileExpression();
    break;
  case SB_LT:
    eat(SB_LT);
    compileExpression();
    break;
  case SB_LE:
    eat(SB_LE);
    compileExpression();
    break;
  case SB_GT:
    eat(SB_GT);
    compileExpression();
    break;
  case SB_GE:
    eat(SB_GE);
    compileExpression();
    break;
  default:
    error(ERR_INVALIDCOMPARATOR, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

void compileExpression(void) {
  assert("Parsing an expression");
  switch (lookAhead->tokenType) {
  case SB_PLUS:
  case SB_MINUS:
    eat(lookAhead->tokenType);
    compileExpression2();
    break;
  default:
    compileExpression2();
    break;
  }
  assert("Expression parsed");
}

void compileExpression2(void) {
  compileTerm();
  compileExpression3();
}


void compileExpression3(void) {
  while (lookAhead->tokenType == SB_PLUS || lookAhead->tokenType == SB_MINUS) {
    eat(lookAhead->tokenType);
    compileTerm();
  }
}

void compileTerm(void) {
  compileFactor();
  compileTerm2();
}

void compileTerm2(void) {
  while (lookAhead->tokenType == SB_TIMES || lookAhead->tokenType == SB_SLASH) {
    eat(lookAhead->tokenType);
    compileFactor();
  }
}

void compileFactor(void) {
  switch (lookAhead->tokenType) {
  case TK_NUMBER:
  case TK_CHAR:
    compileUnsignedConstant();
    break;
  case TK_IDENT:
    eat(TK_IDENT);
    switch (lookAhead->tokenType) {
    case SB_LSEL:
      compileIndexes();
      break;
    case SB_LPAR:
      compileArguments();
      break;
    default:
      break;
    }
    break;
  case SB_LPAR:
    eat(SB_LPAR);
    compileExpression();
    eat(SB_RPAR);
    break;
  default:
    error(ERR_INVALIDFACTOR, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

void compileIndexes(void) {
  while (lookAhead->tokenType == SB_LSEL) {
    eat(SB_LSEL);
    compileExpression();
    eat(SB_RSEL);
  }
}

int compile(char *fileName) {
  if (openInputStream(fileName) == IO_ERROR)
    return IO_ERROR;

  currentToken = NULL;
  lookAhead = getValidToken();

  compileProgram();

  free(currentToken);
  free(lookAhead);
  closeInputStream();
  return IO_SUCCESS;

}