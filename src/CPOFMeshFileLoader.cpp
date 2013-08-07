#include "CPOFMeshFileLoader.h"

#include "ISceneManager.h"
#include "IFileSystem.h"
#include "IReadFile.h"

namespace irr
{
namespace scene
{

CPOFMeshFileLoader::CPOFMeshFileLoader(scene::ISceneManager* smgr, io::IFileSystem* fs)
{
}

bool CPOFMeshFileLoader::isALoadableFileExtension(const io::path& filename) const
{
	return false;
}

IAnimatedMesh* CPOFMeshFileLoader::createMesh(io::IReadFile* f)
{
}

} // end of namespace scene
} // end of namespace irr
