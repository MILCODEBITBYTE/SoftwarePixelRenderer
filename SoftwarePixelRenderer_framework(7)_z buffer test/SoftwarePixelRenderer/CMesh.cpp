#include "pch.h"
#include "CMesh.h"
#include "CModelImportor.h"


CMesh::CMesh()
{
    m_Type = OBJECT_TYPE::MESH;
}


CMesh::CMesh(const CMesh& mesh)
    : CModelObject(mesh)

{
    m_vecModels = mesh.m_vecModels;
}

CMesh::~CMesh()
{
}

CMesh* CMesh::Clone()
{
    return new CMesh(*this);
}

void CMesh::Initialize()
{
}

bool CMesh::LoadFromFile(const wchar_t* szFileName)
{
    CModelImportor imp(szFileName);
    if (false == imp.isOpen()) return false;

    MakeMesh(&imp);

    return true;
}

size_t CMesh::GetModelCount() const
{
    return m_vecModels.size();
}

MESH_MODEL* CMesh::GetModel()
{
    return m_vecModels.data();
}


CMaterial* CMesh::GetMaterial()
{
    return &m_mtlMaterial;
}


void CMesh::Shutdown()
{
    m_vecModels.clear();

    m_mtlMaterial.Clear();
}

void CMesh::MakeMesh()
{
}

void CMesh::MakeMesh(CModelImportor* pImportor)
{
    Shutdown();

    size_t tObjCount = pImportor->GetObjectCount();

    OBJ_INFO* pObj = pImportor->GetObjectInfo();

    MESH_MODEL* pModel;
    for (size_t i = 0; i < tObjCount; ++i)
    {
        //new model
        m_vecModels.push_back(MESH_MODEL());
        pModel = &m_vecModels.back();
        pModel->strMaterialName = pImportor->GetMaterialFileName();
        pModel->strName = pObj[i].strObjName;
        size_t tGroupCount = pObj[i].vecGroupInfo.size();

        for(size_t j = 0 ; j < tGroupCount; ++j)
            MakeFaceGroup(pModel, &pObj[i], &pObj[i].vecGroupInfo[j]);
    }

    m_mtlMaterial.Open(pImportor->GetMaterialFileName());
}

bool CheckNormal(VERTEX* p1, VERTEX* p2, VERTEX* p3, const XMFLOAT3& v3Normal)
{
    XMFLOAT3 v1, v2;
    XMVECTOR vDir1, vDir2, vResult, vTarget;

    vTarget = XMLoadFloat3(&v3Normal);

    v1 = p2->v3Vtx - p1->v3Vtx;
    v2 = p3->v3Vtx - p1->v3Vtx;

    vDir1 = XMLoadFloat3(&v1);
    vDir2 = XMLoadFloat3(&v2);

    vResult = XMVector3Normalize(XMVector3Cross(vDir1, vDir2));
    XMVECTOR equal = XMVectorEqual(vResult, vTarget);

    return !XMVectorGetX(equal);
}

bool CheckCCW(list<VERTEX*> lstNext, VERTEX* pStart, VERTEX* pDir[2], const int step, list<VERTEX*> &lstOut)
{
    size_t element_count = lstNext.size();
    VERTEX* pTarget = nullptr;

    XMFLOAT3 v1, v2;
    XMVECTOR vDir1, vDir2;

    list<VERTEX*>::iterator iter;

    if (element_count)
    {
        list<VERTEX*> lstNextStep;

        size_t count = 0;
        for (size_t i = 0; i < element_count; ++i)
        {
            lstNextStep.clear();

            count = 0;
            for (iter = lstNext.begin(); iter != lstNext.end(); ++iter)
            {
                if (i != count)
                {
                    lstNextStep.push_back(*iter);
                }
                else
                {
                    pTarget = (*iter);
                }
                count++;
            }

            if (step == 0)
            {
                //다음 정점과 비교할 마지막 정점
                pDir[1] = pTarget;
                

                //2개의 정점부터 비교 시작
                if (true == CheckCCW(lstNextStep, pStart, pDir, step + 1, lstOut))
                {
                    lstOut.push_front(pTarget);
                    return true;
                }
            }
            else
            {
                //CCW 확인
                v1 = pDir[0]->v3Vtx - pDir[1]->v3Vtx;
                v2 = pTarget->v3Vtx - pDir[1]->v3Vtx;

                vDir1 = XMVector3Normalize(XMLoadFloat3(&v1));
                vDir2 = XMVector3Normalize(XMLoadFloat3(&v2));

                bool bCCW = XMVectorGetX(XMVector3Dot(vDir1, vDir2)) > 0;

                if (bCCW)
                {
                    pDir[0] = pDir[1];
                    pDir[1] = pTarget;

                    if (true == CheckCCW(lstNextStep, pStart, pDir, step + 1, lstOut))
                    {
                        lstOut.push_front(pTarget);
                        return true;
                    }
                        
                }
            }
        }
    }
    else
    {
        //마지막 방향과 제일 처음 정점과 비교하여 CCW이면 정렬 완료
        v1 = pDir[0]->v3Vtx - pDir[1]->v3Vtx;
        v2 = pStart->v3Vtx - pDir[1]->v3Vtx;

        vDir1 = XMVector3Normalize(XMLoadFloat3(&v1));
        vDir2 = XMVector3Normalize(XMLoadFloat3(&v2));

        return XMVectorGetX(XMVector3Dot(vDir1, vDir2)) > 0;
    }

    return false;
}

