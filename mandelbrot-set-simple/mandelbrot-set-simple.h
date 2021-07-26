#pragma once

struct ComplexNumber
{
   double dReal;
   double dImg;
};

typedef struct ComplexNumber sComplexNumber_t;

struct ComplexArea
{
   sComplexNumber_t sCNTopLeft;
   sComplexNumber_t sCNBottomRight;
};

typedef struct ComplexArea sComplexArea_t;


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

void DrawMandelbrotBitMap(HDC hdc, RECT rect);

enum PaintStates
{
   DrawNewMandelbrot = 0,
   DrawUserRect,
   ClearUserRect
};
