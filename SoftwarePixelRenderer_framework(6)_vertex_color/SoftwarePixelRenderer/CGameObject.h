#pragma once

#include "CBoundbox.h"

class CGameObject : public CModelObject
{
public:
	CGameObject();
	CGameObject(const CGameObject& Obj);
	~CGameObject();

	CGameObject* Clone() override;

	void Render() override;

	CBoundbox* GetBoundbox();

	void AttachModel(CMesh* pModel);
	CMesh* GetModel();
private:
	CBoundbox m_Boundbox;
	CMesh* m_pModel;

};

