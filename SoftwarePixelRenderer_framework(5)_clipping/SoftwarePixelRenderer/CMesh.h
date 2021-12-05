#pragma once

#include "CModelObject.h"
#include "CFace.h"
#include "CModelImportor.h"
#include "CMaterial.h"

typedef struct _tagMeshModel
{
	wstring				strName;
	wstring				strMaterialName;
	vector<CFace>		vecFacies;
	vector<VERTEX>		vecVertices;
	vector<_tagMeshModel>	vecNext;
}MESH_MODEL;

class CMesh : public CModelObject
{
public:
	CMesh();
	CMesh(const CMesh& mesh);
	~CMesh();

	CMesh* Clone() override;

	virtual void Initialize();
	bool LoadFromFile(const wchar_t* szFileName);

	size_t GetModelCount() const;
	MESH_MODEL* GetModel();


	CMaterial* GetMaterial();

	void Shutdown() override;

protected:
	virtual void MakeMesh();
	virtual void MakeMesh(CModelImportor* pImporter);

	void MakeFaceGroup(MESH_MODEL* pMesh, OBJ_INFO *pObjInfo, OBJ_GROUP_INFO* pGroupInfo);

protected:
	CMaterial			m_mtlMaterial;
	vector<MESH_MODEL>	m_vecModels;
};

