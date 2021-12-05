#include "pch.h"
#include "CLight.h"

CLight::CLight()
{
    m_v3Pos = XMFLOAT3(0, 0, 0);
    m_v3CamPos = XMFLOAT3(0, 0, 0);
    m_fRange = 0;
    m_fSpecularPower = 1;
    m_v4AmbientColor = XMFLOAT4(1, 1, 1, 1);
    m_v4DiffuseColor = XMFLOAT4(1, 1, 1, 1);
    m_v4SpecularColor = XMFLOAT4(0.7f, 0.7f, 0.7f, 1);
    m_v4ComputedColor = XMFLOAT4(0, 0, 0, 0);
    m_fIntensity = 1;
    m_v3TargetDistance = XMFLOAT3(0, 0, 0);
    m_type = LIGHT_TYPE::DIRECTIONAL;
}

CLight::~CLight()
{
}

void CLight::SetType(const LIGHT_TYPE type)
{
    m_type = type;
}

LIGHT_TYPE CLight::GetType() const
{
    return m_type;
}

void CLight::SetPosition(const XMFLOAT3 v3Pos)
{
    m_v3Pos = v3Pos;
}

XMFLOAT3 CLight::GetPosition() const
{
    return m_v3Pos;
}

void CLight::SetDirection(const XMFLOAT3 v3Dir)
{
    m_v3Dir = v3Dir;
}

XMFLOAT3 CLight::GetDirection() const
{
    return m_v3Dir;
}

void CLight::SetRange(const float fRange)
{
    m_fRange = fRange;
}

float CLight::GetRange() const
{
    return m_fRange;
}

void CLight::SetCone(const float cone)
{
    m_fCone = cone;
}

float CLight::GetCone() const
{
    return m_fCone;
}

void CLight::SetSpecularPower(const float power)
{
    m_fSpecularPower = power;
}

float CLight::GetSpeclarPower() const
{
    return m_fSpecularPower;
}

void CLight::SetAttenuation(const XMFLOAT3 v3Att)
{
    m_v3Attenuation = v3Att;
}

XMFLOAT3 CLight::GetAttenuation() const
{
    return m_v3Attenuation;
}

void CLight::SetIntensity(const float fIntensity)
{
    m_fIntensity = fIntensity;
}

float CLight::GetIntensity() const
{
    return m_fIntensity;
}

void CLight::SetDiffuseColor(XMFLOAT4 v4Color)
{
    m_v4DiffuseColor = v4Color;
}

XMFLOAT4 CLight::GetDiffuseColor() const
{
    return m_v4DiffuseColor;
}

void CLight::SetAmbientColor(XMFLOAT4 v4Color)
{
    m_v4AmbientColor = v4Color;
}

XMFLOAT4 CLight::GetAmbientColor() const
{
    return m_v4AmbientColor;
}

void CLight::SetSpecularColor(const XMFLOAT4 v4Color)
{
    m_v4SpecularColor = v4Color;
}

XMFLOAT4 CLight::GetSpecularColor() const
{
    return m_v4SpecularColor;
}

void CLight::SetLightToVertexDistance(const XMFLOAT3 v3TargetDistance)
{
    m_v3TargetDistance = v3TargetDistance;
}

XMFLOAT3 CLight::GetLightToVertexDistance() const
{
    return m_v3TargetDistance;
}

void CLight::SetCameraPosition(const XMFLOAT3 v3CamPos)
{
    m_v3CamPos = v3CamPos;
}

XMFLOAT3 CLight::GetCameraPosition() const
{
    return m_v3CamPos;
}

