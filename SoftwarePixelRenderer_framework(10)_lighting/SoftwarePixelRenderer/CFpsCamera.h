#pragma once
#include "CCamera.h"
class CFpsCamera : public CCamera
{
public:
    CFpsCamera();
    ~CFpsCamera();

    void Render(const float fEllipse) override;
};

