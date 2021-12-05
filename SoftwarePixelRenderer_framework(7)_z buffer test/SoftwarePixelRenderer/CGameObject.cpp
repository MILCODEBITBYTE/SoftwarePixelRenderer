#include "pch.h"
#include "CGameObject.h"

CGameObject::CGameObject()
{
	m_pModel = nullptr;
	//set bound box's diffuse color
	OBJ_MATERIAL_INFO info;
	info.Kd[0] = 1;
	info.Kd[1] = 1;
	info.Kd[2] = 1;
	m_Boundbox.GetMaterial()->SetMaterial(L"default", info);
}

CGameObject::CGameObject(const CGameObject& Obj)
	:CModelObject(Obj)
{
	m_Boundbox = Obj.m_Boundbox;
	m_pModel = Obj.m_pModel;
}

CGameObject::~CGameObject()
{
}

CGameObject* CGameObject::Clone()
{
	return new CGameObject(*this);
}

void CGameObject::Render()
{
	CModelObject::Render();

	if (m_pModel)
	{
		m_pModel->SetTransform(m_matModel);
		m_pModel->Render();
	}

	m_Boundbox.SetTransform(m_matModel);
	m_Boundbox.Render();
}

CBoundbox* CGameObject::GetBoundbox()
{
	return &m_Boundbox;
}

void CGameObject::AttachModel(CMesh* pModel)
{
	m_pModel = pModel;

	m_Boundbox.Reset();

	if(pModel)
	{
		size_t tModelCnt = pModel->GetModelCount();
		MESH_MODEL* pModels = pModel->GetModel();

		size_t tVtxCount;
		VERTEX* pVertices;
		for (size_t i = 0; i < tModelCnt; ++i)
		{
			tVtxCount = pModels[i].vecVertices.size();
			pVertices = pModels[i].vecVertices.data();

			m_Boundbox.CalcBound(pVertices, tVtxCount);
		}

		m_Boundbox.MakeVolume();
	}
}

CMesh* CGameObject::GetModel()
{
	return m_pModel;
}
