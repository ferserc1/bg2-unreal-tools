
#include "Bg2MeshParser.h"

#include <iostream>

void writeIntegerValue(std::fstream & file, int value, bool swapBytes)
{
    if (!swapBytes) file.write(reinterpret_cast<const char *>(&value), sizeof(int));
    else {
        const char * c_value = reinterpret_cast<const char*>(&value);
        file.write(&c_value[3], 1);
        file.write(&c_value[2], 1);
        file.write(&c_value[1], 1);
        file.write(&c_value[0], 1);
    }
}

int readIntegerValue(std::fstream & file, int & value, bool swapBytes)
{
    int readed = 0;
    if (!swapBytes)
    {
        readed = static_cast<int>(file.read(reinterpret_cast<char *>(&value), sizeof(int)).gcount());
    }
    else
    {
        union {
            int integer;
            char byte[4];
        } r_value;
        file.read(&r_value.byte[3], 1);
        file.read(&r_value.byte[2], 1);
        file.read(&r_value.byte[1], 1);
        readed = static_cast<int>(file.read(&r_value.byte[0], 1).gcount());
        value = r_value.integer;
    }
    return readed;
}

void writeFloatValue(std::fstream & file, float value, bool swapBytes)
{
    if (!swapBytes)
    {
        file.write(reinterpret_cast<const char *>(&value), sizeof(float));
    }
    else
    {
        const char * c_value = reinterpret_cast<const char*>(&value);
        file.write(&c_value[3], 1);
        file.write(&c_value[2], 1);
        file.write(&c_value[1], 1);
        file.write(&c_value[0], 1);
    }
}

int readFloatValue(std::fstream & file, float & value, bool swapBytes)
{
    int readed = 0;
    if (!swapBytes)
    {
        readed = static_cast<int>(file.read(reinterpret_cast<char *>(&value), sizeof(float)).gcount());
    }
    else
    {
        union {
            float floating;
            char byte[4];
        } r_value;
        file.read(&r_value.byte[3], 1);
        file.read(&r_value.byte[2], 1);
        file.read(&r_value.byte[1], 1);
        readed = static_cast<int>(file.read(&r_value.byte[0], 1).gcount());
        value = r_value.floating;
    }
    return readed;
}

void writeStringValue(std::fstream & file, const std::string & str, bool swapBytes)
{
    writeIntegerValue(file, (int)str.length(), swapBytes);
    const char * buffer = str.c_str();
    file.write(buffer, str.length());
}

void writeFloatArray(std::fstream & file, const std::vector<float> fArray, bool swapBytes)
{
    writeIntegerValue(file, static_cast<int>(fArray.size()),swapBytes);
    for (int i=0;i<static_cast<int>(fArray.size());++i)
    {
        writeFloatValue(file, fArray[i],swapBytes);
    }
}

void writeIntegerArray(std::fstream & file, const std::vector<int> iArray, bool swapBytes)
{
    writeIntegerValue(file, static_cast<int>(iArray.size()),swapBytes);
    for (int i=0;i<static_cast<int>(iArray.size());++i)
    {
        writeIntegerValue(file, iArray[i],swapBytes);
    }
}

void writeIntegerArray(std::fstream & file, const std::vector<unsigned int> iArray, bool swapBytes)
{
    writeIntegerValue(file, static_cast<int>(iArray.size()),swapBytes);
    for (int i=0;i<static_cast<int>(iArray.size());++i)
    {
        writeIntegerValue(file, iArray[i],swapBytes);
    }
}

int readFloatArray(std::fstream & file, std::vector<float> & fArray, bool swapBytes)
{
    int size = 0;
    int readed = 0;
    readed = readIntegerValue(file, size, swapBytes);
    if (readed == 0) return 0;
    fArray.reserve(size);
    float value;

    for (int i=0;i<size;++i)
    {
        readed += readFloatValue(file, value, swapBytes);
        fArray.push_back(value);
    }

    return readed;
}

int readIntegerArray(std::fstream & file, std::vector<int> & iArray, bool swapBytes)
{
    int size = 0;
    int readed = 0;
    readed = readIntegerValue(file, size, swapBytes);
    if (readed == 0) return 0;
    iArray.reserve(size);
    int value;

    for (int i=0;i<size;++i)
    {
        readed += readIntegerValue(file, value,swapBytes);
        iArray.push_back(value);
    }
    return readed;
}

int readIntegerArray(std::fstream & file, std::vector<unsigned int> & iArray, bool swapBytes)
{
    int size = 0;
    int readed = 0;
    readed = readIntegerValue(file, size, swapBytes);
    if (readed == 0) return 0;
    iArray.reserve(size);
    int value;

    for (int i=0;i<size;++i)
    {
        readed += readIntegerValue(file, value, swapBytes);
        iArray.push_back(value);
    }

    return readed;
}

int readStringValue(std::fstream & file, std::string & str, bool swapBytes)
{
    int stringSize;
    int readed = 0;
    char * buffer;
    readed = readIntegerValue(file, stringSize,swapBytes);

    if (readed == 0)
    {
        return 0;
    }

    buffer = new char[stringSize + 1];
    readed += static_cast<int>(file.read(buffer, stringSize).gcount());
    buffer[stringSize] = '\0';
    str = buffer;
    delete[] buffer;

    return readed;
}


