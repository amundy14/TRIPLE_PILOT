#include <libString.h>

// Borrowed shamelessly from ReactOS's source code
int wcsicmp(const wchar_t* str1, const wchar_t* str2){
	while (towlower(*str1) == towlower(*str2)){
		if (*str1 == 0) return 0;
		str1++;
		str2++;
	}
	return towlower(*str1) - towlower(*str2);
}
