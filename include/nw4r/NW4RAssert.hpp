// clang-format off

/*******************************************************************************
 * anti-header guard
 */

#ifdef __MWERKS__
# pragma notonce
#endif

/*******************************************************************************
 * NW4RAssert/NW4RCheck base macros
 */

// IWYU pragma: begin_exports

// <assert.h> behavior
#undef NW4RAssertMessage_FileLine
#undef NW4RCheckMessage_FileLine

#if defined(NW4R_ASSERT)
# define NW4RAssertMessage_FileLine(file_, line_, exp_, ...)	((void)((exp_) || (::nw4r::db::Panic  (file_, line_, __VA_ARGS__), 0)))
# define NW4RCheckMessage_FileLine(file_, line_, exp_, ...)		((void)((exp_) || (::nw4r::db::Warning(file_, line_, __VA_ARGS__), 0)))
#else
# define NW4RAssertMessage_FileLine(file_, line_, exp_, ...)	((void)(0))
# define NW4RCheckMessage_FileLine(file_, line_, exp_, ...)		((void)(0))
#endif // defined(NW4R_ASSERT)

/*******************************************************************************
 * Derived macros
 */

// NW4RAssert family
#define NW4RAssertMessage_File(file_, exp_, ...)		NW4RAssertMessage_FileLine(  file_ , __LINE__, exp_, __VA_ARGS__)
#define NW4RAssertMessage_Line(line_, exp_, ...)		NW4RAssertMessage_FileLine(__FILE__,   line_ , exp_, __VA_ARGS__)
#define NW4RAssertMessage(exp_, ...)					NW4RAssertMessage_FileLine(__FILE__, __LINE__, exp_, __VA_ARGS__)

#define NW4RAssert_FileLine(file_, line_, exp_)			NW4RAssertMessage_FileLine(  file_ ,   line_ , exp_, "NW4R:Failed assertion " #exp_)
#define NW4RAssert_File(file_, exp_)					NW4RAssertMessage_FileLine(  file_ , __LINE__, exp_, "NW4R:Failed assertion " #exp_)
#define NW4RAssert_Line(line_, exp_)					NW4RAssertMessage_FileLine(__FILE__,   line_ , exp_, "NW4R:Failed assertion " #exp_)
#define NW4RAssert(exp_)								NW4RAssertMessage_FileLine(__FILE__, __LINE__, exp_, "NW4R:Failed assertion " #exp_)

#define NW4RAssertHeader_FileLine(file_, line_, exp_)	NW4RAssertMessage_FileLine(  file_ ,   line_ , exp_,      "Failed assertion " #exp_)
#define NW4RAssertHeader_File(file_, exp_)				NW4RAssertMessage_FileLine(  file_ , __LINE__, exp_,      "Failed assertion " #exp_)
#define NW4RAssertHeader_Line(line_, exp_)				NW4RAssertMessage_FileLine(__FILE__,   line_ , exp_,      "Failed assertion " #exp_)
#define NW4RAssertHeader(exp_)							NW4RAssertMessage_FileLine(__FILE__, __LINE__, exp_,      "Failed assertion " #exp_)

// NW4RPanic family
#define NW4RPanicMessage_FileLine(file_, line_, ...)	NW4RAssertMessage_FileLine(  file_ ,   line_ , false, __VA_ARGS__)
#define NW4RPanicMessage_File(file_, ...)				NW4RAssertMessage_FileLine(  file_ , __LINE__, false, __VA_ARGS__)
#define NW4RPanicMessage_Line(line_, ...)				NW4RAssertMessage_FileLine(__FILE__,   line_ , false, __VA_ARGS__)
#define NW4RPanicMessage(...)							NW4RAssertMessage_FileLine(__FILE__, __LINE__, false, __VA_ARGS__)

#define NW4RPanic_FileLine(file_, line_)				NW4RAssert_FileLine(  file_ ,   line_ , false)
#define NW4RPanic_File(file_)							NW4RAssert_FileLine(  file_ , __LINE__, false)
#define NW4RPanic_Line(line_)							NW4RAssert_FileLine(__FILE__,   line_ , false)
#define NW4RPanic()										NW4RAssert_FileLine(__FILE__, __LINE__, false)

