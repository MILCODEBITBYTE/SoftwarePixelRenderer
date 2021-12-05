#pragma once

const enum class LIGHT_TYPE { DIRECTIONAL, POINT, SPOT, PHONG};

class CLight
{
public:
	CLight();
	~CLight();

	void SetType(const LIGHT_TYPE type);
	LIGHT_TYPE GetType() const;
	void SetPosition(const XMFLOAT3 v3Pos);
	XMFLOAT3 GetPosition() const;
	void SetDirection(const XMFLOAT3 v3Dir);
	XMFLOAT3 GetDirection() const;
	void SetRange(const float fRange);
	float GetRange() const;
	void SetCone(const float cone);
	float GetCone() const;
	void SetSpecularPower(const float power);
	float GetSpeclarPower() const;
	void SetAttenuation(const XMFLOAT3 v3Att);
	XMFLOAT3 GetAttenuation() const;
	void SetIntensity(const float fIntensity);
	float GetIntensity() const;
	void SetDiffuseColor(XMFLOAT4 v4Color);
	XMFLOAT4 GetDiffuseColor() const;
	void SetAmbientColor(XMFLOAT4 v4Color);
	XMFLOAT4 GetAmbientColor() const;
	void SetSpecularColor(const XMFLOAT4 v4Color);
	XMFLOAT4 GetSpecularColor() const;
	XMFLOAT3 GetLightToVertexDistance() const;
	void SetCameraPosition(const XMFLOAT3 v3CamPos);
	XMFLOAT3 GetCameraPosition() const;

	XMFLOAT4 GetComputedColor() const;

	void ComputeDirectionalLight(const XMVECTOR vNormal);
	void ComputePointLight(const XMVECTOR vTarget, const XMVECTOR vNormal);
	void ComputeSpotlight(const XMVECTOR vTarget, const XMVECTOR vNormal);
	void ComputePhong(const XMVECTOR vTarget, const XMVECTOR vNormal);

private:

	XMFLOAT3 m_v3Pos;
	XMFLOAT3 m_v3Dir;
	XMFLOAT3 m_v3Attenuation;
	XMFLOAT3 m_v3CamPos;
	float m_fRange;
	float m_fCone;
	float m_fIntensity;
	float m_fSpecularPower;
	XMFLOAT3 m_v3TargetDistance;
	XMFLOAT4 m_v4DiffuseColor;
	XMFLOAT4 m_v4AmbientColor;
	XMFLOAT4 m_v4SpecularColor;
	XMFLOAT4 m_v4ComputedColor;

	LIGHT_TYPE m_type;

};