Bg2MeshParser::Bg2MeshParser()
    :mMode(Bg2MeshParser::kModeClosed)
{
}

Bg2MeshParser::~Bg2MeshParser()
{
    Close();
}

bool Bg2MeshParser::Open(const std::string &path, OpenMode mode)
{
    Close();
    mStream.open(path,((mode==OpenMode::kModeRead) ? std::ios::in:std::ios::out) | std::ios::binary );
    if (mStream.is_open())
    {
        mMode = mode;
    }
    return mMode!=kModeClosed;
}

void Bg2MeshParser::Close()
{
    if (mStream.is_open())
    {
        mStream.close();
    }
    mMode = kModeClosed;
}

bool Bg2MeshParser::WriteByte(unsigned char byte)
{
    if (mMode != kModeWrite)
    {
        return false;
    }
    mStream.write(reinterpret_cast<char*>(&byte), 1);
    return true;
}

bool Bg2MeshParser::WriteBlock(Bg2MeshParser::BlockType b)
{
    if (mMode != kModeWrite)
    {
        return false;
    }
    WriteInteger(static_cast<int>(b));
    return true;
}

bool Bg2MeshParser::WriteInteger(int value)
{
    if (mMode != kModeWrite)
    {
        return false;
    }

    writeIntegerValue(mStream, value, mSwapBytes);
    return true;
}

bool Bg2MeshParser::WriteFloat(float value)
{
    if (mMode != kModeWrite)
    {
        return false;
    }

    writeFloatValue(mStream,value, mSwapBytes);
    return true;
}

bool Bg2MeshParser::WriteString(const std::string & str)
{
    if (mMode != kModeWrite)
    {
        return false;
    }

    writeStringValue(mStream,str, mSwapBytes);
    return true;
}

bool Bg2MeshParser::WriteArray(const std::vector<float> array)
{
    if (mMode != kModeWrite)
    {
        return false;
    }

    writeFloatArray(mStream,array, mSwapBytes);
    return true;
}

bool Bg2MeshParser::WriteArray(const std::vector<int> array)
{
    if (mMode != kModeWrite)
    {
        return false;
    }

    writeIntegerArray(mStream, array, mSwapBytes);
    return true;
}

bool Bg2MeshParser::WriteArray(const std::vector<unsigned int> array)
{
    if (mMode != kModeWrite)
    {
        return false;
    }

    writeIntegerArray(mStream, array, mSwapBytes);
    return true;
}

bool Bg2MeshParser::ReadByte(unsigned char & byte)
{
    if (mMode != kModeRead)
    {
        return false;
    }

    return mStream.read(reinterpret_cast<char*>(&byte), 1).gcount()==1;
}

bool Bg2MeshParser::ReadBlock(Bg2MeshParser::BlockType & b) 
{
    if (mMode != kModeRead)
    {
        return false;
    }

    return ReadInteger(reinterpret_cast<int&>(b))!=0;
}

bool Bg2MeshParser::ReadInteger(int & value)
{
    if (mMode != kModeRead)
    {
        return false;
    }

    return readIntegerValue(mStream, value, mSwapBytes)!=0;
}

bool Bg2MeshParser::ReadFloat(float & value) 
{
    if (mMode != kModeRead)
    {
        return false;
    }

    return readFloatValue(mStream, value, mSwapBytes)!=0;
}

bool Bg2MeshParser::ReadString(std::string & str)
{
    if (mMode != kModeRead)
    {
        return false;
    }

    return readStringValue(mStream, str, mSwapBytes)!=0;
}

bool Bg2MeshParser::ReadArray(std::vector<float> & array)
{
    if (mMode != kModeRead)
    {
        return false;
    }

    return readFloatArray(mStream,array, mSwapBytes)!=0;
}

bool Bg2MeshParser::ReadArray(std::vector<int> & array)
{
    if (mMode != kModeRead)
    {
        return false;
    }

    return readIntegerArray(mStream, array, mSwapBytes)!=0;
}

bool Bg2MeshParser::ReadArray(std::vector<unsigned int> & array)
{
    if (mMode != kModeRead)
    {
        return false;
    }

    return readIntegerArray(mStream, array, mSwapBytes)!=0;
}

bool Bg2MeshParser::IsBigEndian()
{
    unsigned int v = 0x01020304;
    return reinterpret_cast<unsigned char*>(&v)[0] == 1;;
}

bool Bg2MeshParser::IsLittleEndian()
{
    unsigned int v = 0x01020304;
    return reinterpret_cast<unsigned char*>(&v)[0] == 4;
}

void Bg2MeshParser::SetBigEndian()
{
    if (IsBigEndian())
    {
        mSwapBytes = false;
    }
    else 
    {
        mSwapBytes = true;
    }
}

void Bg2MeshParser::SetLittleEndian() 
{
    if (IsLittleEndian()) 
    {
        mSwapBytes = false;
    }
    else 
    {
        mSwapBytes = true;
    }
}

void Bg2MeshParser::SeekForward(int bytes) 
{
    mStream.seekg(bytes, std::ios::cur);
}
