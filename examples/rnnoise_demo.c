/* Copyright (c) 2018 Gregor Richards
 * Copyright (c) 2017 Mozilla */
/*
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

   - Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

   - Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

// emcc -g -O2 -s LINKABLE=1 -fsanitize=undefined  -s ASSERTIONS=1 -s SAFE_HEAP=1 -s WARN_UNALIGNED=1 -s TOTAL_MEMORY=512MB -s ALLOW_MEMORY_GROWTH=1 -s EXPORT_ALL=1 -s EXTRA_EXPORTED_RUNTIME_METHODS='["cwrap"]' -I ../include/ ../src/*.c ../examples/*.c -o rnn_denoise.js

#include <stdio.h>
#include <stdlib.h>
#include "rnnoise.h"
#include "emscripten.h"

#define FRAME_SIZE 480
EMSCRIPTEN_KEEPALIVE
uint64_t Resample_f32(const float *input, float *output, int inSampleRate, int outSampleRate, uint64_t inputSize,
                      uint32_t channels
) {
    if (input == NULL)
        return 0;
    uint64_t outputSize = inputSize * outSampleRate / inSampleRate;

    if (output == NULL)
        return outputSize;
    double stepDist = ((double) inSampleRate / (double) outSampleRate);
    const uint64_t fixedFraction = (1LL << 32);
    const double normFixed = (1.0 / (1LL << 32));
    uint64_t step = ((uint64_t) (stepDist * fixedFraction + 0.5));
    uint64_t curOffset = 0;
    for (uint32_t i = 0; i < outputSize; i += 1) {
        for (uint32_t c = 0; c < channels; c += 1) {
            *output++ = (float) (input[c] + (input[c + channels] - input[c]) * (
                    (double) (curOffset >> 32) + ((curOffset & (fixedFraction - 1)) * normFixed)
            )
            );
        }
        curOffset += step;
        input += (curOffset >> 32) * channels;
        curOffset &= (fixedFraction - 1);
    }
    return outputSize;
}

EMSCRIPTEN_KEEPALIVE
void denoise_proc(float *input, unsigned sampleCount, unsigned sampleRate, unsigned channels) {
    unsigned targetFrameSize = 480;
    unsigned targetSampleRate = 48000;
    unsigned perFrameSize = sampleRate / 100;
    float *frameBuffer = (float *) malloc(sizeof(*frameBuffer) * (channels + 1) * targetFrameSize);
    float *processBuffer = frameBuffer + targetFrameSize * channels;
    DenoiseState **sts = malloc(channels * sizeof(DenoiseState *));
    if (sts == NULL || frameBuffer == NULL) {
        if (sts)
            free(sts);
        if (frameBuffer)
            free(frameBuffer);
        fprintf(stderr, "malloc error.\n");
        return;
    }
    for (int i = 0; i < channels; i++) {
        sts[i] = rnnoise_create(NULL);
        if (sts[i] == NULL) {
            for (int x = 0; x < i; x++) {
                if (sts[x]) {
                    rnnoise_destroy(sts[x]);
                }
            }
            free(sts);
            free(frameBuffer);
            return;
        }
    }
    size_t frameStep = channels * perFrameSize;
    unsigned frames = sampleCount / frameStep;
    unsigned lastFrameSize = (sampleCount % frameStep) / channels;
    for (int i = 0; i < frames; ++i) {
        Resample_f32(input, frameBuffer, sampleRate, targetSampleRate,
                     perFrameSize, channels);
        for (int c = 0; c < channels; c++) {
            for (int k = 0; k < targetFrameSize; k++)
                processBuffer[k] = frameBuffer[k * channels + c];
            rnnoise_process_frame(sts[c], processBuffer, processBuffer);
            for (int k = 0; k < targetFrameSize; k++)
                frameBuffer[k * channels + c] = processBuffer[k];
        }
        Resample_f32(frameBuffer, input, targetSampleRate, sampleRate, targetFrameSize, channels);
        input += frameStep;
    }
    // if (lastFrameSize != 0) {
    //     memset(frameBuffer, 0, targetFrameSize * channels * sizeof(float));
    //     unsigned lastReasmpleSize = Resample_f32(input, frameBuffer, sampleRate,
    //                                              targetSampleRate,
    //                                              lastFrameSize, channels);
    //     for (int c = 0; c < channels; c++) {
    //         for (int k = 0; k < targetFrameSize; k++)
    //             processBuffer[k] = frameBuffer[k * channels + c];
    //         rnnoise_process_frame(sts[c], processBuffer, processBuffer);
    //         for (int k = 0; k < targetFrameSize; k++)
    //             frameBuffer[k * channels + c] = processBuffer[k];
    //     }
    //     Resample_f32(frameBuffer, input, targetSampleRate, sampleRate, lastReasmpleSize,
    //                  channels);
    // }
    for (int i = 0; i < channels; i++) {
        if (sts[i]) {
            rnnoise_destroy(sts[i]);
        }
    }
    free(sts);
    free(frameBuffer);
}

EMSCRIPTEN_KEEPALIVE
int denoise_proc_mono(float *input, unsigned sampleCount) {
  DenoiseState *st;
  st = rnnoise_create(NULL);

  int counter = 0;
  int numFrames = sampleCount / FRAME_SIZE;
  int result = (int) input;
  
  while(counter < numFrames )
  {
    rnnoise_process_frame(st, input, input);
    counter ++;
    input+=FRAME_SIZE;
  }
  // rnnoise_destroy(st);
  return result;
}

int main(int argc, char **argv) {
  // int i;
  // int first = 1;
  // float x[FRAME_SIZE];
  // FILE *f1, *fout;
  // DenoiseState *st;
  // st = rnnoise_create(NULL);
  // if (argc!=3) {
  //   fprintf(stderr, "usage: %s <noisy speech> <output denoised>\n", argv[0]);
  //   return 1;
  // }
  // f1 = fopen(argv[1], "r");
  // fout = fopen(argv[2], "w");
  // while (1) {
  //   short tmp[FRAME_SIZE];
  //   fread(tmp, sizeof(short), FRAME_SIZE, f1);
  //   if (feof(f1)) break;
  //   for (i=0;i<FRAME_SIZE;i++) x[i] = tmp[i];
  //   rnnoise_process_frame(st, x, x);
  //   for (i=0;i<FRAME_SIZE;i++) tmp[i] = x[i];
  //   if (!first) fwrite(tmp, sizeof(short), FRAME_SIZE, fout);
  //   first = 0;
  // }
  // rnnoise_destroy(st);
  // fclose(f1);
  // fclose(fout);
  return 0;
}
