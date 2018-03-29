#ifndef CORE_UTILS
#define CORE_UTILS

#if (defined CORE_DEBUG) 
	#define COMMA ,
	#define LOG(x) OutputDebugStringA("\n"); OutputDebugStringA(x); OutputDebugStringA("\n");	
	#define LOG_STR(x) { char debugBuffer[100]; sprintf_s(debugBuffer, x);  LOG(debugBuffer) }
	#define LOG_STR_1(str, v1)			LOG_STR(str COMMA v1)
	#define LOG_STR_2(str, v1, v2)		LOG_STR(str COMMA v1 COMMA v2)
	#define LOG_STR_3(str, v1, v2, v3)	LOG_STR(str COMMA v1 COMMA v2 COMMA v3)

#else
	#define LOG(x)
	#define LOG_STR(x)
	#define LOG_STR_1(str, v1)			 
	#define LOG_STR_2(str, v1, v2)		 
	#define LOG_STR_3(str, v1, v2, v3)	 
#endif


#endif // !CORE_UTILS