void CLight::SetPointLightDirection(const XMVECTOR vTarget, const XMVECTOR vNormal)
{
    XMVECTOR vPos = XMLoadFloat3(&m_v3Pos);
    XMVECTOR vDir = XMVector3Normalize(vPos);

    //Create the vector between light postion and vertex
    XMVECTOR vLightToPixelVec = XMVectorSubtract(vPos, vTarget);

    //Find the distance between the light pos and pixel pos
    XMVECTOR d = XMVector3Length(vLightToPixelVec);
    m_fFallOffFactor = XMVectorGetX(d);

    vLightToPixelVec = XMVectorDivide(vLightToPixelVec, d);

    m_fIntensity = XMVectorGetX(XMVector3Dot(vNormal, XMVectorNegate(vLightToPixelVec)));
    m_fFaceReflectFactor = XMVectorGetX(XMVector3Dot(vNormal, XMVectorNegate(vDir)));
}

void CLight::SetSpotlightDirection(const XMVECTOR vTarget, const XMVECTOR vNormal)
{
    XMVECTOR vPos = XMLoadFloat3(&m_v3Pos);
    XMVECTOR vDir = XMVector3Normalize(vPos);

    //Create the vector between light postion and vertex
    XMVECTOR vLightToPixelVec = XMVectorSubtract(vPos, vTarget);


    //Find the distance between the light pos and pixel pos
    XMVECTOR d = XMVector3Length(vLightToPixelVec);
    m_fFallOffFactor = XMVectorGetX(d);

    //Trun LightToPixelVec into a unit length vector describling the pixels direction from the lgihts position
    vLightToPixelVec = XMVectorDivide(vLightToPixelVec, d);
    XMStoreFloat3(&m_v3TargetDistance, vLightToPixelVec);

    //Calculate how much light the pixel gets by the angle in which the light strikes the pixels surface
    m_fIntensity = XMVectorGetX(XMVector3Dot(vNormal, vLightToPixelVec));

    m_fFaceReflectFactor = XMVectorGetX(XMVector3Dot(vNormal, XMVectorNegate(vDir)));

}


void CLight::SetPhongDirection(const XMVECTOR vTarget, const XMVECTOR vNormal)
{
    XMVECTOR vLightPos = XMLoadFloat3(&m_v3Pos);
    XMVECTOR vCamPos = XMLoadFloat3(&m_v3CamPos);
    XMVECTOR vLightDir = XMVector3Normalize(XMVectorSubtract(vLightPos, vTarget));

    XMVECTOR vViewDir = XMVector3Normalize(XMVectorSubtract(vCamPos, vTarget));
    XMVECTOR vReflectFace = XMVector3Normalize(XMVector3Reflect(vLightDir, vNormal));

    XMVECTOR D = XMVectorSaturate(XMVector3Dot(XMVectorNegate(vLightDir), vNormal));

    m_fIntensity = XMVectorGetX(D);
    XMStoreFloat3(&m_v3Reflect, vReflectFace);
    XMStoreFloat3(&m_v3ViewDir, vViewDir);
}

void CLight::ComputeDirectionalLight(const XMVECTOR vNormal)
{
    XMVECTOR vLightDir = XMVector3Normalize(XMLoadFloat3(&m_v3Pos));

    XMVECTOR vIntensity = XMVectorSaturate(XMVector3Dot(vNormal, XMVectorNegate(vLightDir)));

    m_fIntensity = XMVectorGetX(vIntensity);

    XMVECTOR vDiffuse = XMLoadFloat4(&m_v4DiffuseColor);
    XMVECTOR vAmbient = XMLoadFloat4(&m_v4AmbientColor);
    XMVECTOR vFinal = vAmbient;
    

    if (m_fIntensity > 0.0f)
    {
        vFinal = XMVectorMultiply(vDiffuse, vIntensity);
        vFinal = XMVectorSaturate(XMVectorAdd(vFinal, vAmbient));
    }


    XMStoreFloat4(&m_v4ComputedColor, vFinal);
}

