#ifndef MACROS_H
#define MACROS_H

// macro stuff

#define STR_(x)					#x
#define STR(x)					STR_(x)

#define CONCAT_(a, b)			a ## b
#define CONCAT(a, b)			CONCAT_(a, b)

// keywords

#ifndef alignas
# define alignas				ATTR_ALIGN
#endif

#ifndef decltype
# define decltype				__decltype__
#endif

#ifndef static_assert
# define static_assert			__static_assert
#endif

#ifndef typeof
# define typeof					__typeof__
#endif

// attributes

#define ATTR_ALIGN(x)			__attribute__((aligned(x)))
#define ATTR_FALLTHROUGH		/* no known attribute, but mwcc doesn't seem to care */
#define ATTR_UNUSED				__attribute__((unused))

#define ATTR_MAYBE_UNUSED		ATTR_UNUSED /* only used in e.g. asserts */

#if defined(__MWERKS__)
# define ATTR_ADDRESS(x)		: x
#else
# define ATTR_ADDRESS(x)
#endif

// helpers

#define ROUND_UP(x, align)		(((x) + ((align) - 1)) & -(align))

#define ARRAY_LENGTH(x)			(sizeof (x) / sizeof (x)[0])

#define BOOLIFY_TERNARY(expr_)	((expr_) ? 1 : 0)

/*******************************************************************************
 * NW4R-specific stuff
 */

#if defined(__cplusplus)

// Macro for for loops using NW4R iterators
# define NW4R_RANGE_FOR(it_, list_)	\
	for (decltype((list_).GetBeginIter()) (it_) = (list_).GetBeginIter(); (it_) != (list_).GetEndIter(); ++(it_))

/* This macro specifically is for the for loops which declare an
 * Iterator currIt = it++; in the body, so that it does not get incremented
 * twice.
 */
# define NW4R_RANGE_FOR_NO_AUTO_INC(it_, list_)	\
	for (decltype((list_).GetBeginIter()) (it_) = (list_).GetBeginIter(); (it_) != (list_).GetEndIter();)

#endif // defined(__cplusplus)

// NW4R constants

#if defined(CHAR_BIT)
# define CHAR_BIT_	CHAR_BIT
#else
# define CHAR_BIT_	8 // most common; default
#endif

// offset in bytes
#define NW4R_BYTE_(byte_, offset_)	\
	(static_cast<unsigned char>(byte_) << CHAR_BIT_ * (offset_))

#define NW4R_FILE_VERSION(MAJOR, MINOR)	\
	(NW4R_BYTE_(MAJOR, 1) | NW4R_BYTE_(MINOR, 0))

#define NW4R_FOUR_BYTE(A, B, C, D)	\
	(NW4R_BYTE_(A, 3) | NW4R_BYTE_(B, 2) | NW4R_BYTE_(C, 1) | NW4R_BYTE_(D, 0))

#endif // MACROS_H
