#include <boost/filesystem.hpp>
#include <iostream>

int main(int argc, char**argv)
{
    for (int i = 1; i < argc; i++)
    {
        boost::filesystem::path p(argv[i]);
        std::cout << "filename and extension : " << p.filename() << std::endl; // file.ext
        std::cout << "filename only          : " << p.stem() << std::endl;     // file
    }
}
