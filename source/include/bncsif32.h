////////////////////////////////////////////////////////////////////
/*
//  functions for messaging DLL
//
//	Written by Adam Brocklesby and Tim Alden, with mods by Dave Yates
//
//////////////////////////////////////////////////////////////////////////
//
//	Version Control

V0.1	8/2/01		First Release Test

V0.2	12/2/01		Added length parameter to calls.
					Maximum length is still 512
					Added more range checking, doesn't clear slot
					pointers on each subsequent attach, only gets a file handle
v0.3	13/2/01		Made the pointer to the file instance-global
					added pointer validation to all calls in case the pointer is gone
	
v0.4	14/2/01		Wrapped GetData() in Semaphore

v0.5	19/2/01		Inserted test delay of 20ms in debug version

v0.6	27/2/01		More range checking of DATA SIZE to prevent GPF

 v1.0	22/3/01		Released as version 1.0. Increased number of slots to 64

v1.1	14/7/04		Modified acLocalCopy to make it private to the Receive func and thus thread safe

2.0.0.1 - 07/09/04	first versioned release
					headers changed to separate out dll messaging and WM_COPYDATA messaging scheme
4.1.0.0
4.2.0.0 - 22/11/2004	removes memory mapped file in favour of shared data segment
						simplifies data types
						Checks for slots still in use
						removes exports of unwanted functions
						removes structured message support
						increases number of slots to 128
4.2.0.1 - 22/11/2004	adds protection to the slotInUse array
4.2.0.2 - 25/11/2004	reinstates memory mapped files but keeps new data structures
						- whilst data_seg works nicely between 32 bit applications
						it doesn't work with a 16 bit app
4.3.0.x - 10/01/2011	Added BNCS32TxMessage2 to allow separate send and receive data lengths.
						This allows us to stop it from *always* copying back the
						data you're trying to send, which is particularly necessary if it should
						be constant (ie read-only).
						Version numbering changed so final digit (build number) not significant for
						knowing what the version is.
						And incremented minor version as it's a significant interface change.

////////////////////////////////////////////////////////////////////
*/
 
#ifndef BNCSIF32_INCLUDED
	#define BNCSIF32_INCLUDED

#ifdef __BORLANDC__
	#define GTHUNKS_API
#endif

#ifdef __BCPLUSPLUS__
	#define GTHUNKS_API
#endif

#ifndef GTHUNKS_API
	#ifdef GTHUNKS_EXPORTS
		#define GTHUNKS_API __declspec(dllexport)
	#else
		#define GTHUNKS_API __declspec(dllimport)
	#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

LRESULT GTHUNKS_API BNCS32RxMessage(HWND hWnd, WPARAM wParam, LPARAM lParam);
LRESULT GTHUNKS_API BNCS32TxMessage(HWND hWnd, WORD msg, WPARAM wParam, LPARAM lParam, int type,
	int iSendLen);
LRESULT GTHUNKS_API BNCS32TxMessage2(HWND hWnd, WORD msg, WPARAM wParam, LPARAM lParam, int type,
	int iSendLen, int iReceiveLen);
long GTHUNKS_API GetData(int Page, LPSTR lpData, int iLen);
int GTHUNKS_API ReturnData(int Page,LPSTR lpString,int iLen);


#ifdef __cplusplus
}
#endif

#endif // BNCSIF32_INCLUDED