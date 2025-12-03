/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */
#include <stdio.h>
#include <stdlib.h>

#include "reader.h"
#include "scanner.h"
#include "parser.h"
#include "error.h"
#include "debug.h"
#include <string.h>

Token *currentToken;
Token *lookAhead;

extern Type* intType;
extern Type* charType;
extern SymTab* symtab;

void scan(void) {
  Token* tmp = currentToken;
  currentToken = lookAhead;
  lookAhead = getValidToken();
  free(tmp);
}

void eat(TokenType tokenType) {
  if (lookAhead->tokenType == tokenType) {
    scan();
  } else missingToken(tokenType, lookAhead->lineNo, lookAhead->colNo);
}

void compileProgram(void) {
  // create program object, enter its scope, parse block, then exit
  eat(KW_PROGRAM);
  if (lookAhead->tokenType == TK_IDENT) {
    // create program object using the identifier string
    Object* program = createProgramObject(lookAhead->string);
    eat(TK_IDENT);

    enterBlock(program->progAttrs->scope);

    eat(SB_SEMICOLON);
    compileBlock();
    eat(SB_PERIOD);

    exitBlock();
  } else {
    missingToken(TK_IDENT, lookAhead->lineNo, lookAhead->colNo);
  }
}

void compileBlock(void) {
  // create and declare constant objects
  if (lookAhead->tokenType == KW_CONST) {
    eat(KW_CONST);

    do {
      // name
      char name[MAX_IDENT_LEN + 1];
      strcpy(name, lookAhead->string);
      eat(TK_IDENT);

      eat(SB_EQ);
      ConstantValue* val = compileConstant();

      Object* obj = createConstantObject(name);
      obj->constAttrs->value = val;
      declareObject(obj);

      eat(SB_SEMICOLON);
    } while (lookAhead->tokenType == TK_IDENT);

    compileBlock2();
  } 
  else compileBlock2();
}

void compileBlock2(void) {
  // create and declare type objects
  if (lookAhead->tokenType == KW_TYPE) {
    eat(KW_TYPE);

    do {
      char name[MAX_IDENT_LEN + 1];
      strcpy(name, lookAhead->string);
      eat(TK_IDENT);

      eat(SB_EQ);
      Type* t = compileType();

      Object* obj = createTypeObject(name);
      obj->typeAttrs->actualType = t;
      declareObject(obj);

      eat(SB_SEMICOLON);
    } while (lookAhead->tokenType == TK_IDENT);

    compileBlock3();
  } 
  else compileBlock3();
}

void compileBlock3(void) {
  // create and declare variable objects
  if (lookAhead->tokenType == KW_VAR) {
    eat(KW_VAR);

    do {
      // support comma-separated identifiers? original grammar may be single ident per line
      char name[MAX_IDENT_LEN + 1];
      strcpy(name, lookAhead->string);
      eat(TK_IDENT);

      eat(SB_COLON);
      Type* t = compileType();

      Object* obj = createVariableObject(name);
      obj->varAttrs->type = t;
      declareObject(obj);

      eat(SB_SEMICOLON);
    } while (lookAhead->tokenType == TK_IDENT);

    compileBlock4();
  } 
  else compileBlock4();
}

void compileBlock4(void) {
  compileSubDecls();
  compileBlock5();
}

void compileBlock5(void) {
  eat(KW_BEGIN);
  compileStatements();
  eat(KW_END);
}

void compileSubDecls(void) {
  while ((lookAhead->tokenType == KW_FUNCTION) || (lookAhead->tokenType == KW_PROCEDURE)) {
    if (lookAhead->tokenType == KW_FUNCTION)
      compileFuncDecl();
    else compileProcDecl();
  }
}

void compileFuncDecl(void)
{
  // TODO: create and declare a function object
  Object *funcObj;
  Type *returnType;
 
  eat(KW_FUNCTION);
  eat(TK_IDENT);
 
  // Tạo function object
  funcObj = createFunctionObject(currentToken->string);
 
  // Khai báo function vào scope hiện tại
  declareObject(funcObj);
 
  // Vào scope của function để compile params
  enterBlock(funcObj->funcAttrs->scope);
 
  compileParams();
  eat(SB_COLON);
 
  // Compile và gán return type
  returnType = compileBasicType();
  funcObj->funcAttrs->returnType = returnType;
 
  eat(SB_SEMICOLON);
  compileBlock();
  eat(SB_SEMICOLON);
 
  // Thoát khỏi scope của function
  exitBlock();
}

