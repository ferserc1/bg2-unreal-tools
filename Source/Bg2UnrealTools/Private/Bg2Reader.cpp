
#include "Bg2Reader.h"


//void readVector2(json::Value * val, float * result) {
//	if (val && (*val)[0] && (*val)[1]) {
//		result[0] = (*val)[0]->floatValue();
//		result[1] = (*val)[1]->floatValue();
//	}
//}
//
//void readVector3(json::Value * val, float * result) {
//	if (val && (*val)[0] && (*val)[1] && (*val)[2]) {
//		result[0] = (*val)[0]->floatValue();
//		result[1] = (*val)[1]->floatValue();
//		result[2] = (*val)[2]->floatValue();
//	}
//}
//
//void readVector4(json::Value * val, float * result) {
//	if (val && (*val)[0] && (*val)[1] && (*val)[2] && (*val)[3]) {
//		result[0] = (*val)[0]->floatValue();
//		result[1] = (*val)[1]->floatValue();
//		result[2] = (*val)[2]->floatValue();
//		result[3] = (*val)[3]->floatValue();
//	}
//}

Bg2Reader::Bg2Reader()
{

}

bool Bg2Reader::Load(const std::string & path)
{

    if (mMeshParser.Open(path, Bg2MeshParser::kModeRead))
    {
        //try 
        //{
            std::string materialsString;
            unsigned int numberOfPlist = 0;
            if (!ReadHeader(materialsString, numberOfPlist))
            {
                return false;
            }

            if (!ReadPolyList(numberOfPlist)) {
                return false;
            }

            mMeshParser.Close();

            ParseMaterialOverride(path);

            return true;
        //}
        //catch (std::exception & e)
        //{
        //    if (mExceptionClosure) 
        //    {
        //        mExceptionClosure(e);
        //    }
        //}
    }
    return false;
}

bool Bg2Reader::LoadHeader(const std::string & path)
{
    return true;
}

bool Bg2Reader::ReadHeader(std::string & materialsString, unsigned int & numberOfPlist)
{
    //// File header
    // File endian 0=big endian, 1=little endian
    unsigned char endian;
    mMeshParser.ReadByte(endian);
    if (endian == 0)
    {
        mMeshParser.SetBigEndian();
    }
    else
    {
        mMeshParser.SetLittleEndian();
    }

    // Version (major, minor, rev)
    uint8_t versionMajor;
    uint8_t versionMinor;
    uint8_t versionRevision;
    mMeshParser.ReadByte(versionMajor);
    mMeshParser.ReadByte(versionMinor);
    mMeshParser.ReadByte(versionRevision);

    if (mVersionClosure)
    {
        mVersionClosure(versionMajor, versionMinor, versionRevision);
    }

    // Header type
    Bg2MeshParser::BlockType btype;
    mMeshParser.ReadBlock(btype);
    if (btype != Bg2MeshParser::kHeader)
    {
        //throw std::runtime_error("Bg2 file format exception. Expecting begin of file header.");
        return false;
    }

    // Number of poly list
    FileMetadata metadata;
    mMeshParser.ReadInteger(metadata.numberOfPolyList);
    numberOfPlist = metadata.numberOfPolyList;
    if (mMetadataClosure)
    {
        mMetadataClosure(metadata);
    }

    // Materials
    Bg2MeshParser::BlockType block;
    mMeshParser.ReadBlock(block);
    if (block != Bg2MeshParser::kMaterials)
    {
        //throw std::runtime_error("Bg2 file format exception. Expecting material list.");
        return false;
    }

    mMeshParser.ReadString(materialsString);
    if (mMaterialClosure)
    {
        mMaterialClosure(materialsString);
    }

    // Shadow projectors are deprecated, this block only skips the projector section
    mMeshParser.ReadBlock(block);
    if (block == Bg2MeshParser::kShadowProjector)
    {
        std::string fileName;
        float proj[16];
        float trans[16];
        float attenuation;
        mMeshParser.ReadString(fileName);
        mMeshParser.ReadFloat(attenuation);
        for (int i = 0; i < 16; ++i)
        {
            mMeshParser.ReadFloat(proj[i]);
        }
        for (int i = 0; i < 16; ++i)
        {
            mMeshParser.ReadFloat(trans[i]);
        }
    }
    else
    {
        mMeshParser.SeekForward(-4);
    }

    // Joint list
    mMeshParser.ReadBlock(block);
    if (block == Bg2MeshParser::kJoint) {
        std::string jointString;
        mMeshParser.ReadString(jointString);

        // TODO: Load joints from json
        //using namespace json;
        // float offset[3];
        // float pitch, roll, yaw;
        //
        //Value * jointData = Parser::ParseString(jointString);
        //Value * input = (*jointData)["input"];
        //if (input) {
        //	readVector3((*input)["offset"], offset);
        //	pitch = (*input)["pitch"] ? (*input)["pitch"]->floatValue() : 0.0f;
        //	yaw = (*input)["yaw"] ? (*input)["yaw"]->floatValue() : 0.0f;
        //	roll = (*input)["roll"] ? (*input)["roll"]->floatValue() : 0.0f;
        //
        //	if (_inJointClosure) _inJointClosure(offset, pitch, roll, yaw);
        //}
        //
        //
        //Value * output = (*jointData)["output"];
        //if (output && (*output)[0]) {
        //	readVector3((*(*output)[0])["offset"], offset);
        //	pitch = (*(*output)[0])["pitch"] ? (*(*output)[0])["pitch"]->floatValue() : 0.0f;
        //	yaw = (*(*output)[0])["yaw"] ? (*(*output)[0])["yaw"]->floatValue() : 0.0f;
        //	roll = (*(*output)[0])["roll"] ? (*(*output)[0])["roll"]->floatValue() : 0.0f;
        //
        //	if (_outJointClosure) _outJointClosure(offset, pitch, roll, yaw);
        //}
    }
    else
    {
        mMeshParser.SeekForward(-4);
    }
    return true;
}