// NW4RCheck family
#define NW4RCheckMessage_File(file_, exp_, ...)			NW4RCheckMessage_FileLine(  file_ , __LINE__, exp_, __VA_ARGS__)
#define NW4RCheckMessage_Line(line_, exp_, ...)			NW4RCheckMessage_FileLine(__FILE__,   line_ , exp_, __VA_ARGS__)
#define NW4RCheckMessage(exp_, ...)						NW4RCheckMessage_FileLine(__FILE__, __LINE__, exp_, __VA_ARGS__)

#define NW4RCheck_FileLine(file_, line_, exp_)			NW4RCheckMessage_FileLine(  file_ ,   line_ , exp_, "NW4R:Failed check " #exp_)
#define NW4RCheck_File(file_, exp_)						NW4RCheckMessage_FileLine(  file_ , __LINE__, exp_, "NW4R:Failed check " #exp_)
#define NW4RCheck_Line(line_, exp_)						NW4RCheckMessage_FileLine(__FILE__,   line_ , exp_, "NW4R:Failed check " #exp_)
#define NW4RCheck(exp_)									NW4RCheckMessage_FileLine(__FILE__, __LINE__, exp_, "NW4R:Failed check " #exp_)

#define NW4RCheckHeader_FileLine(file_, line_, exp_)	NW4RCheckMessage_FileLine(  file_ ,   line_ , exp_,      "Failed check " #exp_)
#define NW4RCheckHeader_File(file_, exp_)				NW4RCheckMessage_FileLine(  file_ , __LINE__, exp_,      "Failed check " #exp_)
#define NW4RCheckHeader_Line(line_, exp_)				NW4RCheckMessage_FileLine(__FILE__,   line_ , exp_,      "Failed check " #exp_)
#define NW4RCheckHeader(exp_)							NW4RCheckMessage_FileLine(__FILE__, __LINE__, exp_,      "Failed check " #exp_)

// NW4RWarning family
#define NW4RWarningMessage_FileLine(file_, line_, ...)	NW4RCheckMessage_FileLine(  file_ ,   line_ , false, __VA_ARGS__)
#define NW4RWarningMessage_File(file_, ...)				NW4RCheckMessage_FileLine(  file_ , __LINE__, false, __VA_ARGS__)
#define NW4RWarningMessage_Line(line_, ...)				NW4RCheckMessage_FileLine(__FILE__,   line_ , false, __VA_ARGS__)
#define NW4RWarningMessage(...)							NW4RCheckMessage_FileLine(__FILE__, __LINE__, false, __VA_ARGS__)

#define NW4RWarning_FileLine(file_, line_)				NW4RCheck_FileLine(  file_ ,   line_ , false)
#define NW4RWarning_File(file_)							NW4RCheck_FileLine(  file_ , __LINE__, false)
#define NW4RWarning_Line(line_)							NW4RCheck_FileLine(__FILE__,   line_ , false)
#define NW4RWarning()									NW4RCheck_FileLine(__FILE__, __LINE__, false)

// Extended asserts

// PointerNonnull
#define NW4RAssertPointerNonnull_FileLine(file_, line_, ptr_)								NW4RAssertMessage_FileLine(  file_ ,   line_ , (ptr_) != 0, "NW4R:Pointer must not be NULL (" #ptr_ ")")
#define NW4RAssertPointerNonnull_File(file_, ptr_)											NW4RAssertMessage_FileLine(  file_ , __LINE__, (ptr_) != 0, "NW4R:Pointer must not be NULL (" #ptr_ ")")
#define NW4RAssertPointerNonnull_Line(line_, ptr_)											NW4RAssertMessage_FileLine(__FILE__,   line_ , (ptr_) != 0, "NW4R:Pointer must not be NULL (" #ptr_ ")")
#define NW4RAssertPointerNonnull(ptr_)														NW4RAssertMessage_FileLine(__FILE__, __LINE__, (ptr_) != 0, "NW4R:Pointer must not be NULL (" #ptr_ ")")

#define NW4RAssertHeaderPointerNonnull_FileLine(file_, line_, ptr_)							NW4RAssertMessage_FileLine(  file_ ,   line_ , (ptr_) != 0,      "Pointer must not be NULL (" #ptr_ ")")
#define NW4RAssertHeaderPointerNonnull_File(file_, ptr_)									NW4RAssertMessage_FileLine(  file_ , __LINE__, (ptr_) != 0,      "Pointer must not be NULL (" #ptr_ ")")
#define NW4RAssertHeaderPointerNonnull_Line(line_, ptr_)									NW4RAssertMessage_FileLine(__FILE__,   line_ , (ptr_) != 0,      "Pointer must not be NULL (" #ptr_ ")")
#define NW4RAssertHeaderPointerNonnull(ptr_)												NW4RAssertMessage_FileLine(__FILE__, __LINE__, (ptr_) != 0,      "Pointer must not be NULL (" #ptr_ ")")

