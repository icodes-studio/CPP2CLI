#include "rapidjson/document.h"
#include "rapidjson/reader.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/error/en.h"

using namespace rapidjson;

#include <string>
#include <vector>
#include <stack>
#include <iostream>

#define EXPECT_TRUE(statement) \
    assert(true == (statement))

#define EXPECT_FALSE(statement) \
    assert(false == (statement))

#define EXPECT_EQ(value1, value2) \
    assert((value1) == (value2))

#define EXPECT_NE(value1, value2) \
    assert((value1) != (value2))

#define EXPECT_THROW(statement, expected) \
    { \
    bool exception = false; \
    try { statement; } \
    catch (expected const& e) { \
    exception = true; std::cout << ">\t " << e.what() << std::endl; } \
    assert(exception); \
    }

#define XTEST(test_name) \
    class test_name { \
    public: static int Test() { \
    std::cout << "> XTEST: " << #test_name << std::endl; \
    OnTest(); return 0; } \
    private: static void OnTest(); }; \
    int result##test_name = test_name::Test(); \
    void test_name::OnTest()

XTEST(simpledom)
{
    // 1. Parse a JSON string into DOM.
    const char* json = "{\"project\":\"rapidjson\",\"stars\":10}";
    Document document;
    document.Parse(json);

    // 2. Modify it by DOM.
    Value& value = document["stars"];
    value.SetInt(value.GetInt() + 1);

    // 3. Stringify the DOM
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    document.Accept(writer);

    // Output {"project":"rapidjson","stars":11}
    std::cout << buffer.GetString() << std::endl;
}

struct MyHandler1
{
    bool Null()
    { 
        std::cout << "Null()" << std::endl;
        return true;
    }

    bool Bool(bool b)
    { 
        std::cout << "Bool(" << std::boolalpha << b << ")" << std::endl;
        return true;
    }

    bool Int(int i)
    { 
        std::cout << "Int(" << i << ")" << std::endl;
        return true;
    }

    bool Uint(unsigned u)
    { 
        std::cout << "Uint(" << u << ")" << std::endl;
        return true;
    }

    bool Int64(int64_t i)
    { 
        std::cout << "Int64(" << i << ")" << std::endl;
        return true;
    }

    bool Uint64(uint64_t u)
    { 
        std::cout << "Uint64(" << u << ")" << std::endl;
        return true;
    }

    bool Double(double d)
    { 
        std::cout << "Double(" << d << ")" << std::endl;
        return true;
    }

    bool RawNumber(const char* str, SizeType length, bool copy)
    {
        std::cout << "Number(" << str << ", " << length << ", " << std::boolalpha << copy << ")" << std::endl;
        return true;
    }

    bool String(const char* str, SizeType length, bool copy)
    {
        std::cout << "String(" << str << ", " << length << ", " << std::boolalpha << copy << ")" << std::endl;
        return true;
    }

    bool StartObject()
    { 
        std::cout << "StartObject()" << std::endl;
        return true;
    }

    bool Key(const char* str, SizeType length, bool copy)
    {
        std::cout << "Key(" << str << ", " << length << ", " << std::boolalpha << copy << ")" << std::endl;
        return true;
    }

    bool EndObject(SizeType memberCount)
    { 
        std::cout << "EndObject(" << memberCount << ")" << std::endl;
        return true;
    }

    bool StartArray()
    { 
        std::cout << "StartArray()" << std::endl;
        return true;
    }

    bool EndArray(SizeType elementCount)
    {
        std::cout << "EndArray(" << elementCount << ")" << std::endl;
        return true;
    }
};

XTEST(simplereader)
{
    const char json[] = " { \"hello\" : \"world\", \"t\" : true , \"f\" : false, \"n\": null, \"i\":123, \"pi\": 3.1416, \"a\":[1, 2, 3, 4] } ";

    Reader reader;
    MyHandler1 handler;
    StringStream stream(json);
    reader.Parse(stream, handler);
}

