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
#include "json/document.h"
#include "json/writer.h"

NS_CC_BEGIN

namespace StringUtils
{

void addValueToJSON(const std::string &key, const Value &value, rapidjson::Document &d, rapidjson::Document::AllocatorType &allocator)
{
    rapidjson::Value jsonValue;
    if (!value.isNull())
    {
        const Value::Type type = value.getType();
        if (type == Value::Type::VECTOR)
        {
            rapidjson::Document documentArray;
            documentArray.SetArray();
            
            for (const Value &vectorValue : value.asValueVector())
            {
                addValueToJSON("", vectorValue, documentArray, allocator);
            }
            
            jsonValue = documentArray.Move();
        }
        else if (type == Value::Type::MAP)
        {
            rapidjson::Document documentObject;
            documentObject.SetObject();
            
            for (const std::pair<std::string, Value> &pair : value.asValueMap())
            {
                addValueToJSON(pair.first, pair.second, documentObject, allocator);
            }
            
            jsonValue = documentObject.Move();
        }
        else
        {
            switch (type)
            {
                case Value::Type::BYTE:
                    jsonValue = value.asByte();
                    break;
                case Value::Type::INTEGER:
                    jsonValue = value.asInt();
                    break;
                case Value::Type::UNSIGNED:
                    jsonValue = value.asUnsignedInt();
                    break;
                case Value::Type::FLOAT:
                    jsonValue = value.asFloat();
                    break;
                case Value::Type::DOUBLE:
                    jsonValue = value.asDouble();
                    break;
                case Value::Type::BOOLEAN:
                    jsonValue = value.asBool();
                    break;
                case Value::Type::STRING:
                    jsonValue = rapidjson::Value(value.asString().c_str(), allocator);
                    break;
                default:
                    CCASSERT(false, "Value type not supported");
                    break;
            }
        }
    }
    
    if (key.length() == 0)
    {
        d.PushBack(jsonValue, allocator);
    }
    else
    {
        d.AddMember(rapidjson::Value(key.c_str(), allocator), jsonValue, allocator);
    }
}

std::string getStringFromValueMap(const ValueMap &map)
{
    rapidjson::Document d;
    d.SetObject();
    
    for (const std::pair<std::string, Value> &pair : map)
    {
        addValueToJSON(pair.first, pair.second, d, d.GetAllocator());
    }
    
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    d.Accept(writer);
    
    return buffer.GetString();
}

std::string getStringFromValueVector(const ValueVector &vector)
{
    rapidjson::Document d;
    d.SetArray();
    
    for (const Value &value : vector)
    {
        addValueToJSON("", value, d, d.GetAllocator());
    }
    
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    d.Accept(writer);
    
    return buffer.GetString();
}

Value getValueFromJSON(rapidjson::Value &v, rapidjson::Document::AllocatorType &allocator)
{
    if (v.IsNull())
    {
        return Value::Null;
    }
    else if (v.IsObject())
    {
        ValueMap map;
        for (rapidjson::Value::ConstMemberIterator it = v.MemberBegin(); it != v.MemberEnd(); ++it)
        {
            const std::string key = it->name.GetString();
            rapidjson::Value value = rapidjson::Value(it->value, allocator);
            Value mapValue = getValueFromJSON(value, allocator);
            
            map.emplace(key, mapValue);
        }
        
        return Value(map);
    }
    else if (v.IsArray())
    {
        ValueVector vector;
        for (rapidjson::Value::ConstValueIterator it = v.Begin(); it != v.End(); ++it)
        {
            rapidjson::Value value = rapidjson::Value(*it, allocator);
            Value vectorValue = getValueFromJSON(value, allocator);
            
            vector.push_back(vectorValue);
        }
        
        return Value(vector);
    }
    else if (v.IsInt())
    {
        return Value(v.GetInt());
    }
    else if (v.IsUint())
    {
        return Value(v.GetUint());
    }
    else if (v.IsFloat())
    {
        return Value(v.GetFloat());
    }
    else if (v.IsBool())
    {
        return Value(v.GetBool());
    }
    else if (v.IsString())
    {
        return Value(v.GetString());
    }
    
    CCASSERT(false, "Unable to convert json value to cocos2d value");
    return Value::Null;
}

ValueMap getValueMapFromString(const std::string string)
{
    rapidjson::Document document;
    document.Parse(string.c_str());
    
    Value value = getValueFromJSON(document, document.GetAllocator());
    CCASSERT(value.getType() == Value::Type::MAP, "Unable to deserialize string to value map");
    
    return value.asValueMap();
}

ValueVector getValueVectorFromString(const std::string string)
{
    rapidjson::Document document;
    document.Parse(string.c_str());
    
    Value value = getValueFromJSON(document, document.GetAllocator());
    CCASSERT(value.getType() == Value::Type::VECTOR, "Unable to deserialize string to value vector");
    
    return value.asValueVector();
}

} // namespace StringUtils {

NS_CC_END