void CLight::ComputePointLight(const XMVECTOR vTarget, const XMVECTOR vNormal)
{
    XMVECTOR vPos = XMLoadFloat3(&m_v3Pos);

    //Create the vector between light postion and vertex
    XMVECTOR vLightToPixelVec = XMVectorSubtract(vPos, vTarget);


    //Find the distance between the light pos and pixel pos
    XMVECTOR d = XMVector3Length(vLightToPixelVec);

    XMVECTOR vDiffuse = XMLoadFloat4(&m_v4DiffuseColor);
    XMVECTOR vFinalAmbient = XMLoadFloat4(&m_v4AmbientColor);
    XMVECTOR vComputeColor = vFinalAmbient;

    if (XMVectorGetX(d) > m_fRange)
    {
        XMStoreFloat4(&m_v4ComputedColor, vFinalAmbient);
        return;
    }

    vLightToPixelVec = XMVectorDivide(vLightToPixelVec, d);

    m_fIntensity = XMVectorGetX(XMVector3Dot(vNormal, XMVectorNegate(vLightToPixelVec)));


    if (m_fIntensity > 0.0f)
    {
        float fFactor = XMVectorGetX(d);

        vComputeColor = XMVectorMultiply(vDiffuse, XMVectorReplicate(m_fIntensity));

        float fFalloff = (m_v3Attenuation.x + m_v3Attenuation.y * fFactor) + (m_v3Attenuation.z * fFactor * fFactor);

        vComputeColor = XMVectorDivide(vComputeColor, XMVectorReplicate(fFalloff));

    }

    vComputeColor = XMVectorSaturate(XMVectorAdd(vComputeColor, vFinalAmbient));
    XMStoreFloat4(&m_v4ComputedColor, vComputeColor);

}



void CLight::ComputeSpotlight(const XMVECTOR vTarget, const XMVECTOR vNormal)
{
    XMVECTOR vPos = XMLoadFloat3(&m_v3Pos);
    XMVECTOR vDir = XMVector3Normalize(vPos);

    //Create the vector between light postion and vertex
    XMVECTOR vLightToPixelVec = XMVectorSubtract(vPos, vTarget);

    //Find the distance between the light pos and pixel pos
    XMVECTOR d = XMVector3Length(vLightToPixelVec);

    //Add the ambient light

    XMVECTOR vDiffuse = XMLoadFloat4(&m_v4DiffuseColor);
    XMVECTOR vFinalAmbient = XMLoadFloat4(&m_v4AmbientColor);
    XMVECTOR vComputeColor = vFinalAmbient;

    //If pixel is too far, return pixel color width ambient light
    if (XMVectorGetX(d) > m_fRange)
    {
        XMStoreFloat4(&m_v4ComputedColor, vFinalAmbient);
        return;

    }

    //Trun LightToPixelVec into a unit length vector describling the pixels direction from the lgihts position
    vLightToPixelVec = XMVectorDivide(vLightToPixelVec, d);

    //Calculate how much light the pixel gets by the angle in which the light strikes the pixels surface
    m_fIntensity = XMVectorGetX(XMVector3Dot(vNormal, vLightToPixelVec));


    //Cone
    if (m_fIntensity > 0.0f)
    {
        float fFactor = XMVectorGetX(d);

        //Add light to the final color of the pixel
        vComputeColor = vDiffuse;

        float fFalloff = (m_v3Attenuation.x + m_v3Attenuation.y * fFactor) + (m_v3Attenuation.z * fFactor * fFactor);

        //Calculate light's distance falloff factor
        vComputeColor = XMVectorDivide(vComputeColor, XMVectorReplicate(fFalloff));

        float fCone = powf(max(XMVectorGetX(XMVector3Dot(XMVectorNegate(vLightToPixelVec), vDir)), 0.0f), m_fCone);

        //Calculate falloff from center to edge of pointlight cone
        vComputeColor = XMVectorMultiply(vComputeColor, XMVectorReplicate(fCone));
    }

    //Directional

    XMVECTOR vReflect = XMVectorSaturate(XMVector3Dot(vNormal, XMVectorNegate(vDir)));
    vDiffuse = XMVectorMultiply(vDiffuse, vReflect);
    vComputeColor = XMVectorAdd(vComputeColor, vDiffuse);

    vComputeColor = XMVectorSaturate(XMVectorAdd(vComputeColor, vFinalAmbient));
    XMStoreFloat4(&m_v4ComputedColor, vComputeColor);
}


