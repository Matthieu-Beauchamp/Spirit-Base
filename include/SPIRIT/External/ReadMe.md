This folder is meant to simplify our includes.

For every external library, our configurations macros 
are/will be handled by ../config.hpp.

To ensure that the macros are correctly passed to those libraries, 
we make a [lib_name].hpp header which looks like this:
```
#include "SPIRIT/Core/config.hpp

#include "[lib_name]/[...].hpp"
...
```
Then from our modules, we can include "SPIRIT/Core/External/[lib_name].hpp"

Note: to make this even better, we can/should use that header to 
forward declare what we need and make an associated .cpp file 
(in src/SPIRIT/Core/External/) which includes the library headers that 
implements our declared functions/classes.
This avoids as much pollution as possible when users include our headers 
and allows easy tracking of what we use.
