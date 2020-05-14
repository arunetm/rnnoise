#include "emscripten.h"
#include "include/rnnoise.h"


EMSCRIPTEN_KEEPALIVE
DenoiseState* demo()
{
    DenoiseState* ds;
    return ds;
}

EMSCRIPTEN_KEEPALIVE
uint8_t* create_buffer(int framesize, int numframes)
{
    return malloc(sizeof(uint8_t) * framesize * numframes);
}

EMSCRIPTEN_KEEPALIVE
void destroy_buffer(uint8_t* p)
{
    free(p);
}
