#include "pch.h"
#include "CGameObject.h"

CGameObject::CGameObject()
{
	m_pModel = nullptr;
}

CGameObject::CGameObject(const CGameObject& Obj)
	:CModelObject(Obj)
{
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

}


void CGameObject::AttachModel(CMesh* pModel)
{
	m_pModel = pModel;
}

CMesh* CGameObject::GetModel()
{
	return m_pModel;
}