void CLight::ComputePhong(const XMVECTOR vTarget, const XMVECTOR vNormal)
{
    XMVECTOR vLightPos = XMLoadFloat3(&m_v3Pos);
    XMVECTOR vCamPos = XMLoadFloat3(&m_v3CamPos);
    XMVECTOR vLightDir = XMVector3Normalize(XMVectorSubtract(vLightPos, vTarget));

    XMVECTOR vViewDir = XMVector3Normalize(XMVectorSubtract(vCamPos, vTarget));
    XMVECTOR vReflectFace = XMVector3Normalize(XMVector3Reflect(vLightDir, vNormal));

    XMVECTOR D = XMVector3Dot(XMVectorNegate(vLightDir), vNormal);

    m_fIntensity = XMVectorGetX(D);

    XMVECTOR vFinalAmbient = XMLoadFloat4(&m_v4AmbientColor);
    XMVECTOR vComputeColor = vFinalAmbient;

 
    if (m_fIntensity > 0.0f)
    {
        float fFactor = saturatef(XMVectorGetX(XMVector3Dot(vReflectFace, XMVectorNegate(vViewDir))));
        fFactor = powf(fFactor, m_fSpecularPower);
        XMVECTOR vFactor = XMVectorReplicate(fFactor);
        vFinalAmbient = XMVectorAdd(vFactor, vFinalAmbient);
        vComputeColor = XMVectorSaturate(XMVectorAdd(D, vFinalAmbient));

    }

    XMStoreFloat4(&m_v4ComputedColor, vComputeColor);
}






XMFLOAT4 CLight::GetComputedColor() const
{
    return m_v4ComputedColor;
}



void CLight::ComputeDirectionalLight(const XMFLOAT4 v4DiffuseColor)
{

    XMVECTOR vDiffuse = XMLoadFloat4(&v4DiffuseColor);
    XMVECTOR vFinalAmbient = XMVectorMultiply(vDiffuse, XMLoadFloat4(&m_v4AmbientColor));
    XMVECTOR vFinal = vFinalAmbient;

    if (m_fIntensity > 0)
    {
        vFinal = XMVectorMultiply(XMLoadFloat4(&m_v4DiffuseColor), XMVectorReplicate(m_fIntensity));
        vFinal = XMVectorMultiply(vDiffuse, vFinal);
        vFinal = XMVectorSaturate(XMVectorAdd(vFinal, vFinalAmbient));
        
    }

    XMStoreFloat4(&m_v4ComputedColor, vFinal);
}


void CLight::ComputePointLight(const XMFLOAT4 v4DiffuseColor)
{

    XMVECTOR vDiffuse = XMLoadFloat4(&v4DiffuseColor);
    XMVECTOR vFinalAmbient = XMVectorMultiply(vDiffuse, XMLoadFloat4(&m_v4AmbientColor));
    XMVECTOR vComputeColor = vFinalAmbient;


    if (m_fFallOffFactor > m_fRange)
    {
        XMStoreFloat4(&m_v4ComputedColor, vFinalAmbient);
        return;
    }

    if (m_fIntensity > 0.0f)
    {
        vDiffuse = XMVectorMultiply(vDiffuse, XMVectorReplicate(m_fIntensity));
        vComputeColor = XMVectorMultiply(vDiffuse, XMLoadFloat4(&m_v4DiffuseColor));

        float fFalloff = (m_v3Attenuation.x + m_v3Attenuation.y * m_fFallOffFactor) + (m_v3Attenuation.z * m_fFallOffFactor * m_fFallOffFactor);

        vComputeColor = XMVectorDivide(vComputeColor, XMVectorReplicate(fFalloff));
    }

    vComputeColor = XMVectorSaturate(XMVectorAdd(vComputeColor, vFinalAmbient));
    XMStoreFloat4(&m_v4ComputedColor, vComputeColor);
}


