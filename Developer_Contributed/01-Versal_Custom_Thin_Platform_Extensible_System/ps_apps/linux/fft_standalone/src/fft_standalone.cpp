#include <chrono>
#include <cmath>
#include <cstdio>
#include <xrt/xrt_aie.h>
#include <xrt/xrt_device.h>
#include <xrt/xrt_graph.h>
#include <xrt/xrt_kernel.h>

using std::string;

struct cint16 {
  short real;
  short imag;
};

void FillInputBuffer(cint16 *data, int length = 512, int bin = 0, int ampl = 10, int blocks = 1)
{
  printf("Filling payload\n");
  for (int b = 0; b < blocks; b++) {
    for (int i = 0; i < length; i++) {

      float phase = 2.0f * 3.14159 * i * (bin + b) / (float)length;
      data[b * length + i].real = (int16_t)((float)ampl * cos(phase));
      data[b * length + i].imag = (int16_t)((float)ampl * sin(phase));
    }
  }
}

int CheckOutputBuffer(cint16 *data, int bin = 0, int ampl = 10, int blocks = 1)
{
  printf("Checking output...\n");

  int err = 0;

  for (int b = 0; b < blocks; b++) {
    for (int i = 0; i < 512; i++) {
      if (i == bin) {
        if ((data[b * 512 + i].real < 512.0 * ampl * 0.8) || (data[b * 512 + i].imag > 512.0 * ampl * 0.2)) {
          printf("Error in sample %d : (%d, %d)\n", i, data[i].real, data[i].imag);
          err++;
        }
      } else {
        if ((data[b * 512 + i].real > 512.0 * ampl * 0.2) || (data[b * 512 + i].imag > 512.0 * ampl * 0.2)) {
          printf("Error in sample %d : (%d, %d)\n", i, data[i].real, data[i].imag);
          err++;
        }
      }
    }
  }

  return err;
}

void log_elapsed(string txt, std::chrono::time_point<std::chrono::high_resolution_clock> ts_start,
                 std::chrono::time_point<std::chrono::high_resolution_clock> ts_stop)
{
  //printf("%s: %d\n", txt.c_str(), std::chrono::duration_cast<std::chrono::microseconds>(ts_stop - ts_start));
  std::cout << txt.c_str() << ": " << std::chrono::duration_cast<std::chrono::microseconds>(ts_stop - ts_start).count() << std::endl;
}

int main(int argc, char *argv[])
{
  int blocks = 1;
  int length = 512;
  int bin = 3;
  int ampl = 10;
  int buffer_size = blocks * length * sizeof(cint16);

  std::chrono::time_point<std::chrono::high_resolution_clock> ts_start, ts_stop;

  ts_start = std::chrono::high_resolution_clock::now();
  xrt::device device = xrt::device(0);
  xrt::uuid xclbin_uuid = device.load_xclbin(argv[1]);
  ts_stop = std::chrono::high_resolution_clock::now();
  log_elapsed("Elapsed time device/xclbin init", ts_start, ts_stop);

  ts_start = std::chrono::high_resolution_clock::now();
  xrt::kernel input_kernel(device, xclbin_uuid, "hls_fftb_reader:{read_fft}");
  xrt::graph fft_graph(device, xclbin_uuid, "fft_graph_top");
  xrt::kernel output_kernel(device, xclbin_uuid, "hls_fftb_writer:{write_fft}");
  ts_stop = std::chrono::high_resolution_clock::now();
  log_elapsed("Elapsed time kernel load", ts_start, ts_stop);

  ts_start = std::chrono::high_resolution_clock::now();
  xrt::bo input_buffer = xrt::bo(device, buffer_size, XCL_BO_FLAGS_NONE, input_kernel.group_id(0));
  FillInputBuffer(input_buffer.map<cint16 *>(), length, bin, ampl, blocks);
  input_buffer.sync(XCL_BO_SYNC_BO_TO_DEVICE);
  ts_stop = std::chrono::high_resolution_clock::now();
  log_elapsed("Elapsed time input buffer load", ts_start, ts_stop);

  ts_start = std::chrono::high_resolution_clock::now();
  xrt::bo output_buffer = xrt::bo(device, buffer_size, XCL_BO_FLAGS_NONE, output_kernel.group_id(0));
  ts_stop = std::chrono::high_resolution_clock::now();
  log_elapsed("Elapsed time output buffer allocate", ts_start, ts_stop);

  ts_start = std::chrono::high_resolution_clock::now();
  fft_graph.run();
  xrt::run output_run = output_kernel(output_buffer, nullptr, buffer_size / 8);
  xrt::run input_run = input_kernel(input_buffer, nullptr, buffer_size / 8);
  output_run.wait(1000);
  ts_stop = std::chrono::high_resolution_clock::now();
  log_elapsed("Elapsed time run and wait", ts_start, ts_stop);

  ts_start = std::chrono::high_resolution_clock::now();
  output_buffer.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
  int err = CheckOutputBuffer(output_buffer.map<cint16 *>(), bin, ampl, blocks);
  ts_stop = std::chrono::high_resolution_clock::now();
  log_elapsed("Elapsed time output buffer write", ts_start, ts_stop);
  printf("Found %d errors in result.\n", err);

  fft_graph.end();
  printf("Program end.\n");
};
