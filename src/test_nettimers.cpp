#include <gtest/gtest.h>
#include "fbynet.h"
using namespace std;

TEST(TimeoutTimer,TimeoutTimer)
{
    int timerTriggeredCount(0);
    IntervalTimeoutObjectPtr timeout(
        new TimeoutObject([&timerTriggeredCount]()
                          {
                              ++timerTriggeredCount;
                          },
            100)
        );
    long next_time;
    
    EXPECT_TRUE(timerTriggeredCount == 0);
    next_time = timeout->processTimeStepGetNextTime(50);
    EXPECT_TRUE(timerTriggeredCount == 0);
    EXPECT_TRUE(next_time == 50);
    
    next_time = timeout->processTimeStepGetNextTime(50);
    EXPECT_TRUE(timerTriggeredCount == 1);
    EXPECT_TRUE(next_time == 0);
}


TEST(IntervalTimer,IntervalTimer)
{
    int timerTriggeredCount(0);
    IntervalTimeoutObjectPtr timeout(
        new IntervalObject([&timerTriggeredCount]()
                          {
                              ++timerTriggeredCount;
                          },
            100)
        );
    long next_time;
    
    EXPECT_TRUE(timerTriggeredCount == 0);
    next_time = timeout->processTimeStepGetNextTime(50);
    EXPECT_TRUE(timerTriggeredCount == 0);
    EXPECT_TRUE(next_time == 50);
    
    next_time = timeout->processTimeStepGetNextTime(50);
    EXPECT_TRUE(timerTriggeredCount == 1);
    EXPECT_TRUE(next_time == 100);
    
    next_time = timeout->processTimeStepGetNextTime(99);
    EXPECT_TRUE(timerTriggeredCount == 1);
    EXPECT_TRUE(next_time == 1);

    next_time = timeout->processTimeStepGetNextTime(50);
    EXPECT_TRUE(timerTriggeredCount == 2);
    cout << "Got time " << next_time << endl;
    EXPECT_TRUE(next_time == 51);
}
