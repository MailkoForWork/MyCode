#pragma once
#ifdef DLL_IMPLEMENT 

#define DLL_API __declspec(dllexport) 

#else 

#define DLL_API __declspec(dllimport) 

#endif 

#include<string>
#include "d3dx9.h"

#define GET_SET_FUNCTION(CType, ValueName)  VOID Set##Value(CType value){ m_##ValueName = value;} const CType& Get##Value(){return m_##ValueName;}
#define GET_FUNCTION(CType, ValueName) CType Get##ValueName(){ return m_##ValueName;}
#define SET_FUNCTION(CTYPE, VALUENAME) VOID Set##VALUENAME(CTYPE value){ m_##VALUENAME = value;}
#define SAFE_RELEASE(T) {if( T!= NULL){ T->Release(); T = NULL;}}
#define SAFE_DELETE(T) {if( T!= NULL){ delete[] T; T = NULL;}}
#define HR(p) {hr=(p);if(FAILED(hr))return E_FAIL;}

#define EXIT_GAME exit(0);

#define LOG(Text) OutputDebugString(Text);

typedef std::string STRING;