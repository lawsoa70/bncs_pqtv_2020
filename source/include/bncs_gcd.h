/*
 * BBC Custom Control Library : Header File
 * (c) BBC 2006
 *
 * file: bncs_gcd.h
 *
 * $Id: bncs_gcd.h,v 1.1 2009/06/26 10:46:52 rockld40 Exp $
 *
 */
#ifndef	_bncs_gcd_INCLUDE_FILE_
#define _bncs_gcd_INCLUDE_FILE_

#ifdef __cplusplus
extern "C" {  // only need to export C interface if
              // used by C++ source code
#endif

#define BNCS_GCD_MAXPATH	512
#define BNCS_GCD_DEFAULT_PATH			"c:\\bncs_config.ini"
#define BNCS_GCD_DEFAULT_TEMP_PATH	"c:\\csi_temp"

void GetBNCSConfigDirectory(char *, int);
void GetBNCSSystemDirectory(char *, int);
void GetBNCSTempDirectory(char *, int);
int GetWorkstationNumber( void );

#ifdef __cplusplus
}
#endif


#endif	/* _bncs_gcd_INCLUDE_FILE_*/
