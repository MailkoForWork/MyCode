#include<d3d9.h>
#ifndef SHADERMODULEFILE_H
#define SHADERMODULEFILE_H

#define FILENAME   "ShmFile.shm" 
#define FILEHEADTAG  "SHMFILE" 
#include "Common.h"

//---------------------------------------------------------------
// ��Դ����
//---------------------------------------------------------------
enum  RESTYPE
{
	// Model
	ENU_MODELX = 0X00100001, // X Model
	ENU_MODELOTHER = 0X00100002, // �������͵�ģ��
	ENU_MODELXSET = 0X00100003, // �ɶ�� x �ļ���ɵı��ζ�����
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
	ENU_SGT = 0X00400001, // �����ļ�
	ENU_DLS = 0X00400002, // DLS �ļ�
						  // Path File
	ENU_ANIMATION = 0X00500001, // ·���ļ�
};

//--------------------------------------------------------------
// �ļ�ͷ�ṹ
//--------------------------------------------------------------
struct SSHADERMODULEFILEHEAD          // 40
{
	char    s_szHead[16];             // 16 ��־
	int     s_iVerHigh;               // 4  �߰汾
	int     s_iVerLow;                // 4  �Ͱ汾
									  //
	int     s_iPack;                  // 4  ����
	int     s_iResourceFileNum;       // 4  ��Դ��
	int     s_iCheck[2];              // 8  У��
									  //
};
//--------------------------------------------------------------
//
//--------------------------------------------------------------
struct SRESOURCEINFO
{
	TCHAR   s_szFileName[24];       // �ļ���
	RESTYPE s_enuResType;             // ��Դ����
	int     s_iOffset;                // ƫ�ƣ����ڴ���ļ��е�ƫ��
};

#endif

#ifndef SHMFILE_H
#define SHMFILE_H

class DLL_API CSHMFILE
{
private:
	int               m_iResNum;      // ��Դ����
	SRESOURCEINFO*    m_pszFile;      // ��Ϣ��ʼ�ĵط�
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

