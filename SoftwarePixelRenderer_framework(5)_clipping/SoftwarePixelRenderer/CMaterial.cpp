#include "pch.h"
#include "CMaterial.h"

#include <fstream>

CMaterial::CMaterial()
{
}

CMaterial::~CMaterial()
{
}

bool CMaterial::Open(const wstring strPath, const wstring strFileName)
{
	Clear();

	wifstream fi(strPath + strFileName);

	if (false == fi.is_open()) return false;
	if (dfBMPRENDERTARGET() == nullptr) return false;

	MakeMaterialInfo(fi, strPath);

	return true;
}

bool CMaterial::Open(const wstring strPathName)
{
	Clear();

	wifstream fi(strPathName);

	if (false == fi.is_open()) return false;
	if (dfBMPRENDERTARGET() == nullptr) return false;

	MakeMaterialInfo(fi, L"");

	return true;
}

size_t CMaterial::GetTotalTextureCount() const
{
	return m_mapTextures.size();
}

CWicBitmap* CMaterial::GetTexture(const wstring strTextureName)
{
	map<wstring, CWicBitmap*>::iterator iter;
	iter = m_mapTextures.find(strTextureName);
	if (iter == m_mapTextures.end()) return nullptr;
	return iter->second;
}

void CMaterial::SetMaterial(const wstring strMaterialName, const OBJ_MATERIAL_INFO& material)
{
	m_mapMtlInfo[strMaterialName] = material;
}

OBJ_MATERIAL_INFO* CMaterial::GetMaterialInfo(const wstring strMaterialName)
{
	return &m_mapMtlInfo[strMaterialName];
}

void CMaterial::Clear()
{
	map<wstring, CWicBitmap*>::iterator iter;

	for (iter = m_mapTextures.begin(); iter != m_mapTextures.end(); ++iter)
	{
		iter->second->ShutdownResource();
		delete iter->second;
	}

	m_mapTextures.clear();

	m_mapMtlInfo.clear();
}

