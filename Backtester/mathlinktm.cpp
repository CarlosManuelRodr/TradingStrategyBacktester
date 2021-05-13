/*
 * This file automatically produced by mprep.exe from:
 *	C:\Users\hp\Documents\Programacion\GrammarBasedTradingStrategyV2\Modules\Backtester\mathlink.tm
 * mprep Revision 19 Copyright (c) Wolfram Research, Inc. 1990-2020
 */

#define MPREP_REVISION 19


#include "mathlink.h"
#include <windows.h>


int MLAbort = 0;
int MLDone  = 0;
long MLSpecialCharacter = '\0';
HANDLE MLInstance = (HANDLE)0;
HWND MLIconWindow = (HWND)0;

MLINK stdlink = 0;
MLEnvironment stdenv = 0;
MLYieldFunctionObject stdyielder = (MLYieldFunctionObject)0;
MLMessageHandlerObject stdhandler = (MLMessageHandlerObject)0;

#if defined(__GNUC__)

#	ifdef TCHAR
#		undef TCHAR
#	endif
#	define TCHAR char

#	ifdef PTCHAR
#		undef PTCHAR
#	endif
#	define PTCHAR char *

#	ifdef __TEXT
#		undef __TEXT
#	endif
#	define __TEXT(arg) arg

#	ifdef _tcsrchr
#		undef _tcsrchr
#	endif
#	define _tcsrchr strrchr

#	ifdef _tcscat
#		undef _tcscat
#	endif
#	define _tcscat strcat

#	ifdef _tcsncpy
#		undef _tcsncpy
#	endif
#	define _tcsncpy _fstrncpy
#else
#	include <tchar.h>
#endif

#include <stdlib.h>
#include <string.h>
#if (WIN32_MATHLINK || WIN64_MATHLINK || __GNUC__) && !defined(_fstrncpy)
#       define _fstrncpy strncpy
#endif

#ifndef CALLBACK
#define CALLBACK FAR PASCAL
typedef LONG LRESULT;
typedef unsigned int UINT;
typedef WORD WPARAM;
typedef DWORD LPARAM;
#endif


