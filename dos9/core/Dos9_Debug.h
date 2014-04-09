#ifndef DOS9_DEBUG_H

#ifdef DOS9_DBG_MODE

    #define DOS9_DBG(format, ...) Dos9_ShowDebug(format, ##__VA_ARGS__ )

#else

    #define DOS9_DBG(format, ...)

#endif

void Dos9_ShowDebug(const char* format,...);

#endif // DOS9_DEBUG_H
