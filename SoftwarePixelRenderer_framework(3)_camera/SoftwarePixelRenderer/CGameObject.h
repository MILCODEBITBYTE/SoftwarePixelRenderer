#pragma once

#include "CMesh.h"

class CGameObject : public CModelObject
{
public:
	CGameObject();
	CGameObject(const CGameObject& Obj);
	~CGameObject();

	CGameObject* Clone() override;

	void Render() override;


	void AttachModel(CMesh* pModel);
	CMesh* GetModel();
private:
	CMesh* m_pModel;

};

