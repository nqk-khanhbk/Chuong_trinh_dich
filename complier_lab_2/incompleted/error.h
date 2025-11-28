/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#ifndef __ERROR_H__
#define __ERROR_H__
#include "token.h"

// mã lỗi
typedef enum {
  ERR_ENDOFCOMMENT, // kết thúc comment không đúng
  ERR_IDENTTOOLONG, // tên định danh quá dài
  ERR_INVALIDCHARCONSTANT,  // hằng ký tự không đúng
  ERR_INVALIDSYMBOL, // ký tự không hợp lệ
  ERR_INVALIDCONSTANT,  // hằng không đúng
  ERR_INVALIDTYPE,      // kiểu không đúng
  ERR_INVALIDBASICTYPE, // kiểu cơ bản không đúng
  ERR_INVALIDPARAM,   // tham số không đúng
  ERR_INVALIDSTATEMENT, // câu lệnh không đúng
  ERR_INVALIDARGUMENTS,   // đối số không đúng
  ERR_INVALIDCOMPARATOR, // toán tử so sánh không đúng
  ERR_INVALIDEXPRESSION, // biểu thức không đúng
  ERR_INVALIDTERM, // đơn thức không đúng
  ERR_INVALIDFACTOR,   // nhân tử không đúng
  ERR_INVALIDCONSTDECL, // khai báo hằng không đúng
  ERR_INVALIDTYPEDECL, // khai báo kiểu không đúng
  ERR_INVALIDVARDECL, // khai báo biến không đúng
  ERR_INVALIDSUBDECL, // khai báo thủ tục/ hàm không đúng
} ErrorCode;


#define ERM_ENDOFCOMMENT "End of comment expected!"
#define ERM_IDENTTOOLONG "Identification too long!"
#define ERM_INVALIDCHARCONSTANT "Invalid const char!"
#define ERM_INVALIDSYMBOL "Invalid symbol!"
#define ERM_INVALIDCONSTANT "Invalid constant!"
#define ERM_INVALIDTYPE "Invalid type!"
#define ERM_INVALIDBASICTYPE "Invalid basic type!"
#define ERM_INVALIDPARAM "Invalid parameter!"
#define ERM_INVALIDSTATEMENT "Invalid statement!"
#define ERM_INVALIDARGUMENTS "Invalid arguments!"
#define ERM_INVALIDCOMPARATOR "Invalid comparator!"
#define ERM_INVALIDEXPRESSION "Invalid expression!"
#define ERM_INVALIDTERM "Invalid term!"
#define ERM_INVALIDFACTOR "Invalid factor!"
#define ERM_INVALIDCONSTDECL "Invalid constant declaration!"
#define ERM_INVALIDTYPEDECL "Invalid type declaration!"
#define ERM_INVALIDVARDECL "Invalid variable declaration!"
#define ERM_INVALIDSUBDECL "Invalid subroutine declaration!"

void error(ErrorCode err, int lineNo, int colNo);
void missingToken(TokenType tokenType, int lineNo, int colNo);
void assert(char *msg);

#endif