XTEST(simplewriter)
{
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);

    writer.StartObject();               // Between StartObject()/EndObject(), 
    writer.Key("hello");                // output a key,
    writer.String("world");             // follow by a value.
    writer.Key("t");
    writer.Bool(true);
    writer.Key("f");
    writer.Bool(false);
    writer.Key("n");
    writer.Null();
    writer.Key("i");
    writer.Uint(123);
    writer.Key("pi");
    writer.Double(3.1416);
    writer.Key("a");
    writer.StartArray();                // Between StartArray()/EndArray(),
    for (unsigned i = 0; i < 4; i++)
        writer.Uint(i);                 // all values are elements of the array.
    writer.EndArray();
    writer.EndObject();

    // {"hello":"world","t":true,"f":false,"n":null,"i":123,"pi":3.1416,"a":[0,1,2,3]}
    std::cout << buffer.GetString() << std::endl;
}

// If you can require C++11, you could use std::to_string here
template <typename T> std::string stringify(T x)
{
    return std::to_string(x);
    //std::stringstream ss;
    //ss << x;
    //return ss.str();
}

struct MyHandler2
{
    public: const char* type;
    public: std::string data;

    public: MyHandler2() : type(), data()
    {
    }

    public: bool Null()
    { 
        type = "Null";
        data.clear();
        return true;
    }

    public: bool Bool(bool b)
    { 
        type = "Bool:";
        data = b ? "true" : "false";
        return true;
    }

    public: bool Int(int i)
    { 
        type = "Int:"; 
        data = stringify(i);
        return true;
    }

    public: bool Uint(unsigned u)
    { 
        type = "Uint:"; 
        data = stringify(u); 
        return true;
    }

    public: bool Int64(int64_t i)
    { 
        type = "Int64:";
        data = stringify(i);
        return true;
    }

    public: bool Uint64(uint64_t u)
    {
        type = "Uint64:";
        data = stringify(u);
        return true;
    }

    public: bool Double(double d)
    { 
        type = "Double:";
        data = stringify(d);
        return true;
    }

    public: bool RawNumber(const char* str, SizeType length, bool)
    { 
        type = "Number:";
        data = std::string(str, length);
        return true;
    }

    public: bool String(const char* str, SizeType length, bool)
    { 
        type = "String:";
        data = std::string(str, length);
        return true;
    }

    public: bool StartObject()
    { 
        type = "StartObject";
        data.clear();
        return true;
    }

    public: bool Key(const char* str, SizeType length, bool)
    { 
        type = "Key:";
        data = std::string(str, length);
        return true;
    }

    public: bool EndObject(SizeType memberCount)
    {
        type = "EndObject:";
        data = stringify(memberCount);
        return true;
    }

    public: bool StartArray()
    {
        type = "StartArray";
        data.clear();
        return true;
    }

    public: bool EndArray(SizeType elementCount)
    {
        type = "EndArray:";
        data = stringify(elementCount);
        return true;
    }

    // noncopyable
    private: MyHandler2(const MyHandler2& noCopyConstruction);
    private: MyHandler2& operator = (const MyHandler2& noAssignment);
};

XTEST(simplepullreader)
{
    const char json[] = " { \"hello\" : \"world\", \"t\" : true , \"f\" : false, \"n\": null, \"i\":123, \"pi\": 3.1416, \"a\":[1, 2, 3, 4] } ";
    StringStream stream(json);

    Reader reader;
    MyHandler2 handler;
    reader.IterativeParseInit();
    while (!reader.IterativeParseComplete())
    {
        reader.IterativeParseNext<kParseDefaultFlags>(stream, handler);
        std::cout << handler.type << handler.data << std::endl;
    }
}

class Person
{
    private: std::string name_;
    private: unsigned age_;

    public: Person(const std::string& name, unsigned age) : name_(name), age_(age)
    {
    }

    public: Person(const Person& rhs) : name_(rhs.name_), age_(rhs.age_)
    {
    }

    public: virtual ~Person()
    {
    }

    public: Person& operator=(const Person& rhs)
    {
        name_ = rhs.name_;
        age_ = rhs.age_;
        return *this;
    }

