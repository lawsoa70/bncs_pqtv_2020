////////////////////////////////////////////////////////////////////
//
//  functions for WM_COPYDATA messaging
//
// 4.1.0.0 - 09/09/04	first versioned release
// 4.1.1.0 - 06/02/20   Corrects use of CBDATA_SIZE_POINTER because it's in a pickle
////////////////////////////////////////////////////////////////////

#ifndef BNCSWM32_INCLUDED
	#define BNCSWM32_INCLUDED

// this is the v4 scheme:
#ifdef __cplusplus
extern "C" {
#endif
  
LRESULT BNCS32SendMessage(HWND hWndThisApp, HWND hWndTargetApp, UINT uMsg, WPARAM wParam, LPARAM lParam, int iReturnDataLen, int iDataType);
void BNCS32ReceiveMessage(HWND hWndThisApp, HWND hWndTargetApp, PCOPYDATASTRUCT lpCDS);
//void BNCS32RxMessage(HWND hWndThisApp, HWND hWndTargetApp, PCOPYDATASTRUCT lpCDS);

#ifdef __cplusplus
}
#endif



#endif // BNCSWM32_INCLUDED