void compileProcDecl(void) {
  // create and declare a procedure object
  eat(KW_PROCEDURE);

  char name[MAX_IDENT_LEN + 1];
  strcpy(name, lookAhead->string);
  eat(TK_IDENT);

  Object* proc = createProcedureObject(name);
  declareObject(proc);
  // enter procedure scope first so parameters are declared inside it
  enterBlock(proc->procAttrs->scope);

  compileParams();

  eat(SB_SEMICOLON);

  compileBlock();

  exitBlock();
  eat(SB_SEMICOLON);
}

ConstantValue* compileUnsignedConstant(void) {
  // create and return an unsigned constant value
  ConstantValue* constValue = NULL;

  switch (lookAhead->tokenType) {
  case TK_NUMBER: {
    int val = lookAhead->value;
    eat(TK_NUMBER);
    constValue = makeIntConstant(val);
    break;
  }
  case TK_IDENT: {
    // constant defined by identifier
    char name[MAX_IDENT_LEN + 1];
    strcpy(name, lookAhead->string);
    eat(TK_IDENT);
    Object* obj = lookupObject(name);
    if (obj != NULL && obj->kind == OBJ_CONSTANT) {
      constValue = duplicateConstantValue(obj->constAttrs->value);
    } else {
      error(ERR_UNDECLARED_CONSTANT, lookAhead->lineNo, lookAhead->colNo);
    }
    break;
  }
  case TK_CHAR: {
    char ch = lookAhead->string[0];
    eat(TK_CHAR);
    constValue = makeCharConstant(ch);
    break;
  }
  default:
    error(ERR_INVALID_CONSTANT, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
  return constValue;
}

ConstantValue* compileConstant(void) {
  // create and return a (possibly signed) constant
  ConstantValue* constValue = NULL;

  switch (lookAhead->tokenType) {
  case SB_PLUS:
    eat(SB_PLUS);
    constValue = compileConstant2();
    break;
  case SB_MINUS:
    eat(SB_MINUS);
    constValue = compileConstant2();
    if (constValue != NULL && constValue->type == TP_INT) {
      constValue->intValue = -(constValue->intValue);
    } else if (constValue != NULL && constValue->type == TP_CHAR) {
      // invalid: can't negate char, report error
      error(ERR_INVALID_CONSTANT, lookAhead->lineNo, lookAhead->colNo);
    }
    break;
  case TK_CHAR:
    constValue = compileUnsignedConstant();
    break;
  default:
    constValue = compileConstant2();
    break;
  }
  return constValue;
}

// phan tich phan con lai cua hang so
ConstantValue* compileConstant2(void) {
  // create and return a constant value (no sign)
  ConstantValue* constValue = NULL;

  switch (lookAhead->tokenType) {
  case TK_NUMBER: {
    int val = lookAhead->value;
    eat(TK_NUMBER);
    constValue = makeIntConstant(val);
    break;
  }
  case TK_IDENT: {
    char name[MAX_IDENT_LEN + 1];
    strcpy(name, lookAhead->string);
    eat(TK_IDENT);
    Object* obj = lookupObject(name);
    if (obj != NULL && obj->kind == OBJ_CONSTANT) {
      constValue = duplicateConstantValue(obj->constAttrs->value);
    } else {
      error(ERR_UNDECLARED_CONSTANT, lookAhead->lineNo, lookAhead->colNo);
    }
    break;
  }
  default:
    error(ERR_INVALID_CONSTANT, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
  return constValue;
}

Type* compileType(void) {
  // create and return a type
  Type* type = NULL;

  switch (lookAhead->tokenType) {
  case KW_INTEGER:
    eat(KW_INTEGER);
    type = makeIntType();
    break;
  case KW_CHAR:
    eat(KW_CHAR);
    type = makeCharType();
    break;
  case KW_ARRAY: {
    eat(KW_ARRAY);
    eat(SB_LSEL);
    if (lookAhead->tokenType == TK_NUMBER) {
      int size = lookAhead->value;
      eat(TK_NUMBER);
      eat(SB_RSEL);
      eat(KW_OF);
      Type* elemType = compileType();
      type = makeArrayType(size, elemType);
    } else {
      error(ERR_INVALID_TYPE, lookAhead->lineNo, lookAhead->colNo);
    }
    break;
  }
  case TK_IDENT: {
    char name[MAX_IDENT_LEN + 1];
    strcpy(name, lookAhead->string);
    eat(TK_IDENT);
    Object* obj = lookupObject(name);
    if (obj != NULL && obj->kind == OBJ_TYPE) {
      type = duplicateType(obj->typeAttrs->actualType);
    } else {
      error(ERR_UNDECLARED_TYPE, lookAhead->lineNo, lookAhead->colNo);
    }
    break;
  }
  default:
    error(ERR_INVALID_TYPE, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
  return type;
}

Type* compileBasicType(void) {
  // create and return a basic type
  Type* type = NULL;

  switch (lookAhead->tokenType) {
  case KW_INTEGER:
    eat(KW_INTEGER);
    type = makeIntType();
    break;
  case KW_CHAR:
    eat(KW_CHAR);
    type = makeCharType();
    break;
  default:
    error(ERR_INVALID_BASICTYPE, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
  return type;
}

void compileParams(void) {
  if (lookAhead->tokenType == SB_LPAR) {
    eat(SB_LPAR);
    compileParam();
    while (lookAhead->tokenType == SB_SEMICOLON) {
      eat(SB_SEMICOLON);
      compileParam();
    }
    eat(SB_RPAR);
  }
}

void compileParam(void)
{
  // TODO: create and declare a parameter
  Object *param;
  Type *type;
  enum ParamKind paramKind;
 
  switch (lookAhead->tokenType)
  {
  case TK_IDENT:
    paramKind = PARAM_VALUE;
    eat(TK_IDENT);
    // Tạo parameter object
    param = createParameterObject(currentToken->string, paramKind, symtab->currentScope->owner);
    eat(SB_COLON);
    type = compileBasicType();
    param->paramAttrs->type = type;
    // Khai báo parameter
    declareObject(param);
    break;
  case KW_VAR:
    paramKind = PARAM_REFERENCE;
    eat(KW_VAR);
    eat(TK_IDENT);
    // Tạo parameter object (tham chiếu)
    param = createParameterObject(currentToken->string, paramKind, symtab->currentScope->owner);
    eat(SB_COLON);
    type = compileBasicType();
    param->paramAttrs->type = type;
    // Khai báo parameter
    declareObject(param);
    break;
  default:
    error(ERR_INVALID_PARAMETER, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

void compileStatements(void) {
  compileStatement();
  while (lookAhead->tokenType == SB_SEMICOLON) {
    eat(SB_SEMICOLON);
    compileStatement();
  }
}

void compileStatement(void) {
  switch (lookAhead->tokenType) {
  case TK_IDENT:
    compileAssignSt();
    break;
  case KW_CALL:
    compileCallSt();
    break;
  case KW_BEGIN:
    compileGroupSt();
    break;
  case KW_IF:
    compileIfSt();
    break;
  case KW_WHILE:
    compileWhileSt();
    break;
  case KW_FOR:
    compileForSt();
    break;
    // EmptySt needs to check FOLLOW tokens
  case SB_SEMICOLON:
  case KW_END:
  case KW_ELSE:
    break;
    // Error occurs
  default:
    error(ERR_INVALID_STATEMENT, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

void compileLValue(void) {
  eat(TK_IDENT);
  compileIndexes();
}

void compileAssignSt(void) {
  compileLValue();
  eat(SB_ASSIGN);
  compileExpression();
}

void compileCallSt(void) {
  eat(KW_CALL);
  eat(TK_IDENT);
  compileArguments();
}

void compileGroupSt(void) {
  eat(KW_BEGIN);
  compileStatements();
  eat(KW_END);
}

void compileIfSt(void) {
  eat(KW_IF);
  compileCondition();
  eat(KW_THEN);
  compileStatement();
  if (lookAhead->tokenType == KW_ELSE) 
    compileElseSt();
}

void compileElseSt(void) {
  eat(KW_ELSE);
  compileStatement();
}

void compileWhileSt(void) {
  eat(KW_WHILE);
  compileCondition();
  eat(KW_DO);
  compileStatement();
}

void compileForSt(void) {
  eat(KW_FOR);
  eat(TK_IDENT);
  eat(SB_ASSIGN);
  compileExpression();
  eat(KW_TO);
  compileExpression();
  eat(KW_DO);
  compileStatement();
}

void compileArgument(void) {
  compileExpression();
}

void compileArguments(void) {
  switch (lookAhead->tokenType) {
  case SB_LPAR:
    eat(SB_LPAR);
    compileArgument();

    while (lookAhead->tokenType == SB_COMMA) {
      eat(SB_COMMA);
      compileArgument();
    }

    eat(SB_RPAR);
    break;
    // Check FOLLOW set 
  case SB_TIMES:
  case SB_SLASH:
  case SB_PLUS:
  case SB_MINUS:
  case KW_TO:
  case KW_DO:
  case SB_RPAR:
  case SB_COMMA:
  case SB_EQ:
  case SB_NEQ:
  case SB_LE:
  case SB_LT:
  case SB_GE:
  case SB_GT:
  case SB_RSEL:
  case SB_SEMICOLON:
  case KW_END:
  case KW_ELSE:
  case KW_THEN:
    break;
  default:
    error(ERR_INVALID_ARGUMENTS, lookAhead->lineNo, lookAhead->colNo);
  }
}

void compileCondition(void) {
  compileExpression();
  switch (lookAhead->tokenType) {
  case SB_EQ:
    eat(SB_EQ);
    break;
  case SB_NEQ:
    eat(SB_NEQ);
    break;
  case SB_LE:
    eat(SB_LE);
    break;
  case SB_LT:
    eat(SB_LT);
    break;
  case SB_GE:
    eat(SB_GE);
    break;
  case SB_GT:
    eat(SB_GT);
    break;
  default:
    error(ERR_INVALID_COMPARATOR, lookAhead->lineNo, lookAhead->colNo);
  }

  compileExpression();
}

void compileExpression(void) {
  switch (lookAhead->tokenType) {
  case SB_PLUS:
    eat(SB_PLUS);
    compileExpression2();
    break;
  case SB_MINUS:
    eat(SB_MINUS);
    compileExpression2();
    break;
  default:
    compileExpression2();
  }
}

void compileExpression2(void) {
  compileTerm();
  compileExpression3();
}


void compileExpression3(void) {
  switch (lookAhead->tokenType) {
  case SB_PLUS:
    eat(SB_PLUS);
    compileTerm();
    compileExpression3();
    break;
  case SB_MINUS:
    eat(SB_MINUS);
    compileTerm();
    compileExpression3();
    break;
    // check the FOLLOW set
  case KW_TO:
  case KW_DO:
  case SB_RPAR:
  case SB_COMMA:
  case SB_EQ:
  case SB_NEQ:
  case SB_LE:
  case SB_LT:
  case SB_GE:
  case SB_GT:
  case SB_RSEL:
  case SB_SEMICOLON:
  case KW_END:
  case KW_ELSE:
  case KW_THEN:
    break;
  default:
    error(ERR_INVALID_EXPRESSION, lookAhead->lineNo, lookAhead->colNo);
  }
}

void compileTerm(void) {
  compileFactor();
  compileTerm2();
}

void compileTerm2(void) {
  switch (lookAhead->tokenType) {
  case SB_TIMES:
    eat(SB_TIMES);
    compileFactor();
    compileTerm2();
    break;
  case SB_SLASH:
    eat(SB_SLASH);
    compileFactor();
    compileTerm2();
    break;
    // check the FOLLOW set
  case SB_PLUS:
  case SB_MINUS:
  case KW_TO:
  case KW_DO:
  case SB_RPAR:
  case SB_COMMA:
  case SB_EQ:
  case SB_NEQ:
  case SB_LE:
  case SB_LT:
  case SB_GE:
  case SB_GT:
  case SB_RSEL:
  case SB_SEMICOLON:
  case KW_END:
  case KW_ELSE:
  case KW_THEN:
    break;
  default:
    error(ERR_INVALID_TERM, lookAhead->lineNo, lookAhead->colNo);
  }
}

void compileFactor(void) {
  switch (lookAhead->tokenType) {
  case TK_NUMBER:
    eat(TK_NUMBER);
    break;
  case TK_CHAR:
    eat(TK_CHAR);
    break;
  case TK_IDENT:
    eat(TK_IDENT);
    switch (lookAhead->tokenType) {
    case SB_LPAR:
      compileArguments();
      break;
    case SB_LSEL:
      compileIndexes();
      break;
    default:
      break;
    }
    break;
  default:
    error(ERR_INVALID_FACTOR, lookAhead->lineNo, lookAhead->colNo);
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

  initSymTab();

  compileProgram();

  printObject(symtab->program,0);

  cleanSymTab();

  free(currentToken);
  free(lookAhead);
  closeInputStream();
  return IO_SUCCESS;

}
