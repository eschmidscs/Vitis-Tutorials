#pragma once

#include <adf.h>
#include <fft_ifft_dit_1ch_graph.hpp>

// ------------------------------------------------------------
// FFT Namespace
// ------------------------------------------------------------
// fft function parameter configuration
namespace fft1 {
typedef cint16 TT_TYPE;
typedef cint16 TT_TWIDDLE;
static constexpr int TP_POINT_SIZE = 512;
static constexpr int TP_FFT_NIFFT = 1;
static constexpr int TP_SHIFT = 0; // Excludes twiddle shift
static constexpr int TP_CASC_LEN = 1;
static constexpr int TP_DYN_PT_SIZE = 0;
static constexpr int TP_WINDOW_SIZE = 16*512;
static constexpr int TP_API = 0; // Windows
static constexpr int TP_PARALLEL_POWER = 0;
}; // namespace fft1

// ------------------------------------------------------------
// Top Level Graph
// ------------------------------------------------------------

class fft_graph : public graph {
public:
private:
  // FFT class:
  using TT_FFT = xf::dsp::aie::fft::dit_1ch::fft_ifft_dit_1ch_graph<
      fft1::TT_TYPE, fft1::TT_TWIDDLE, fft1::TP_POINT_SIZE, fft1::TP_FFT_NIFFT,
      fft1::TP_SHIFT, fft1::TP_CASC_LEN, fft1::TP_DYN_PT_SIZE,
      fft1::TP_WINDOW_SIZE, fft1::TP_API, fft1::TP_PARALLEL_POWER>;

  TT_FFT fft_dut;

public:
  input_plio fft_i;
  output_plio fft_o;

  fft_graph(void) : fft_dut() {
    fft_i = input_plio::create("DataInFFT", plio_64_bits, "data/input.csv", 330);
    fft_o = output_plio::create("DataOutFFT", plio_64_bits, "data/output.csv", 330);
    auto in = connect<>(fft_i.out[0], fft_dut.in[0]);
    auto out = connect<>(fft_dut.out[0], fft_o.in[0]);
  }
};