    protected: template <typename Writer> 
    void Serialize(Writer& writer) const
    {
        // This base class just write out name-value pairs, without wrapping within an object.
        writer.String("name");
#if RAPIDJSON_HAS_STDSTRING
        writer.String(name_);
#else
        writer.String(name_.c_str(), static_cast<SizeType>(name_.length())); // Supplying length of string is faster.
#endif
        writer.String("age");
        writer.Uint(age_);
    }
};

class Education
{
    private: std::string school_;
    private: double GPA_;

    public: Education(const std::string& school, double GPA) : school_(school), GPA_(GPA)
    {
    }

    public: Education(const Education& rhs) : school_(rhs.school_), GPA_(rhs.GPA_)
    {
    }

    public: template <typename Writer>
    void Serialize(Writer& writer) const
    {
        writer.StartObject();
        writer.String("school");
#if RAPIDJSON_HAS_STDSTRING
        writer.String(school_);
#else
        writer.String(school_.c_str(), static_cast<SizeType>(school_.length()));
#endif
        writer.String("GPA");
        writer.Double(GPA_);
        writer.EndObject();
    }
};

class Dependent : public Person
{
    private: Education* education_;

    public: Dependent(const std::string& name, unsigned age, Education* education = 0) : Person(name, age), education_(education)
    {
    }

    public: Dependent(const Dependent& rhs) : Person(rhs), education_(0)
    { 
        education_ = (rhs.education_ == 0) ? 0 : new Education(*rhs.education_);
    }

    public: ~Dependent()
    {
        delete education_;
    }

    public: Dependent& operator=(const Dependent& rhs)
    {
        if (this == &rhs)
            return *this;

        delete education_;
        education_ = (rhs.education_ == 0) ? 0 : new Education(*rhs.education_);
        return *this;
    }

    public: template <typename Writer>
    void Serialize(Writer& writer) const
    {
        writer.StartObject();
        Person::Serialize(writer);
        writer.String("education");
        if (education_)
            education_->Serialize(writer);
        else
            writer.Null();
        writer.EndObject();
    }
};

class Employee : public Person
{
    private: std::vector<Dependent> dependents_;
    private: bool married_;

    public: Employee(const std::string& name, unsigned age, bool married) : Person(name, age), dependents_(), married_(married)
    {
    }

    public: Employee(const Employee& rhs) : Person(rhs), dependents_(rhs.dependents_), married_(rhs.married_)
    {
    }

    public: virtual ~Employee()
    {
    }

    public: Employee& operator=(const Employee& rhs)
    {
        static_cast<Person&>(*this) = rhs;
        dependents_ = rhs.dependents_;
        married_ = rhs.married_;
        return *this;
    }

    public: void AddDependent(const Dependent& dependent)
    {
        dependents_.push_back(dependent);
    }

    public: template <typename Writer>
    void Serialize(Writer& writer) const
    {
        writer.StartObject();
        Person::Serialize(writer);
        writer.String("married");
        writer.Bool(married_);
        writer.String(("dependents"));
        writer.StartArray();
        for (std::vector<Dependent>::const_iterator dependentItr = dependents_.begin(); dependentItr != dependents_.end(); ++dependentItr)
            dependentItr->Serialize(writer);
        writer.EndArray();
        writer.EndObject();
    }
};

XTEST(serialize)
{
    std::vector<Employee> employees;
    employees.push_back(Employee("Milo YIP", 34, true));
    employees.back().AddDependent(Dependent("Lua YIP", 3, new Education("Happy Kindergarten", 3.5)));
    employees.back().AddDependent(Dependent("Mio YIP", 1));
    employees.push_back(Employee("Percy TSE", 30, false));

    StringBuffer buffer;
    PrettyWriter<StringBuffer> writer(buffer);

    writer.StartArray();
    for (std::vector<Employee>::const_iterator i = employees.begin(); i != employees.end(); ++i)
        i->Serialize(writer);
    writer.EndArray();

    puts(buffer.GetString());
    //[
    //    {
    //        "name": "Milo YIP",
    //        "age": 34,
    //        "married": true,
    //        "dependents": [
    //            {
    //                "name": "Lua YIP",
    //                "age": 3,
    //                "education": {
    //                    "school": "Happy Kindergarten",
    //                    "GPA": 3.5
    //                }
    //            },
    //            {
    //                "name": "Mio YIP",
    //                "age": 1,
    //                "education": null
    //            }
    //        ]
    //    },
    //    {
    //        "name": "Percy TSE",
    //        "age": 30,
    //        "married": false,
    //        "dependents": []
    //    }
    //]
}

