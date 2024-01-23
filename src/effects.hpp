
#pragma once
 
//***********************************************************//
// Homebrew Headers
//***********************************************************//
#include "effects.h"


Effects::Effects () {
    gst_init(nullptr, nullptr);
    initEffects ();
}

Effects::~Effects () {
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
    gst_deinit();
}

bool Effects::isEOSReached() {
    return reachedEOS;
}

bool Effects::isPipelineRunning () {
    GstState state;
    gboolean result;
    
    result = gst_element_get_state(pipeline, &state, NULL, GST_CLOCK_TIME_NONE);
    
    if (result == GST_STATE_CHANGE_FAILURE)
        return false;
    
    return state == GST_STATE_PLAYING;
}

void Effects::togglePipelineState () {
    GstState state;
    gboolean result;
    
    result = gst_element_get_state(pipeline, &state, NULL, GST_CLOCK_TIME_NONE);
    
    if (result == GST_STATE_CHANGE_FAILURE)
        return;
    
    gst_element_set_state(pipeline, (state == GST_STATE_PLAYING ? GST_STATE_PAUSED : GST_STATE_PLAYING));
}

void Effects::waitForPipelineState () {
    gst_element_get_state(pipeline, NULL, NULL, GST_CLOCK_TIME_NONE);
}

void Effects::initEffects () {
    pipeline  = gst_pipeline_new         ("audio-player");
    
    filesrc   = gst_element_factory_make ("filesrc",          "source");
    decodebin = gst_element_factory_make ("decodebin",        "decoder");
    convert1  = gst_element_factory_make ("audioconvert",     "convert");
    limiter   = gst_element_factory_make ("audiocheblimit",   "limiter");
    convert2  = gst_element_factory_make ("audioconvert",     "convert2");
    panorama  = gst_element_factory_make ("audiopanorama",    "panorama");
    delay     = gst_element_factory_make ("audioecho",        "echo");
    dynamic   = gst_element_factory_make ("audiodynamic",     "dynamic");
    equalizer = gst_element_factory_make ("equalizer-10bands","equalizer");
    volume    = gst_element_factory_make ("volume",           "volume");
    pitch     = gst_element_factory_make ("pitch",            "pitch");
    resample  = gst_element_factory_make ("audioresample",    "resample");
    spectrum  = gst_element_factory_make ("spectrum",         "spectrum");
    audiosink = gst_element_factory_make ("autoaudiosink",    "sink");
    
    if (!pipeline  || !filesrc   || !decodebin || 
        !convert1  || !limiter   || !convert2  ||
        !panorama  || !delay     || !dynamic   || 
        !equalizer || !volume    || !pitch     ||
        !resample  || !spectrum  || !audiosink) {
        g_printerr("One of the elements can't be created. Bruh.\n");
        return;
    }
    
    g_object_set (G_OBJECT(limiter),  "mode", 0, "cutoff", 48000.0, NULL);
    g_object_set (G_OBJECT(delay),    "max-delay", 5000000000, NULL);
    g_object_set (G_OBJECT(spectrum), "bands", 10, "interval", 25000000, NULL);
    
    gst_bin_add_many (GST_BIN(pipeline), 
                        filesrc, 
                        decodebin, 
                        convert1, 
                        limiter,
                        convert2, 
                        panorama, 
                        delay, 
                        dynamic, 
                        equalizer,
                        volume, 
                        pitch, 
                        resample, 
                        spectrum,
                        audiosink, NULL);
    
    g_signal_connect (decodebin, "pad-added",  G_CALLBACK(padAddedCallback),  convert1);
    
    if (!gst_element_link (filesrc, decodebin) || 
        !gst_element_link_many (
                        convert1, 
                        limiter,
                        convert2, 
                        panorama, 
                        delay, 
                        dynamic, 
                        equalizer,
                        volume, 
                        pitch, 
                        resample, 
                        spectrum,
                        audiosink, NULL)) {
        g_printerr ("Element linking error. Bruh.\n");
        gst_object_unref (pipeline);
        return;
    }

    bus = gst_element_get_bus (pipeline);
    gst_bus_add_watch (bus, busCallback, this);
    gst_object_unref (bus);
}

