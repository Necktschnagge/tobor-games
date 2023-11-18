#pragma once

#ifdef _MSC_VER 
//1 > C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.38.33130\include\xutility(1141, 31) : warning C4996 : 'stdext::checked_array_iterator<T *>' : warning STL4043 : stdext::checked_array_iterator, stdext::unchecked_array_iterator, and related factory functions are non - Standard extensions and will be removed in the future.std::span(since C++20) and gsl::span can be used instead.You can define _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING or _SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS to suppress this warning.
#define _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING
#endif // _MSC_VER 

