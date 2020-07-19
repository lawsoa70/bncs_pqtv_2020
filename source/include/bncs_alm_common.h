////////////////////////
//al_common.h
//header file for common elements for alarm product
//
///////////////////////////////////////////////////
#define STATIC_LIB

//define data constants
#define BNCS_AL_UNKNOWN		-1
#define BNCS_AL_OFF			0
#define BNCS_AL_ON			1
#define BNCS_AL_FORCE_OFF	2
#define BNCS_AL_FORCE_ON	3
#define BNCS_AL_ACK			4
#define BNCS_AL_IGNORE		5
#define BNCS_AL_LATCHED		6	//added 7/1/2004 M.Baldry

// 7 to 10 are For override use only (ie isn't stored as a state)  
#define BNCS_AL_ACK_ONLY	7
		//		Ack if failed.
		//		don't clear inhibits or Force on/off.
#define BNCS_AL_CLEAR		8
		//		Clear inhibit.  
		//		Clear Force on/off.  Don't ack.
#define BNCS_AL_ACK_CONTRIBUTORS_IMMEDIATE	9
#define BNCS_AL_ACK_CONTRIBUTORS_ALL	10

// values for 'Calculated' flag.  Could be boolean.
#define BNCS_AL_UNCALCULATED	0
#define BNCS_AL_CALCULATED		1

//define process Function types
#define BNCS_ALM_FUNC_INPUT		1
#define BNCS_ALM_FUNC_AND		2
#define BNCS_ALM_FUNC_OR		3
#define BNCS_ALM_FUNC_NOT		4
#define BNCS_ALM_FUNC_TRUTH		5
#define BNCS_ALM_FUNC_TIME		6
#define BNCS_ALM_FUNC_HOLDOFF	7		//added 7/1/2004 M.Baldry
#define BNCS_ALM_FUNC_INVALID	-1

//define alarm error cause	(added by KeseruK for error diagnostics)
#define BNCS_AL_UNDEFINED			-1
#define BNCS_AL_INVALID_RULE		1
#define BNCS_AL_UNCALCULATED_INPUT	2
#define BNCS_AL_UNCALCULATED_MASK	3
#define BNCS_AL_INTERNAL_ERROR		4	// not really meaningful error cause, debug may give some more details