gboolean Effects::busCallback (GstBus *, GstMessage *msg, gpointer userdata) {
    gchar   *debug_info;
    GError  *err;
    const GstStructure *structure;
    const gchar *name;
    
    Effects *effects = static_cast<Effects*>(userdata);
    
    switch (GST_MESSAGE_TYPE(msg)) {
        case GST_MESSAGE_EOS:
            gst_element_set_state(effects->pipeline, GST_STATE_PAUSED);
            effects->waitForPipelineState();
            effects->reachedEOS = true;
            break;
        case GST_MESSAGE_ERROR:
            gst_message_parse_error(msg, &err, &debug_info);
            g_printerr("Error received from element %s: %s\n", GST_OBJECT_NAME(msg->src), err->message);
            g_printerr("Debugging information: %s\n", debug_info ? debug_info : "none");
            g_clear_error(&err);
            g_free(debug_info);
            gst_element_set_state(effects->pipeline, GST_STATE_NULL);
            effects->waitForPipelineState();
            effects->reachedEOS = true;
            break;
        case GST_MESSAGE_STREAM_START:
            effects->reachedEOS = false;
            break;
        case GST_MESSAGE_ELEMENT:
            structure = gst_message_get_structure(msg);
            name = gst_structure_get_name(structure);
            if (strcmp(name, "spectrum") == 0) 
                effects->updateAudioSpectrum(structure);
            break;
//         case GST_MESSAGE_TAG:
//             effects->updateAudioTags(msg);
//             break;
        default:
            break;
    }
    return TRUE;
}

void Effects::padAddedCallback (GstElement *, GstPad *pad, gpointer userdata) {
    GstElement* audioconvert = (GstElement*)userdata;
    GstPad *sinkpad = gst_element_get_static_pad(GST_ELEMENT(audioconvert), "sink");
    gst_pad_link(pad, sinkpad); 
    gst_object_unref(sinkpad);
}

bool Effects::updateAudioInfo () {
    GstPad  *pad  = gst_element_get_static_pad(GST_ELEMENT(convert1), "sink");
    gboolean result;
    
    if (pad == nullptr)
        return false;
        
    GstCaps *caps = gst_pad_get_current_caps(pad);
    if (caps == nullptr) {
        gst_object_unref(pad);
        return false;
    }
        
    GstStructure *structure = gst_caps_get_structure(caps, 0);
    if (structure == nullptr) {
        gst_caps_unref(caps);
        gst_object_unref(pad);
        return false;
    }
        
    result  = gst_structure_get_int(structure, "rate", &sampleRate);
    result |= gst_structure_get_int(structure, "channels", &channels);
    
    gst_caps_unref(caps);
    gst_object_unref(pad);
    
    return result != 0;
}

void Effects::updateAudioSpectrum (const GstStructure *structure) {
    if (!updateAudioInfo() || magnitudesChanged.load())
        return;

    previousMagnitudes = currentMagnitudes;
    currentMagnitudes.clear();
        
    GValue bands = G_VALUE_INIT;
    const GValue *mag = gst_structure_get_value(structure, "magnitude");
    
    g_object_get_property(G_OBJECT(spectrum), "bands", &bands);
    spectrumBands = g_value_get_uint(&bands);
    g_value_unset(&bands);
    
    for (uint i = 0; i < spectrumBands; ++i)
        currentMagnitudes.push_back(g_value_get_float(gst_value_list_get_value(mag, i)));
        
    magnitudesChanged.store(true);
}

void Effects::updateAudioDuration () {
      if (!gst_element_query_duration(pipeline, GST_FORMAT_TIME, &audioDuration)) {
          g_printerr("Unable to retrieve current duration. Bruh\n");
      }
}

void Effects::updateAudioPosition () {
    if (!gst_element_query_position(pipeline, GST_FORMAT_TIME, &audioPosition)) {
        g_printerr("Unable to retrieve current position. Bruh\n");
        return;
    }
}

