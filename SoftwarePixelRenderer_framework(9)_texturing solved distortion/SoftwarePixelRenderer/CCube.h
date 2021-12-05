#pragma once
#include "CMesh.h"
class CCube :
    public CMesh
{
public:
    CCube();
    CCube(const CCube& mesh);
    ~CCube();

    CCube* Clone() override;

    void Initialize() override;
};

