#ifndef __C_POF_MESH_FILE_LOADER_H_INCLUDED__
#define __C_POF_MESH_FILE_LOADER_H_INCLUDED__

#include "IMeshLoader.h"

namespace irr
{
namespace io
{
	class IFileSystem;
	class IReadFile;
} // end of namespace io
namespace scene
{
class ISceneManager;

class CPOFMeshFileLoader : public IMeshLoader
{
public:
	CPOFMeshFileLoader(scene::ISceneManager* smgr, io::IFileSystem* fs);

	virtual bool isALoadableFileExtension(const io::path& filename) const;

	virtual IAnimatedMesh* createMesh(io::IReadFile* file);
};

} //end of namespace scene
} // end of namespace irr

#endif
