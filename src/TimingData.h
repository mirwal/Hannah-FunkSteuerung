// TimingData.h
#pragma once
#include <Arduino.h>

struct TimingData
{
    uint32_t loopInterval = 0;
    uint32_t maxLoopInterval = 0;
    uint32_t packetInterval = 0;
};

class TimingDataManager
{
public:
    TimingDataManager() = default;
    void resetMaxLoopInterval() { timingData.maxLoopInterval = 0; }

    void updateLoopInterval(uint32_t currentTime)
    {
        if (lastLoopTime != 0)
        {
            timingData.loopInterval =
                currentTime - lastLoopTime;

            if (timingData.loopInterval >
                timingData.maxLoopInterval)
            {
                timingData.maxLoopInterval =
                    timingData.loopInterval;
            }
        }

        lastLoopTime = currentTime;
    }

    const TimingData &getTimingData() const { return timingData; }

private:
    TimingData timingData;
    uint32_t lastLoopTime = 0;
};