#ifndef __LEO_PROTOCOLS_H__
#define __LEO_PROTOCOLS_H__

#if defined(WIN32) && defined(_BUILD_DLL)
# define LEO_EXPORT __declspec(dllexport)
#else
# define LEO_EXPORT
#endif

#endif /* __LEO_PROTOCOLS_H__ */