XTEST(pretty)
{
    // Prepare input stream.
    char readBuffer[65536];
    FILE* read; fopen_s(&read, "input.json", "r");
    FileReadStream is(/*stdin*/read, readBuffer, sizeof(readBuffer));

    // Prepare writer and output stream.
    char writeBuffer[65536];
    FILE* write; fopen_s(&write, "output.json", "w");
    FileWriteStream os(/*stdout*/write, writeBuffer, sizeof(writeBuffer));
    PrettyWriter<FileWriteStream> writer(os);

    // JSON reader parse from the input stream and let writer generate the output.
    Reader reader;
    if (!reader.Parse<kParseValidateEncodingFlag>(is, writer))
    {
        fprintf(
            stderr,
            "\nError(%u): %s\n",
            static_cast<unsigned>(reader.GetErrorOffset()),
            GetParseError_En(reader.GetParseErrorCode()));
    }

    fclose(read);
    fclose(write);
}

template <typename OutputHandler>
class FilterKeyHandler
{
    private: OutputHandler& outputHandler_;
    private: const char* keyString_;
    private: const SizeType keyLength_;
    private: unsigned filterValueDepth_;
    private: std::stack<SizeType> filteredKeyCount_;

    public: FilterKeyHandler(OutputHandler& outputHandler, const char* keyString, SizeType keyLength) : 
        outputHandler_(outputHandler),
        keyString_(keyString),
        keyLength_(keyLength),
        filterValueDepth_(),
        filteredKeyCount_()
    {
    }

    public: bool Null()
    {
        return filterValueDepth_ > 0 ? EndValue() : outputHandler_.Null() && EndValue();
    }

    public: bool Bool(bool b)
    { 
        return filterValueDepth_ > 0 ? EndValue() : outputHandler_.Bool(b) && EndValue();
    }

    public: bool Int(int i)
    { 
        return filterValueDepth_ > 0 ? EndValue() : outputHandler_.Int(i) && EndValue();
    }

    public: bool Uint(unsigned u)
    { 
        return filterValueDepth_ > 0 ? EndValue() : outputHandler_.Uint(u) && EndValue();
    }

    public: bool Int64(int64_t i)
    {
        return filterValueDepth_ > 0 ? EndValue() : outputHandler_.Int64(i) && EndValue();
    }

    public: bool Uint64(uint64_t u)
    {
        return filterValueDepth_ > 0 ? EndValue() : outputHandler_.Uint64(u) && EndValue();
    }

    public: bool Double(double d)
    {
        return filterValueDepth_ > 0 ? EndValue() : outputHandler_.Double(d) && EndValue();
    }

    public: bool RawNumber(const char* str, SizeType len, bool copy)
    {
        return filterValueDepth_ > 0 ? EndValue() : outputHandler_.RawNumber(str, len, copy) && EndValue();
    }

    public: bool String(const char* str, SizeType len, bool copy)
    {
        return filterValueDepth_ > 0 ? EndValue() : outputHandler_.String(str, len, copy) && EndValue();
    }

    public: bool StartObject()
    {
        if (filterValueDepth_ > 0)
        {
            filterValueDepth_++;
            return true;
        }
        else
        {
            filteredKeyCount_.push(0);
            return outputHandler_.StartObject();
        }
    }

