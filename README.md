# base64
Simple, open source, header-only base64 encoder

# Usage
```cpp
#include <string>
#include <iostream>
#include "base64.hpp"

std::string encoded = base64::encode("hello, world");

std::cout << "encoded: " << encoded << std::endl
  << "decoded: " << base64::decode(encoded);
  
// Inplace decoding
base64::decode_inplace(encoded);

```

# License
The header file is put into the public domain, so everyone is allowed to do whatever they want.
