/**
 * @file TRestDetectorExperimentalReadoutModule.h
 * @brief Defines the TRestDetectorExperimentalReadoutModule class.
 */

#ifndef REST_TRESTDETECTOREXPERIMENTALREADOUTCHANNEL_H
#define REST_TRESTDETECTOREXPERIMENTALREADOUTCHANNEL_H

#include <map>

#include "TRestDetectorExperimentalReadoutPixel.h"

/**
 * @class TRestDetectorExperimentalReadoutChannel
 * @brief Represents a channel in an experimental readout detector.
 * @author Luis Obis
 */
class TRestDetectorExperimentalReadoutChannel {
   private:
    std::vector<TRestDetectorExperimentalReadoutPixel> fPixels; /**< The pixels of the channel. */
    unsigned short fChannel;                                    /**< The channel of the pixel. */

   public:
    TRestDetectorExperimentalReadoutChannel() = default;
    ~TRestDetectorExperimentalReadoutChannel() = default;

    ClassDef(TRestDetectorExperimentalReadoutChannel, 1);
};

#endif  // REST_TRESTDETECTOREXPERIMENTALREADOUTCHANNEL_H
