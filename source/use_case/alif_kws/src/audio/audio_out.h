#ifndef AUDIO_OUT_H
#define AUDIO_OUT_H

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

int32_t audio_out_init(uint32_t sample_rate);
int32_t audio_out_transmit(const void *data, uint32_t num);
void audio_out_set_volume(uint8_t volume);

#ifdef __cplusplus
}
#endif

#endif // #ifndef AUDIO_OUT_H
