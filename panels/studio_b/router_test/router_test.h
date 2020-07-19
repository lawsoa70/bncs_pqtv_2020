#ifndef router_test_INCLUDED
	#define router_test_INCLUDED

#include <bncs_script_helper.h>

#ifdef WIN32
	#ifdef DOEXPORT_SCRIPT
		#define EXPORT_SCRIPT __declspec(dllexport) 
	#else
		#define EXPORT_SCRIPT 
	#endif
#else 
	#define EXPORT_SCRIPT
#endif
 
class router_test : public bncs_script_helper
{
public:
	router_test( bncs_client_callback * parent, const char* path );
	virtual ~router_test();

	void buttonCallback( buttonNotify *b );
	int revertiveCallback( revertiveNotify * r );
	void databaseCallback( revertiveNotify * r );
	bncs_string parentCallback( parentNotify *p );
	void timerCallback( int );
	
private:
	bncs_string m_myParam;
	bncs_string m_instance;
	bncs_stringlist m_sl_src_range; // comma sep list of discontinuous ranges of sources
	bncs_stringlist m_sl_dest_range; // comma sep list of discontinuous ranges of destinations
	int m_int_src_from;
	int m_int_src_to;
	int m_int_dest_from;
	int m_int_dest_to;
	int m_int_current_src;
	int m_int_router_device;
	bncs_string m_str_flags;
	int m_int_prog_bar_x;
	int m_int_prog_bar_y;
	int m_int_prog_bar_width;
	bool m_bool_busy;
	bool m_bool_timed_out;
	bool m_bool_auto;
	int m_int_keyboard_callback;

	//Internal methods
	//void handleSrcRangeChanged (bncs_stringlist sl);
	//void handleSrcFromChanged(bncs_string s);
	//void handleSrcToChanged(bncs_string s);
	//handleDestRangeChanged(bncs_stringlist sl);
	//void handleDestFromChanged(bncs_string s);
	//void handleDestToChanged(bncs_string s);
	void takeSrc(int src);
	int getFirst(bncs_stringlist list);
	int getLast(bncs_stringlist list);
	int getNext(int n, int from, int to, bncs_stringlist list);
	int isInRange(int n, int from, int to, bncs_stringlist list);
	void makeRoute(int src, int dest);
	void updateRegisterRevertives();
	int countOnes();
	void setBusyState(bool busy);
	void updateSrcRange(bncs_stringlist range);
	void updateSrcFrom(int from);
	void updateSrcTo(int to);
	void updateDestRange(bncs_stringlist range);
	void updateDestFrom(int from);
	void updateDestTo(int to);
	void openKeyboard(int variable, bncs_string text);
};


#endif // router_test_INCLUDED