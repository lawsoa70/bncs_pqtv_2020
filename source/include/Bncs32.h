/***************************************************************************
* CSI 32 bit interfacing : Message packer and cracker                      *
* (c) BBC 1993-2000		 : Author Simon Dowson						            *
****************************************************************************

Version History
29/09/99	V1 First version
18/11/99 V2 Added reverse message for return data
25/01/00 V3 Added NUMERIC/POINTER option
03/04/00 V4 modification to allow SendMessage() results to be passed back
22/03/01 v7 Modification for DLL scheme. Tim Alden/Adam Brocklesby
07/09/04 V4.1	Now uses individual DLL32.h and bncsWM32.h files

	Note: version 4 messaging is the default. 
	Define BNCS32_VERSION as 7 for DLL functionality
***************************************************************************/

#ifndef  __HEX_VERSION__

#ifndef BNCS32_INCLUDED
	#define BNCS32_INCLUDED

#define CBDATA_SIZE_NUMERIC	16

#pragma message("bncs32.h: this value for CBDATA_SIZE_POINTER is incorrect - the size should be 512, but where a buffer might want to be 513 (for null term)")
#pragma message("bncs32.h: use CBDATA_SIZE_POINTER_BUFFER OR CBDATA_SIZE_POINTER_DATA")
#define CBDATA_SIZE_POINTER	513		// this value misued to size buffers and as the size of data to pass about. This should be 512 bytes
#define CBDATA_SIZE_POINTER_BUFFER 513	// the size of a null terminated buffer 
#define CBDATA_SIZE_POINTER_DATA 512	// the maximum size of the data to be passed 

#define CBDATA_TYPE_NUMERIC	0
#define CBDATA_TYPE_POINTER	1


#endif // BNCS32_INCLUDED

#endif //HEXVERSION - this stops CSI compile breaking ths
