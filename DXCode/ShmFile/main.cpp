//-------------------------------------------------------------------------------------------
// �ļ������� shm �ļ�
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
	printf("\nShmFile.shm �ļ��������� �����д  V 1.0");
	printf("\n���磺ע��ģ���ļ���Ч���Ѿ���������");
	printf("\n       ShmFile.exe 1 ֻʹ����ɫ����");
	printf("\n       ShmFile.exe 2 ʹ����ɫ����ͷ�������");
	printf("\n       ShmFile.exe 3 ʹ����ɫ������������͸߹�����");
	printf("\n       ShmFile.exe 4 ֻʹ����������ɫ����");
	printf("\n       ShmFile.exe 5 ֻʹ���������");
	printf("\n       ShmFile.exe 6 ʹ����ɫ������������͸߶�����");
	printf("\n       ShmFile.exe 7 ʹ�ö�ģ�ͣ�������ɫ����");
	printf("\n       ShmFile.exe 8 ʹ�ö�ģ�ͣ�����ɫ����");
	printf("\n       ShmFile.exe 9 ֮��������������ɫ������͹����");
	printf("\n===========================================================");
	if (2 == argc)
	{
		int iInput = atoi(argv[1]); // ע�⣺argv[ 0 ] �Ǳ����������
		if ((iInput > 0) && (iInput <= 9))
		{
			//
			ShmFile.InitContent(iInput);
			printf("\n��Դ����%d", ShmFile.ReturnRes());
			if (S_OK == ShmFile.WriteToFile())
			{
				printf("\n�ɹ�д���ļ�");
			}
		}
		else printf("\n��ʾ�����뷶Χ����ȷ");
	}
	else printf("\n��ʾ�������������Ҫ��");

	/*
	if( S_OK == ShmFile.ReadFromFile( ))
	{
	printf( "\n�ɹ������ļ�" );
	ShmFile.Out( );
	}
	*/
	printf("\n���س����˳�");
	_getch();
}




