#include "android/config.h"
#include "mad/mad.h"
#include "mad/stream.h"

//#define SHRT_MAX (32767)
#define INPUT_BUFFER_SIZE	(5*8192)
#define OUTPUT_BUFFER_SIZE	8192 /* Must be an integer multiple of 4. */

#define     PI          3.14159265358979
#define MYTEST 1

//void swap(float* a, float* b);
