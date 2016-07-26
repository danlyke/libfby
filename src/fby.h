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


namespace Fby {
	class BaseObj;
};

namespace boost
{

	void intrusive_ptr_add_ref(Fby::BaseObj * p);
	void intrusive_ptr_release(Fby::BaseObj * p);
} // namespace boost

#include <boost/intrusive_ptr.hpp>


#define FBYENUM(c) enum c
#define FBYCLASS(c) class c
#define FBYCLASSUNMANAGED(c) class c
#define FBYCLASSUNMANAGEDPTR(c) class c; typedef c* c##Ptr
#define FBYABSTRACTCLASS(c) class c
#define FBYCLASSPTR(c) class c; typedef boost::intrusive_ptr<c> c##Ptr
//#define FBYCLASSLITEPTR(c) class c; typedef boost::shared_ptr<c> c##Ptr
//#define FBYCLASSLITEPTR(c) class c; typedef boost::shared_ptr<c> c##Ptr


#define FBYSTRINGIFY_(x) #x
#define FBYSTRINGIFY(x) FBYSTRINGIFY_(x)


namespace Fby
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


namespace Fby 
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


    typedef unsigned char byte;
    typedef unsigned short ushort;

#define STATICARRAYSIZE(a) (sizeof(a)/sizeof(*a))


} // end of namespace Fby;

#endif /* #ifdef INCLUDED_FBY_H */