LRESULT CALLBACK MLEXPORT
IconProcedure( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK MLEXPORT
IconProcedure( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch( msg){
	case WM_CLOSE:
		MLDone = 1;
		MLAbort = 1;
		break;
	case WM_QUERYOPEN:
		return 0;
	}
	return DefWindowProc( hWnd, msg, wParam, lParam);
}


#ifdef _UNICODE
#define _APISTR(i) L ## #i
#else
#define _APISTR(i) #i
#endif

#define APISTR(i) _APISTR(i)

HWND MLInitializeIcon( HINSTANCE hInstance, int nCmdShow)
{
	TCHAR path_name[260];
	PTCHAR icon_name;

	WNDCLASS  wc;
	HMODULE hdll;

	MLInstance = hInstance;
	if( ! nCmdShow) return (HWND)0;

	hdll = GetModuleHandle( __TEXT("ml32i" APISTR(MLINTERFACE)));

	(void)GetModuleFileName( hInstance, path_name, 260);

	icon_name = _tcsrchr( path_name, '\\') + 1;
	*_tcsrchr( icon_name, '.') = '\0';


	wc.style = 0;
	wc.lpfnWndProc = IconProcedure;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;

	if( hdll)
		wc.hIcon = LoadIcon( hdll, __TEXT("MLIcon"));
	else
		wc.hIcon = LoadIcon( NULL, IDI_APPLICATION);

	wc.hCursor = LoadCursor( NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)( COLOR_WINDOW + 1);
	wc.lpszMenuName =  NULL;
	wc.lpszClassName = __TEXT("mprepIcon");
	(void)RegisterClass( &wc);

	MLIconWindow = CreateWindow( __TEXT("mprepIcon"), icon_name,
			WS_OVERLAPPEDWINDOW | WS_MINIMIZE, CW_USEDEFAULT,
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
			(HWND)0, (HMENU)0, hInstance, (void FAR*)0);

	if( MLIconWindow){
		ShowWindow( MLIconWindow, SW_MINIMIZE);
		UpdateWindow( MLIconWindow);
	}
	return MLIconWindow;
}


MLYDEFN( int, MLDefaultYielder, ( MLINK mlp, MLYieldParameters yp))
{
	MSG msg;

	mlp = mlp; /* suppress unused warning */
	yp = yp; /* suppress unused warning */

	if( PeekMessage( &msg, (HWND)0, 0, 0, PM_REMOVE)){
		TranslateMessage( &msg);
		DispatchMessage( &msg);
	}
	return MLDone;
}


/********************************* end header *********************************/


# line 1 "C:\\Users\\hp\\Documents\\Programacion\\GrammarBasedTradingStrategyV2\\Modules\\Backtester\\mathlink.tm"
/****************************
*                           *
*    Dataset Management     *
*                           *
****************************/

# line 176 "C:\\Users\\hp\\Documents\\Programacion\\GrammarBasedTradingStrategyV2\\Modules\\Backtester\\mathlinktm.cpp"


# line 39 "C:\\Users\\hp\\Documents\\Programacion\\GrammarBasedTradingStrategyV2\\Modules\\Backtester\\mathlink.tm"
/****************************
*                           *
*     Dataset accesors      *
*                           *
****************************/

# line 186 "C:\\Users\\hp\\Documents\\Programacion\\GrammarBasedTradingStrategyV2\\Modules\\Backtester\\mathlinktm.cpp"


# line 93 "C:\\Users\\hp\\Documents\\Programacion\\GrammarBasedTradingStrategyV2\\Modules\\Backtester\\mathlink.tm"
/****************************
*                           *
*      Backtesting API      *
*                           *
****************************/

# line 196 "C:\\Users\\hp\\Documents\\Programacion\\GrammarBasedTradingStrategyV2\\Modules\\Backtester\\mathlinktm.cpp"


# line 107 "C:\\Users\\hp\\Documents\\Programacion\\GrammarBasedTradingStrategyV2\\Modules\\Backtester\\mathlink.tm"
/*************************************
*    get_strategy_execution_data     *
*************************************/

# line 204 "C:\\Users\\hp\\Documents\\Programacion\\GrammarBasedTradingStrategyV2\\Modules\\Backtester\\mathlinktm.cpp"


# line 135 "C:\\Users\\hp\\Documents\\Programacion\\GrammarBasedTradingStrategyV2\\Modules\\Backtester\\mathlink.tm"
/*********************
*    get_returns     *
*********************/

# line 212 "C:\\Users\\hp\\Documents\\Programacion\\GrammarBasedTradingStrategyV2\\Modules\\Backtester\\mathlinktm.cpp"


# line 163 "C:\\Users\\hp\\Documents\\Programacion\\GrammarBasedTradingStrategyV2\\Modules\\Backtester\\mathlink.tm"
/**********************************
*    get_returns_for_all_stocks   *
**********************************/

# line 220 "C:\\Users\\hp\\Documents\\Programacion\\GrammarBasedTradingStrategyV2\\Modules\\Backtester\\mathlinktm.cpp"


# line 191 "C:\\Users\\hp\\Documents\\Programacion\\GrammarBasedTradingStrategyV2\\Modules\\Backtester\\mathlink.tm"
/****************************
*                           *
*          Testing          *
*                           *
****************************/

# line 230 "C:\\Users\\hp\\Documents\\Programacion\\GrammarBasedTradingStrategyV2\\Modules\\Backtester\\mathlinktm.cpp"


void load_dataset_from_directory P(( const char * _tp1));

#if MLPROTOTYPES
static int _tr0( MLINK mlp)
#else
static int _tr0(mlp) MLINK mlp;
#endif
{
	int	res = 0;
	const char * _tp1;
	if ( ! MLGetString( mlp, &_tp1) ) goto L0;
	if ( ! MLNewPacket(mlp) ) goto L1;

	load_dataset_from_directory(_tp1);

	res = 1;
L1:	MLReleaseString(mlp, _tp1);

L0:	return res;
} /* _tr0 */


void unload_dataset P(( void));

#if MLPROTOTYPES
static int _tr1( MLINK mlp)
#else
static int _tr1(mlp) MLINK mlp;
#endif
{
	int	res = 0;
		if( !mlp) return res; /* avoid unused parameter warning */

	unload_dataset();

	res = 1;

	return res;
} /* _tr1 */


int get_number_of_loaded_stocks P(( void));

#if MLPROTOTYPES
static int _tr2( MLINK mlp)
#else
static int _tr2(mlp) MLINK mlp;
#endif
{
	int	res = 0;
		int _rp0;

	_rp0 = get_number_of_loaded_stocks();

	res = MLAbort ?
		MLPutFunction( mlp, "Abort", 0) : MLPutInteger( mlp, _rp0);

	return res;
} /* _tr2 */


void get_stock_names_of_dataset P(( void));

#if MLPROTOTYPES
static int _tr3( MLINK mlp)
#else
static int _tr3(mlp) MLINK mlp;
#endif
{
	int	res = 0;
		if( !mlp) return res; /* avoid unused parameter warning */

	get_stock_names_of_dataset();

	res = 1;

	return res;
} /* _tr3 */


void get_date P(( const char * _tp1, int _tp2));

#if MLPROTOTYPES
static int _tr4( MLINK mlp)
#else
static int _tr4(mlp) MLINK mlp;
#endif
{
	int	res = 0;
	const char * _tp1;
	int _tp2;
	if ( ! MLGetString( mlp, &_tp1) ) goto L0;
	if ( ! MLGetInteger( mlp, &_tp2) ) goto L1;
	if ( ! MLNewPacket(mlp) ) goto L2;

	get_date(_tp1, _tp2);

	res = 1;
L2: L1:	MLReleaseString(mlp, _tp1);

L0:	return res;
} /* _tr4 */


void get_dates_list P(( const char * _tp1));

#if MLPROTOTYPES
static int _tr5( MLINK mlp)
#else
static int _tr5(mlp) MLINK mlp;
#endif
{
	int	res = 0;
	const char * _tp1;
	if ( ! MLGetString( mlp, &_tp1) ) goto L0;
	if ( ! MLNewPacket(mlp) ) goto L1;

	get_dates_list(_tp1);

	res = 1;
L1:	MLReleaseString(mlp, _tp1);

L0:	return res;
} /* _tr5 */


void get_indicator P(( const char * _tp1, const char * _tp2, int _tp3));

#if MLPROTOTYPES
static int _tr6( MLINK mlp)
#else
static int _tr6(mlp) MLINK mlp;
#endif
{
	int	res = 0;
	const char * _tp1;
	const char * _tp2;
	int _tp3;
	if ( ! MLGetString( mlp, &_tp1) ) goto L0;
	if ( ! MLGetString( mlp, &_tp2) ) goto L1;
	if ( ! MLGetInteger( mlp, &_tp3) ) goto L2;
	if ( ! MLNewPacket(mlp) ) goto L3;

	get_indicator(_tp1, _tp2, _tp3);

	res = 1;
L3: L2:	MLReleaseString(mlp, _tp2);
L1:	MLReleaseString(mlp, _tp1);

L0:	return res;
} /* _tr6 */


void get_indicator_timeseries P(( const char * _tp1, const char * _tp2));

#if MLPROTOTYPES
static int _tr7( MLINK mlp)
#else
static int _tr7(mlp) MLINK mlp;
#endif
{
	int	res = 0;
	const char * _tp1;
	const char * _tp2;
	if ( ! MLGetString( mlp, &_tp1) ) goto L0;
	if ( ! MLGetString( mlp, &_tp2) ) goto L1;
	if ( ! MLNewPacket(mlp) ) goto L2;

	get_indicator_timeseries(_tp1, _tp2);

	res = 1;
L2:	MLReleaseString(mlp, _tp2);
L1:	MLReleaseString(mlp, _tp1);

L0:	return res;
} /* _tr7 */


void get_quantile_indicator P(( const char * _tp1, const char * _tp2, const char * _tp3, int _tp4));

#if MLPROTOTYPES
static int _tr8( MLINK mlp)
#else
static int _tr8(mlp) MLINK mlp;
#endif
{
	int	res = 0;
	const char * _tp1;
	const char * _tp2;
	const char * _tp3;
	int _tp4;
	if ( ! MLGetString( mlp, &_tp1) ) goto L0;
	if ( ! MLGetString( mlp, &_tp2) ) goto L1;
	if ( ! MLGetString( mlp, &_tp3) ) goto L2;
	if ( ! MLGetInteger( mlp, &_tp4) ) goto L3;
	if ( ! MLNewPacket(mlp) ) goto L4;

	get_quantile_indicator(_tp1, _tp2, _tp3, _tp4);

	res = 1;
L4: L3:	MLReleaseString(mlp, _tp3);
L2:	MLReleaseString(mlp, _tp2);
L1:	MLReleaseString(mlp, _tp1);

L0:	return res;
} /* _tr8 */


void get_quantile_indicator_timeseries P(( const char * _tp1, const char * _tp2, const char * _tp3));

#if MLPROTOTYPES
static int _tr9( MLINK mlp)
#else
static int _tr9(mlp) MLINK mlp;
#endif
{
	int	res = 0;
	const char * _tp1;
	const char * _tp2;
	const char * _tp3;
	if ( ! MLGetString( mlp, &_tp1) ) goto L0;
	if ( ! MLGetString( mlp, &_tp2) ) goto L1;
	if ( ! MLGetString( mlp, &_tp3) ) goto L2;
	if ( ! MLNewPacket(mlp) ) goto L3;

	get_quantile_indicator_timeseries(_tp1, _tp2, _tp3);

	res = 1;
L3:	MLReleaseString(mlp, _tp3);
L2:	MLReleaseString(mlp, _tp2);
L1:	MLReleaseString(mlp, _tp1);

L0:	return res;
} /* _tr9 */


void get_strategy_values P(( const char * _tp1, const char * _tp2));

#if MLPROTOTYPES
static int _tr10( MLINK mlp)
#else
static int _tr10(mlp) MLINK mlp;
#endif
{
	int	res = 0;
	const char * _tp1;
	const char * _tp2;
	if ( ! MLGetString( mlp, &_tp1) ) goto L0;
	if ( ! MLGetString( mlp, &_tp2) ) goto L1;
	if ( ! MLNewPacket(mlp) ) goto L2;

	get_strategy_values(_tp1, _tp2);

	res = 1;
L2:	MLReleaseString(mlp, _tp2);
L1:	MLReleaseString(mlp, _tp1);

L0:	return res;
} /* _tr10 */


void get_strategy_execution_data_stoploss_profittake P(( const char * _tp1, const char * _tp2, double _tp3, double _tp4, double _tp5));

#if MLPROTOTYPES
static int _tr11( MLINK mlp)
#else
static int _tr11(mlp) MLINK mlp;
#endif
{
	int	res = 0;
	const char * _tp1;
	const char * _tp2;
	double _tp3;
	double _tp4;
	double _tp5;
	if ( ! MLGetString( mlp, &_tp1) ) goto L0;
	if ( ! MLGetString( mlp, &_tp2) ) goto L1;
	if ( ! MLGetReal( mlp, &_tp3) ) goto L2;
	if ( ! MLGetReal( mlp, &_tp4) ) goto L3;
	if ( ! MLGetReal( mlp, &_tp5) ) goto L4;
	if ( ! MLNewPacket(mlp) ) goto L5;

	get_strategy_execution_data_stoploss_profittake(_tp1, _tp2, _tp3, _tp4, _tp5);

	res = 1;
L5: L4: L3: L2:	MLReleaseString(mlp, _tp2);
L1:	MLReleaseString(mlp, _tp1);

L0:	return res;
} /* _tr11 */


void get_strategy_execution_data_timestop_hit P(( const char * _tp1, const char * _tp2, int _tp3, double _tp4));

#if MLPROTOTYPES
static int _tr12( MLINK mlp)
#else
static int _tr12(mlp) MLINK mlp;
#endif
{
	int	res = 0;
	const char * _tp1;
	const char * _tp2;
	int _tp3;
	double _tp4;
	if ( ! MLGetString( mlp, &_tp1) ) goto L0;
	if ( ! MLGetString( mlp, &_tp2) ) goto L1;
	if ( ! MLGetInteger( mlp, &_tp3) ) goto L2;
	if ( ! MLGetReal( mlp, &_tp4) ) goto L3;
	if ( ! MLNewPacket(mlp) ) goto L4;

	get_strategy_execution_data_timestop_hit(_tp1, _tp2, _tp3, _tp4);

	res = 1;
L4: L3: L2:	MLReleaseString(mlp, _tp2);
L1:	MLReleaseString(mlp, _tp1);

L0:	return res;
} /* _tr12 */


void get_strategy_execution_data_markettiming P(( const char * _tp1, const char * _tp2, double _tp3));

#if MLPROTOTYPES
static int _tr13( MLINK mlp)
#else
static int _tr13(mlp) MLINK mlp;
#endif
{
	int	res = 0;
	const char * _tp1;
	const char * _tp2;
	double _tp3;
	if ( ! MLGetString( mlp, &_tp1) ) goto L0;
	if ( ! MLGetString( mlp, &_tp2) ) goto L1;
	if ( ! MLGetReal( mlp, &_tp3) ) goto L2;
	if ( ! MLNewPacket(mlp) ) goto L3;

	get_strategy_execution_data_markettiming(_tp1, _tp2, _tp3);

	res = 1;
L3: L2:	MLReleaseString(mlp, _tp2);
L1:	MLReleaseString(mlp, _tp1);

L0:	return res;
} /* _tr13 */


void get_returns_stoploss_profittake P(( const char * _tp1, const char * _tp2, const char * _tp3, double _tp4, double _tp5, double _tp6, int _tp7));

#if MLPROTOTYPES
static int _tr14( MLINK mlp)
#else
static int _tr14(mlp) MLINK mlp;
#endif
{
	int	res = 0;
	const char * _tp1;
	const char * _tp2;
	const char * _tp3;
	double _tp4;
	double _tp5;
	double _tp6;
	int _tp7;
	if ( ! MLGetString( mlp, &_tp1) ) goto L0;
	if ( ! MLGetString( mlp, &_tp2) ) goto L1;
	if ( ! MLGetString( mlp, &_tp3) ) goto L2;
	if ( ! MLGetReal( mlp, &_tp4) ) goto L3;
	if ( ! MLGetReal( mlp, &_tp5) ) goto L4;
	if ( ! MLGetReal( mlp, &_tp6) ) goto L5;
	if ( ! MLGetInteger( mlp, &_tp7) ) goto L6;
	if ( ! MLNewPacket(mlp) ) goto L7;

	get_returns_stoploss_profittake(_tp1, _tp2, _tp3, _tp4, _tp5, _tp6, _tp7);

	res = 1;
L7: L6: L5: L4: L3:	MLReleaseString(mlp, _tp3);
L2:	MLReleaseString(mlp, _tp2);
L1:	MLReleaseString(mlp, _tp1);

L0:	return res;
} /* _tr14 */


void get_returns_timestop_hit P(( const char * _tp1, const char * _tp2, const char * _tp3, int _tp4, double _tp5, int _tp6));

#if MLPROTOTYPES
static int _tr15( MLINK mlp)
#else
static int _tr15(mlp) MLINK mlp;
#endif
{
	int	res = 0;
	const char * _tp1;
	const char * _tp2;
	const char * _tp3;
	int _tp4;
	double _tp5;
	int _tp6;
	if ( ! MLGetString( mlp, &_tp1) ) goto L0;
	if ( ! MLGetString( mlp, &_tp2) ) goto L1;
	if ( ! MLGetString( mlp, &_tp3) ) goto L2;
	if ( ! MLGetInteger( mlp, &_tp4) ) goto L3;
	if ( ! MLGetReal( mlp, &_tp5) ) goto L4;
	if ( ! MLGetInteger( mlp, &_tp6) ) goto L5;
	if ( ! MLNewPacket(mlp) ) goto L6;

	get_returns_timestop_hit(_tp1, _tp2, _tp3, _tp4, _tp5, _tp6);

	res = 1;
L6: L5: L4: L3:	MLReleaseString(mlp, _tp3);
L2:	MLReleaseString(mlp, _tp2);
L1:	MLReleaseString(mlp, _tp1);

L0:	return res;
} /* _tr15 */


void get_returns_markettiming P(( const char * _tp1, const char * _tp2, const char * _tp3, double _tp4, int _tp5));

#if MLPROTOTYPES
static int _tr16( MLINK mlp)
#else
static int _tr16(mlp) MLINK mlp;
#endif
{
	int	res = 0;
	const char * _tp1;
	const char * _tp2;
	const char * _tp3;
	double _tp4;
	int _tp5;
	if ( ! MLGetString( mlp, &_tp1) ) goto L0;
	if ( ! MLGetString( mlp, &_tp2) ) goto L1;
	if ( ! MLGetString( mlp, &_tp3) ) goto L2;
	if ( ! MLGetReal( mlp, &_tp4) ) goto L3;
	if ( ! MLGetInteger( mlp, &_tp5) ) goto L4;
	if ( ! MLNewPacket(mlp) ) goto L5;

	get_returns_markettiming(_tp1, _tp2, _tp3, _tp4, _tp5);

	res = 1;
L5: L4: L3:	MLReleaseString(mlp, _tp3);
L2:	MLReleaseString(mlp, _tp2);
L1:	MLReleaseString(mlp, _tp1);

L0:	return res;
} /* _tr16 */


void get_returns_for_all_stocks_stoploss_profittake P(( const char * _tp1, const char * _tp2, double _tp3, double _tp4, double _tp5, int _tp6));

#if MLPROTOTYPES
static int _tr17( MLINK mlp)
#else
static int _tr17(mlp) MLINK mlp;
#endif
{
	int	res = 0;
	const char * _tp1;
	const char * _tp2;
	double _tp3;
	double _tp4;
	double _tp5;
	int _tp6;
	if ( ! MLGetString( mlp, &_tp1) ) goto L0;
	if ( ! MLGetString( mlp, &_tp2) ) goto L1;
	if ( ! MLGetReal( mlp, &_tp3) ) goto L2;
	if ( ! MLGetReal( mlp, &_tp4) ) goto L3;
	if ( ! MLGetReal( mlp, &_tp5) ) goto L4;
	if ( ! MLGetInteger( mlp, &_tp6) ) goto L5;
	if ( ! MLNewPacket(mlp) ) goto L6;

	get_returns_for_all_stocks_stoploss_profittake(_tp1, _tp2, _tp3, _tp4, _tp5, _tp6);

	res = 1;
L6: L5: L4: L3: L2:	MLReleaseString(mlp, _tp2);
L1:	MLReleaseString(mlp, _tp1);

L0:	return res;
} /* _tr17 */


void get_returns_for_all_stocks_timestop_hit P(( const char * _tp1, const char * _tp2, int _tp3, double _tp4, int _tp5));

#if MLPROTOTYPES
static int _tr18( MLINK mlp)
#else
static int _tr18(mlp) MLINK mlp;
#endif
{
	int	res = 0;
	const char * _tp1;
	const char * _tp2;
	int _tp3;
	double _tp4;
	int _tp5;
	if ( ! MLGetString( mlp, &_tp1) ) goto L0;
	if ( ! MLGetString( mlp, &_tp2) ) goto L1;
	if ( ! MLGetInteger( mlp, &_tp3) ) goto L2;
	if ( ! MLGetReal( mlp, &_tp4) ) goto L3;
	if ( ! MLGetInteger( mlp, &_tp5) ) goto L4;
	if ( ! MLNewPacket(mlp) ) goto L5;

	get_returns_for_all_stocks_timestop_hit(_tp1, _tp2, _tp3, _tp4, _tp5);

	res = 1;
L5: L4: L3: L2:	MLReleaseString(mlp, _tp2);
L1:	MLReleaseString(mlp, _tp1);

L0:	return res;
} /* _tr18 */


void get_returns_for_all_stocks_markettiming P(( const char * _tp1, const char * _tp2, double _tp3, int _tp4));

#if MLPROTOTYPES
static int _tr19( MLINK mlp)
#else
static int _tr19(mlp) MLINK mlp;
#endif
{
	int	res = 0;
	const char * _tp1;
	const char * _tp2;
	double _tp3;
	int _tp4;
	if ( ! MLGetString( mlp, &_tp1) ) goto L0;
	if ( ! MLGetString( mlp, &_tp2) ) goto L1;
	if ( ! MLGetReal( mlp, &_tp3) ) goto L2;
	if ( ! MLGetInteger( mlp, &_tp4) ) goto L3;
	if ( ! MLNewPacket(mlp) ) goto L4;

	get_returns_for_all_stocks_markettiming(_tp1, _tp2, _tp3, _tp4);

	res = 1;
L4: L3: L2:	MLReleaseString(mlp, _tp2);
L1:	MLReleaseString(mlp, _tp1);

L0:	return res;
} /* _tr19 */


void strategy_compilation_info P(( const char * _tp1));

#if MLPROTOTYPES
static int _tr20( MLINK mlp)
#else
static int _tr20(mlp) MLINK mlp;
#endif
{
	int	res = 0;
	const char * _tp1;
	if ( ! MLGetString( mlp, &_tp1) ) goto L0;
	if ( ! MLNewPacket(mlp) ) goto L1;

	strategy_compilation_info(_tp1);

	res = 1;
L1:	MLReleaseString(mlp, _tp1);

L0:	return res;
} /* _tr20 */


static struct func {
	int   f_nargs;
	int   manual;
	int   (*f_func)P((MLINK));
	const char  *f_name;
	} _tramps[21] = {
		{ 1, 0, _tr0, "load_dataset_from_directory" },
		{-1, 1, _tr1, "unload_dataset" },
		{-1, 1, _tr2, "get_number_of_loaded_stocks" },
		{-1, 1, _tr3, "get_stock_names_of_dataset" },
		{ 2, 0, _tr4, "get_date" },
		{ 1, 0, _tr5, "get_dates_list" },
		{ 3, 0, _tr6, "get_indicator" },
		{ 2, 0, _tr7, "get_indicator_timeseries" },
		{ 4, 0, _tr8, "get_quantile_indicator" },
		{ 3, 0, _tr9, "get_quantile_indicator_timeseries" },
		{ 2, 0, _tr10, "get_strategy_values" },
		{ 5, 0, _tr11, "get_strategy_execution_data_stoploss_profittake" },
		{ 4, 0, _tr12, "get_strategy_execution_data_timestop_hit" },
		{ 3, 0, _tr13, "get_strategy_execution_data_markettiming" },
		{ 7, 0, _tr14, "get_returns_stoploss_profittake" },
		{ 6, 0, _tr15, "get_returns_timestop_hit" },
		{ 5, 0, _tr16, "get_returns_markettiming" },
		{ 6, 0, _tr17, "get_returns_for_all_stocks_stoploss_profittake" },
		{ 5, 0, _tr18, "get_returns_for_all_stocks_timestop_hit" },
		{ 4, 0, _tr19, "get_returns_for_all_stocks_markettiming" },
		{ 1, 0, _tr20, "strategy_compilation_info" }
		};

#define CARDOF_EVALSTRS 0

static int _definepattern P(( MLINK, char*, char*, int));

int  _MLDoCallPacket P(( MLINK, struct func[], int));


#if MLPROTOTYPES
int MLInstall( MLINK mlp)
#else
int MLInstall(mlp) MLINK mlp;
#endif
{
	int _res;
	_res = MLConnect(mlp);
	if (_res) _res = _definepattern(mlp, (char *)"BTLoadDataset[path_String]", (char *)"{ path }", 0);
	if (_res) _res = _definepattern(mlp, (char *)"BTUnloadDataset[]", (char *)"Manual", 1);
	if (_res) _res = _definepattern(mlp, (char *)"BTGetNumberOfLoadedStocks[]", (char *)"Manual", 2);
	if (_res) _res = _definepattern(mlp, (char *)"BTGetStockNamesOfDataset[]", (char *)"Manual", 3);
	if (_res) _res = _definepattern(mlp, (char *)"BTGetDate[stock_String, time_Integer]", (char *)"{ stock, time }", 4);
	if (_res) _res = _definepattern(mlp, (char *)"BTGetDates[stock_String]", (char *)"{ stock }", 5);
	if (_res) _res = _definepattern(mlp, (char *)"BTGetIndicator[indicator_String, stock_String, time_Integer]", (char *)"{ indicator, stock, time }", 6);
	if (_res) _res = _definepattern(mlp, (char *)"BTGetIndicatorTimeSeries[indicator_String, stock_String]", (char *)"{ indicator, stock }", 7);
	if (_res) _res = _definepattern(mlp, (char *)"BTGetQuantileIndicator[indicator_String, percentile_String, stock_String, time_Integer]", (char *)"{ indicator, percentile, stock, time }", 8);
	if (_res) _res = _definepattern(mlp, (char *)"BTGetQuantileIndicatorTimeSeries[indicator_String, percentile_String, stock_String]", (char *)"{ indicator, percentile, stock }", 9);
	if (_res) _res = _definepattern(mlp, (char *)"BTGetStrategyValues[strategy_String, stock_String]", (char *)"{ strategy, stock }", 10);
	if (_res) _res = _definepattern(mlp, (char *)"BTGetStrategyExecutionDataStoplossProfitTake[strategy_String, stock_String, profitTake_Real, stopLoss_Real, transactionCost_Real]", (char *)"{ strategy, stock, profitTake, stopLoss, transactionCost }", 11);
	if (_res) _res = _definepattern(mlp, (char *)"BTGetStrategyExecutionDataTimestopHit[strategy_String, stock_String, timePeriod_Integer, transactionCost_Real]", (char *)"{ strategy, stock, timePeriod, transactionCost }", 12);
	if (_res) _res = _definepattern(mlp, (char *)"BTGetStrategyExecutionDataMarketTiming[strategy_String, stock_String, transactionCost_Real]", (char *)"{ strategy, stock, transactionCost }", 13);
	if (_res) _res = _definepattern(mlp, (char *)"BTGetStrategyReturnsStoplossProfitTake[returnType_String, strategy_String, stock_String, profitTake_Real, stopLoss_Real, transactionCost_Real, minibatchSize_Integer: -1]", (char *)"{ returnType, strategy, stock, profitTake, stopLoss, transactionCost, minibatchSize }", 14);
	if (_res) _res = _definepattern(mlp, (char *)"BTGetStrategyReturnsTimestopHit[returnType_String, strategy_String, stock_String, timePeriod_Integer, transactionCost_Real, minibatchSize_Integer: -1]", (char *)"{ returnType, strategy, stock, profitTake, stopLoss, transactionCost, minibatchSize }", 15);
	if (_res) _res = _definepattern(mlp, (char *)"BTGetStrategyReturnsMarketTiming[returnType_String, strategy_String, stock_String, transactionCost_Real, minibatchSize_Integer: -1]", (char *)"{ returnType, strategy, stock, transactionCost, minibatchSize }", 16);
	if (_res) _res = _definepattern(mlp, (char *)"BTGetStrategyReturnsAllStocksStoplossProfitTake[returnType_String, strategy_String, profitTake_Real, stopLoss_Real, transactionCost_Real, minibatchSize_Integer: -1]", (char *)"{ returnType, strategy, profitTake, stopLoss, transactionCost, minibatchSize }", 17);
	if (_res) _res = _definepattern(mlp, (char *)"BTGetStrategyReturnsAllStocksTimestopHit[returnType_String, strategy_String, timePeriod_Integer, transactionCost_Real, minibatchSize_Integer: -1]", (char *)"{ returnType, strategy, timePeriod, transactionCost, minibatchSize }", 18);
	if (_res) _res = _definepattern(mlp, (char *)"BTGetStrategyReturnsAllStocksMarketTiming[returnType_String, strategy_String, transactionCost_Real, minibatchSize_Integer: -1]", (char *)"{ returnType, strategy, transactionCost, minibatchSize }", 19);
	if (_res) _res = _definepattern(mlp, (char *)"BTStrategyCompilationInfo[strategy_String]", (char *)"{ strategy }", 20);
	if (_res) _res = MLPutSymbol( mlp, "End");
	if (_res) _res = MLFlush( mlp);
	return _res;
} /* MLInstall */


#if MLPROTOTYPES
int MLDoCallPacket( MLINK mlp)
#else
int MLDoCallPacket( mlp) MLINK mlp;
#endif
{
	return _MLDoCallPacket( mlp, _tramps, 21);
} /* MLDoCallPacket */

/******************************* begin trailer ********************************/

#ifndef EVALSTRS_AS_BYTESTRINGS
#	define EVALSTRS_AS_BYTESTRINGS 1
#endif

#if CARDOF_EVALSTRS
static int  _doevalstr( MLINK mlp, int n)
{
	long bytesleft, charsleft, bytesnow;
#if !EVALSTRS_AS_BYTESTRINGS
	long charsnow;
#endif
	char **s, **p;
	char *t;

	s = (char **)evalstrs;
	while( n-- > 0){
		if( *s == 0) break;
		while( *s++ != 0){}
	}
	if( *s == 0) return 0;
	bytesleft = 0;
	charsleft = 0;
	p = s;
	while( *p){
		t = *p; while( *t) ++t;
		bytesnow = (long)(t - *p);
		bytesleft += bytesnow;
		charsleft += bytesnow;
#if !EVALSTRS_AS_BYTESTRINGS
		t = *p;
		charsleft -= MLCharacterOffset( &t, t + bytesnow, bytesnow);
		/* assert( t == *p + bytesnow); */
#endif
		++p;
	}


	MLPutNext( mlp, MLTKSTR);
#if EVALSTRS_AS_BYTESTRINGS
	p = s;
	while( *p){
		t = *p; while( *t) ++t;
		bytesnow = (long)(t - *p);
		bytesleft -= bytesnow;
		MLPut8BitCharacters( mlp, bytesleft, (unsigned char*)*p, bytesnow);
		++p;
	}
#else
	MLPut7BitCount( mlp, (long_st)charsleft, (long_st)bytesleft);

	p = s;
	while( *p){
		t = *p; while( *t) ++t;
		bytesnow = t - *p;
		bytesleft -= bytesnow;
		t = *p;
		charsnow = bytesnow - MLCharacterOffset( &t, t + bytesnow, bytesnow);
		/* assert( t == *p + bytesnow); */
		charsleft -= charsnow;
		MLPut7BitCharacters(  mlp, charsleft, *p, bytesnow, charsnow);
		++p;
	}
#endif
	return MLError( mlp) == MLEOK;
}
#endif /* CARDOF_EVALSTRS */


static int  _definepattern( MLINK mlp, char *patt, char *args, int func_n)
{
	MLPutFunction( mlp, "DefineExternal", (long)3);
	  MLPutString( mlp, patt);
	  MLPutString( mlp, args);
	  MLPutInteger( mlp, func_n);
	return !MLError(mlp);
} /* _definepattern */


int _MLDoCallPacket( MLINK mlp, struct func functable[], int nfuncs)
{
#if MLINTERFACE >= 4
	int len;
#else
	long len;
#endif
	int n, res = 0;
	struct func* funcp;

	if( ! MLGetInteger( mlp, &n) ||  n < 0 ||  n >= nfuncs) goto L0;
	funcp = &functable[n];

	if( funcp->f_nargs >= 0
#if MLINTERFACE >= 4
	&& ( ! MLTestHead(mlp, "List", &len)
#else
	&& ( ! MLCheckFunction(mlp, "List", &len)
#endif
	     || ( !funcp->manual && (len != funcp->f_nargs))
	     || (  funcp->manual && (len <  funcp->f_nargs))
	   )
	) goto L0;

	stdlink = mlp;
	res = (*funcp->f_func)( mlp);

L0:	if( res == 0)
		res = MLClearError( mlp) && MLPutSymbol( mlp, "$Failed");
	return res && MLEndPacket( mlp) && MLNewPacket( mlp);
} /* _MLDoCallPacket */


mlapi_packet MLAnswer( MLINK mlp)
{
	mlapi_packet pkt = 0;
#if MLINTERFACE >= 4
	int waitResult;

	while( ! MLDone && ! MLError(mlp)
		&& (waitResult = MLWaitForLinkActivity(mlp),waitResult) &&
		waitResult == MLWAITSUCCESS && (pkt = MLNextPacket(mlp), pkt) &&
		pkt == CALLPKT)
	{
		MLAbort = 0;
		if(! MLDoCallPacket(mlp))
			pkt = 0;
	}
#else
	while( !MLDone && !MLError(mlp) && (pkt = MLNextPacket(mlp), pkt) && pkt == CALLPKT){
		MLAbort = 0;
		if( !MLDoCallPacket(mlp)) pkt = 0;
	}
#endif
	MLAbort = 0;
	return pkt;
}



/*
	Module[ { me = $ParentLink},
		$ParentLink = contents of RESUMEPKT;
		Message[ MessageName[$ParentLink, "notfe"], me];
		me]
*/

static int refuse_to_be_a_frontend( MLINK mlp)
{
	int pkt;

	MLPutFunction( mlp, "EvaluatePacket", 1);
	  MLPutFunction( mlp, "Module", 2);
	    MLPutFunction( mlp, "List", 1);
		  MLPutFunction( mlp, "Set", 2);
		    MLPutSymbol( mlp, "me");
	        MLPutSymbol( mlp, "$ParentLink");
	  MLPutFunction( mlp, "CompoundExpression", 3);
	    MLPutFunction( mlp, "Set", 2);
	      MLPutSymbol( mlp, "$ParentLink");
	      MLTransferExpression( mlp, mlp);
	    MLPutFunction( mlp, "Message", 2);
	      MLPutFunction( mlp, "MessageName", 2);
	        MLPutSymbol( mlp, "$ParentLink");
	        MLPutString( mlp, "notfe");
	      MLPutSymbol( mlp, "me");
	    MLPutSymbol( mlp, "me");
	MLEndPacket( mlp);

	while( (pkt = MLNextPacket( mlp), pkt) && pkt != SUSPENDPKT)
		MLNewPacket( mlp);
	MLNewPacket( mlp);
	return MLError( mlp) == MLEOK;
}


int MLEvaluate( MLINK mlp, char *s)
{
	if( MLAbort) return 0;
	return MLPutFunction( mlp, "EvaluatePacket", 1L)
		&& MLPutFunction( mlp, "ToExpression", 1L)
		&& MLPutString( mlp, s)
		&& MLEndPacket( mlp);
}


int MLEvaluateString( MLINK mlp, char *s)
{
	int pkt;
	if( MLAbort) return 0;
	if( MLEvaluate( mlp, s)){
		while( (pkt = MLAnswer( mlp), pkt) && pkt != RETURNPKT)
			MLNewPacket( mlp);
		MLNewPacket( mlp);
	}
	return MLError( mlp) == MLEOK;
} /* MLEvaluateString */


MLMDEFN( void, MLDefaultHandler, ( MLINK mlp, int message, int n))
{
	mlp = (MLINK)0; /* suppress unused warning */
	n = 0;          /* suppress unused warning */

	switch (message){
	case MLTerminateMessage:
		MLDone = 1;
	case MLInterruptMessage:
	case MLAbortMessage:
		MLAbort = 1;
	default:
		return;
	}
}



static int _MLMain( char **argv, char **argv_end, char *commandline)
{
	MLINK mlp;
	int err;

#if MLINTERFACE >= 4
	if( !stdenv)
		stdenv = MLInitialize( (MLEnvironmentParameter)0);
#else
	if( !stdenv)
		stdenv = MLInitialize( (MLParametersPointer)0);
#endif

	if( stdenv == (MLEnvironment)0) goto R0;

	if( !stdyielder)
		stdyielder = (MLYieldFunctionObject)MLDefaultYielder;


	if( !stdhandler)
		stdhandler = (MLMessageHandlerObject)MLDefaultHandler;


	mlp = commandline
		? MLOpenString( stdenv, commandline, &err)
		: MLOpenArgcArgv( stdenv, (int)(argv_end - argv), argv, &err);
	if( mlp == (MLINK)0){
		MLAlert( stdenv, MLErrorString( stdenv, err));
		goto R1;
	}

	if( MLIconWindow){
#define TEXTBUFLEN 64
		TCHAR textbuf[TEXTBUFLEN];
		PTCHAR tmlname;
		const char *mlname;
		size_t namelen, i;
		int len;
		len = GetWindowText(MLIconWindow, textbuf, 62 );
		mlname = MLName(mlp);
		namelen = strlen(mlname);
		tmlname = (PTCHAR)malloc((namelen + 1)*sizeof(TCHAR));
		if(tmlname == NULL) goto R2;

		for(i = 0; i < namelen; i++){
			tmlname[i] = mlname[i];
		}
		tmlname[namelen] = '\0';
		
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
		_tcscat_s( textbuf + len, TEXTBUFLEN - len, __TEXT("("));
		_tcsncpy_s(textbuf + len + 1, TEXTBUFLEN - len - 1, tmlname, TEXTBUFLEN - len - 3);
		textbuf[TEXTBUFLEN - 2] = '\0';
		_tcscat_s(textbuf, TEXTBUFLEN, __TEXT(")"));
#else
		_tcscat( textbuf + len, __TEXT("("));
		_tcsncpy( textbuf + len + 1, tmlname, TEXTBUFLEN - len - 3);
		textbuf[TEXTBUFLEN - 2] = '\0';
		_tcscat( textbuf, __TEXT(")"));
#endif
		textbuf[len + namelen + 2] = '\0';
		free(tmlname);
		SetWindowText( MLIconWindow, textbuf);
	}

	if( MLInstance){
		if( stdyielder) MLSetYieldFunction( mlp, stdyielder);
		if( stdhandler) MLSetMessageHandler( mlp, stdhandler);
	}

	if( MLInstall( mlp))
		while( MLAnswer( mlp) == RESUMEPKT){
			if( ! refuse_to_be_a_frontend( mlp)) break;
		}

R2:	MLClose( mlp);
R1:	MLDeinitialize( stdenv);
	stdenv = (MLEnvironment)0;
R0:	return !MLDone;
} /* _MLMain */


int MLMainString( char *commandline)
{
	return _MLMain( (char **)0, (char **)0, commandline);
}

int MLMainArgv( char** argv, char** argv_end) /* note not FAR pointers */
{   
	static char FAR * far_argv[128];
	int count = 0;
	
	while(argv < argv_end)
		far_argv[count++] = *argv++;
		 
	return _MLMain( far_argv, far_argv + count, (char *)0);

}

int MLMain( int argc, char **argv)
{
 	return _MLMain( argv, argv + argc, (char *)0);
}
 
