
#ifndef META_TYPES_H
#define META_TYPES_H

//***********************************************************//
// Yaml-cpp
//***********************************************************//
#include <yaml-cpp/yaml.h>                                  // YAML::Node YAML::convert

//***********************************************************//
// Qt5
//***********************************************************//
#include <QString>
#include <QListWidget>
#include <qmetatype.h>                                      // Q_DECLARE_METATYPE


class MainWindowUI;
// Defining structures to be stored in QVariant
struct PlaylistButtonValue {
    QString          filePath;
    QListWidget     *listWidget;
    MainWindowUI    *mainwindow;
    QListWidgetItem *item;
};

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
    int  delay         {1};
    bool surroundDelay {false};
    int  intensity     {0};
    int  maxDelay      {5000};
    int  feedback      {0};
};

struct FilterPreset {
    bool   mode       {false};
    int    cutoff     {48000};
    double ripple     {0.25};
    int    poles      {4};
    bool   filterType {false};
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
    int  ratio     {10};
    int  threshold {0};
};

// Registers a custom data type for QVariant
Q_DECLARE_METATYPE(PlaylistButtonValue);
Q_DECLARE_METATYPE(EqualizerPreset);
Q_DECLARE_METATYPE(DelayPreset);
Q_DECLARE_METATYPE(FilterPreset);
Q_DECLARE_METATYPE(PitchPreset);
Q_DECLARE_METATYPE(CompressorPreset);

// Defining behaviour when converting and reading from a custom data type
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
    
    template<>
    struct convert<QStringList> {
        static Node encode (const QStringList &value) {
            Node node;
            foreach (const QString &string, value)
                node.push_back(string.toStdString());
            return node;
        }
        static bool decode (const Node &node, QStringList &value) {
            if (!node.IsSequence())
                return false;
            for (YAML::const_iterator it = node.begin(); it != node.end(); ++it)
                if (it->IsScalar())
                    value << QString::fromStdString(it->as<std::string>());
            return true;
        }
    };
    template<>
    struct convert<QString> {
        static Node encode (const QString &value) {
            return Node (value.toStdString());
        }
        static bool decode (const Node &node, QString &value) {
            if (!node.IsScalar())
                return false;
            value = QString::fromStdString(node.as<std::string>());
            return true;
        }
    };
}
#endif // META_TYPES_H
