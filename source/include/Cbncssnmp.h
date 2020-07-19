/*	Version
	v1.00	28.10.02

*/

#include <stdio.h>
#include <winsnmp.h>
#include <string>

#define MAX_STRING 255

class oid  
{
public:
	char* get(){return str;}
	oid(char*s){strcpy(str,s);}
	char *getstring()const{return (char*)str;}
	virtual ~oid(){};

private:
	char str[MAX_STRING];
};


class CsnmpBncs
{
private:
	virtual HSNMP_VBL SNMPTrap(CsnmpBncs*, HSNMP_PDU hPDU);		//used to forward trap messages
	virtual HSNMP_VBL SNMPReply(CsnmpBncs*, HSNMP_PDU hPDU);	//used to reply to messages
	virtual void SNMPError(CsnmpBncs*, UINT iError, UINT iIndex);//used to indicate message erorr number
	HSNMP_SESSION		hSnmpSession;		//Handle to the SNMP session
	HSNMP_VBL			hVbl;				//Handle to Variable binding list
	HSNMP_PDU			hPdu;				//Handle to SNMP protocol data unit (PDU). 
	HSNMP_ENTITY		hDest;				//Handle to SNMP destination
	HSNMP_CONTEXT		hCtx;				//Handle to SNMP context
	smiOCTETS			dCtx;				//Structure holding community name
	static UINT			uInstance;			//Number of instances of class

	static SNMPAPI_STATUS CALLBACK snmp_callback(HSNMP_SESSION ,HWND , UINT ,WPARAM ,LPARAM ,LPVOID );
	void debug(LPCSTR szFmt, ...);

public:
	int GetRetransMode();
	int setRetransMode(int iMode);
	int getTimeout();
	int setTimeout(int iTime);
	int setRetry(int iRetry);
	int getRetry();
	CsnmpBncs(void);
	~CsnmpBncs(void);
	int init_snmp(LPSTR szIpAdd,LPSTR szCommunity);
	int close_snmp(void);
	int SendSnmpMessage(int iOperation, char szOID[], LPSTR szData, int iDataType, UINT uMssgeNum);
	int GetValue(HSNMP_VBL vbl, int iEntry, int iSize, LPSTR lpString);
	int GetOid(HSNMP_VBL vbl, int iEntry, int iSize, LPSTR lpString);
	int CmpOid(HSNMP_VBL vbl, int iEntry, LPSTR lpString, LPINT result);
	int CmpValueOid(HSNMP_VBL vbl, int iEntry, LPSTR lpString, LPINT result);

	
	//********** do not use externally *********
	SNMPAPI_STATUS CALLBACK instance_callback(HSNMP_SESSION ,HWND , UINT ,WPARAM ,LPARAM);

};




