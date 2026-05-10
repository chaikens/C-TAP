#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ 
#else
#error Non Gnu C++? Cannot detect endianess or we're big endian and that's not supported
#endif


  
