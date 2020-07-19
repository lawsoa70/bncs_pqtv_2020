// this slightly nasty bit of C-ism is to allow for the Qt cached storage of instance to be available to panel classes
// this is supported in the bncs_config dll/export lib

extern "C"
{
	bool __declspec(dllexport) instanceLookup( const char* instance, const char* param, bncs_string & altId, int * device, int * offset, int * slot, char *drivertype );
	bool __declspec(dllexport) instanceLookupComposite( const char * instance, const char * group, bncs_string & ret );
	bool __declspec(dllexport) instanceLookupType( const char * instance, bncs_string & ret );
}