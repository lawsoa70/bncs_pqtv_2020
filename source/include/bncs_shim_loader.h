/****************************************************/
/* Written by David Yates                           */
/* Copyright Siemens IT Solutions and Services 2008 */
/****************************************************/
#ifndef BNCS_SHIM_LOADER_INCLUDED
	#define BNCS_SHIM_LOADER_INCLUDED

#ifdef WIN32
	#ifdef DOEXPORT_SHIMLOADER
		#define EXPORT_SHIMLOADER __declspec(dllexport) 
	#else
		#define EXPORT_SHIMLOADER __declspec(dllimport) 
	#endif
#else 
	#define EXPORT_SHIMLOADER 
#endif

class bncs_dll_client_shim;

class EXPORT_SHIMLOADER bncs_shim_loader
{
public:
	bncs_shim_loader();
	virtual ~bncs_shim_loader();
	bncs_dll_client_shim * getShim( void );
	static bncs_dll_client_shim * csiShim( void );
private:
	static int lock;
};

#endif
  