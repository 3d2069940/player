
#ifndef EFFECTS_H
#define EFFECTS_H


//***********************************************************//
// GStreamer
//***********************************************************//
#include <gst/gst.h>


namespace CODES {
    enum EQUALIZER {
        Volume = 0, 
        Slider29Hz, 
        Slider59Hz, 
        Slider119Hz, 
        Slider237Hz, 
        Slider474Hz, 
        Slider1kHz, 
        Slider2kHz, 
        Slider4kHz, 
        Slider8kHz, 
        Slider16kHz
    };
    enum DELAY {
        Delay = 0, 
        SurroundDelay, 
        Feedback, 
        MaxDelay, 
        Intensity
    };
    enum FILTER {
        Mode = 0, 
        Cutoff, 
        Ripple, 
        Poles, 
        FilterType
    };
    enum PITCH {
        Tempo = 0, 
        Pitch, 
        Rate, 
        OutputRate
    };
    enum COMPRESSOR {
        Compressor = 0, 
        Knee, 
        Ratio, 
        Threshold
    };
    enum PANORAMA {
        Method = 0, 
        Position
    };
};

class Effects {

private:
//  VARIABLES
    GstBus *bus;
    bool   reachedEOS;
//  FUNCTIONS 
    void initEffects ();
//  CALLBACKS
    static gboolean busCallback      (GstBus*,     GstMessage*,      gpointer);
    static void     padAddedCallback (GstElement*, GstPad*,          gpointer);

public:
    
    enum {
        ARTIST_ID = 0, 
        ALBUM_ID,
        CODEC_ID, 
        IMAGE_ID, 
        TITLE_ID
    };
    
    GstElement *pipeline, 
               *filesrc,
               *decodebin, 
               *convert1,
               *limiter,
               *convert2, 
               *panorama,
               *delay,
               *dynamic,
               *equalizer,
               *volume,
               *pitch,
               *audiosink;
               
    gint64 audioDuration, audioPosition;
    
    guint imageLength;

    Effects ();
    ~Effects ();
    
    bool isEOSReached         (); 
    bool isPipelineRunning    ();
    void togglePipelineState  ();
    void waitForPipelineState ();
    
    void updateAudioDuration ();
    void updateAudioPosition ();
    void updateAudioInfo     ();
    void updateAudioTags     ();
    
    void seekPlayingPosition (gint64);
    
    void changePlayingAudio  (std::string);
    
    void changeEqualizerProps  (CODES::EQUALIZER,  int);
    void changeDelayProps      (CODES::DELAY,      int    newValue = 0);
    void changeFilterProps     (CODES::FILTER,     gfloat newValue = 0);
    void changePitchProps      (CODES::PITCH,      gfloat);
    void changeCompressorProps (CODES::COMPRESSOR, int    newValue = 0);
    void changePanoramaProps   (CODES::PANORAMA,   int    newValue = 0);

};

#endif // EFFECTS_H
