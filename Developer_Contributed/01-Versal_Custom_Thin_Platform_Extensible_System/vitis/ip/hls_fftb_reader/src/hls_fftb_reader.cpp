//
// Copyright (C) 2023, Advanced Micro Devices, Inc. All rights reserved.
// SPDX-License-Identifier: X11
//

#include "hls_fftb_reader.h"

#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>

extern "C" {

  void hls_fftb_reader(mem_type *mem, hls::stream<beat_type> &s, int size) {
#pragma HLS INTERFACE m_axi port = mem offset = slave bundle = gmem depth = MAX_WORDS

#pragma HLS INTERFACE axis port = s

#pragma HLS INTERFACE s_axilite port = mem bundle = control
#pragma HLS INTERFACE s_axilite port = size bundle = control
#pragma HLS INTERFACE s_axilite port = return bundle = control

    for (int i = 0; i < size; i++) {
#pragma HLS PIPELINE II = 1
      hls::axis<ap_int<WORD_SIZE>, 0, 0, 0> x;
      x.data = mem[i];
      x.keep = -1;
#ifndef __SYNTHESIS__
      std::cout << "read word " << i << ": " << std::hex << x.data << std::endl;
#endif
      s.write(x);
    }
  }
}
