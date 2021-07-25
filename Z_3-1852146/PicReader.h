/******************************************************************
* ！注意！                                                         
* 本头文件中为你封装了WinAPI中WIC底层函数，方便你进行图片读取而不必引  
* 入或安装其他的外部库，但是我们有一定的约束条件，请你仔细阅读以下规定  
*     若函数存在问题，请及时联系老师或助教！                          
*     readPic()函数为你封装了WinAPI中的方法，可以将图片读取为RGBA的   
* bitmap数据，但并不代表你可以通过修改函数或者调用WinAPI实现你的压缩
******************************************************************/
#ifndef PIC_READER_H
#define PIC_READER_H

#include <stdio.h>
#include <windows.h>
#include <wincodec.h>
#include <commdlg.h>
#include <graphics.h>

template <typename T>
inline void SafeRelease(T *&p) {
	if (nullptr != p) {
		p->Release();
		p = nullptr;
	}
}

class PicReader {
public:
	PicReader();
	~PicReader();

	void readPic(LPCSTR);
	void getData(BYTE *&,UINT &, UINT &);
	void showPic(const BYTE*, const UINT, const UINT);
private:
	void init();
	bool checkHR(HRESULT);
	void quitWithError(LPCSTR);

	HWND                    hWnd;
	HANDLE                  hFile;
	IWICImagingFactory     *m_pIWICFactory;
	IWICFormatConverter    *m_pConvertedSourceBitmap;
};
#endif