//disable warnings on 255 char debug symbols
#pragma warning (disable : 4786)

#include <windows.h>

#include <list>
#include <map>
#include <string>
#include <time.h>
using namespace std;


#ifdef CALARM_PROCESS_EXPORTS
#define CALARM_PROCESS_API __declspec(dllexport)
#else
#define CALARM_PROCESS_API __declspec(dllimport)
#endif

struct almTime{int hour; int minutes;};

class QTime;

// This class is exported from the Calarm_Process.dll
class  Calarm_Process 
{

public:
	CALARM_PROCESS_API void setInitialState(int iState);
	CALARM_PROCESS_API int getAckSlot();
	CALARM_PROCESS_API int getAckBlock();
	CALARM_PROCESS_API bool SetInput(int iState);
	CALARM_PROCESS_API Calarm_Process(int, char* szName, bool debug=false);
	CALARM_PROCESS_API ~Calarm_Process(void);
	CALARM_PROCESS_API bool LoadSettings(char* szInput, DWORD iSize);
	CALARM_PROCESS_API void Override(int iState, int iMaxTime=0);
	CALARM_PROCESS_API bool InitProc(int* iStates, int* iCalcFlag, int iProcSize);
	CALARM_PROCESS_API int ProcessType(void);
	CALARM_PROCESS_API bool CalcNow(void);
	CALARM_PROCESS_API string ProcName(void);

private:
	void Debug(char* szFmt, ...);
	int Calc_Holdoff();
	int iHoldoff;
	bool LoadHoldoff(char* szInput);
	int compareLevels(int a, int b);
	bool bDebug;
	int iInputState;
	int Calc_Time();
	almTime tStart;
	almTime tStop;
	bool bTimeDay[7];
	bool LoadTime(char* szInput);
	time_t timeOverride;
	int iOverrideInput;
	int iProcessNumber;
	int iMaxProcs;
	int Calc_Input();
	int Calc_Truth();
	int Calc_Not();
	int Calc_Or();
	int Calc_And();
	void UpdateOutput(int iState);
	int LoadTruth(char* szInput);
	int LoadInputs(char* szInputlist);
	char* FindEntry(char* szInput, char* szFind);
	int iMaxOverrideTime;
	int iMask;
	int iRule;
	bool bLatch;
	bool bAckOnce;
	bool bRetrigger;
	bool bAcknowledged;
	int iCurrentState;
	int* iCalcFlagArray;
	int* iStateArray;
	list <int> lInput;
	list <string> lTruth;
	string sName;
	int iAcqInput;		//Input number of acquisition block
	int iAcqBlock;		//Acquisition block number
	int * iLastInputs;
	void saveLastInputs();
	int checkRetrigger();
	int iInputLastState;
	int iOutputLastState;
	clock_t tHoldoff;

};


