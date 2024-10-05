#include "graphs/fft_graph.hpp"

#include <iostream>

fft_graph fft_graph_top;

#if defined(__AIESIM__) || defined(__X86SIM__)

// initialize and run the dataflow graph
int main(void) {
  fft_graph_top.init();
  std::cout << "Init done" << std::endl;

  fft_graph_top.run(3);

  fft_graph_top.end();

  return 0;
}
#endif