// PointerValid
#define NW4R_POINTER_VALID_TEST_(ptr_)						\
	   (((unsigned long)(ptr_) & 0xff000000) == 0x80000000	\
	 || ((unsigned long)(ptr_) & 0xff800000) == 0x81000000	\
	 || ((unsigned long)(ptr_) & 0xf8000000) == 0x90000000	\
	 || ((unsigned long)(ptr_) & 0xff000000) == 0xc0000000	\
	 || ((unsigned long)(ptr_) & 0xff800000) == 0xc1000000	\
	 || ((unsigned long)(ptr_) & 0xf8000000) == 0xd0000000	\
	 || ((unsigned long)(ptr_) & 0xffffc000) == 0xe0000000)

#define NW4RAssertPointerValid_FileLine(file_, line_, ptr_)									NW4RAssertMessage_FileLine(  file_ ,   line_ , NW4R_POINTER_VALID_TEST_(ptr_), "NW4R:Pointer Error\n" #ptr_ "(=%p) is not valid pointer.", ptr_)
#define NW4RAssertPointerValid_File(file_, ptr_)											NW4RAssertMessage_FileLine(  file_ , __LINE__, NW4R_POINTER_VALID_TEST_(ptr_), "NW4R:Pointer Error\n" #ptr_ "(=%p) is not valid pointer.", ptr_)
#define NW4RAssertPointerValid_Line(line_, ptr_)											NW4RAssertMessage_FileLine(__FILE__,   line_ , NW4R_POINTER_VALID_TEST_(ptr_), "NW4R:Pointer Error\n" #ptr_ "(=%p) is not valid pointer.", ptr_)
#define NW4RAssertPointerValid(ptr_)														NW4RAssertMessage_FileLine(__FILE__, __LINE__, NW4R_POINTER_VALID_TEST_(ptr_), "NW4R:Pointer Error\n" #ptr_ "(=%p) is not valid pointer.", ptr_)

#define NW4RAssertHeaderPointerValid_FileLine(file_, line_, ptr_)							NW4RAssertMessage_FileLine(  file_ ,   line_ , NW4R_POINTER_VALID_TEST_(ptr_),      "Pointer Error\n" #ptr_ "(=%p) is not valid pointer.", ptr_)
#define NW4RAssertHeaderPointerValid_File(file_, ptr_)										NW4RAssertMessage_FileLine(  file_ , __LINE__, NW4R_POINTER_VALID_TEST_(ptr_),      "Pointer Error\n" #ptr_ "(=%p) is not valid pointer.", ptr_)
#define NW4RAssertHeaderPointerValid_Line(line_, ptr_)										NW4RAssertMessage_FileLine(__FILE__,   line_ , NW4R_POINTER_VALID_TEST_(ptr_),      "Pointer Error\n" #ptr_ "(=%p) is not valid pointer.", ptr_)
#define NW4RAssertHeaderPointerValid(ptr_)													NW4RAssertMessage_FileLine(__FILE__, __LINE__, NW4R_POINTER_VALID_TEST_(ptr_),      "Pointer Error\n" #ptr_ "(=%p) is not valid pointer.", ptr_)

// Aligned
#define NW4R_IS_ALIGNED_(x, align)	(((unsigned long)(x) & ((align) - 1)) == 0) // just redefine instead of pulling in <macros.h>

#define NW4RAssertAligned_FileLine(file_, line_, val_, align_)								NW4RAssertMessage_FileLine(  file_ ,   line_ , NW4R_IS_ALIGNED_(val_, align_), "NW4R:Alignment Error(0x%x)\n" #val_ " must be aligned to " #align_ " bytes boundary.", val_)
#define NW4RAssertAligned_File(file_, val_, align_)											NW4RAssertMessage_FileLine(  file_ , __LINE__, NW4R_IS_ALIGNED_(val_, align_), "NW4R:Alignment Error(0x%x)\n" #val_ " must be aligned to " #align_ " bytes boundary.", val_)
#define NW4RAssertAligned_Line(line_, val_, align_)											NW4RAssertMessage_FileLine(__FILE__,   line_ , NW4R_IS_ALIGNED_(val_, align_), "NW4R:Alignment Error(0x%x)\n" #val_ " must be aligned to " #align_ " bytes boundary.", val_)
#define NW4RAssertAligned(val_, align_)														NW4RAssertMessage_FileLine(__FILE__, __LINE__, NW4R_IS_ALIGNED_(val_, align_), "NW4R:Alignment Error(0x%x)\n" #val_ " must be aligned to " #align_ " bytes boundary.", val_)

