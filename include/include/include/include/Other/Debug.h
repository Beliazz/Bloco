#ifndef DEBUG_H
#define DEBUG_H

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifdef _DEBUG
	#define DEBUG_CLIENTBLOCK new( _CLIENT_BLOCK, __FILE__, __LINE__)
#else
	#define DEBUG_CLIENTBLOCK new
#endif

#endif