#include <vector>
#include <JuceHeader.h>
#include <cmath>

struct AllPassSchroeder
{
    void prepare(double sampleRate, int maxBlockSize, float maxDelayMs)
    {
        mSampleRate = (float)sampleRate;
        const int maxDelaySamples = (int)std::ceil(maxDelayMs * 0.001f * mSampleRate) + maxBlockSize + 8;

        xDelay.assign((size_t)maxDelaySamples, 0.0f);
        yDelay.assign((size_t)maxDelaySamples, 0.0f);
    }

    void reset()
    {
        std::fill(xDelay.begin(), xDelay.end(), 0.0f);
        std::fill(yDelay.begin(), yDelay.end(), 0.0f);
        mWritePos = 0;
    }

    void setEnabled(bool enabled)
    {
        mEnabled = enabled;
    }

    void setDelay(float delayMs)
    {
        const int delaySamples = (int)std::ceil(delayMs * 0.001f * mSampleRate);

        mDelaySamples = juce::jlimit(1, (int)xDelay.size() - 1, delaySamples);
    }

    void setGain(float gain)
    {
        mGain = juce::jlimit(-0.999f, 0.999f, gain);
    }

    float process(float x)
    {
        if (!mEnabled)
        {
            return x;
        }

        const int size = (int)xDelay.size();
        const int readPos = (mWritePos - mDelaySamples + size) % size;

        const float xD = xDelay[(size_t)readPos];
        const float yD = yDelay[(size_t)readPos];

        // y = -g*x + x[n-D] + g*y[n-D]
        const float y = (-mGain * x) + xD + (mGain * yD);

        xDelay[(size_t)mWritePos] = x;
        yDelay[(size_t)mWritePos] = y;

        if (++mWritePos >= size)
        {
            mWritePos = 0;
        }

        return y;
    }

    std::vector<float> xDelay, yDelay;
    bool mEnabled = false;
    float mSampleRate = 44100.0f;
    int mWritePos = 0;
    int mDelaySamples = 1;
    float mGain = 0.7f;
};
