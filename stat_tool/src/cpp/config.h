#if defined(_WIN32)

#if defined(VPTOOLS_NODLL)
#undef STAT_TOOL_MAKEDLL
#undef STAT_TOOL_DLL
#else
#ifndef STAT_TOOL_DLL
#define STAT_TOOL_DLL
#endif
#endif

#if defined(STAT_TOOL_MAKEDLL)
#ifndef STAT_TOOL_DLL
#define STAT_TOOL_DLL
#endif
#endif

#ifdef STAT_TOOL_DLL

#ifdef STAT_TOOL_MAKEDLL             /* create a vptool DLL library */
#define STAT_TOOL_API  __declspec(dllexport)
#undef VPTOOLS_FWDEF
#else                                                   /* use a vptool DLL library */
#define STAT_TOOL_API  __declspec(dllimport)
#endif

#define STAT_TOOL_TEMPLATE_API(T) template class STAT_TOOL_API T;
#endif

#else // OS != _WIN32

#undef STAT_TOOL_MAKEDLL             /* ignore these for other platforms */
#undef STAT_TOOL_DLL

#endif

#ifndef STAT_TOOL_API
#define STAT_TOOL_API
#define STAT_TOOL_TEMPLATE_API(T) 
#endif