#ifndef __LEO_UTILITIES_H__
#define __LEO_UTILITIES_H__

#if defined(WIN32) && defined(_BUILD_DLL)
# define LEO_EXPORT __declspec(dllexport)
#else
#  define LEO_EXPORT
#endif


#endif /* __LEO_UTILITIES_H__ */

