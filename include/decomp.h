#ifndef DECOMP_H
#define DECOMP_H

/*******************************************************************************
 * headers
 */

#include <macros.h>

/*******************************************************************************
 * Macro helpers
 */

#define DF_SWALLOW_SEMICOLON()	static_assert(1, "")
#define DF_TYPEOF				typeof

#define DF_CONCAT3_(a, b, c)	a ## b ## c
#define DF_CONCAT3(a, b, c)		DF_CONCAT3_(a, b, c)

#define DF_UNIQUE_IDENT(ident_)	DF_CONCAT3(ident_, _, __LINE__)

/*******************************************************************************
 * DECOMP_FORCE macro internals
 */

#define DF_FUNCTION_DECLARATOR_WITH_PROTO(ident_)	\
	extern void (ident_)(void);			\
	void (ident_)(void)

// this is done to prevent default promotion of arguments to variadic functions
#define DF_FUNCTION_CALL(ident_, arg_)		\
	extern void (ident_)(DF_TYPEOF(arg_));	\
	(ident_)(arg_)

/*******************************************************************************
 * DECOMP_FORCE macros
 */

/* Use for orphaned data, early references, or other shenanigans.
 * Only works at file or namespace scope.
 *
 * Examples:
 * DECOMP_FORCE(1.0f);
 * DECOMP_FORCE("I am a string");
 * DECOMP_FORCE(UI2D_CONSTANT);
 */

#define DECOMP_FORCE(arg_)												\
	DF_FUNCTION_DECLARATOR_WITH_PROTO(DF_UNIQUE_IDENT(DECOMP_FORCE))	\
	{																	\
		DF_FUNCTION_CALL(DF_UNIQUE_IDENT(DECOMP_FORCE_CALL), arg_);		\
	}																	\
	DF_SWALLOW_SEMICOLON()

/* Use for forcing when the syntax to instantiate the unused data is more
 * complicated and doesn't neatly fit into a DECOMP_FORCE call.
 * Only works at file or namespace scope.
 *
 * Examples:
 * DECOMP_FORCE_EX()
 * {
 *     (void)(new SomeClass)->~SomeClass();
 * }
 */

#define DECOMP_FORCE_EX()	\
	DF_FUNCTION_DECLARATOR_WITH_PROTO(DF_UNIQUE_IDENT(DECOMP_FORCE_EX))

/*******************************************************************************
 * DECOMP_FORCE helpers
 */

// Conversion constants from integer to floating-point

extern   signed int DECOMP_SI;
extern unsigned int DECOMP_UI;

#define SI2D_CONSTANT	((float)(DECOMP_SI))
#define UI2D_CONSTANT	((float)(DECOMP_UI))

// clang-format on

#endif // DECOMP_H
