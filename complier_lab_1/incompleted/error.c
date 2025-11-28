/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include "error.h"

// ham bao bao loi
void error(ErrorCode err, int lineNo, int colNo) {
  switch (err) {
    // in ra thong bao loi tuong ung "End of comment expected!"
  case ERR_ENDOFCOMMENT:
    printf("%d-%d:%s\n", lineNo, colNo, ERM_ENDOFCOMMENT);
    break;
    // in ra thong bao loi tuong ung "Identifier too long!"
  case ERR_IDENTTOOLONG:
    printf("%d-%d:%s\n", lineNo, colNo, ERM_IDENTTOOLONG);
    break;
    // in ra thong bao loi tuong ung "Invalid character constant!"
  case ERR_INVALIDCHARCONSTANT:
    printf("%d-%d:%s\n", lineNo, colNo, ERM_INVALIDCHARCONSTANT);
    break;
    // in ra thong bao loi tuong ung "Invalid symbol!"
  case ERR_INVALIDSYMBOL:
    printf("%d-%d:%s\n", lineNo, colNo, ERM_INVALIDSYMBOL);
    break;
  // default:
  //   /* Unknown error code: print numeric code for debugging */
  //   printf("%d-%d:Unknown error code %d\n", lineNo, colNo, (int)err);
  //   break;
  }
  exit(-1); // thoát chương trình với mã lỗi -1,không tiếp tục phân tích 
} 

