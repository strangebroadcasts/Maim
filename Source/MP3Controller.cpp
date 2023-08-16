/*
  ==============================================================================

    MP3Controller.cpp
    Created: 18 Apr 2023 5:57:57pm
    Author:  Arden Butterfield

  ==============================================================================
*/

#include "MP3Controller.h"

bool MP3Controller::init(const int sampleRate,
                         const int maxsampsperblock,
                         const int br)
{
    deInit();
    std::cout << name << " init " << sampleRate << " " << maxsampsperblock << " " << br << "\n";
    samplerate = validate_samplerate(sampleRate);
    bitrate = validate_bitrate(br);
    maxSamplesPerBlock = maxsampsperblock;
    
    input_buf_size = maxSamplesPerBlock;
    // From LAME api: mp3buf_size in bytes = 1.25*num_samples + 7200
    mp3_buf_size = MP3FRAMESIZE * 1.25 + 7200;
    mp3Buffer.resize(mp3_buf_size);
    std::fill(mp3Buffer.begin(), mp3Buffer.end(), 0);

    outputBufferL = std::make_unique<QueueBuffer<float>>(1152 * 4 + maxSamplesPerBlock, 0.f);
    outputBufferR = std::make_unique<QueueBuffer<float>>(1152 * 4 + maxSamplesPerBlock, 0.f);

    if (!init_encoder()) {
        return false;
    }


    lame_dec_handler = hip_decode_init();

    flushEncoder();

    bInitialized = true;
    return true;
}

void MP3Controller::flushEncoder()
{
    float left[1152];
    float right[1152];
    std::memset(left, 0, 1152 * sizeof(float));
    std::memset(right, 0, 1152 * sizeof(float));
    for (int i = 0; i < 4; ++i) {
        auto encResult = encodesamples(left, right);
        int decResult = hip_decode(lame_dec_handler,
            (unsigned char*)&mp3Buffer[0],
            encResult,
            decodedLeftChannel.data(),
            decodedRightChannel.data());

        std::cout << "flushed " << decResult << "\n";
    }
}

void MP3Controller::deInit()
{
    bInitialized = false;
    deinit_encoder();
    
    if (lame_dec_handler) {
        hip_decode_exit(lame_dec_handler);
        lame_dec_handler = nullptr;
    }
    
    outputBufferL.reset(nullptr);
    outputBufferR.reset(nullptr);
    
    mp3Buffer.resize(0);
}

bool MP3Controller::processFrame (float* left, float* right)
{
    auto encResult = encodesamples(left, right);
    if (encResult <= 0) {
        std::cout << "encoding error: " << encResult << "\n";
        return false;
    }

    int decResult = hip_decode(lame_dec_handler,
        (unsigned char*)&mp3Buffer[0],
        encResult,
        decodedLeftChannel.data(),
        decodedRightChannel.data());
    if (decResult != 1152) {
        std::cout << "decoding error: " << decResult << "\n";
        return false;
    }

    float amp;
    for (int i = 0; i < decResult; ++i) {
        left[i] = pcmConvert(decodedLeftChannel[i]);
        right[i] = pcmConvert(decodedRightChannel[i]);
    }

    return true;
}
