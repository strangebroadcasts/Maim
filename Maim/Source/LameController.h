/*
  ==============================================================================

    LameController.h
    Created: 4 Apr 2023 8:59:01am
    Author:  Arden Butterfield

  ==============================================================================
*/

#pragma once

#include "QueueBuffer.h"

#include <vector>
#include <array>
#include <lame.h>
#include <stdlib.h>
#include <memory>
#include <cstring>
#include <iostream>


class LameController
{
public:
    LameController();
    ~LameController();
    bool init(const int sampleRate,
              const int maxSamplesPerBlock,
              const int bitrate);
    void deInit();
    void addNextInput(float *left_input, float* right_input, const int num_block_samples);
    bool hasReadyOutput();
    bool copyOutput(float* left, float* right, const int num_block_samples);
    int samples_in_output_queue();
    bool initialFlush();
    
private:
    BendFlagsAndData* bendFlagsAndData;
    bool bInitialized = false;
    lame_global_flags *lame_enc_handler = nullptr;
    hip_global_flags *lame_dec_handler = nullptr;
    std::vector<unsigned char> mp3Buffer;
    std::array<short, 20000> decodedLeftChannel = {0};
    std::array<short, 20000> decodedRightChannel = {0};
    std::vector<float> readBuf;
    int max_samples_per_block;
    std::unique_ptr<QueueBuffer<float>> outputBufferL;
    std::unique_ptr<QueueBuffer<float>> outputBufferR;
    
    int input_buf_size;
    int mp3_buf_size;
    
    // Values from the LAME documentation
    const std::array<int, 9> allowed_samplerates = {
        8000,
        11025,
        12000,
        16000,
        22050,
        24000,
        32000,
        44100,
        48000
    };
    
};
