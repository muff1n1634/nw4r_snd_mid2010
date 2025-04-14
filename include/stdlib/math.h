#ifndef __STDCXX_MATH_H__
#define __STDCXX_MATH_H__

#ifdef __cplusplus
	extern "C" {
#endif

extern double (pow)(double x, double y) __attribute__((nothrow));
extern float (powf)(float x, float y) __attribute__((nothrow));

#if __MWERKS__
# pragma cplusplus on
#endif

inline float (powf)(float x, float y) __attribute__((nothrow))
{
	return static_cast<float>(
		(::pow)(static_cast<double>(x), static_cast<double>(y)));
}

#if __MWERKS__
# pragma cplusplus reset
#endif

#ifdef __cplusplus
	}
#endif

#endif // __STDCXX_MATH_H__