void SortCCW(CFace &face, VERTEX* pVertices[4], size_t count)
{
    list<VERTEX*> lstNext, lstResult;
    VERTEX* pDir[2];
    VERTEX* pResult[4];
    for (size_t i = 0; i < count; ++i)
    {
        lstNext.clear();
        lstResult.clear();

        pDir[0] = pVertices[i];
        for (size_t j = 0; j < count; ++j)
        {
            if (i != j)
            {
                lstNext.push_back(pVertices[j]);
            }
        }

        //0번째 스텝 시작
        

        if (true == CheckCCW(lstNext, pDir[0], pDir, 0, lstResult))
        {
            lstResult.push_back(pDir[0]);
            size_t size = lstResult.size();

            list<VERTEX*>::iterator iter;
            int cnt = 0;
            for (iter = lstResult.begin(); iter != lstResult.end(); ++iter)
                pResult[cnt++] = *iter;
            
            face.SetFace(*pResult[0], *pResult[1], *pResult[2], *pResult[3]);

            break;
        }
        else
        {
            continue;
        }
    }
}


void CMesh::MakeFaceGroup(MESH_MODEL* pMesh, OBJ_INFO *pObjInfo, OBJ_GROUP_INFO* pGroupInfo)
{
    size_t tGroupCount = pGroupInfo->vecNext.size();
    if (tGroupCount != 0)
    {
        for (size_t i = 0; i < tGroupCount; ++i)
        {
            //for face group
            pMesh->vecNext.push_back(MESH_MODEL());
            MakeFaceGroup(&pMesh->vecNext.back(), pObjInfo, &pGroupInfo->vecNext[i]);
        }
    }
    else
    {
        VERTEX *newVertices[4];
        size_t tVtxCount = pObjInfo->pvecVertexInfo->size();
        //OBJ_VERTEX_INFO* pVertices = pObjInfo->pvecVertexInfo->data();
        
        size_t tUvCount = pObjInfo->pvecTextureInfo->size();
        OBJ_TEXTURE_INFO* pUV = pObjInfo->pvecTextureInfo->data();
        size_t tNormalCount = pObjInfo->pvecNormalInfo->size();
        OBJ_NORMAL_INFO* pNormals = pObjInfo->pvecNormalInfo->data();

        size_t tFaceCount = pGroupInfo->vecFaceInfo.size();
        OBJ_FACE_INFO* pFacies = pGroupInfo->vecFaceInfo.data();

        //insert vertices
        pMesh->vecVertices.resize(pObjInfo->pvecVertexInfo->size());

        for (size_t i = 0; i < pObjInfo->pvecVertexInfo->size(); ++i)
        {
            pMesh->vecVertices[i].v3Vtx.x = (*pObjInfo->pvecVertexInfo)[i].x;
            pMesh->vecVertices[i].v3Vtx.y = (*pObjInfo->pvecVertexInfo)[i].y;
            pMesh->vecVertices[i].v3Vtx.z = (*pObjInfo->pvecVertexInfo)[i].z;
        }
        VERTEX* pVertices = pMesh->vecVertices.data();

        //VERTEX* pFaceInVertices;

        pMesh->vecFacies.resize(tFaceCount);

        for (size_t i = 0; i < tFaceCount; ++i)
        {
            for (int cnt = 0; cnt < pFacies[i].NumOfElements; ++cnt)
            {
                DWORD idxVtx    =   pFacies[i].vecElements[cnt].vertex - 1;
                DWORD idxUV     =   pFacies[i].vecElements[cnt].texture - 1;
                DWORD idxNormal =   pFacies[i].vecElements[cnt].normal - 1;

                newVertices[cnt] = &pVertices[idxVtx];

                //update texture
                newVertices[cnt]->v2UV.x = pUV[idxUV].u;
                newVertices[cnt]->v2UV.y = pUV[idxUV].v;

                ////update normal
                newVertices[cnt]->v3Normal.x = pNormals[idxNormal].x;
                newVertices[cnt]->v3Normal.y = pNormals[idxNormal].y;
                newVertices[cnt]->v3Normal.z = pNormals[idxNormal].z;

                newVertices[cnt]->uiIndex = cnt;

            }

            CFace& face = pMesh->vecFacies[i];

            if (3 == pFacies[i].NumOfElements)
            {
                face.SetFace(*newVertices[0], *newVertices[1], *newVertices[2]);
            }
            else
            {
                //SortCCW(face, newVertices, 4);

                face.SetFace(*newVertices[0], *newVertices[1], *newVertices[2], *newVertices[3]);

                
            }

            pMesh->strMaterialName = pGroupInfo->strMaterialName;
            pMesh->strName = pGroupInfo->strGroupName;
            
            face.SetMaterialName(pMesh->strMaterialName);
        }
    }
}
