#ifndef __INTERNAL_STD_VA_LIST_HPP__
#define __INTERNAL_STD_VA_LIST_HPP__

extern "C"
{
	struct __va_list_struct
	{
		char gpr;
		char fpr;
		char reserved[2];
		char *input_arg_area;
		char *reg_save_area;
	};
} // extern "C"

namespace std
{
	typedef struct ::__va_list_struct va_list[1];
} // namespace std

#endif // __INTERNAL_STD_VA_LIST_HPP__
