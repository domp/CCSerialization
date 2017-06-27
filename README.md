# CCSerialization

**Compatible with cocos2d-x v3.15**

Simply add the files to your project and include the header:

```
#include "CCSerialization.h"
```

To serialize a value map to a string, call the following method:

```
ValueMap map;
map.emplace("key1", Value(1));
map.emplace("key2", Value(2));
map.emplace("key3", Value(3));
    
std::string string = StringUtils::getStringFromValueMap(map);
```

After that you can deserialize a string to a value map again:

```
ValueMap map = StringUtils::getValueMapFromString(string);
```

The same thing also works for value vectors by calling `getStringFromValueVector()` and `getValueVectorFromString()`.

**Limitations:**

* JSON for Modern C++ which is used for serialization uses certain `std` functions that are not available when compiling for android using the default cocos2d-x `APP_STL gnustl_static`. Switching to c++_static is needed in order to successfully compile for android.
* All value types of CCValue are supported, except for int key maps. This is because there is no way to determine if a map is a value map or an int key map while deserializing since both could have only integers as keys.

