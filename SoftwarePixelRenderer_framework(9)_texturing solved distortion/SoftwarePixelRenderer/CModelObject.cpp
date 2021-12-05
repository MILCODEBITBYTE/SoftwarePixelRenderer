#include "pch.h"
#include "CModelObject.h"

CModelObject::CModelObject()
{
	m_v3Pos = XMFLOAT3(0, 0, 0);
	XMStoreFloat4x4(&m_matTransform, XMMatrixIdentity());
	m_Type = OBJECT_TYPE::EMPTY;
}

CModelObject::CModelObject(const OBJECT_TYPE type)
{
	m_v3Pos = XMFLOAT3(0, 0, 0);
	XMStoreFloat4x4(&m_matTransform, XMMatrixIdentity());
	m_Type = type;
}

CModelObject::CModelObject(const CModelObject& Obj)
{
	m_v3Pos = Obj.m_v3Pos;
	m_matTransform = Obj.m_matTransform;
	m_Type = Obj.m_Type;
}

CModelObject::~CModelObject()
{
}

OBJECT_TYPE CModelObject::GetType() const
{
	return m_Type;
}

CModelObject* CModelObject::Clone()
{
	return new CModelObject(*this);
}

void CModelObject::SetPosition(XMFLOAT3 v3Pos)
{
	
	m_v3Pos = v3Pos;
}

void CModelObject::GetPosition(XMFLOAT3* pPos)
{
	*pPos = m_v3Pos;
}

void CModelObject::GetPosition(XMVECTOR* pPos)
{
	*pPos = XMLoadFloat3(&m_v3Pos);
}

void CModelObject::SetTransform(const XMFLOAT4X4 matTrans)
{
	m_matTransform = matTrans;

}
void CModelObject::SetTransform(const XMMATRIX matTrans)
{
	XMStoreFloat4x4(&m_matTransform, matTrans);
}


XMMATRIX CModelObject::GetTransform() const
{
	return XMLoadFloat4x4(&m_matTransform);
}

XMMATRIX CModelObject::GetModelMatrix() const
{
	return XMLoadFloat4x4(&m_matModel);
}

void CModelObject::Render()
{
	XMMATRIX matTransform = XMLoadFloat4x4(&m_matTransform);
	XMMATRIX matTranslation = XMMatrixTranslation(m_v3Pos.x, m_v3Pos.y, m_v3Pos.z);
	matTransform = XMMatrixMultiply(matTranslation, matTransform);
	XMStoreFloat4x4(&m_matModel, matTransform);
}

void CModelObject::Shutdown()
{
}