    public: bool Key(const char* str, SizeType len, bool copy)
    {
        if (filterValueDepth_ > 0)
        {
            return true;
        }
        else if (len == keyLength_ && std::memcmp(str, keyString_, len) == 0)
        {
            filterValueDepth_ = 1;
            return true;
        }
        else
        {
            ++filteredKeyCount_.top();
            return outputHandler_.Key(str, len, copy);
        }
    }

    public: bool EndObject(SizeType)
    {
        if (filterValueDepth_ > 0)
        {
            filterValueDepth_--;
            return EndValue();
        }
        else
        {
            // Use our own filtered memberCount
            SizeType memberCount = filteredKeyCount_.top();
            filteredKeyCount_.pop();
            return outputHandler_.EndObject(memberCount) && EndValue();
        }
    }

    public: bool StartArray()
    {
        if (filterValueDepth_ > 0)
        {
            filterValueDepth_++;
            return true;
        }
        else
        {
            return outputHandler_.StartArray();
        }
    }

    public: bool EndArray(SizeType elementCount)
    {
        if (filterValueDepth_ > 0)
        {
            filterValueDepth_--;
            return EndValue();
        }
        else
        {
            return outputHandler_.EndArray(elementCount) && EndValue();
        }
    }

    private: bool EndValue()
    {
        // Just at the end of value after filtered key
        if (filterValueDepth_ == 1)
            filterValueDepth_ = 0;

        return true;
    }

    // noncopyable
    private: FilterKeyHandler(const FilterKeyHandler&);
    private: FilterKeyHandler& operator = (const FilterKeyHandler&);
};

XTEST(filterkey)
{
    // Prepare input stream.
    char readBuffer[65536];
    FILE* read; fopen_s(&read, "input.json", "r");
    FileReadStream is(/*stdin*/read, readBuffer, sizeof(readBuffer));

    // Prepare JSON writer and output stream.
    char writeBuffer[65536];
    FILE* write; fopen_s(&write, "output.json", "w");
    FileWriteStream os(/*stdout*/write, writeBuffer, sizeof(writeBuffer));
    Writer<FileWriteStream> writer(os);

    // Prepare Filter
    const char* key = "dependents";
    FilterKeyHandler<Writer<FileWriteStream>> filter(writer, key, static_cast<SizeType>(strlen(key)));

    // JSON reader parse from the input stream, filter handler filters the events, and forward to writer.
    // i.e. the events flow is: reader -> filter -> writer
    Reader reader;
    if (!reader.Parse<kParseValidateEncodingFlag>(is, filter))
    {
        fprintf(
            stderr,
            "\nError(%u): %s\n",
            static_cast<unsigned>(reader.GetErrorOffset()),
            GetParseError_En(reader.GetParseErrorCode()));
    }

    fclose(read);
    fclose(write);
}

template <typename InputStream>
class FilterKeyReader
{
    private: InputStream& is_;
    private: const char* keyString_;
    private: const SizeType keyLength_;
    private: ParseResult parseResult_;

    public: FilterKeyReader(InputStream& is, const char* keyString, SizeType keyLength) :
        is_(is),
        keyString_(keyString),
        keyLength_(keyLength),
        parseResult_()
    {
    }

    // SAX event flow: reader -> filter -> handler
    public: template <typename Handler>
    bool operator()(Handler& handler)
    {
        FilterKeyHandler<Handler> filter(handler, keyString_, keyLength_);
        Reader reader;
        parseResult_ = reader.Parse(is_, filter);
        return parseResult_;
    }

    public: const ParseResult& GetParseResult() const
    { 
        return parseResult_;
    }

    private: FilterKeyReader(const FilterKeyReader&);
    private: FilterKeyReader& operator = (const FilterKeyReader&);
};

