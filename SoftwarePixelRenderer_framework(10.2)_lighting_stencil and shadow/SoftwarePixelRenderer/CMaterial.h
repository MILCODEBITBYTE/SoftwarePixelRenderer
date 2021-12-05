#pragma once

#include "CWicBitmap.h"

typedef struct _tagObjMaterialInfo
{
	wstring strMaterialName;
	float Ns;
	float Ka[3];
	float Kd[3];
	float Ks[3];
	float Ke[3];
	float Ni;
	float d;
	int		illum;

	wstring map_Ka;
	wstring map_Kd;
	wstring map_Ks;
	wstring map_Ns;
	wstring map_d;
	wstring map_bump;
	wstring bump;
	wstring disp;
	wstring decal;

	_tagObjMaterialInfo()
	{
		strMaterialName = L"";
		Ns = 0;
		
		for (int i = 0 ; i < 3; ++i)
		{
			Ka[i] = 0;
			Kd[i] = 0;
			Ks[i] = 0;
			Ke[i] = 0;
		}

		Ni = 0;
		d = 0;
		illum = 0;

		map_Ka = L"";
		map_Kd = L"";
		map_Ks = L"";
		map_Ns = L"";
		map_d = L"";
		map_bump = L"";
		bump = L"";
		disp = L"";
		decal = L"";
	}

}OBJ_MATERIAL_INFO;

class CMaterial
{
public:
	CMaterial();
	~CMaterial();


	bool Open(const wstring strPath, const wstring strFileName);
	bool Open(const wstring strPathName);

	size_t GetTotalTextureCount() const;
	CWicBitmap* GetTexture(const wstring strTextureName);
	void SetMaterial(const wstring strMaterialName, const OBJ_MATERIAL_INFO& material);
	OBJ_MATERIAL_INFO* GetMaterialInfo(const wstring strMaterialName);

	void Clear();

protected:
	void MakeMaterialInfo(wifstream& fi, const wstring strPath);

private:
	map<wstring, CWicBitmap*>	m_mapTextures;
	map<wstring, OBJ_MATERIAL_INFO>		m_mapMtlInfo;
};