// void Effects::updateAudioInfo () {
//     GstPad  *pad  = gst_element_get_static_pad(GST_ELEMENT(convert1), "sink");
//     if (pad == nullptr)
//         return;
//     GstCaps *caps = gst_pad_get_current_caps(pad);
//     if (caps == nullptr)
//         return;
//     GstStructure *structure = gst_caps_get_structure(caps, 0);
//     if (structure == nullptr)
//         return;
//     
//     
//     format = gst_structure_get_string (structure, "format");
//     
//     gst_structure_get_int (structure, "channels", &channels);
//     gst_structure_get_int (structure, "rate", &sampleRate);
//         
//     gst_caps_unref(caps);
//     gst_object_unref(pad);
// }

void Effects::seekPlayingPosition (gint64 position) {
    GstSeekFlags seekFlags = (GstSeekFlags)(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_ACCURATE);    
    gst_element_set_state   (pipeline, GST_STATE_PAUSED);
    gst_element_seek_simple (pipeline, GST_FORMAT_TIME, seekFlags, position);
    gst_element_set_state   (pipeline, GST_STATE_PLAYING);
    waitForPipelineState    ();
}

void Effects::changePlayingAudio (std::string filePath) {
    gst_element_set_state (pipeline, GST_STATE_READY);
    gst_element_set_state (filesrc, GST_STATE_NULL);
    
    g_object_set (filesrc, "location", filePath.c_str(), NULL);
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    waitForPipelineState ();
}

void Effects::changeEqualizerProps(CODES::EQUALIZER code, int newValue) {
    using namespace CODES;
    switch (code) {
        case Volume:
            g_object_set(G_OBJECT(volume),    "volume", newValue/10.0, NULL);
            break;
        case Slider29Hz:
            g_object_set(G_OBJECT(equalizer), "band0", newValue/10.0, NULL);
            break;
        case Slider59Hz:
            g_object_set(G_OBJECT(equalizer), "band1", newValue/10.0, NULL);
            break;
        case Slider119Hz:
            g_object_set(G_OBJECT(equalizer), "band2", newValue/10.0, NULL);
            break;
        case Slider237Hz:
            g_object_set(G_OBJECT(equalizer), "band3", newValue/10.0, NULL);
            break;
        case Slider474Hz:
            g_object_set(G_OBJECT(equalizer), "band4", newValue/10.0, NULL);
            break;
        case Slider1kHz:
            g_object_set(G_OBJECT(equalizer), "band5", newValue/10.0, NULL);
            break;
        case Slider2kHz:
            g_object_set(G_OBJECT(equalizer), "band6", newValue/10.0, NULL);
            break;
        case Slider4kHz:
            g_object_set(G_OBJECT(equalizer), "band7", newValue/10.0, NULL);
            break;
        case Slider8kHz:
            g_object_set(G_OBJECT(equalizer), "band8", newValue/10.0, NULL);
            break;
        case Slider16kHz:
            g_object_set(G_OBJECT(equalizer), "band9", newValue/10.0, NULL);
            break;
    }
}

void Effects::changeDelayProps (CODES::DELAY code, int newValue) {
    using namespace CODES;
    GValue value = G_VALUE_INIT;
    
    switch (code) {
        case Delay:
            g_object_set(G_OBJECT(delay), "delay", newValue*GST_MSECOND, NULL);
            break;
        case SurroundDelay:
            g_object_get_property(G_OBJECT(delay), "surround-delay", &value);
            g_object_set(G_OBJECT(delay), "surround-delay", (g_value_get_boolean(&value) ? FALSE : TRUE), NULL);
            break;
        case Feedback:
            g_object_set(G_OBJECT(delay), "feedback", (gdouble)(newValue/10.0), NULL);
            break;
        case MaxDelay:
            g_object_set(G_OBJECT(delay), "max-delay", newValue, NULL);
            break;
        case Intensity:
            g_object_set(G_OBJECT(delay), "intensity", (gdouble)(newValue/10.0), NULL);
            break;
    }
    g_value_unset(&value);
}