XTEST(filterkeydom)
{
    // Prepare input stream.
    char readBuffer[65536];
    FILE* read; fopen_s(&read, "input.json", "r");
    FileReadStream is(/*stdin*/read, readBuffer, sizeof(readBuffer));

    // Prepare Filter
    const char* key = "dependents";
    FilterKeyReader<FileReadStream> reader(is, key, static_cast<SizeType>(strlen(key)));

    // Populates the filtered events from reader
    Document document;
    document.Populate(reader);
    ParseResult result = reader.GetParseResult();
    if (!result)
    {
        fprintf(stderr, "\nError(%u): %s\n", static_cast<unsigned>(result.Offset()), GetParseError_En(result.Code()));
        fclose(read);
        return;
    }

    // Prepare JSON writer and output stream.
    char writeBuffer[65536];
    FILE* write; fopen_s(&write, "output.json", "w");
    FileWriteStream os(/*stdout*/write, writeBuffer, sizeof(writeBuffer));
    Writer<FileWriteStream> writer(os);

    // Write the document to standard output
    document.Accept(writer);

    fclose(read);
    fclose(write);
}

XTEST(tutorial)
{
    ///
    /// 1. PARSE A JSON TEXT STRING TO A DOCUMENT. ///
    ///
 
    const char json[] = " { \"hello\" : \"world\", \"t\" : true , \"f\" : false, \"n\": null, \"i\":123, \"pi\": 3.1416, \"a\":[1, 2, 3, 4] } ";
    printf("Original JSON:\n %s\n", json);

    // Default template parameter uses UTF8 and MemoryPoolAllocator.
    Document document;  

#if 0
    // "normal" parsing, decode strings to new buffers. Can use other input stream via ParseStream().
    if (document.Parse(json).HasParseError())
        return;
#else
    // In-situ parsing, decode strings directly in the source string. Source must be string.
    char buffer[sizeof(json)];
    memcpy(buffer, json, sizeof(json));
    if (document.ParseInsitu(buffer).HasParseError())
        return;
#endif

    printf("\nParsing to document succeeded.\n");

    ///
    /// 2. ACCESS VALUES IN DOCUMENT. ///
    ///

    printf("\nAccess values in document:\n");

    // Document is a JSON value represents the root of DOM. Root can be either an object or array.
    EXPECT_EQ(document.GetType(), Type::kObjectType);
    EXPECT_TRUE(document.IsObject());
    EXPECT_FALSE(document.IsArray());

    EXPECT_TRUE(document.HasMember("hello"));
    EXPECT_TRUE(document["hello"].IsString());
    printf("hello = %s\n", document["hello"].GetString());

    // Since version 0.2, you can use single lookup to check the existing of member and its value:
    Value::MemberIterator hello = document.FindMember("hello");
    EXPECT_NE(hello, document.MemberEnd());
    EXPECT_TRUE(hello->value.IsString());
    EXPECT_TRUE(strcmp("world", hello->value.GetString()) == 0);
    (void)hello;

    // JSON true/false are bool. Can also uses more specific function IsTrue().
    EXPECT_TRUE(document["t"].IsBool());
    printf("t = %s\n", document["t"].GetBool() ? "true" : "false");

    EXPECT_TRUE(document["f"].IsBool());
    printf("f = %s\n", document["f"].GetBool() ? "true" : "false");

    printf("n = %s\n", document["n"].IsNull() ? "null" : "?");

    // Number is a JSON type, but C++ needs more specific type.
    // In this case, IsUint()/IsInt64()/IsUint64() also return true.
    EXPECT_TRUE(document["i"].IsNumber());
    EXPECT_TRUE(document["i"].IsInt());
    EXPECT_TRUE(document["i"].IsUint());
    EXPECT_TRUE(document["i"].IsInt64());
    EXPECT_TRUE(document["i"].IsUint64());

    // Alternative (int)document["i"]
    printf("i = %d\n", document["i"].GetInt());
    // but... NOT working!
    //printf("i = %d\n", (int)document["i"]);

    EXPECT_TRUE(document["pi"].IsNumber());
    EXPECT_TRUE(document["pi"].IsDouble());
    printf("pi = %g\n", document["pi"].GetDouble());

    {
        // Using a reference for consecutive access is handy and faster.
        const Value& a = document["a"]; 
        EXPECT_TRUE(a.IsArray());

        // rapidjson uses SizeType instead of size_t.
        for (SizeType i = 0; i < a.Size(); i++)
            printf("a[%d] = %d\n", i, a[i].GetInt());

        int y = a[0].GetInt();
        (void)y;

        // Iterating array with iterators
        printf("a = ");
        for (Value::ConstValueIterator itr = a.Begin(); itr != a.End(); ++itr)
            printf("%d ", itr->GetInt());
        printf("\n");
    }

    // Iterating object members
    // enum Type {
    //    kNullType = 0,      //!< null
    //    kFalseType = 1,     //!< false
    //    kTrueType = 2,      //!< true
    //    kObjectType = 3,    //!< object
    //    kArrayType = 4,     //!< array 
    //    kStringType = 5,    //!< string
    //    kNumberType = 6     //!< number
    // };
    static const char* kTypeNames[] = { "Null", "False", "True", "Object", "Array", "String", "Number" };
    for (Value::ConstMemberIterator itr = document.MemberBegin(); itr != document.MemberEnd(); ++itr)
        printf("Type of member %s is %s\n", itr->name.GetString(), kTypeNames[itr->value.GetType()]);

    ///
    /// 3. Modify values in document. ///
    /// 

    // Change i to a bigger number
    {
        // compute factorial of 20
        uint64_t f20 = 1;
        for (uint64_t j = 1; j <= 20; j++)
            f20 *= j;

        // Alternate form: document["i"].SetUint64(f20)
        document["i"] = f20;

        // No longer can be cast as int or uint.
        EXPECT_FALSE(document["i"].IsInt());
    }

    // Adding values to array.
    {
        // This time we uses non-const reference.
        Value& a = document["a"];
        Document::AllocatorType& allocator = document.GetAllocator();
        
        // May look a bit strange, allocator is needed for potentially realloc. We normally uses the document's.
        for (int i = 5; i <= 10; i++)
            a.PushBack(i, allocator);

        // Fluent API
        a.PushBack("Lua", allocator).PushBack("Mio", allocator);
    }

    // Making string values.
    // This version of SetString() just store the pointer to the string.
    // So it is for literal and string that exists within value's life-cycle.
    {
        // This will invoke strlen()
        document["hello"] = "rapidjson";
        printf("hello = %s\n", document["hello"].GetString());

        // Faster version:
        document["hello"].SetString("rapidjson", 9);
        printf("hello = %s\n", document["hello"].GetString());

        // test for value's life-cycle.
        char rapidjson[] = "rapidjson";
        document["hello"].SetString(rapidjson, 9);
        EXPECT_TRUE(strcmp(document["hello"].GetString(), "rapidjson") == 0);
        strcpy_s(rapidjson, "world");
        EXPECT_TRUE(strcmp(document["hello"].GetString(), "world") == 0);
    }

    // This version of SetString() needs an allocator, which means it will allocate a new buffer and copy the the string into the buffer.
    Value author;
    {
        char buffer2[10];
        int len = sprintf_s(buffer2, "%s %s", "Milo", "Yip");  // synthetic example of dynamically created string.

        author.SetString(buffer2, static_cast<SizeType>(len), document.GetAllocator());
        // Shorter but slower version:
        // document["hello"].SetString(buffer, document.GetAllocator());

        // Constructor version: 
        // Value author(buffer, len, document.GetAllocator());
        // Value author(buffer, document.GetAllocator());
        memset(buffer2, 0, sizeof(buffer2)); // For demonstration purpose.
    }
    EXPECT_TRUE(author.IsString());
    // Variable 'buffer' is unusable now but 'author' has already made a copy.
    document.AddMember("author", author, document.GetAllocator());
    // Move semantic for assignment. After this variable is assigned as a member, the variable becomes null.
    EXPECT_TRUE(author.IsNull());

    ///
    /// 4. Stringify JSON ///
    /// 

    printf("\nModified JSON with reformatting:\n");

    StringBuffer sb;
    PrettyWriter<StringBuffer> writer(sb);

    // Accept() traverses the DOM and generates Handler events.
    document.Accept(writer);
    puts(sb.GetString());
}