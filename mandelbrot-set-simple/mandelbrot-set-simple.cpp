// mandelbrot-set-simple.cpp : Defines the entry point for the application
//Anton Kesy

#include "mandelbrot-set-simple.h"

#include "ColorUtility.h"

#include "Resource.h"

//global to keep given function declarations
COLORREF* aColref = nullptr;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                      _In_opt_ HINSTANCE hPrevInstance,
                      _In_ LPWSTR lpCmdLine,
                      _In_ int nCmdShow)
{
   UNREFERENCED_PARAMETER(hPrevInstance);
   UNREFERENCED_PARAMETER(lpCmdLine);


   // Initialize global strings
   LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
   LoadStringW(hInstance, IDC_MANDELBROTSETSIMPLE, szWindowClass, MAX_LOADSTRING);
   MyRegisterClass(hInstance);

   // Perform application initialization:
   if (!InitInstance(hInstance, nCmdShow))
   {
      return FALSE;
   }

   HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MANDELBROTSETSIMPLE));

   MSG msg;

   // Main message loop:
   while (GetMessage(&msg, nullptr, 0, 0))
   {
      if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
      {
         TranslateMessage(&msg);
         DispatchMessage(&msg);
      }
   }

   free(aColref);

   return static_cast<int>(msg.wParam);
}


//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
   WNDCLASSEXW wcex;

   wcex.cbSize = sizeof(WNDCLASSEX);

   wcex.style = CS_HREDRAW | CS_VREDRAW;
   wcex.lpfnWndProc = WndProc;
   wcex.cbClsExtra = 0;
   wcex.cbWndExtra = 0;
   wcex.hInstance = hInstance;
   wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDC_MANDELBROTSETSIMPLE));
   wcex.hCursor = LoadCursor(nullptr, IDC_CROSS);
   wcex.hbrBackground = reinterpret_cast<HBRUSH>((COLOR_WINDOW + 1));
   wcex.lpszMenuName = nullptr;
   wcex.lpszClassName = szWindowClass;
   wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

   return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
                             CW_USEDEFAULT, CW_USEDEFAULT, 900, 600, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   RECT rect;
   static sComplexArea_t sComplexArea = {{-2.0, 1.0}, {1.0, -1.0}};
   static int iX;
   static int iY;
   int lastIX = iX;
   int lastIY = iY;
   static POINT rectEndPoint;
   static bool bSelectNewArea;
   static PaintStates eDrawState;
   HDC hdc;
   sComplexNumber_t sCN_NewTL;
   int iNewRight;
   int iNewBottom;
   switch (message)
   {
   case WM_COMMAND:
      break;
   case WM_PAINT:
      {
         PAINTSTRUCT ps;
         hdc = BeginPaint(hWnd, &ps);
         GetClientRect(hWnd, &rect);

         if (aColref == nullptr)
         {
            //alloc colorref array for bitmap once
            aColref = static_cast<COLORREF*>(malloc(rect.bottom * rect.right * sizeof(COLORREF)));
         }

         if (aColref != nullptr)
         {
            switch (eDrawState)
            {
            case DrawNewMandelbrot:
               //draw new mandelbrot
               ShowCursor(false);
               DrawMandelbrot(hdc, rect, sComplexArea);
               ShowCursor(true);
               break;
            case DrawUserRect:
               DrawMandelbrotBitMap(hdc, rect);
               //set brush transparent <- not correctly working because it'saturation in move
               SelectObject(hdc, GetStockObject(DC_PEN));
               //SelectObject(hdc, GetStockObject(DC_BRUSH));
               SetDCPenColor(hdc, RGB(0xFF, 0, 0));
               //draw rect 
               Rectangle(hdc, iX, iY, rectEndPoint.x, rectEndPoint.y);
               break;
            case ClearUserRect:
               DrawMandelbrotBitMap(hdc, rect);
               break;
            }
            //resets draw state
            eDrawState = DrawNewMandelbrot;
         }
         EndPaint(hWnd, &ps);
      }
      break;

   case WM_LBUTTONDOWN:
      //start new area selection
      bSelectNewArea = true;
      iX = LOWORD(lParam);
      iY = HIWORD(lParam);
      break;

   case WM_LBUTTONUP:
      bSelectNewArea = false;
      iX = LOWORD(lParam);
      iY = HIWORD(lParam);
      GetClientRect(hWnd, &rect);

      //swaps coordinates to keep orientation
      if (iX < lastIX)
      {
         const int iTmp = lastIX;
         lastIX = iX;
         iX = iTmp;
      }

      if (iY < lastIY)
      {
         const int iTmp = lastIY;
         lastIY = iY;
         iY = iTmp;
      }

      //
      sCN_NewTL.dReal = lastIX;
      sCN_NewTL.dImg = lastIY;
      //Transformation of iX,iY - store in sCN_NewTL
      sCN_NewTL = DoTransformationPixel2ComplexNumber(lastIX, lastIY, sComplexArea, rect);
      //

      //  sets steps equal to window aspect ratio to reduce distortion
      //calc new rect
      iNewRight = iX - lastIX;
      iNewBottom = iY - lastIY;
      //keep bigger rect side as picked
      if (iNewBottom > iNewRight)
      {
         //height
         iNewRight = static_cast<int>(static_cast<double>(iNewBottom) / (static_cast<double>(rect.bottom) / rect.
            right));
         iX = lastIX + iNewRight;
      }
      else
      {
         //width
         iNewBottom = static_cast<int>(static_cast<double>(iNewRight) * (static_cast<double>(rect.bottom) / rect.
            right));
         iY = lastIY + iNewBottom;
      }
      sComplexArea = {sCN_NewTL, DoTransformationPixel2ComplexNumber(iX, iY, sComplexArea, rect)};
      InvalidateRect(hWnd, nullptr, FALSE);
      break;
   case WM_RBUTTONUP:
      //Reset complex area to original values
      sComplexArea = {{-2.0, 1.0}, {1.0, -1.0}};
      InvalidateRect(hWnd, nullptr, FALSE);
      break;
   case WM_DESTROY:
      PostQuitMessage(0);
      break;
   case WM_MOUSEMOVE:
      //draw rect while selecting new area
      if (bSelectNewArea)
      {
         rectEndPoint.x = LOWORD(lParam);
         rectEndPoint.y = HIWORD(lParam);
         eDrawState = DrawUserRect;
         RedrawWindow(hWnd, nullptr, nullptr, RDW_INVALIDATE);
      }
      break;
   case WM_SIZE:
      //reset colorRef array
      aColref = nullptr;
      InvalidateRect(hWnd, nullptr, FALSE);
      break;
   default:
      return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
   UNREFERENCED_PARAMETER(lParam);
   switch (message)
   {
   case WM_INITDIALOG:
      return TRUE;

   case WM_COMMAND:
      if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
      {
         EndDialog(hDlg, LOWORD(wParam));
         return TRUE;
      }
      break;
   }
   return FALSE;
}

