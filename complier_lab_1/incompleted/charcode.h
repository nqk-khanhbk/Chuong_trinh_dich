/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#ifndef __CHARCODE_H__
#define __CHARCODE_H__

// danh sach ma ky tu
typedef enum {
  CHAR_SPACE, // blank space
  CHAR_LETTER, // alphabetic letters A-Z, a-z
  CHAR_DIGIT, // digits 0-9
  CHAR_PLUS, // '+'
  CHAR_MINUS, // '-'
  CHAR_TIMES, // '*'
  CHAR_SLASH, // '/'
  CHAR_LT,    // '<'
  CHAR_GT,   // '>'
  CHAR_EXCLAIMATION,  // '!'
  CHAR_EQ, // '='
  CHAR_COMMA, // ','
  CHAR_PERIOD, // '.'
  CHAR_COLON, // ':'
  CHAR_SEMICOLON,  // ';'
  CHAR_SINGLEQUOTE, // '\''
  CHAR_LPAR,  // '('
  CHAR_RPAR, // ')'
  CHAR_UNKNOWN // other characters
} CharCode;

#endif
