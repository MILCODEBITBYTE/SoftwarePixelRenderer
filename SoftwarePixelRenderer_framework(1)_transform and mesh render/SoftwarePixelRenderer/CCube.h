#pragma once
#include "CMesh.h"
class CCube :
    public CMesh
{
public:
    CCube();
    CCube(const CCube& mesh);
    ~CCube();

    void Initialize() override;
};

