#pragma once
#include "CFpsCamera.h"
class CTpsCamera : public CFpsCamera
{
public:
	CTpsCamera();
	~CTpsCamera();

	void SetZoom(const float iFactor);

	void Render(const float fEllipse) override;
	void MakeMatrix() override;
private:
	float m_fZoomFactor;
};

