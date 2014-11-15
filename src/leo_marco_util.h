#ifndef __LEO_MARCO_UTIL_H__
#define __LEO_MARCO_UTIL_H__

#include "leo_utilities.h"


/* �������鳤�� */
template <int N> 
struct temp_struct
{
  char size[N];
};
template <class T, int N>
temp_struct<N> temp_struct_size(T (&)[N]);
#ifndef ARRAY_COUNT
#define ARRAY_COUNT(array) sizeof(temp_struct_size(array).size)
#endif

/* �ڴ氲ȫ�ͷ� */
#ifndef SAFE_DELETE
#  define SAFE_DELETE(p) { if((p) != NULL) { delete (p); (p)=NULL; } }
#endif

#ifndef SAFE_DELETE_ARRAY
#  define SAFE_DELETE_ARRAY(p) { if((p) != NULL) { delete[] (p); (p)=NULL; } }
#endif

#endif /* __LEO_MARCO_UTIL_H__ */
