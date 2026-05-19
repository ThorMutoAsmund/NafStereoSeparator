#include "AllPassSchroeder.cpp"

struct Decorrelator
{
    void prepare(double sampleRate, int maxBlockSize, float size)
    {
        // Keep delays very short to avoid audible echo
        ap1.prepare(sampleRate, maxBlockSize, size);
        ap2.prepare(sampleRate, maxBlockSize, size);
        ap3.prepare(sampleRate, maxBlockSize, size);
        ap4.prepare(sampleRate, maxBlockSize, size);
        ap5.prepare(sampleRate, maxBlockSize, size);
    }

    void resetSize(float d1, float d2, float d3, float d4, float d5,
        float g1, float g2, float g3, float g4, float g5)
    {
        // Initial delays
        ap1.setDelay(d1);
        ap2.setDelay(d2);
        ap3.setDelay(d3);
        ap4.setDelay(d4);
        ap5.setDelay(d5);

        // Initial Gains: lower = subtler, higher = more phase swirl/metallic risk
        ap1.setGain(g1);
        ap2.setGain(g2);
        ap3.setGain(g3);
        ap4.setGain(g4);
        ap5.setGain(g5);

        ap1.reset();
        ap2.reset();
        ap3.reset();
        ap4.reset();
        ap5.reset();
    }

    void resetEnabled(bool e1, bool e2, bool e3, bool e4, bool e5)
    {
        ap1.setEnabled(e1);
        ap2.setEnabled(e2);
        ap3.setEnabled(e3);
        ap4.setEnabled(e4);
        ap5.setEnabled(e5);
    }

    float process(float x)
    {
        x = ap1.process(x);
        x = ap2.process(x);
        x = ap3.process(x);
        x = ap4.process(x);
        x = ap5.process(x);

        return x;
    }

    AllPassSchroeder ap1, ap2, ap3, ap4, ap5;
};
