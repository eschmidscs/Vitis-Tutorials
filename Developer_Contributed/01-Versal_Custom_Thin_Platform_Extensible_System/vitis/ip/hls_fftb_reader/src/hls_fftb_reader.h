#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>

#define WORD_SIZE 64
#define MAX_WORDS 1024

using mem_type = ap_int<WORD_SIZE>;
using beat_type = hls::axis<ap_int<WORD_SIZE>, 0, 0, 0>;

extern "C" {
  void hls_fftb_reader(mem_type *mem, hls::stream<beat_type> &s, int size);
}
