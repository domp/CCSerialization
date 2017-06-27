/****************************************************************************
Copyright (c) 2017 domp http://github.com/domp

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
****************************************************************************/

#include "CCSerialization.h"
#include "json.hpp"

using json = nlohmann::json;

NS_CC_BEGIN

namespace StringUtils
{

void addValueToJSON(const std::string &key, const Value &value, json &j)
{
    if (value.isNull())
    {
        j.emplace(key, nullptr);
    }
    else
    {
        const Value::Type type = value.getType();
        if (type == Value::Type::VECTOR)
        {
            json jsonVector;
            for (Value vectorValue : value.asValueVector())
            {
                addValueToJSON("", vectorValue, jsonVector);
            }
            
            if (key.length() == 0)
            {
                j.push_back(jsonVector);
            }
            else
            {
                j.emplace(key, jsonVector);
            }
        }
        else if (type == Value::Type::MAP)
        {
            json jsonMap;
            for (std::pair<std::string, Value> pair : value.asValueMap())
            {
                addValueToJSON(pair.first, pair.second, jsonMap);
            }
            
            if (key.length() == 0)
            {
                j.push_back(jsonMap);
            }
            else
            {
                j.emplace(key, jsonMap);
            }
        }
        else
        {
            switch (type)
            {
                case Value::Type::BYTE:
                    if (key.length() == 0) j.push_back(value.asByte());
                    else j.emplace(key, value.asByte());
                    break;
                case Value::Type::INTEGER:
                    if (key.length() == 0) j.push_back(value.asInt());
                    else j.emplace(key, value.asInt());
                    break;
                case Value::Type::UNSIGNED:
                    if (key.length() == 0) j.push_back(value.asUnsignedInt());
                    else j.emplace(key, value.asUnsignedInt());
                    break;
                case Value::Type::FLOAT:
                    if (key.length() == 0) j.push_back(value.asFloat());
                    else j.emplace(key, value.asFloat());
                    break;
                case Value::Type::DOUBLE:
                    if (key.length() == 0) j.push_back(value.asDouble());
                    else j.emplace(key, value.asDouble());
                    break;
                case Value::Type::BOOLEAN:
                    if (key.length() == 0) j.push_back(value.asBool());
                    else j.emplace(key, value.asBool());
                    break;
                case Value::Type::STRING:
                    if (key.length() == 0) j.push_back(value.asString());
                    else j.emplace(key, value.asString());
                    break;
                default:
                    CCASSERT(false, "Value type not supported");
                    break;
            }
        }
    }
}

std::string getStringFromValueMap(const ValueMap &map)
{
    json j;
    for (std::pair<std::string, Value> pair : map)
    {
        addValueToJSON(pair.first, pair.second, j);
    }
    
    return j.dump();
}

std::string getStringFromValueVector(const ValueVector &vector)
{
    json j;
    for (Value value : vector)
    {
        addValueToJSON("", value, j);
    }
    
    return j.dump();
}

Value getValueFromJSON(json &j)
{
    if (j.is_null())
    {
        return Value::Null;
    }
    else if (j.is_object())
    {
        ValueMap map;
        for (json::iterator it = j.begin(); it != j.end(); ++it)
        {
            const std::string key = it.key();
            json element = j.at(key);
            Value mapValue = getValueFromJSON(element);
            
            map.emplace(key, mapValue);
        }
        
        return Value(map);
    }
    else if (j.is_array())
    {
        ValueVector vector;
        for (json element : j)
        {
            Value vectorValue = getValueFromJSON(element);
            vector.push_back(vectorValue);
        }
        
        return Value(vector);
    }
    else if (j.is_number_integer())
    {
        return Value(j.get<int>());
    }
    else if (j.is_number_unsigned())
    {
        return Value(j.get<unsigned int>());
    }
    else if (j.is_number_float())
    {
        return Value(j.get<float>());
    }
    else if (j.is_boolean())
    {
        return Value(j.get<bool>());
    }
    else if (j.is_string())
    {
        return Value(j.get<std::string>());
    }
    
    CCASSERT(false, "Unable to convert json object to value");
    return Value::Null;
}

ValueMap getValueMapFromString(const std::string string)
{
    json j = json::parse(string);
    Value value = getValueFromJSON(j);
    CCASSERT(value.getType() == Value::Type::MAP, "Unable to deserialize string to value map");
    
    return value.asValueMap();
}

ValueVector getValueVectorFromString(const std::string string)
{
    json j = json::parse(string);
    Value value = getValueFromJSON(j);
    CCASSERT(value.getType() == Value::Type::VECTOR, "Unable to deserialize string to value vector");
    
    return value.asValueVector();
}

} // namespace StringUtils {

NS_CC_END
