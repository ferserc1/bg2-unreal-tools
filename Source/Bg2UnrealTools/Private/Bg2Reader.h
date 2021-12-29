#pragma once


#include "Bg2MeshParser.h"

#include <functional>


class Bg2Reader {
public:
    struct FileMetadata {
        int numberOfPolyList;
    };

    typedef std::function<void(uint8_t, uint8_t, uint8_t)> VersionClosure;
    typedef std::function<void(const std::exception & e)> ExceptionClosure;
    typedef std::function<void(const FileMetadata & m)> MetadataClosure;
    typedef std::function<void(float offset[3], float pitch, float roll, float yaw)> JointClosure;
    typedef std::function<void(const std::string &)> StringClosure;
    typedef std::function<void(const std::vector<float> &)> VectorClosure;
    typedef std::function<void(const std::vector<unsigned int> &)> IndexClosure;

    Bg2Reader();

    bool Load(const std::string & path);
    inline bool Load(const char * path) { return Load(std::string(path)); }
    bool LoadHeader(const std::string & path);
    inline bool LoadHeader(const char * path) { return Load(std::string(path)); }

    inline Bg2Reader & Version(VersionClosure c) { mVersionClosure = c; return *this; }
    inline Bg2Reader & Materials(StringClosure c) { mMaterialClosure = c; return *this; }
    inline Bg2Reader & Error(ExceptionClosure c) { mExceptionClosure = c; return *this; }
    inline Bg2Reader & Metadata(MetadataClosure c) { mMetadataClosure = c; return *this; }
    inline Bg2Reader & InJoint(JointClosure c) { mInJointClosure = c; return *this; }
    inline Bg2Reader & OutJoint(JointClosure c) { mOutJointClosure = c; return *this; }
    inline Bg2Reader & PlistName(StringClosure c) { mPlistNameClosure = c; return *this; }
    inline Bg2Reader & MaterialName(StringClosure c) { mMaterialNameClosure = c; return *this; }
    inline Bg2Reader & Vertex(VectorClosure c) { mVertexClosure = c; return *this; }
    inline Bg2Reader & Normal(VectorClosure c) { mNormalClosure = c; return *this; }
    inline Bg2Reader & Uv0(VectorClosure c) { mUv0Closure = c; return *this; }
    inline Bg2Reader & Uv1(VectorClosure c) { mUv1Closure = c; return *this; }
    inline Bg2Reader & Uv2(VectorClosure c) { mUv2Closure = c; return *this; }
    inline Bg2Reader & Index(IndexClosure c) { mIndexClosure = c; return *this; }
    inline Bg2Reader & MaterialOverrides(StringClosure c) { mMaterialOverrideClosure = c; return *this; }

protected:

    bool ReadHeader(std::string & materialsString, unsigned int & numberOfPlist);
    bool ReadPolyList(int numberOfPlist);
    bool ReadSinglePolyList();
    void ParseMaterialOverride(const std::string &);

    Bg2MeshParser mMeshParser;

    VersionClosure mVersionClosure;
    StringClosure mMaterialClosure;
    ExceptionClosure mExceptionClosure;
    MetadataClosure mMetadataClosure;
    JointClosure mInJointClosure;
    JointClosure mOutJointClosure;
    StringClosure mPlistNameClosure;
    StringClosure mMaterialNameClosure;
    VectorClosure mVertexClosure;
    VectorClosure mNormalClosure;
    VectorClosure mUv0Closure;
    VectorClosure mUv1Closure;
    VectorClosure mUv2Closure;
    IndexClosure mIndexClosure;
    StringClosure mMaterialOverrideClosure;
};
