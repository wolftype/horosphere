#pragma once

///@file hsMacro.hpp
///@brief RUN macro for conditional APP

namespace hs {

#define RUN(X) int main(int argc, char * argv[]) {\
  if (argc > 1) {\
    if (!strncmp(argv[1],"-n",2)){\
      printf("Network Broadcast\n");\
      hs::Simulator<X> app("192.168.10.255");\
      app.start();\
    } else if (!strncmp(argv[1],"-l",2)){\
      printf("%s \t", argv[1] );\
      printf("Local Broadcast\n");\
      hs::Simulator<X> app("127.0.0.1");\
      app.start();\
    } else if (!strncmp(argv[1],"-r",2)){\
      hs::RenderApp<X> app;\
      app.start();\
    }\
  } else {\
    printf("please feed an argument:\n-n for network\n-l for local\n-r for render\n");\
  }\
   return 0;\
}

} //hs::