#define NW4RAssertHeaderAligned_FileLine(file_, line_, val_, align_)						NW4RAssertMessage_FileLine(  file_ ,   line_ , NW4R_IS_ALIGNED_(val_, align_),      "Alignment Error(0x%x)\n" #val_ " must be aligned to " #align_ " bytes boundary.", val_)
#define NW4RAssertHeaderAligned_File(file_, val_, align_)									NW4RAssertMessage_FileLine(  file_ , __LINE__, NW4R_IS_ALIGNED_(val_, align_),      "Alignment Error(0x%x)\n" #val_ " must be aligned to " #align_ " bytes boundary.", val_)
#define NW4RAssertHeaderAligned_Line(line_, val_, align_)									NW4RAssertMessage_FileLine(__FILE__,   line_ , NW4R_IS_ALIGNED_(val_, align_),      "Alignment Error(0x%x)\n" #val_ " must be aligned to " #align_ " bytes boundary.", val_)
#define NW4RAssertHeaderAligned(val_, align_)												NW4RAssertMessage_FileLine(__FILE__, __LINE__, NW4R_IS_ALIGNED_(val_, align_),      "Alignment Error(0x%x)\n" #val_ " must be aligned to " #align_ " bytes boundary.", val_)

// MinimumValue
#define NW4RAssertMinimumValue_FileLine(file_, line_, var_, minValue_)						NW4RAssertMessage_FileLine(  file_ ,   line_ , (var_) >= (minValue_), "NW4R:" #var_ " is out of bounds(%d)\n%d <= " #var_ " not satisfied.", (int)(var_), (int)(minValue_))
#define NW4RAssertMinimumValue_File(file_, var_, minValue_)									NW4RAssertMessage_FileLine(  file_ , __LINE__, (var_) >= (minValue_), "NW4R:" #var_ " is out of bounds(%d)\n%d <= " #var_ " not satisfied.", (int)(var_), (int)(minValue_))
#define NW4RAssertMinimumValue_Line(line_, var_, minValue_)									NW4RAssertMessage_FileLine(__FILE__,   line_ , (var_) >= (minValue_), "NW4R:" #var_ " is out of bounds(%d)\n%d <= " #var_ " not satisfied.", (int)(var_), (int)(minValue_))
#define NW4RAssertMinimumValue(var_, minValue_)												NW4RAssertMessage_FileLine(__FILE__, __LINE__, (var_) >= (minValue_), "NW4R:" #var_ " is out of bounds(%d)\n%d <= " #var_ " not satisfied.", (int)(var_), (int)(minValue_))

#define NW4RAssertHeaderMinimumValue_FileLine(file_, line_, var_, minValue_)				NW4RAssertMessage_FileLine(  file_ ,   line_ , (var_) >= (minValue_),         #var_ " is out of bounds(%d)\n%d <= " #var_ " not satisfied.", (int)(var_), (int)(minValue_))
#define NW4RAssertHeaderMinimumValue_File(file_, var_, minValue_)							NW4RAssertMessage_FileLine(  file_ , __LINE__, (var_) >= (minValue_),         #var_ " is out of bounds(%d)\n%d <= " #var_ " not satisfied.", (int)(var_), (int)(minValue_))
#define NW4RAssertHeaderMinimumValue_Line(line_, var_, minValue_)							NW4RAssertMessage_FileLine(__FILE__,   line_ , (var_) >= (minValue_),         #var_ " is out of bounds(%d)\n%d <= " #var_ " not satisfied.", (int)(var_), (int)(minValue_))
#define NW4RAssertHeaderMinimumValue(var_, minValue_)										NW4RAssertMessage_FileLine(__FILE__, __LINE__, (var_) >= (minValue_),         #var_ " is out of bounds(%d)\n%d <= " #var_ " not satisfied.", (int)(var_), (int)(minValue_))

