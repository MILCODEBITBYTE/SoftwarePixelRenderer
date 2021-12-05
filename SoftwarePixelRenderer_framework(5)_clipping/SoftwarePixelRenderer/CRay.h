#pragma once

class CRay
{
public:
	CRay();
	~CRay();

	void SetOrigin(const XMFLOAT3& v3Org);
	void SetDirection(const XMFLOAT3& v3Dir);

	XMFLOAT3 GetOrigin() const;
	XMFLOAT3 GetDirection() const;

private:

	XMFLOAT3 m_v3Origin;
	XMFLOAT3 m_v3Direction;

};

