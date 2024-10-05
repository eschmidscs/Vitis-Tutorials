#include "fft_graph.hpp"
#include "mygraph.h"

fft_graph fft_graph_top;
mygraph<512, 0, 4>  mygraph_top;

#ifdef __AIESIM__
// initialize and run the dataflow graph
int main(void) 
{
  mygraph_top.init();
  mygraph_top.run();
  mygraph_top.end();
  return 0;
}
#endif

