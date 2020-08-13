// encoder for flac using libsndfile

#ifndef ENCODERSNDFILEFLAC_H
#define ENCODERSNDFILEFLAC_H


#include "encoder/encoderflacsettings.h"
#ifdef Q_OS_WIN
//Enable unicode in libsndfile on Windows
//(sf_open uses UTF-8 otherwise)
#include <windows.h>
#define ENABLE_SNDFILE_WINDOWS_PROTOTYPES 1
#endif
#include <sndfile.h>

#include "util/types.h"
#include "encoder/encoderwave.h"

class EncoderCallback;

class EncoderSndfileFlac : public EncoderWave {
  public:
    EncoderSndfileFlac(EncoderCallback* pCallback = nullptr);
    ~EncoderSndfileFlac() override = default;

    void setEncoderSettings(const EncoderSettings& settings) override;

  protected:
    void initStream() override;
  private:
    double m_compression;
};

#endif //ENCODERWAVE_H
