#pragma once
//Anton Kesy


#include <cmath>

#include "framework.h"

#include <vector>


#define MAX_LOADSTRING 100

#define AMOUNT_OF_THREADS 4U


#define MAX_ITERATIONS 255


struct ComplexNumber
{
   double dReal;
   double dImg;
};

typedef struct ComplexNumber sComplexNumber_t;
typedef sComplexNumber_t* psComplexNumber_t;

struct ComplexArea
{
   sComplexNumber_t sCNTopLeft;
   sComplexNumber_t sCNBottomRight;
};

typedef struct ComplexArea sComplexArea_t;
typedef sComplexArea_t* psComplexArea_t;


typedef struct
{
   unsigned int uiXStart;
   unsigned int uiXStop;
   RECT rect;
   sComplexArea_t sComplexArea;
   HDC hdc;
   COLORREF* paColRef;
} sDrawMandelbrotThreadData_t;

void DrawMandelbrot(HDC hdc, RECT rect, sComplexArea_t sComplexArea);
sComplexNumber_t DoTransformationPixel2ComplexNumber(unsigned int uiX, unsigned int uiY, sComplexArea_t sComplexArea,
                                                     RECT rect);
unsigned int GetMandelbrotIterations(sComplexNumber_t sComplexNumber);
double AbsComplexNumber(sComplexNumber_t sComplexNumber);

DWORD WINAPI ThreadDrawMandelbrot(void* pParam);

// Global Variables:
HINSTANCE hInst; // current instance
WCHAR szTitle[MAX_LOADSTRING]; // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING]; // the main window class name

// Forward declarations of functions included in this code module:
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);


void DrawMandelbrotBitMap(HDC hdc, RECT rect);

enum PaintStates
{
   DrawNewMandelbrot = 0,
   DrawUserRect,
   ClearUserRect
};
