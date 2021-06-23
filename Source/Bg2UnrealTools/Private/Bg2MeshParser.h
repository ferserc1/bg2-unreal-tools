#pragma once

#include <string>
#include <vector>
#include <fstream>

class Bg2MeshParser
{
public:
    enum OpenMode
    {
        kModeClosed,
        kModeRead,
        kModeWrite
    };

    enum BlockType {
        kHeader = 'hedr',
        kPolyList = 'plst',
        kVertexArray = 'varr',
        kNormalArray = 'narr',
        kTexCoord0Array = 't0ar',
        kTexCoord1Array = 't1ar',
        kTexCoord2Array = 't2ar',
        kTexCoord3Array = 't3ar',
        kTexCoord4Array = 't4ar',
        kIndexArray = 'indx',
        kMaterials = 'mtrl',
        kPlistName = 'pnam',
        kMatName = 'mnam',
        kShadowProjector = 'proj',
        kJoint = 'join',
        kEnd = 'endf'
    };

    Bg2MeshParser();
    virtual ~Bg2MeshParser();

    bool Open(const std::string & path, OpenMode mode);

    void Close();

    bool WriteByte(unsigned char byte);
    bool WriteBlock(BlockType b);
    bool WriteInteger(int value);
    bool WriteFloat(float value);
    bool WriteString(const std::string & str);
    bool WriteArray(const std::vector<float> array);
    bool WriteArray(const std::vector<int> array);
    bool WriteArray(const std::vector<unsigned int> array);

    bool ReadByte(unsigned char & byte);
    bool ReadBlock(BlockType & b);
    bool ReadInteger(int & value);
    bool ReadFloat(float & value);
    bool ReadString(std::string & str);
    bool ReadArray(std::vector<float> & array);
    bool ReadArray(std::vector<int> & array);
    bool ReadArray(std::vector<unsigned int> & array);

    void SetBigEndian();
    void SetLittleEndian();

    void SeekForward(int bytes);

    std::fstream & Stream() { return mStream; }

    bool IsBigEndian();
    bool IsLittleEndian();

protected:
    std::fstream mStream;
    OpenMode mMode;
    bool mSwapBytes;
};
