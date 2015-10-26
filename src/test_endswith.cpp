#include <gtest/gtest.h>
#include "fbystring.h"

using namespace std;
using namespace Fby;

TEST(EndsWithWorks,EndsWithWorks)
{
    string indexdothtml("index.html");
    string bladotjs("bla.js");
    string dothtml(".html");
    string dotjs(".js");
    string shortstr("a");

    EXPECT_TRUE(ends_with(indexdothtml, dothtml));
    EXPECT_FALSE(ends_with(indexdothtml, dotjs));

    EXPECT_FALSE(ends_with(bladotjs, dothtml));
    EXPECT_TRUE(ends_with(bladotjs, dotjs));

    EXPECT_FALSE(ends_with(shortstr,dotjs));
    EXPECT_FALSE(ends_with(dothtml, indexdothtml));
    EXPECT_FALSE(ends_with(dotjs, indexdothtml));
    EXPECT_FALSE(ends_with(dothtml, bladotjs));
    EXPECT_FALSE(ends_with(dotjs, bladotjs));
}
