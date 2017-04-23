#define DLL_IMPLEMENT
#include "ShmFile.h"
#include <string>
//--------------------------------------------------------
//
//--------------------------------------------------------
CSHMFILE::CSHMFILE()
{
	m_iResNum = 2;                // 模型文件和 FileFX 文件
	m_pszFile = NULL;
}
//--------------------------------------------------------
//
//--------------------------------------------------------
CSHMFILE::~CSHMFILE()
{
	if (m_pszFile) delete[]m_pszFile;
	m_pszFile = NULL;
}
//--------------------------------------------------------
//
//--------------------------------------------------------
HRESULT CSHMFILE::InitContent(int iInput)
{
	HRESULT hr = S_OK;
	switch (iInput)
	{
	case 1:   // 一般的颜色纹理
		m_iResNum = iInput + m_iResNum;
		if (NULL == (m_pszFile = new SRESOURCEINFO[m_iResNum])) return E_FAIL;
		// 模型文件
		lstrcpy(m_pszFile[0].s_szFileName, "Model.x");
		m_pszFile[0].s_enuResType = ENU_MODELX;
		m_pszFile[0].s_iOffset = 0;
		// 效果文件
		lstrcpy(m_pszFile[1].s_szFileName, "FileFX.fx");
		m_pszFile[1].s_enuResType = ENU_FXFILE;
		m_pszFile[1].s_iOffset = 0;
		// 颜色纹理
		lstrcpy(m_pszFile[2].s_szFileName, "ColorMap.JPG");
		m_pszFile[2].s_enuResType = ENU_TEXCOLORMAP;
		m_pszFile[2].s_iOffset = 0;
		break;
	case 2:   // 有法线纹理
		m_iResNum = iInput + m_iResNum;
		if (NULL == (m_pszFile = new SRESOURCEINFO[m_iResNum])) return E_FAIL;
		// 模型文件
		lstrcpy(m_pszFile[0].s_szFileName, "Model.x");
		m_pszFile[0].s_enuResType = ENU_MODELX;
		m_pszFile[0].s_iOffset = 0;
		// 效果文件
		lstrcpy(m_pszFile[1].s_szFileName, "FileFX.fx");
		m_pszFile[1].s_enuResType = ENU_FXFILE;
		m_pszFile[1].s_iOffset = 0;
		// 颜色纹理
		lstrcpy(m_pszFile[2].s_szFileName, "ColorMap.dds");
		m_pszFile[2].s_enuResType = ENU_TEXCOLORMAP;
		m_pszFile[2].s_iOffset = 0;
		// 法线纹理
		lstrcpy(m_pszFile[3].s_szFileName, "NormalMap.dds");
		m_pszFile[3].s_enuResType = ENU_TEXNORMALMAP;
		m_pszFile[3].s_iOffset = 0;
		break;
	case 3:   // 有高光纹理
		m_iResNum = iInput + m_iResNum;
		if (NULL == (m_pszFile = new SRESOURCEINFO[m_iResNum])) return E_FAIL;
		// 模型文件
		lstrcpy(m_pszFile[0].s_szFileName, "Model.x");
		m_pszFile[0].s_enuResType = ENU_MODELX;
		m_pszFile[0].s_iOffset = 0;
		// 效果文件
		lstrcpy(m_pszFile[1].s_szFileName, "FileFX.fx");
		m_pszFile[1].s_enuResType = ENU_FXFILE;
		m_pszFile[1].s_iOffset = 0;
		// 颜色纹理
		lstrcpy(m_pszFile[2].s_szFileName, "ColorMap.dds");
		m_pszFile[2].s_enuResType = ENU_TEXCOLORMAP;
		m_pszFile[2].s_iOffset = 0;
		// 法线纹理
		lstrcpy(m_pszFile[3].s_szFileName, "NormalMap.dds");
		m_pszFile[3].s_enuResType = ENU_TEXNORMALMAP;
		m_pszFile[3].s_iOffset = 0;
		// 高光纹理
		lstrcpy(m_pszFile[4].s_szFileName, "SpecularMap.dds");
		m_pszFile[4].s_enuResType = ENU_SPECULARMAP;
		m_pszFile[4].s_iOffset = 0;
		break;
	case 4:   // 立方体纹理
		m_iResNum = m_iResNum + 1;
		if (NULL == (m_pszFile = new SRESOURCEINFO[m_iResNum])) return E_FAIL;
		// 模型文件
		lstrcpy(m_pszFile[0].s_szFileName, "Model.x");
		m_pszFile[0].s_enuResType = ENU_MODELX;
		m_pszFile[0].s_iOffset = 0;
		// 效果文件
		lstrcpy(m_pszFile[1].s_szFileName, "FileFX.fx");
		m_pszFile[1].s_enuResType = ENU_FXFILE;
		m_pszFile[1].s_iOffset = 0;
		// 颜色纹理
		lstrcpy(m_pszFile[2].s_szFileName, "CubeMap.dds");
		m_pszFile[2].s_enuResType = ENU_CUBEMAP;
		m_pszFile[2].s_iOffset = 0;
		break;
	case 5:// 体积纹理
		m_iResNum = m_iResNum + 1;
		if (NULL == (m_pszFile = new SRESOURCEINFO[m_iResNum])) return E_FAIL;
		// 模型文件
		lstrcpy(m_pszFile[0].s_szFileName, "Model.x");
		m_pszFile[0].s_enuResType = ENU_MODELX;
		m_pszFile[0].s_iOffset = 0;
		// 效果文件
		lstrcpy(m_pszFile[1].s_szFileName, "FileFX.fx");
		m_pszFile[1].s_enuResType = ENU_FXFILE;
		m_pszFile[1].s_iOffset = 0;
		// 颜色纹理
		lstrcpy(m_pszFile[2].s_szFileName, "Volume.dds");
		m_pszFile[2].s_enuResType = ENU_CUBEMAP;
		m_pszFile[2].s_iOffset = 0;
		break;
	case 6:
		m_iResNum = 5;
		if (NULL == (m_pszFile = new SRESOURCEINFO[m_iResNum])) return E_FAIL;
		// 模型文件
		lstrcpy(m_pszFile[0].s_szFileName, "Model.x");
		m_pszFile[0].s_enuResType = ENU_MODELX;
		m_pszFile[0].s_iOffset = 0;
		// 效果文件
		lstrcpy(m_pszFile[1].s_szFileName, "FileFX.fx");
		m_pszFile[1].s_enuResType = ENU_FXFILE;
		m_pszFile[1].s_iOffset = 0;
		// 颜色纹理
		lstrcpy(m_pszFile[2].s_szFileName, "ColorMap.dds");
		m_pszFile[2].s_enuResType = ENU_TEXCOLORMAP;
		m_pszFile[2].s_iOffset = 0;
		// 法线纹理
		lstrcpy(m_pszFile[3].s_szFileName, "NormalMap.dds");
		m_pszFile[3].s_enuResType = ENU_TEXNORMALMAP;
		m_pszFile[3].s_iOffset = 0;
		// 高光纹理
		lstrcpy(m_pszFile[4].s_szFileName, "HeightMap.dds");
		m_pszFile[4].s_enuResType = ENU_TEXHEIGHTMAP;
		m_pszFile[4].s_iOffset = 0;
		break;
	case 7:
		m_iResNum = 3;
		if (NULL == (m_pszFile = new SRESOURCEINFO[m_iResNum])) return E_FAIL;
		// 模型文件
		lstrcpy(m_pszFile[0].s_szFileName, "ModelSet.mph");
		m_pszFile[0].s_enuResType = ENU_MODELXSET;
		m_pszFile[0].s_iOffset = 0;
		// 效果文件
		lstrcpy(m_pszFile[1].s_szFileName, "FileFX.fx");
		m_pszFile[1].s_enuResType = ENU_FXFILE;
		m_pszFile[1].s_iOffset = 0;
		// 颜色纹理
		lstrcpy(m_pszFile[2].s_szFileName, "ColorMap.dds");
		m_pszFile[2].s_enuResType = ENU_TEXCOLORMAP;
		m_pszFile[2].s_iOffset = 0;
		break;
	case 8:
		m_iResNum = 3;
		if (NULL == (m_pszFile = new SRESOURCEINFO[m_iResNum])) return E_FAIL;
		// 模型文件
		lstrcpy(m_pszFile[0].s_szFileName, "ModelSet.mph");
		m_pszFile[0].s_enuResType = ENU_MODELXSET;
		m_pszFile[0].s_iOffset = 0;
		// 效果文件
		lstrcpy(m_pszFile[1].s_szFileName, "FileFX.fx");
		m_pszFile[1].s_enuResType = ENU_FXFILE;
		m_pszFile[1].s_iOffset = 0;
		// 颜色纹理
		lstrcpy(m_pszFile[2].s_szFileName, "ColorMapSet.mph");
		m_pszFile[2].s_enuResType = ENU_TEXCOLORMAPSET;
		m_pszFile[2].s_iOffset = 0;
		break;
	case 9: // 使用立方体、颜色、凹凸纹理
		m_iResNum = 5;
		if (NULL == (m_pszFile = new SRESOURCEINFO[m_iResNum])) return E_FAIL;
		// 模型文件
		lstrcpy(m_pszFile[0].s_szFileName, "Model.x");
		m_pszFile[0].s_enuResType = ENU_MODELX;
		m_pszFile[0].s_iOffset = 0;
		// 效果文件
		lstrcpy(m_pszFile[1].s_szFileName, "FileFX.fx");
		m_pszFile[1].s_enuResType = ENU_FXFILE;
		m_pszFile[1].s_iOffset = 0;
		// 
		lstrcpy(m_pszFile[2].s_szFileName, "CubeMap.dds");
		m_pszFile[2].s_enuResType = ENU_CUBEMAP;
		m_pszFile[2].s_iOffset = 0;
		// 效果文件
		lstrcpy(m_pszFile[3].s_szFileName, "ColorMap.jpg");
		m_pszFile[3].s_enuResType = ENU_TEXCOLORMAP;
		m_pszFile[3].s_iOffset = 0;
		// 颜色纹理
		lstrcpy(m_pszFile[4].s_szFileName, "BumpMap.dds");
		m_pszFile[4].s_enuResType = ENU_TEXBUMPMAP;
		m_pszFile[4].s_iOffset = 0;
		break;
	default:
		m_iResNum = 0;
		break;
	}
	return hr;
}
//--------------------------------------------------------
//
//--------------------------------------------------------
HRESULT CSHMFILE::WriteToFile()
{
	HRESULT hr = S_OK;
	// 
	SSHADERMODULEFILEHEAD pFileHead[1];
	strcpy(pFileHead->s_szHead, FILEHEADTAG);
	pFileHead->s_iVerHigh = 1;
	pFileHead->s_iVerLow = 0;
	pFileHead->s_iPack = 0XFFFFFFFF;
	pFileHead->s_iResourceFileNum = m_iResNum;
	pFileHead->s_iCheck[0] = 0XFFFFFFFF;
	pFileHead->s_iCheck[1] = 0XFFFFFFFF;
	//
	BOOL bReturn;
	HANDLE hAndle;
	DWORD  uByte;
	if (INVALID_HANDLE_VALUE == (hAndle = CreateFile(FILENAME, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL)))
	{
		return 0;
	}
	if (NULL == (bReturn = WriteFile(hAndle, pFileHead, sizeof(SSHADERMODULEFILEHEAD), &uByte, NULL)))
	{
		CloseHandle(hAndle);
		return 0;
	}
	if (NULL == (bReturn = WriteFile(hAndle, m_pszFile, m_iResNum * sizeof(SRESOURCEINFO), &uByte, NULL)))
	{
		CloseHandle(hAndle);
		return 0;
	}
	CloseHandle(hAndle);
	delete[]m_pszFile;
	m_pszFile = NULL;
	return hr;
}
//--------------------------------------------------------
//
//--------------------------------------------------------
HRESULT CSHMFILE::ReadFromFile()
{
	HRESULT hr = S_OK;
	SSHADERMODULEFILEHEAD pFileHead[1];
	BOOL bReturn;
	HANDLE hAndle;
	DWORD  uByte;
	if (INVALID_HANDLE_VALUE == (hAndle = CreateFile(FILENAME, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL)))
	{
		return 0;
	}
	if (NULL == (bReturn = ReadFile(hAndle, &pFileHead, sizeof(SSHADERMODULEFILEHEAD), &uByte, NULL)))
	{
		CloseHandle(hAndle);
		return E_FAIL;
	}

	if (NULL != strcmp(FILEHEADTAG, &pFileHead->s_szHead[0]))
	{
		CloseHandle(hAndle);
		return E_FAIL;
	}
	//----------------- 读取渲染对象的数目 --------------------
	m_iResNum = pFileHead->s_iResourceFileNum;
	m_iResNum = m_iResNum;
	if (NULL == (m_pszFile = new SRESOURCEINFO[m_iResNum])) return E_FAIL;
	ZeroMemory(m_pszFile, m_iResNum * sizeof(SRESOURCEINFO));
	if (NULL == (bReturn = ReadFile(hAndle, m_pszFile, m_iResNum * sizeof(SRESOURCEINFO), &uByte, NULL)))
	{
		delete[]m_pszFile;
		CloseHandle(hAndle);
		return E_FAIL;
	}
	CloseHandle(hAndle);
	return hr;
}
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
void CSHMFILE::Out()
{
	SRESOURCEINFO*  szpBuffer = m_pszFile;
	//for (int i = 0; i < m_iResNum; i++)
	//{
	//	/*TCHAR szOut[24];*/
	//	wprintf(L"\n%s", szpBuffer[i].s_szFileName);
	//	//_snwprintf( szOut ,24 , L"%s" ,szpBuffer[ i ].s_szFileName );
	//}
	delete[]m_pszFile;
	m_pszFile = NULL;
}

