//-------------------------------------------------------------------------------------------
// 文件：创建 shm 文件
//
//-------------------------------------------------------------------------------------------
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include "Target.h"


int main(int argc, char*  argv[])
{
	//
	CSHMFILE ShmFile;

	printf("\n===========================================================");
	printf("\nShmFile.shm 文件创建程序 李健波编写  V 1.0");
	printf("\n例如：注意模型文件和效果已经包含在内");
	printf("\n       ShmFile.exe 1 只使用颜色纹理");
	printf("\n       ShmFile.exe 2 使用颜色纹理和法线纹理");
	printf("\n       ShmFile.exe 3 使用颜色纹理、法线纹理和高光纹理");
	printf("\n       ShmFile.exe 4 只使用立方体颜色纹理");
	printf("\n       ShmFile.exe 5 只使用体积纹理");
	printf("\n       ShmFile.exe 6 使用颜色纹理、法线纹理和高度纹理");
	printf("\n       ShmFile.exe 7 使用多模型，单个颜色纹理");
	printf("\n       ShmFile.exe 8 使用多模型，多颜色纹理");
	printf("\n       ShmFile.exe 9 之用立方体纹理、颜色纹理、凹凸纹理");
	printf("\n===========================================================");
	if (2 == argc)
	{
		int iInput = atoi(argv[1]); // 注意：argv[ 0 ] 是本程序的名字
		if ((iInput > 0) && (iInput <= 9))
		{
			//
			ShmFile.InitContent(iInput);
			printf("\n资源数：%d", ShmFile.ReturnRes());
			if (S_OK == ShmFile.WriteToFile())
			{
				printf("\n成功写入文件");
			}
		}
		else printf("\n提示：输入范围不正确");
	}
	else printf("\n提示：输入参数不符要求");

	/*
	if( S_OK == ShmFile.ReadFromFile( ))
	{
	printf( "\n成功读入文件" );
	ShmFile.Out( );
	}
	*/
	printf("\n按回车键退出");
	_getch();
}




