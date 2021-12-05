#include "pch.h"
#include "CModelImportor.h"

#include <fstream>

CModelImportor::CModelImportor()
{
	m_bOpend = false;
}

CModelImportor::CModelImportor(const wchar_t* szFileName)
{
	wifstream fi(szFileName);

	m_bOpend = fi.is_open();

	if (true == m_bOpend)
	{
		MakeObjectInfo(fi);
	}
}

CModelImportor::~CModelImportor()
{
}

bool CModelImportor::isOpen() const
{
	return m_bOpend;
}

bool CModelImportor::Open(const wchar_t* szFileName)
{
	wifstream fi(szFileName);
	m_bOpend = fi.is_open();

	if (true == m_bOpend)
	{
		MakeObjectInfo(fi);
	}

	return m_bOpend;
}

size_t CModelImportor::GetObjectCount() const
{
	return m_vecObjectInfo.size();
}

OBJ_INFO* CModelImportor::GetObjectInfo()
{
	return m_vecObjectInfo.data();
}

wstring CModelImportor::GetMaterialFileName() const
{
	return m_strMaterialFileName;
}


void CModelImportor::Clear()
{
	m_vecObjectInfo.clear();
	m_vecVertexInfo.clear();
	m_vecTextureInfo.clear();
	m_vecNormalInfo.clear();
}


void GetFaceValue(const wchar_t* szFaceInfo, const size_t count, vector<DWORD>& vecOut)
{
	vecOut.clear();

	wchar_t szValue[20] = { 0, };

	int iValueIndex = 0;
	for (size_t i = 0; i < count; ++i)
	{
		if (szFaceInfo[i] == L'/' || szFaceInfo[i] == L'\r' || szFaceInfo[i] == '\n' || szFaceInfo[i] == '\0')
		{
			szValue[iValueIndex] = L'\0';
			vecOut.push_back((DWORD)_wtol(szValue));

			iValueIndex = 0;
		}
		else
			szValue[iValueIndex++] = szFaceInfo[i];
	}
}

#define CheckObject(o)							\
{												\
	if(nullptr == o)							\
	{											\
		m_vecObjectInfo.push_back(OBJ_INFO());	\
		pObj = &m_vecObjectInfo.back();			\
		pObj->strObjName = L"default";			\
	}											\
}

#define CheckGroup(o, g)								\
{														\
	if (nullptr == g)									\
	{													\
		o->vecGroupInfo.push_back(OBJ_GROUP_INFO());	\
		pGroup = &o->vecGroupInfo.back();				\
		pGroup->strGroupName = L"default";				\
	}													\
														\
}

inline OBJ_GROUP_INFO* FindGroup(vector<OBJ_GROUP_INFO>* pvecGroup, const wstring strGroupName)
{
	for (size_t i = 0; i < pvecGroup->size(); ++i)
	{
		if ((*pvecGroup)[i].strGroupName == strGroupName) return &(*pvecGroup)[i];
	}
	return nullptr;;
}


