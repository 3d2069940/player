
#ifndef PRESET_TYPES_H
#define PRESET_TYPES_H

#include <yaml-cpp/yaml.h>
#include <qmetatype.h>


struct EqualizerPreset {
    int slider29Hz;
    int slider59Hz;
    int slider119Hz;
    int slider237Hz;
    int slider474Hz;
    int slider1kHz;
    int slider2kHz;
    int slider4kHz;
    int slider8kHz;
    int slider16kHz;
};

struct DelayPreset {
    int  delay;
    bool surroundDelay;
    int  intensity;
    int  maxDelay;
    int  feedback;
};

struct FilterPreset {
    bool   mode;
    int    cutoff;
    double ripple;
    int    poles;
    bool   filterType;
};

struct PitchPreset {
    int tempo;
    int pitch;
    int rate;
    int outputRate;
};

struct CompressorPreset {
    bool compressor;
    bool kneeType;
    int  ratio;
    int  threshold;
};

Q_DECLARE_METATYPE(EqualizerPreset);
Q_DECLARE_METATYPE(DelayPreset);
Q_DECLARE_METATYPE(FilterPreset);
Q_DECLARE_METATYPE(PitchPreset);
Q_DECLARE_METATYPE(CompressorPreset);

namespace YAML {
    template<>
    struct convert<EqualizerPreset> {
        static Node encode (const EqualizerPreset &preset) {
            Node node;
            node.push_back(preset.slider29Hz);
            node.push_back(preset.slider59Hz);
            node.push_back(preset.slider119Hz);
            node.push_back(preset.slider237Hz);
            node.push_back(preset.slider474Hz);
            node.push_back(preset.slider1kHz);
            node.push_back(preset.slider2kHz);
            node.push_back(preset.slider4kHz);
            node.push_back(preset.slider8kHz);
            node.push_back(preset.slider16kHz);
            return node;
        };
        static bool decode (const Node &node, EqualizerPreset &preset) {
            if (!node.IsSequence() || node.size() != 10)
                return false;
            preset.slider29Hz  = node[0].as<int>();
            preset.slider59Hz  = node[1].as<int>();
            preset.slider119Hz = node[2].as<int>();
            preset.slider237Hz = node[3].as<int>();
            preset.slider474Hz = node[4].as<int>();
            preset.slider1kHz  = node[5].as<int>();
            preset.slider2kHz  = node[6].as<int>();
            preset.slider4kHz  = node[7].as<int>();
            preset.slider8kHz  = node[8].as<int>();
            preset.slider16kHz = node[9].as<int>();
            return true;
        }
    };
    
    template<>
    struct convert<DelayPreset> {
        static Node encode (const DelayPreset &preset) {
            Node node;
            node.push_back(preset.delay);
            node.push_back(preset.surroundDelay);
            node.push_back(preset.intensity);
            node.push_back(preset.maxDelay);
            node.push_back(preset.feedback);
            return node;
        }
        static bool decode (const Node &node, DelayPreset &preset) {
            if (!node.IsSequence() || node.size() != 5)
                return false;
            preset.delay         = node[0].as<int>();
            preset.surroundDelay = node[1].as<bool>();
            preset.intensity     = node[2].as<int>();
            preset.maxDelay      = node[3].as<int>();
            preset.feedback      = node[4].as<int>();
            return true;
        }
    };
    
    template<>
    struct convert<FilterPreset> {
        static Node encode (const FilterPreset &preset) {
            Node node;
            node.push_back(preset.mode);
            node.push_back(preset.cutoff);
            node.push_back(preset.ripple);
            node.push_back(preset.poles);
            node.push_back(preset.filterType);
            return node;
        };
        static bool decode (const Node &node, FilterPreset &preset) {
            if (!node.IsSequence() || node.size() != 5)
                return false;
            preset.mode       = node[0].as<bool>   ();
            preset.cutoff     = node[1].as<int>    ();
            preset.ripple     = node[2].as<double> ();
            preset.poles      = node[3].as<int>    ();
            preset.filterType = node[4].as<bool>   ();
            return true;
        }
    };
    
    template<>
    struct convert<PitchPreset> {
        static Node encode (const PitchPreset &preset) {
            Node node;
            node.push_back(preset.tempo);
            node.push_back(preset.pitch);
            node.push_back(preset.rate);
            node.push_back(preset.outputRate);
            return node;
        };
        static bool decode (const Node &node, PitchPreset &preset) {
            if (!node.IsSequence() || node.size() != 4)
                return false;
            preset.tempo      = node[0].as<int>();
            preset.pitch      = node[1].as<int>();
            preset.rate       = node[2].as<int>();
            preset.outputRate = node[3].as<int>();
            return true;
        }
    };
    
    template<>
    struct convert<CompressorPreset> {
        static Node encode (const CompressorPreset &preset) {
            Node node;
            node.push_back(preset.compressor);
            node.push_back(preset.kneeType);
            node.push_back(preset.ratio);
            node.push_back(preset.threshold);
            return node;
        };
        static bool decode (const Node &node, CompressorPreset &preset) {
            if (!node.IsSequence() || node.size() != 4)
                return false;
            preset.compressor = node[0].as<bool> ();
            preset.kneeType   = node[1].as<bool> ();
            preset.ratio      = node[2].as<int>  ();
            preset.threshold  = node[3].as<int>  ();
            return true;
        }
    };
}
#endif // PRESET_TYPES_H