bool Bg2Reader::ReadPolyList(int numberOfPlist)
{
    Bg2MeshParser::BlockType block;

    mMeshParser.ReadBlock(block);
    if (block != Bg2MeshParser::kPolyList)
    {
        //throw std::runtime_error("Bg2: File format exception. Expecting poly list");
        return false;
    }

    for (int i = 0; i < numberOfPlist; ++i)
    {
        ReadSinglePolyList();
    }
    return true;
}

bool Bg2Reader::ReadSinglePolyList()
{
    Bg2MeshParser::BlockType block;
    bool done = false;
    bool vertexFound = false;
    bool normalFound = false;
    bool tex0Found = false;
    bool tex1Found = false;
    bool tex2Found = false;
    bool indexFound = false;
    std::vector<float> vector;
    std::vector<unsigned int> ivector;
    std::string name;

    while (!done)
    {
        vector.clear();
        mMeshParser.ReadBlock(block);

        switch (block)
        {
        case Bg2MeshParser::kPlistName:
            mMeshParser.ReadString(name);
            if (mPlistNameClosure)
            {
                mPlistNameClosure(name);
            }
            break;

        case Bg2MeshParser::kMatName:
            mMeshParser.ReadString(name);
            if (mMaterialNameClosure)
            {
                mMaterialNameClosure(name);
            }
            break;

        case Bg2MeshParser::kVertexArray:
            if (vertexFound)
            {
                //throw std::runtime_error("Bg2: File format exception. duplicate vertex array");
                return false;
            }
            vertexFound = true;
            mMeshParser.ReadArray(vector);
            if (mVertexClosure)
            {
                mVertexClosure(vector);
            }
            break;

        case Bg2MeshParser::kNormalArray:
            if (normalFound)
            {
                //throw std::runtime_error("Bg2: File format exception. duplicate normal array");
                return false;
            }
            normalFound = true;
            mMeshParser.ReadArray(vector);
            if (mNormalClosure)
            {
                mNormalClosure(vector);
            }
            break;

        case Bg2MeshParser::kTexCoord0Array:
            if (tex0Found)
            {
                //throw std::runtime_error("Bg2: File format exception. duplicate texcoord0 array");
                return false;
            }
            tex0Found = true;
            mMeshParser.ReadArray(vector);
            if (mUv0Closure)
            {
                mUv0Closure(vector);
            }
            break;

        case Bg2MeshParser::kTexCoord1Array:
            if (tex1Found)
            {
                //throw std::runtime_error("Bg2: File format exception. duplicate texcoord1 array");
                return false;
            }
            tex1Found = true;
            mMeshParser.ReadArray(vector);
            if (mUv1Closure)
            {
                mUv1Closure(vector);
            }
            break;

        case Bg2MeshParser::kTexCoord2Array:
            if (tex2Found)
            {
                //throw std::runtime_error("Bg2: File format exception. duplicate texcoord2 array");
                return false;
            }
            tex2Found = true;
            mMeshParser.ReadArray(vector);
            if (mUv2Closure)
            {
                mUv2Closure(vector);
            }
            break;

        case Bg2MeshParser::kIndexArray:
            if (indexFound)
            {
                //throw std::runtime_error("Bg2: File format exception. duplicate index array");
                return false;
            }
            indexFound = true;
            mMeshParser.ReadArray(ivector);
            if (mIndexClosure)
            {
                mIndexClosure(ivector);
            }
            break;

        case Bg2MeshParser::kPolyList:
        case Bg2MeshParser::kEnd:
            // TODO: parse component data
            done = true;
            break;
        default:
            //throw std::runtime_error("Bg2: File format exception. Unexpected poly list member found");
            return false;
        }
    }
    return true;
}

void Bg2Reader::ParseMaterialOverride(const std::string & path)
{
    if (mMaterialOverrideClosure)
    {
        size_t index = path.find_last_of(".");
        std::string bg2matPath = path.substr(0, index) + ".bg2mat";
        //std::cout << bg2matPath << std::endl;
        std::ifstream file;
        file.open(bg2matPath);
        if (file.good())
        {
            std::string result((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            mMaterialOverrideClosure(result);
            file.close();
        }
    }
}

