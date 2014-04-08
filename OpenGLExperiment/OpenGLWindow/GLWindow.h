#pragma once
#include <Windows.h>

class GLWindow
{
public:
	GLWindow(LPCSTR title, UINT width, UINT height, UINT bits, BOOL fullscreenflag);
	~GLWindow();

	void			Resize(UINT32 width, UINT32 height) { ReSizeGLScene(width, height); }
	const TCHAR*	Title() const { return szTitle; }
	UINT			Width() const { return wndWidth; }
	UINT			Height() const { return wndHeight; }
	HDC				GDIDeviceContext() { return hDC; }
	HGLRC			RenderContext() { return hRC; }
	HWND			WindowHanle() { return hWnd; }
	HINSTANCE		ApplicationInstance() { return hInst; }

protected:
	BOOL	CreateGLWindow(LPCSTR title, UINT width, UINT height, UINT bits, BOOL fullscreenflag, WNDPROC wndProc);
	GLvoid	KillGLWindow(GLvoid);
	int		DrawGLScene(GLvoid);
	GLvoid	ReSizeGLScene(GLsizei width, GLsizei height);
	int		InitGL(GLvoid);
protected:
	static const UINT MAX_LOADSTRING = 100U;
	HDC			hDC = NULL;		// Private GDI Device Context
	HGLRC		hRC = NULL;		// Permanent Rendering Context
	HWND		hWnd = NULL;		// Holds Our Window Handle
	HINSTANCE	hInst = NULL;		// Holds The Instance Of The Application

	uint32_t	wndWidth;
	uint32_t	wndHeight;

	TCHAR		szTitle[MAX_LOADSTRING];					// The title bar text
	TCHAR		szWindowClass[MAX_LOADSTRING];			// the main window class name

	BOOL		active = TRUE;		// Window Active Flag Set To TRUE By Default
	BOOL		fullscreen = TRUE;	// Fullscreen Flag Set To Fullscreen Mode By Default

	BOOL		keys[256];			// Array Used For The Keyboard Routine
	BOOL		buttons[3];			// Array for mouse buttons

};

