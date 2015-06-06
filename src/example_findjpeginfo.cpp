#include <fbyimageutils.h>
#include <iostream>
using namespace std;

int main(int argc, char **argv)
{
    for (int i = 1; i < argc; ++i)
    {
        map<string,string> attributes;
        int width = -1, height = -1;
        FindJPEGSize(argv[i],
                     width, height,
                     attributes);
        cout << argv[i] << " is " << width << " by " << height << endl;
        for (auto a : attributes)
        {
            cout << a.first << " : " << a.second << endl;
        }
    }
    return 0;
}
