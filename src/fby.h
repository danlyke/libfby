#ifndef INCLUDED_FBY_H
#define INCLUDED_FBY_H

/* 
** 
** This is a combination of helpers to abstract away the details of which
** underlying Boost pointer implementation to use, and the remnants of a
** system I wrote to abstract away the differences between Microsoft's
** Managed C++,
** 
** There are still remnants of the latter. At this point, I'm hoping that
** any new C++ projects have to interop with Managed .NET the objects can
** be wholely inside unmanaged code, and I don't have to do all the mixed
** code.
** 
*/

#define FBYUNCOPYABLE(C) private:\
    C(const C &);  \
    C& operator=(const C&);  \


namespace FbyHelpers {
	class BaseObj;
};

namespace boost
{

	void intrusive_ptr_add_ref(FbyHelpers::BaseObj * p);
	void intrusive_ptr_release(FbyHelpers::BaseObj * p);
} // namespace boost

#include <boost/intrusive_ptr.hpp>


#define FBYENUM(c) enum c
#define FBYCLASS(c) class c
#define FBYCLASSUNMANAGED(c) class c
#define FBYCLASSUNMANAGEDPTR(c) class c; typedef c* c##Ptr
#define FBYABSTRACTCLASS(c) class c
#define FBYCLASSPTR(c) class c; typedef boost::intrusive_ptr<c> c##Ptr
#define FBYCLASSLITEPTR(c) class c; typedef boost::shared_ptr<c> c##Ptr
//#define FBYCLASSLITEPTR(c) class c; typedef boost::shared_ptr<c> c##Ptr


namespace FbyHelpers
{
#define BASEOBJINIT(t) ""#t, sizeof(t)
	class BaseObj
	{
        FBYUNCOPYABLE(BaseObj);
	public:
		int baseObjReferences;
		const char *baseObjName;
		int baseObjSize;
		friend void ::boost::intrusive_ptr_add_ref(BaseObj* p);
		friend void ::boost::intrusive_ptr_release(BaseObj* p);
		void Wipe();
	public:
		BaseObj(const char *name, int size);
		virtual ~BaseObj();	
	};
}



#include <vector>
#include <string>
#include <boost/smart_ptr.hpp>
#include <boost/intrusive_ptr.hpp>
// #include "fby_core_unix_array.h"

#define FBYNULL NULL
#define FBYINITNULL
#define FBYTYPEDNULL(t) t()
#define FBYPTR(t) boost::intrusive_ptr<t>
#define FBYPARENTPTR(t) t *
#define RAWPTR(t) t##Ptr *
#define DYNAMIC_CAST(t,o) boost::dynamic_pointer_cast<t>(o)
#define FBYARRAYPTR(t) ARRAY(t)
#define FBYDYNARRAYPTR(t) DYNARRAY(t)
#define FBYPTRNARRAY(t,n) FBYPTR(NARRAY(t,n))


// #define CLEARDYNARRAY(a) a->Clear()
// #define ADDDYNARRAY(a,o) a->Add(o)
// #define ADDDYNARRAYRANGE(a,o) a->AddRange(o)
// #define REMOVEDYNARRAYIDX(a, i) a->RemoveAt(i)
// #define REMOVEDYNARRAYRANGE(a, i1, i2) a->RemoveRange(i1, i2 - i1)
// #define REMOVEDYNARRAYELEM(a, e) a->Remove(e)


namespace FbyHelpers 
{

	class FbyBaseException;
	typedef boost::shared_ptr<FbyBaseException> FbyBaseExceptionPtr;

	class FbyBaseException
	{
        FBYUNCOPYABLE(FbyBaseException);
	public:
	FbyBaseException(std::string s, int line, const char *file) : Message(s), line(line), file(file) {}
		std::string Message;
        int line;
        std::string file;
	};

#define THROWEXCEPTION(s) throw FbyBaseExceptionPtr(new FbyBaseException(s, __LINE__, __FILE__ ))


	namespace FbyHelpers {
		typedef unsigned char byte;
		typedef unsigned short ushort;
	}

	inline std::string NumToString(float value)
	{
		char ach[16];
		snprintf(ach, sizeof(ach), "%f", value);
		return std::string(ach);
	}

	inline std::string NumToString(double value)
	{
		char ach[16];
		snprintf(ach, sizeof(ach), "%f", value);
		return std::string(ach);
	}

	inline std::string NumToString(int value)
	{
		char ach[16];
		snprintf(ach, sizeof(ach),"%d", value);
		return std::string(ach);
	}


	inline std::string NumToString(int value, int places)
	{
		char ach[16];

		snprintf(ach, sizeof(ach),"%*.*d", places, places, value);
		return std::string(ach);
	}

	inline std::string NumToString(float value, int places)
	{
		char ach[16];

		snprintf(ach, sizeof(ach),"%*.*f", places, places, value);
		return std::string(ach);
	}

	inline std::string NumToString(float value, int places1, int places2)
	{
		char ach[16];

		snprintf(ach, sizeof(ach),"%*.*f", places1, places2, value);
		return std::string(ach);
	}

	inline int Math_Min(int a, int b)
	{
		return (a < b) ? a : b;
	}

	inline float Math_Min(float a, float b)
	{
		return (a < b) ? a : b;
	}
#if 1
	template <typename T> inline T Math_Max(T a, T b)
	{
		return (a > b) ? a : b;
	}
#else
	inline int Math_Max(int a, int b)
	{
		return (a > b) ? a : b;
	}

	inline float Math_Max(float a, float b)
	{
		return (a > b) ? a : b;
	}

	inline double Math_Max(double a, double b)
	{
		return (a > b) ? a : b;
	}
#endif
	inline float Math_Round(float a)
	{
		return (float)(int)(a + .5);
	}


	int StringToInt(std::string s);
	bool StringToInt(std::string s, int *i);
	float StringToFloat(std::string s);

#define STATICARRAYSIZE(a) (sizeof(a)/sizeof(*a))

#define PI 3.1415926538


#define STATIC_H_INIT(x)
#define STATIC_C_INIT(t,c,v,x) t c::v(x)
#define STATIC_C_DECL(t,c,v) t c::v
#define FBYNULLINIT ()
#define FBYNULLCONSTRUCTOR


class DateTime
{
private:
	time_t ltime;
public:
	DateTime();
	std::string ToString();
	static DateTime Parse(std::string s);
};


} // end of namespace FbyHelpers;

#endif /* #ifdef INCLUDED_FBY_H */
