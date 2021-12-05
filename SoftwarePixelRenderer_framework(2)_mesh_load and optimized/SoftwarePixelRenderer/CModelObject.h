#pragma once

const enum class OBJECT_TYPE { EMPTY, MESH};

class CModelObject
{
public:
	CModelObject();
	CModelObject(const OBJECT_TYPE type);
	CModelObject(const CModelObject& Obj);
	virtual ~CModelObject();

	OBJECT_TYPE GetType() const;

	virtual CModelObject* Clone();

	void SetPosition(XMFLOAT3 v3Pos);
	void GetPosition(XMFLOAT3* pPos);
	void GetPosition(XMVECTOR* pPos);
	void SetTransform(const XMFLOAT4X4 matTrans);
	void SetTransform(const XMMATRIX matTrans);

	XMMATRIX GetTransform() const;
	XMMATRIX GetModelMatrix() const;
	virtual void Render();

	virtual void Shutdown();
	

protected:
	XMFLOAT4X4 m_matTransform;
	XMFLOAT4X4 m_matModel;
	XMFLOAT3 m_v3Pos;

	OBJECT_TYPE	m_Type;
};

