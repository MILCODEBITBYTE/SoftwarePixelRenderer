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
	void SetLightToVertexDistance(const XMFLOAT3 v3TargetDistance);
	XMFLOAT3 GetLightToVertexDistance() const;
	void SetCameraPosition(const XMFLOAT3 v3CamPos);
	XMFLOAT3 GetCameraPosition() const;

	void SetPointLightDirection(const XMVECTOR vTarget, const XMVECTOR vNormal);
	void SetSpotlightDirection(const XMVECTOR vTarget, const XMVECTOR vNormal);
	void SetPhongDirection(const XMVECTOR vTarget, const XMVECTOR vNormal);

	XMFLOAT4 GetComputedColor() const;

	//For fast
	void ComputeDirectionalLight(const XMVECTOR vNormal);
	void ComputePointLight(const XMVECTOR vTarget, const XMVECTOR vNormal);
	void ComputeSpotlight(const XMVECTOR vTarget, const XMVECTOR vNormal);
	void ComputePhong(const XMVECTOR vTarget, const XMVECTOR vNormal);

	//For smooth
	void ComputeDirectionalLight(const XMFLOAT4 v4DiffuseColor);
	void ComputePointLight(const XMFLOAT4 v4DiffuseColor);
	void ComputeSpotlight(const XMFLOAT4 v4DiffuseColor);
	void ComputePhong(const XMFLOAT4 v4DiffuseColor);



private:

	XMFLOAT3 m_v3Pos;
	XMFLOAT3 m_v3Dir;
	XMFLOAT3 m_v3Attenuation;
	XMFLOAT3 m_v3CamPos;
	XMFLOAT3 m_v3Reflect;
	XMFLOAT3 m_v3ViewDir;
	float m_fRange;
	float m_fCone;
	float m_fIntensity;
	float m_fSpecularPower;
	XMFLOAT3 m_v3TargetDistance;
	XMFLOAT4 m_v4DiffuseColor;
	XMFLOAT4 m_v4AmbientColor;
	XMFLOAT4 m_v4SpecularColor;
	XMFLOAT4 m_v4ComputedColor;

	float m_fFallOffFactor;
	float m_fFaceReflectFactor;

	LIGHT_TYPE m_type;

};