void CLight::ComputeSpotlight(const XMFLOAT4 v4DiffuseColor)
{
    XMVECTOR vDir = XMVector3Normalize((XMLoadFloat3(&m_v3Pos)));

    //Add the ambient light

    XMVECTOR vDiffuse = XMLoadFloat4(&v4DiffuseColor);
    XMVECTOR vFinalAmbient = XMVectorMultiply(vDiffuse, XMLoadFloat4(&m_v4AmbientColor));
    XMVECTOR vComputeColor = vFinalAmbient;

    //If pixel is too far, return pixel color width ambient light
    if (m_fFallOffFactor > m_fRange)
    {
        XMStoreFloat4(&m_v4ComputedColor, vFinalAmbient);
        return;

    }

    //Trun LightToPixelVec into a unit length vector describling the pixels direction from the lgihts position
    XMVECTOR vLightToPixelVec = XMLoadFloat3(&m_v3TargetDistance);

    if (m_fIntensity > 0.0f)
    {
        //Add light to the final color of the pixel
        vComputeColor = XMVectorMultiply(vDiffuse, XMLoadFloat4(&m_v4DiffuseColor));

        float fFalloff = (m_v3Attenuation.x + m_v3Attenuation.y * m_fFallOffFactor) + (m_v3Attenuation.z * m_fFallOffFactor * m_fFallOffFactor);

        //Calculate light's distance falloff factor
        vComputeColor = XMVectorDivide(vComputeColor, XMVectorReplicate(fFalloff));

        float fCone = powf(max(XMVectorGetX(XMVector3Dot(XMVectorNegate(vLightToPixelVec), vDir)), 0.0f), m_fCone);

        //Calculate falloff from center to edge of pointlight cone
        vComputeColor = XMVectorMultiply(vComputeColor, XMVectorReplicate(fCone));
    }

    //Directional
    XMVECTOR vReflect = XMVectorReplicate(m_fFaceReflectFactor);
    vDiffuse = XMVectorMultiply(vDiffuse, vReflect);
    vComputeColor = XMVectorAdd(vDiffuse, vComputeColor);

    vComputeColor = XMVectorSaturate(XMVectorAdd(vComputeColor, vFinalAmbient));
    XMStoreFloat4(&m_v4ComputedColor, vComputeColor);
}


void CLight::ComputePhong(const XMFLOAT4 v4DiffuseColor)
{
    XMVECTOR vReflectFace = XMLoadFloat3(&m_v3Reflect);
    XMVECTOR vViewDir = XMLoadFloat3(&m_v3ViewDir);

    XMVECTOR D = XMVectorReplicate(m_fIntensity);

    XMVECTOR vDiffuse = XMLoadFloat4(&v4DiffuseColor);
    XMVECTOR vFinalAmbient = XMVectorMultiply(vDiffuse, XMLoadFloat4(&m_v4AmbientColor));
    XMVECTOR vComputeColor = vFinalAmbient;


    if (m_fIntensity > 0.0f)
    {

        float fFactor = saturatef(XMVectorGetX(XMVector3Dot(vReflectFace, XMVectorNegate(vViewDir))));
        fFactor = powf(fFactor, m_fSpecularPower);
        vFinalAmbient = XMVectorAdd(vFinalAmbient, XMVectorReplicate(fFactor));
        vDiffuse = XMVectorMultiply(vDiffuse, D);
        vComputeColor = XMVectorSaturate(XMVectorAdd(vDiffuse, vFinalAmbient));

    }

    XMStoreFloat4(&m_v4ComputedColor, vComputeColor);
}

void CLight::SetLightViewMatrix(const XMMATRIX& matView)
{
    m_matView = matView;
}

XMMATRIX CLight::GetLightViewMatrix() const
{
    return m_matView;
}