// ClampedLValue
#define NW4RAssertClampedLValue_FileLine(file_, line_, var_, minValue_, maxValue_)			NW4RAssertMessage_FileLine(  file_ ,   line_ , (var_) >= (minValue_) && (var_) <  (maxValue_), "NW4R:" #var_ " is out of bounds(%d)\n%d <= " #var_ " < %d not satisfied.", (int)(var_), (int)(minValue_), (int)(maxValue_))
#define NW4RAssertClampedLValue_File(file_, var_, minValue_, maxValue_)						NW4RAssertMessage_FileLine(  file_ , __LINE__, (var_) >= (minValue_) && (var_) <  (maxValue_), "NW4R:" #var_ " is out of bounds(%d)\n%d <= " #var_ " < %d not satisfied.", (int)(var_), (int)(minValue_), (int)(maxValue_))
#define NW4RAssertClampedLValue_Line(line_, var_, minValue_, maxValue_)						NW4RAssertMessage_FileLine(__FILE__,   line_ , (var_) >= (minValue_) && (var_) <  (maxValue_), "NW4R:" #var_ " is out of bounds(%d)\n%d <= " #var_ " < %d not satisfied.", (int)(var_), (int)(minValue_), (int)(maxValue_))
#define NW4RAssertClampedLValue(var_, minValue_, maxValue_)									NW4RAssertMessage_FileLine(__FILE__, __LINE__, (var_) >= (minValue_) && (var_) <  (maxValue_), "NW4R:" #var_ " is out of bounds(%d)\n%d <= " #var_ " < %d not satisfied.", (int)(var_), (int)(minValue_), (int)(maxValue_))

#define NW4RAssertHeaderClampedLValue_FileLine(file_, line_, var_, minValue_, maxValue_)	NW4RAssertMessage_FileLine(  file_ ,   line_ , (var_) >= (minValue_) && (var_) <  (maxValue_),         #var_ " is out of bounds(%d)\n%d <= " #var_ " < %d not satisfied.", (int)(var_), (int)(minValue_), (int)(maxValue_))
#define NW4RAssertHeaderClampedLValue_File(file_, var_, minValue_, maxValue_)				NW4RAssertMessage_FileLine(  file_ , __LINE__, (var_) >= (minValue_) && (var_) <  (maxValue_),         #var_ " is out of bounds(%d)\n%d <= " #var_ " < %d not satisfied.", (int)(var_), (int)(minValue_), (int)(maxValue_))
#define NW4RAssertHeaderClampedLValue_Line(line_, var_, minValue_, maxValue_)				NW4RAssertMessage_FileLine(__FILE__,   line_ , (var_) >= (minValue_) && (var_) <  (maxValue_),         #var_ " is out of bounds(%d)\n%d <= " #var_ " < %d not satisfied.", (int)(var_), (int)(minValue_), (int)(maxValue_))
#define NW4RAssertHeaderClampedLValue(var_, minValue_, maxValue_)							NW4RAssertMessage_FileLine(__FILE__, __LINE__, (var_) >= (minValue_) && (var_) <  (maxValue_),         #var_ " is out of bounds(%d)\n%d <= " #var_ " < %d not satisfied.", (int)(var_), (int)(minValue_), (int)(maxValue_))

// ClampedLRValue
#define NW4RAssertClampedLRValue_FileLine(file_, line_, var_, minValue_, maxValue_)			NW4RAssertMessage_FileLine(  file_ ,   line_ , (var_) >= (minValue_) && (var_) <= (maxValue_), "NW4R:" #var_ " is out of bounds(%d)\n%d <= " #var_ " <= %d not satisfied.", (int)(var_), (int)(minValue_), (int)(maxValue_))
#define NW4RAssertClampedLRValue_File(file_, var_, minValue_, maxValue_)					NW4RAssertMessage_FileLine(  file_ , __LINE__, (var_) >= (minValue_) && (var_) <= (maxValue_), "NW4R:" #var_ " is out of bounds(%d)\n%d <= " #var_ " <= %d not satisfied.", (int)(var_), (int)(minValue_), (int)(maxValue_))
#define NW4RAssertClampedLRValue_Line(line_, var_, minValue_, maxValue_)					NW4RAssertMessage_FileLine(__FILE__,   line_ , (var_) >= (minValue_) && (var_) <= (maxValue_), "NW4R:" #var_ " is out of bounds(%d)\n%d <= " #var_ " <= %d not satisfied.", (int)(var_), (int)(minValue_), (int)(maxValue_))
#define NW4RAssertClampedLRValue(var_, minValue_, maxValue_)								NW4RAssertMessage_FileLine(__FILE__, __LINE__, (var_) >= (minValue_) && (var_) <= (maxValue_), "NW4R:" #var_ " is out of bounds(%d)\n%d <= " #var_ " <= %d not satisfied.", (int)(var_), (int)(minValue_), (int)(maxValue_))

