
#ifndef CONNECTOR_H
#define CONNECTOR_H


class MainWindowUI;

class Connector : public QObject {
public:
    static inline void connectWidgets          (MainWindowUI* ptr);
    static inline void connectPlayerTab        (MainWindowUI* ptr);
      static inline void connectEqualizer      (MainWindowUI* ptr);
      static inline void connectDelay          (MainWindowUI* ptr);
      static inline void connectFilter         (MainWindowUI* ptr);
      static inline void connectPitch          (MainWindowUI* ptr);
      static inline void connectCompressor     (MainWindowUI* ptr);
      static inline void connectPanorama       (MainWindowUI* ptr);
    static inline void connectVisualizationTab (MainWindowUI* ptr);
    static inline void connectSettingsTab      (MainWindowUI* ptr);
};

#endif // CONNECTOR_H
