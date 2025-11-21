/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#ifndef __READER_H__
#define __READER_H__

#define IO_ERROR 0
#define IO_SUCCESS 1

// ham doc ky tu tu kenh dau vao
int readChar(void);

// ham mo kenh dau vao
int openInputStream(char *fileName);

// ham dong kenh dau vao
void closeInputStream(void);

#endif