#define NW4RAssertHeaderClampedLRValue_FileLine(file_, line_, var_, minValue_, maxValue_)	NW4RAssertMessage_FileLine(  file_ ,   line_ , (var_) >= (minValue_) && (var_) <= (maxValue_),         #var_ " is out of bounds(%d)\n%d <= " #var_ " <= %d not satisfied.", (int)(var_), (int)(minValue_), (int)(maxValue_))
#define NW4RAssertHeaderClampedLRValue_File(file_, var_, minValue_, maxValue_)				NW4RAssertMessage_FileLine(  file_ , __LINE__, (var_) >= (minValue_) && (var_) <= (maxValue_),         #var_ " is out of bounds(%d)\n%d <= " #var_ " <= %d not satisfied.", (int)(var_), (int)(minValue_), (int)(maxValue_))
#define NW4RAssertHeaderClampedLRValue_Line(line_, var_, minValue_, maxValue_)				NW4RAssertMessage_FileLine(__FILE__,   line_ , (var_) >= (minValue_) && (var_) <= (maxValue_),         #var_ " is out of bounds(%d)\n%d <= " #var_ " <= %d not satisfied.", (int)(var_), (int)(minValue_), (int)(maxValue_))
#define NW4RAssertHeaderClampedLRValue(var_, minValue_, maxValue_)							NW4RAssertMessage_FileLine(__FILE__, __LINE__, (var_) >= (minValue_) && (var_) <= (maxValue_),         #var_ " is out of bounds(%d)\n%d <= " #var_ " <= %d not satisfied.", (int)(var_), (int)(minValue_), (int)(maxValue_))

/*******************************************************************************
 * Strings, for deadstripping
 */

#define NW4RAssert_String(exp_)							"NW4R:Failed assertion " #exp_
#define NW4RAssertHeader_String(exp_)					     "Failed assertion " #exp_
#define NW4RCheck_String(exp_)							"NW4R:Failed check " #exp_
#define NW4RCheckHeader_String(exp_)					     "Failed check " #exp_

#define NW4RAssertPointerNonnull_String(ptr_)			"NW4R:Pointer must not be NULL (" #ptr_ ")"
#define NW4RAssertHeaderPointerNonnull_String(ptr_)		     "Pointer must not be NULL (" #ptr_ ")"
#define NW4RAssertPointerValid_String(ptr_)				"NW4R:Pointer Error\n" #ptr_ "(=%p) is not valid pointer."
#define NW4RAssertHeaderPointerValid_String(ptr_)		     "Pointer Error\n" #ptr_ "(=%p) is not valid pointer."
#define NW4RAssertAligned_String(val_, align_)			"NW4R:Alignment Error(0x%x)\n" #val_ " must be aligned to " #align_ " bytes boundary."
#define NW4RAssertHeaderAligned_String(val_, align_)	     "Alignment Error(0x%x)\n" #val_ " must be aligned to " #align_ " bytes boundary."
#define NW4RAssertMinimumValue_String(var_)				"NW4R:" #var_ " is out of bounds(%d)\n%d <= " #var_ " not satisfied."
#define NW4RAssertHeaderMinimumValue_String(var_)		        #var_ " is out of bounds(%d)\n%d <= " #var_ " not satisfied."
#define NW4RAssertClampedLValue_String(var_)			"NW4R:" #var_ " is out of bounds(%d)\n%d <= " #var_ " < %d not satisfied."
#define NW4RAssertHeaderClampedLValue_String(var_)		        #var_ " is out of bounds(%d)\n%d <= " #var_ " < %d not satisfied."
#define NW4RAssertClampedLRValue_String(var_)			"NW4R:" #var_ " is out of bounds(%d)\n%d <= " #var_ " <= %d not satisfied."
#define NW4RAssertHeaderClampedLRValue_String(var_)		        #var_ " is out of bounds(%d)\n%d <= " #var_ " <= %d not satisfied."

/*******************************************************************************
 * Declarations
 */

namespace nw4r { namespace db
{
	__attribute__((weak)) extern void Panic(const char *file, int line, const char *msg, ...);
	__attribute__((weak)) extern void Warning(const char *file, int line, const char *msg, ...);
}} // namespace nw4r::db

// IWYU pragma: end_exports

// clang-format on