void CModelImportor::MakeObjectInfo(wifstream &fi)
{
	wchar_t szLine[256] = { 0, };

	Clear();

	size_t len = 0;

	vector<wstring> vecValues;

	OBJ_GROUP_INFO* pGroup = nullptr;

	OBJ_INFO* pObj = nullptr;

	while (!fi.eof())
	{
		fi.getline(szLine, 256);
		len = wcslen(szLine);

		if (szLine[0] == L'#') continue;


		GetLineValue(szLine, len+1, vecValues);

		if (vecValues[0] == L"mtllib")
		{
			size_t cnt = vecValues.size();
			wstring strNames = vecValues[cnt- 1];

			for (size_t i = cnt - 2; i >= 1; --i)
				strNames = vecValues[i] + L" " + strNames;

			m_strMaterialFileName = strNames;

			//TBD
		}
		else if (vecValues[0] == L"o")
		{
			m_vecObjectInfo.push_back(OBJ_INFO());
			pObj = &m_vecObjectInfo.back();
			pObj->strObjName = vecValues[1];
			pObj->pvecVertexInfo = &m_vecVertexInfo;
			pObj->pvecTextureInfo = &m_vecTextureInfo;
			pObj->pvecNormalInfo = &m_vecNormalInfo;
			pGroup = nullptr;
		}
		else if (vecValues[0] == L"v")
		{
			CheckObject(pObj);

			OBJ_VERTEX_INFO info;

			for (size_t i = 1; i < vecValues.size(); ++i)
			{
				switch (i)
				{
				case 1:
					info.x = (float)_wtof(vecValues[i].c_str());
					break;
				case 2:
					info.y = (float)_wtof(vecValues[i].c_str());
					break;
				case 3:
					info.z = (float)_wtof(vecValues[i].c_str());
					break;
				}
			}

			m_vecVertexInfo.push_back(info);
		}
		else if (vecValues[0] == L"vt")
		{
			CheckObject(pObj);

			OBJ_TEXTURE_INFO info;

			for (size_t i = 1; i < vecValues.size(); ++i)
			{
				switch (i)
				{
				case 1:
					info.u = (float)_wtof(vecValues[i].c_str());
					break;
				case 2:
					info.v = (float)_wtof(vecValues[i].c_str());
					break;
				}
			}

			m_vecTextureInfo.push_back(info);
		}
		else if (vecValues[0] == L"vn")
		{
			CheckObject(pObj);

			OBJ_NORMAL_INFO info;

			for (size_t i = 1; i < vecValues.size(); ++i)
			{
				switch (i)
				{
				case 1:
					info.x = (float)_wtof(vecValues[i].c_str());
					break;
				case 2:
					info.y = (float)_wtof(vecValues[i].c_str());
					break;
				case 3:
					info.z = (float)_wtof(vecValues[i].c_str());
					break;
				}

			}
			m_vecNormalInfo.push_back(info);
		}
		else if (vecValues[0] == L"g")
		{
			CheckObject(pObj);

			if (nullptr == pGroup)
			{
				vector<OBJ_GROUP_INFO>* pvecGroups = &pObj->vecGroupInfo;
				for (size_t i = vecValues.size() - 1; i > 0; --i)
				{
					pGroup = FindGroup(pvecGroups, vecValues[i]);

					if (nullptr == pGroup)
					{
						pvecGroups->push_back(OBJ_GROUP_INFO());
						pGroup = &pvecGroups->back();
						pGroup->strGroupName = vecValues[i];
						pvecGroups = &pGroup->vecNext;
					}
					else
					{
						pvecGroups = &pGroup->vecNext;
					}
				}
			}
			else
			{
				vector<OBJ_GROUP_INFO>* pvecGroups = &pGroup->vecNext;
				size_t tStart = 1;
				if(pGroup->strGroupName == L"default")
				{
					pGroup->strGroupName = vecValues[vecValues.size() - 1];
					tStart = vecValues.size() - 2;
				}
				else
				{
					pvecGroups = &pObj->vecGroupInfo;
					tStart = vecValues.size() - 1;
				}
				
				for (size_t i = tStart; i > 0; --i)
				{
					pGroup = FindGroup(pvecGroups, vecValues[i]);

					if (nullptr == pGroup)
					{
						pvecGroups->push_back(OBJ_GROUP_INFO());
						pGroup = &pvecGroups->back();
						pGroup->strGroupName = vecValues[i];
						pvecGroups = &pGroup->vecNext;
					}
					else
					{
						pvecGroups = &pGroup->vecNext;
					}
				}
			}


		}
		else if (vecValues[0] == L"usemtl")
		{
			CheckObject(pObj);
			CheckGroup(pObj, pGroup);

			pGroup->strMaterialName = vecValues[1];
		}
		else if (vecValues[0] == L"s")
		{
			CheckObject(pObj);
			CheckGroup(pObj, pGroup);

			if (vecValues[1] != L"off")
			{
				pGroup->smoothing = _wtol(vecValues[1].c_str());
			}
			else
				pGroup->smoothing = 0;
		}
		else if (vecValues[0] == L"f")
		{
			CheckObject(pObj);
			CheckGroup(pObj, pGroup);

			//Face count 3
			if (vecValues.size() <= 4)
			{
				OBJ_FACE_INFO info;
				info.NumOfElements = 3;
				info.vecElements.resize(3);

				vector<DWORD> vecFaceValues;
				GetFaceValue(vecValues[1].c_str(), wcslen(vecValues[1].c_str())+1, vecFaceValues);
				info.vecElements[0].vertex = vecFaceValues[0];
				info.vecElements[0].texture = vecFaceValues[1];
				info.vecElements[0].normal = vecFaceValues[2];

				GetFaceValue(vecValues[2].c_str(), wcslen(vecValues[2].c_str())+1, vecFaceValues);
				info.vecElements[1].vertex = vecFaceValues[0];
				info.vecElements[1].texture = vecFaceValues[1];
				info.vecElements[1].normal = vecFaceValues[2];

				GetFaceValue(vecValues[3].c_str(), wcslen(vecValues[3].c_str())+1, vecFaceValues);
				info.vecElements[2].vertex = vecFaceValues[0];
				info.vecElements[2].texture = vecFaceValues[1];
				info.vecElements[2].normal = vecFaceValues[2];


				pGroup->vecFaceInfo.push_back(info);
				
			}
			//Face count 4
			else
			{
				OBJ_FACE_INFO info;
				info.NumOfElements = 4;
				info.vecElements.resize(4);

				vector<DWORD> vecFaceValues;
				GetFaceValue(vecValues[1].c_str(), wcslen(vecValues[1].c_str())+1, vecFaceValues);
				info.vecElements[0].vertex = vecFaceValues[0];
				info.vecElements[0].texture = vecFaceValues[1];
				info.vecElements[0].normal = vecFaceValues[2];

				GetFaceValue(vecValues[2].c_str(), wcslen(vecValues[2].c_str())+1, vecFaceValues);
				info.vecElements[1].vertex = vecFaceValues[0];
				info.vecElements[1].texture = vecFaceValues[1];
				info.vecElements[1].normal = vecFaceValues[2];

				GetFaceValue(vecValues[3].c_str(), wcslen(vecValues[3].c_str())+1, vecFaceValues);
				info.vecElements[2].vertex = vecFaceValues[0];
				info.vecElements[2].texture = vecFaceValues[1];
				info.vecElements[2].normal = vecFaceValues[2];

				GetFaceValue(vecValues[4].c_str(), wcslen(vecValues[4].c_str())+1, vecFaceValues);
				info.vecElements[3].vertex = vecFaceValues[0];
				info.vecElements[3].texture = vecFaceValues[1];
				info.vecElements[3].normal = vecFaceValues[2];


				pGroup->vecFaceInfo.push_back(info);
			}
		}
	}
}