void Effects::changeFilterProps (CODES::FILTER code, gfloat newValue) {
    GValue value = G_VALUE_INIT;
    using namespace CODES;
    switch (code) {
        case Mode:
            g_object_get_property(G_OBJECT(limiter), "mode", &value);
            g_object_set(G_OBJECT(limiter), "mode", 1-g_value_get_enum(&value), NULL);
            break;
        case Cutoff:
            g_object_set(G_OBJECT(limiter), "cutoff", newValue, NULL);
            break;
        case Ripple:
            g_object_set(G_OBJECT(limiter), "ripple", newValue, NULL);
            break;
        case Poles:
            g_object_set(G_OBJECT(limiter), "poles",  (gint)newValue, NULL);
            break;
        case FilterType:
            g_object_get_property(G_OBJECT(limiter), "type", &value);
            g_object_set(G_OBJECT(limiter), "type", 3-g_value_get_int(&value), NULL);
            break;
    }
    g_value_unset(&value);
}

void Effects::changePitchProps (CODES::PITCH code, gfloat newValue) {
    GValue value = G_VALUE_INIT;
    using namespace CODES;
    switch (code) {
        case Tempo:
            updateAudioPosition ();
            g_object_get_property(G_OBJECT(pitch), "tempo", &value);
            g_object_set(G_OBJECT(pitch), "tempo", newValue, NULL);
            seekPlayingPosition(this->audioPosition/newValue*g_value_get_float(&value));
            break;
        case Pitch:
            g_object_set(G_OBJECT(pitch), "pitch", newValue, NULL);
            break;
        case Rate:
            updateAudioPosition ();
            g_object_get_property(G_OBJECT(pitch), "rate", &value);
            g_object_set(G_OBJECT(pitch), "rate", newValue, NULL);
            seekPlayingPosition(this->audioPosition/newValue*g_value_get_float(&value));
            break;
        case OutputRate:
            g_object_set(G_OBJECT(pitch), "output-rate", newValue, NULL);
            break;
    }
    g_value_unset(&value);
}

void Effects::changeCompressorProps (CODES::COMPRESSOR code, int newValue) {
    GValue value = G_VALUE_INIT;
    using namespace CODES;
    switch (code) {
        case Compressor:
            g_object_get_property(G_OBJECT(dynamic), "mode", &value);
            g_object_set(G_OBJECT(dynamic), "mode", 1-g_value_get_enum(&value), NULL);
            break;
        case Knee:
            g_object_get_property(G_OBJECT(dynamic), "characteristics", &value);
            g_object_set(G_OBJECT(dynamic), "characteristics", 1-g_value_get_enum(&value), NULL);
            break;
        case Ratio:
            g_object_set(G_OBJECT(dynamic), "ratio", (gfloat)(newValue/10.0), NULL);
            break;
        case Threshold:
            g_object_set(G_OBJECT(dynamic), "threshold", (gfloat)(newValue/10.0), NULL);
            break;
    }
    g_value_unset(&value);
} 

void Effects::changePanoramaProps (CODES::PANORAMA code, int newValue) {
    GValue value = G_VALUE_INIT;
    using namespace CODES;
    switch (code) {
        case Method:
            g_object_get_property(G_OBJECT(panorama), "method", &value);
            g_object_set(G_OBJECT(panorama), "method", 1-g_value_get_enum(&value), NULL);
            break;
        case Position:
            g_object_set(G_OBJECT(panorama), "panorama", (gfloat)newValue/1000.0, NULL);
            break;
    }
    g_value_unset(&value);
}

void Effects::changeSpectrumProps (CODES::SPECTRUM code, int64_t newValue) {
    using namespace CODES;
    switch (code) {
        case Bands:
            g_object_set(G_OBJECT(spectrum), "bands", newValue, NULL);
            break;
        case UpdateRate:
            g_object_set(G_OBJECT(spectrum), "interval", newValue*1000000, NULL);
            break;
        case AudioThreshold:
            g_object_set(G_OBJECT(spectrum), "threshold", newValue, NULL);
            break;
    }
}

