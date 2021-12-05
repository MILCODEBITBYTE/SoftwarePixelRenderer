#pragma once

typedef struct _tagObjVertexInfo
{
	float x, y, z;
}OBJ_VERTEX_INFO;

typedef struct _tagObjTextureInfo
{
	float u, v;
}OBJ_TEXTURE_INFO;

typedef struct _tagObjNormalInfo
{
	float x, y, z;
}OBJ_NORMAL_INFO;

typedef struct _tagFaceElement
{
	DWORD	vertex;
	DWORD	texture;
	DWORD	normal;
}FACE_ELEMENT;

typedef struct _tagObjFaceInfo
{
	unsigned char NumOfElements;
	vector<FACE_ELEMENT> vecElements;
}OBJ_FACE_INFO;

typedef struct _tagObjGroupInfo
{
	wstring						strGroupName;
	wstring						strMaterialName;
	DWORD						smoothing;

	vector<OBJ_FACE_INFO>		vecFaceInfo;
	vector<_tagObjGroupInfo>		vecNext;
}OBJ_GROUP_INFO;

typedef struct _tagObjInfo
{
	wstring							strObjName;
	vector<OBJ_VERTEX_INFO>*	pvecVertexInfo;
	vector<OBJ_TEXTURE_INFO>*	pvecTextureInfo;
	vector<OBJ_NORMAL_INFO>*	pvecNormalInfo;
	vector<OBJ_GROUP_INFO>			vecGroupInfo;
}OBJ_INFO;

class CModelImportor
{
public :
	CModelImportor();
	CModelImportor(const wchar_t* szFileName);
	~CModelImportor();

	bool isOpen() const;
	bool Open(const wchar_t* szFileName);

	size_t GetObjectCount() const;
	OBJ_INFO* GetObjectInfo();


	wstring GetMaterialFileName() const;

	void Clear();

private:
	void MakeObjectInfo(wifstream &fi);
	bool m_bOpend;

private:

	vector<OBJ_INFO>				m_vecObjectInfo;
	wstring						m_strMaterialFileName;

	vector<OBJ_VERTEX_INFO>			m_vecVertexInfo;
	vector<OBJ_TEXTURE_INFO>		m_vecTextureInfo;
	vector<OBJ_NORMAL_INFO>			m_vecNormalInfo;
	
};