//Loads COLORREF array with values and  calls draw bitmap
void DrawMandelbrot(HDC hdc, RECT rect, sComplexArea_t sComplexArea)
{
   //create AMOUNT_OF_THREADS threads
   auto pThreads = static_cast<HANDLE*>(malloc(AMOUNT_OF_THREADS * sizeof(HANDLE)));
   auto pSingleThreadData = static_cast<sDrawMandelbrotThreadData_t*>(malloc(
      AMOUNT_OF_THREADS * sizeof(sDrawMandelbrotThreadData_t)));

   //sanity check threads
   if (pThreads != nullptr && pSingleThreadData != nullptr)
   {
      bool bThreadsAvailable = true;
      for (unsigned int i = 0U; i < AMOUNT_OF_THREADS && bThreadsAvailable; ++i)
      {
         const unsigned int cuiXStart = rect.right / AMOUNT_OF_THREADS * i;
         const unsigned int cuiXStop = rect.right / AMOUNT_OF_THREADS * (i + 1U);

         //Data for every single thread
         pSingleThreadData[i] = {
            cuiXStart, cuiXStop, rect, sComplexArea, hdc, aColref
         };

         pThreads[i] = CreateThread(nullptr, 0, ThreadDrawMandelbrot, &pSingleThreadData[i], 0, nullptr);
         //stops if thread is not available
         bThreadsAvailable = pThreads[i] != nullptr;
      }

      //if thread creation was successful
      if (bThreadsAvailable)
      {
         WaitForMultipleObjects(AMOUNT_OF_THREADS, pThreads, TRUE, INFINITE);

         for (unsigned int i = 0U; i < AMOUNT_OF_THREADS; ++i)
         {
            CloseHandle(pThreads[i]);
         }
      }

      free(pThreads);
      free(pSingleThreadData);

      //call draw bitmap
      DrawMandelbrotBitMap(hdc, rect);
   }
}

