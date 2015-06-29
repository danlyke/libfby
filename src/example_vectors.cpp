#include <iostream>
#include <vector>
#include <string>
using namespace std;

class Base {
private:
    string name;
public:
    Base(string name) : name(name) {}
    string Name() { return name; }
    virtual void PrintOtherStuff() { cout << "Base class " << Name() << endl; }
    virtual ~Base() { cout << "Base destructor " << Name() <<  endl; }
};

class DerivedA : public Base
{
public:
    DerivedA(string name) : Base(name) {}
    virtual void PrintOtherStuff() { cout << "This is Derived A " << Name() << endl; }
    virtual ~DerivedA() { cout << "Derived destructor " << Name() << endl; }
};

vector<DerivedA *> BuildArray()
{
    vector<Base *> v;
    string thisName;

    for (int i = 0; i < 5; ++i)
    {
        char s[2] = "0";
        s[0] += i;
        string n(s);
        v.push_back(new DerivedA(string(s)));
    }
    vector<Base *> *pv(&v);
    vector<DerivedA *> *pva( static_cast< vector<DerivedA *> *>((void *)pv) );;
    
    return *pva;
}

void DestroyArray(vector<Base *> v)
{
    while (!v.empty())
    {
        Base *x = v.back();
        delete x;
        v.pop_back();
    }
}
    
int main(int, char **)
{
    vector<DerivedA *> v(BuildArray());
    for (auto x : v)
    {
        x->PrintOtherStuff();
    }
    DestroyArray(* static_cast< vector<Base *> *>((void *)&v));
    return 0;
}
