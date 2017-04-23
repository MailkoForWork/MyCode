#include<d3d9.h>
#ifndef SHADERMODULEFILE_H
#define SHADERMODULEFILE_H

#define FILENAME   "ShmFile.shm" 
#define FILEHEADTAG  "SHMFILE" 
#include "Common.h"

//---------------------------------------------------------------
// 资源类型
//---------------------------------------------------------------
enum  RESTYPE
{
	// Model
	ENU_MODELX = 0X00100001, // X Model
	ENU_MODELOTHER = 0X00100002, // 其他类型的模型
	ENU_MODELXSET = 0X00100003, // 由多个 x 文件组成的变形动画组
								// Texture Type
	ENU_TEXCOLORMAP = 0X00200001, // Color Map
	ENU_TEXNORMALMAP = 0X00200002, // Normal Map
	ENU_TEXBUMPMAP = 0X00200003, // Bump Map
	ENU_SPECULARMAP = 0x00200004, // Specular Map
	ENU_CUBEMAP = 0x00200005, // Cube Map
	ENU_VOLUMEMAP = 0x00200006, // Volume Map
	ENU_TEXHEIGHTMAP = 0x00200007, // Height Map
	ENU_TEXCOLORMAPSET = 0x00210001, // Color Map Set
									 // Fx File
	ENU_FXFILE = 0X00300001,
	ENU_FXFILEBIN = 0X00300002,
	// Music        
	ENU_SGT = 0X00400001, // 音乐文件
	ENU_DLS = 0X00400002, // DLS 文件
						  // Path File
	ENU_ANIMATION = 0X00500001, // 路径文件
};

//--------------------------------------------------------------
// 文件头结构
//--------------------------------------------------------------
struct SSHADERMODULEFILEHEAD          // 40
{
	char    s_szHead[16];             // 16 标志
	int     s_iVerHigh;               // 4  高版本
	int     s_iVerLow;                // 4  低版本
									  //
	int     s_iPack;                  // 4  保留
	int     s_iResourceFileNum;       // 4  资源数
	int     s_iCheck[2];              // 8  校验
									  //
};
//--------------------------------------------------------------
//
//--------------------------------------------------------------
struct SRESOURCEINFO
{
	TCHAR   s_szFileName[24];       // 文件名
	RESTYPE s_enuResType;             // 资源类型
	int     s_iOffset;                // 偏移，用于打包文件中的偏移
};

#endif

#ifndef SHMFILE_H
#define SHMFILE_H

class DLL_API CSHMFILE
{
private:
	int               m_iResNum;      // 资源个数
	SRESOURCEINFO*    m_pszFile;      // 信息开始的地方
public:
	CSHMFILE();
	~CSHMFILE();
	//
	int ReturnRes() { return m_iResNum; }
	// 
	HRESULT InitContent(int);
	HRESULT WriteToFile();
	HRESULT ReadFromFile();
	void    Out();
};


#endif

