#include "fby.h"

#include <string.h>


using namespace std;

namespace boost
{

	void intrusive_ptr_add_ref(Fby::BaseObj * p)
	{
            // increment reference count of object *p
            ++(p->baseObjReferences);
	}



	void intrusive_ptr_release(Fby::BaseObj * p)
	{
            // decrement reference count, and delete object when reference count reaches 0
            if (--(p->baseObjReferences) == 0)
                delete p;
            //p->Wipe();
	} 


} // namespace boost



Fby::BaseObj::BaseObj(const char *name, int size)
	: baseObjReferences(0), baseObjName(name), baseObjSize(size)
{
}

void Fby::BaseObj::Wipe()
{
	const char *name = baseObjName;
	int size = baseObjSize;

	memset(this, 0x00, size);
	baseObjSize = size;
	baseObjName = name;
}

Fby::BaseObj::~BaseObj()
{
	Wipe();
}