//Draws COLORREF array into bitmap and onto screen
void DrawMandelbrotBitMap(HDC hdc, RECT rect)
{
   // Creating temp bitmap
   HBITMAP map = CreateBitmap(rect.right, rect.bottom, 1, sizeof(COLORREF) * 8, (void*)aColref);

   //sanity check
   if (map != nullptr)
   {
      HDC src = CreateCompatibleDC(hdc);
      // hdc - Device context for window, I've got earlier with GetDC(hWnd) or GetDC(NULL);
      SelectObject(src, map); // Inserting picture into our temp HDC
      // Copy image from temp HDC to window
      BitBlt(hdc, // Destination
             0, // x and
             0, // y - upper-left corner of place, where we'd like to copy
             rect.right, // width of the region
             rect.bottom, // height
             src, // source
             0, // x and
             0, // y of upper left corner  of part of the source, from where we'd like to copy
             SRCCOPY); // Defined DWORD to juct copy pixels. Watch more on msdn;

      DeleteObject(map);
      DeleteDC(src); // Deleting temp HDC
   }
}

//transforms pixel coordinates to complex number
sComplexNumber_t DoTransformationPixel2ComplexNumber(unsigned int uiX, unsigned int uiY,
                                                     sComplexArea_t sComplexArea, RECT rect)
{
   sComplexNumber_t sComplexNumberRet;
   //calculate cReal
   sComplexNumberRet.dReal = static_cast<double>(uiX) / rect.right *
      (sComplexArea.sCNBottomRight.dReal - sComplexArea.sCNTopLeft.dReal)
      + sComplexArea.sCNTopLeft.dReal;

   //calculate cImg
   sComplexNumberRet.dImg = (static_cast<double>(rect.bottom) - uiY) / rect.bottom *
      (sComplexArea.sCNTopLeft.dImg - sComplexArea.sCNBottomRight.dImg)
      + sComplexArea.sCNBottomRight.dImg;

   return sComplexNumberRet;
}

//gets iterations of mandelbrot
unsigned int GetMandelbrotIterations(const sComplexNumber_t sComplexNumberC)
{
   unsigned int uiIterations = 0U;
   sComplexNumber_t sComplexNumberZ;
   sComplexNumber_t sComplexNumberDummy = {0.0, 0.0};

   //mandelbrot iterations
   while (uiIterations <= MAX_ITERATIONS && AbsComplexNumber(sComplexNumberDummy) <= 2)
   {
      //complex multi
      sComplexNumberZ.dReal = sComplexNumberDummy.dReal * sComplexNumberDummy.dReal - sComplexNumberDummy.dImg *
         sComplexNumberDummy.dImg + sComplexNumberC.dReal;

      sComplexNumberZ.dImg = 2 * sComplexNumberDummy.dReal * sComplexNumberDummy.dImg + sComplexNumberC.dImg;
      //Update dummy
      sComplexNumberDummy.dReal = sComplexNumberZ.dReal;
      sComplexNumberDummy.dImg = sComplexNumberZ.dImg;

      uiIterations++;
   }


   return uiIterations;
}

//calculates abs of complex number
double AbsComplexNumber(sComplexNumber_t sComplexNumber)
{
   return sqrt(sComplexNumber.dImg * sComplexNumber.dImg + sComplexNumber.dReal * sComplexNumber.dReal);
}


DWORD WINAPI ThreadDrawMandelbrot(void* pParam)
{
   const sDrawMandelbrotThreadData_t csDrawData = *static_cast<sDrawMandelbrotThreadData_t*>(pParam);

   //go through x-pixels from start to stop of assigned thread 
   for (unsigned int uiX = csDrawData.uiXStart; uiX < csDrawData.uiXStop; ++uiX)
   {
      //go through whole row for every x
      for (int iY = 0; iY < csDrawData.rect.bottom; ++iY)
      {
         const sComplexNumber_t csComplexNumber = DoTransformationPixel2ComplexNumber(
            uiX, iY, csDrawData.sComplexArea, csDrawData.rect);

         const unsigned int cuiIterations = GetMandelbrotIterations(csComplexNumber);
         //set color in array
         hsvColor color2;
         color2.hue = 360.0 * (static_cast<double>(cuiIterations) / MAX_ITERATIONS);
         color2.saturation = 1;
         color2.value = color2.hue > 360 ? 0 : 1;
         const rgbColor color = hsv2rgb(color2);
         csDrawData.paColRef[csDrawData.rect.right * iY + uiX] = RGB(static_cast<int>(color.r * 255),
                                                                     static_cast<int>(color.g * 255),
                                                                     static_cast<int>(color.b * 255));
      }
   }

   return 0;
}
