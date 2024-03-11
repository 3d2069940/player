
#ifndef _METATYPES_H_
#define _METATYPES_H_


#include <qmetatype.h>

struct EqualizerPreset {
    int slider29Hz  {0};
    int slider59Hz  {0};
    int slider119Hz {0};
    int slider237Hz {0};
    int slider474Hz {0};
    int slider1kHz  {0};
    int slider2kHz  {0};
    int slider4kHz  {0};
    int slider8kHz  {0};
    int slider16kHz {0};
};

struct DelayPreset {
    int delay         {1};
    int surroundDelay {0};
    int intensity     {0};
    int maxDelay      {5000};
    int feedback      {0};
};

struct FilterPreset {
    int    mode       {0};
    int    cutoff     {48000};
    double ripple     {0.25};
    int    poles      {4};
    int    filterType {0};
};

struct PitchPreset {
    int tempo      {10};
    int pitch      {10};
    int rate       {10};
    int outputRate {10};
};

struct CompressorPreset {
    int compressor {0};
    int kneeType   {0};
    int ratio      {10};
    int threshold  {0};
};

// Registers a custom data type for QVariant
Q_DECLARE_METATYPE(EqualizerPreset);
Q_DECLARE_METATYPE(DelayPreset);
Q_DECLARE_METATYPE(FilterPreset);
Q_DECLARE_METATYPE(PitchPreset);
Q_DECLARE_METATYPE(CompressorPreset);

#endif // _METATYPES_H_