void CMaterial::MakeMaterialInfo(wifstream& fi, const wstring strPath)
{
	wchar_t szLine[256] = { 0, };

	size_t len = 0;

	vector<wstring> vecValues;

	OBJ_MATERIAL_INFO *pInfo = nullptr;

	while (!fi.eof())
	{
		fi.getline(szLine, 256);
		len = wcslen(szLine);

		if (szLine[0] == L'#') continue;

		GetLineValue(szLine, len + 1, vecValues);

		if (vecValues[0] == L"newmtl")
		{
			pInfo = &m_mapMtlInfo[vecValues[1]];

			pInfo->strMaterialName = vecValues[1];
		}
		else if (vecValues[0] == L"Ns")
		{
			if (nullptr == pInfo) break;

			pInfo->Ns = (float)_wtof(vecValues[1].c_str());
		}
		else if (vecValues[0] == L"Ka")
		{
			if (nullptr == pInfo) break;

			for (size_t i = 1; i < vecValues.size(); ++i)
			{
				pInfo->Ka[i - 1] = (float)_wtof(vecValues[i].c_str());
			}
		}
		else if (vecValues[0] == L"Kd")
		{
			if (nullptr == pInfo) break;

			for (size_t i = 1; i < vecValues.size(); ++i)
			{
				pInfo->Kd[i - 1] = (float)_wtof(vecValues[i].c_str());
			}
		}
		else if (vecValues[0] == L"Ks")
		{
			if (nullptr == pInfo) break;

			for (size_t i = 1; i < vecValues.size(); ++i)
			{
				pInfo->Ks[i - 1] = (float)_wtof(vecValues[i].c_str());
			}
		}
		else if (vecValues[0] == L"Ke")
		{
			if (nullptr == pInfo) break;

			for (size_t i = 1; i < vecValues.size(); ++i)
			{
				pInfo->Ke[i - 1] = (float)_wtof(vecValues[i].c_str());
			}
		}
		else if (vecValues[0] == L"Ni")
		{
			if (nullptr == pInfo) break;

			pInfo->Ni = (float)_wtof(vecValues[1].c_str());
		}
		else if (vecValues[0] == L"d")
		{
			if (nullptr == pInfo) break;

			pInfo->d = (float)_wtof(vecValues[1].c_str());
		}
		else if (vecValues[0] == L"illum")
		{
			if (nullptr == pInfo) break;

			pInfo->illum = _wtoi(vecValues[1].c_str());
		}
		else if (vecValues[0] == L"map_Ka")
		{
			if (nullptr == pInfo) break;

			pInfo->map_Ka = vecValues[1];
			
			CWicBitmap* bmp = new CWicBitmap;
			if (TRUE == bmp->LoadWicBitmap(dfBMPRENDERTARGET(), (strPath + pInfo->map_Ka).c_str()))
			{
				m_mapTextures.insert(make_pair(pInfo->map_Ka, bmp));
			}
			else
				delete bmp;


		}
		else if (vecValues[0] == L"map_Kd")
		{
			if (nullptr == pInfo) break;

			pInfo->map_Kd = vecValues[1];
			CWicBitmap* bmp = new CWicBitmap;
			if (TRUE == bmp->LoadWicBitmap(dfBMPRENDERTARGET(), (strPath + pInfo->map_Kd).c_str()))
			{
				m_mapTextures.insert(make_pair(pInfo->map_Kd, bmp));
			}
			else
				delete bmp;
		}
		else if (vecValues[0] == L"map_Ks")
		{
			if (nullptr == pInfo) break;

			pInfo->map_Ks = vecValues[1];
			CWicBitmap *bmp = new CWicBitmap;
			if (TRUE == bmp->LoadWicBitmap(dfBMPRENDERTARGET(), (strPath + pInfo->map_Ks).c_str()))
			{
				m_mapTextures.insert(make_pair(pInfo->map_Ks, bmp));
			}
			else
				delete bmp;

		}
		else if (vecValues[0] == L"map_Ns")
		{
			if (nullptr == pInfo) break;

			pInfo->map_Ns = vecValues[1];
			CWicBitmap *bmp = new CWicBitmap;
			if (TRUE == bmp->LoadWicBitmap(dfBMPRENDERTARGET(), (strPath + pInfo->map_Ns).c_str()))
			{
				m_mapTextures.insert(make_pair(pInfo->map_Ns, bmp));
			}
			else
				delete bmp;
		}
		else if (vecValues[0] == L"map_d")
		{
			if (nullptr == pInfo) break;

			pInfo->map_d = vecValues[1];
			CWicBitmap *bmp = new CWicBitmap;
			if (TRUE == bmp->LoadWicBitmap(dfBMPRENDERTARGET(), (strPath + pInfo->map_d).c_str()))
			{
				m_mapTextures.insert(make_pair(pInfo->map_d, bmp));
			}
			else
				delete bmp;
		}
		else if (vecValues[0] == L"map_bump")
		{
			if (nullptr == pInfo) break;

			pInfo->map_bump = vecValues[1];
			CWicBitmap *bmp = new CWicBitmap;
			if (TRUE == bmp->LoadWicBitmap(dfBMPRENDERTARGET(), (strPath + pInfo->map_bump).c_str()))
			{
				m_mapTextures.insert(make_pair(pInfo->map_bump, bmp));
			}
			else
				delete bmp;
		}
		else if (vecValues[0] == L"bump")
		{
			if (nullptr == pInfo) break;

			pInfo->bump = vecValues[1];
			CWicBitmap *bmp = new CWicBitmap;
			if (TRUE == bmp->LoadWicBitmap(dfBMPRENDERTARGET(), (strPath + pInfo->bump).c_str()))
			{
				m_mapTextures.insert(make_pair(pInfo->bump, bmp));
			}
			else
				delete bmp;
		}
		else if (vecValues[0] == L"disp")
		{
			if (nullptr == pInfo) break;

			pInfo->disp = vecValues[1];
			CWicBitmap *bmp = new CWicBitmap;
			if (TRUE == bmp->LoadWicBitmap(dfBMPRENDERTARGET(), (strPath + pInfo->disp).c_str()))
			{
				m_mapTextures.insert(make_pair(pInfo->disp, bmp));
			}
			else
				delete bmp;
		}
		else if (vecValues[0] == L"decal")
		{
			if (nullptr == pInfo) break;

			pInfo->decal = vecValues[1];
			CWicBitmap *bmp = new CWicBitmap;
			if (TRUE == bmp->LoadWicBitmap(dfBMPRENDERTARGET(), (strPath + pInfo->decal).c_str()))
			{
				m_mapTextures.insert(make_pair(pInfo->decal, bmp));
			}
		}
	}

}
