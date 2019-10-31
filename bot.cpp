#include <windows.h>
#include <iostream>
#include <ctime>
using namespace std;

struct pto{
    int x,y;
    pto(int x,int y):x(x),y(y){}
};

static const COLORREF latita=RGB(231,0,49);
static const COLORREF bolaroja=RGB(255,247,247);
static const COLORREF bolaazul=RGB(247,255,255);
static const COLORREF bolaverde=RGB(156,255,49);
static const COLORREF bolaverde2=RGB(123,231,0);
static const COLORREF bolaverde3=RGB(140,255,8);
static const COLORREF gotaazul=RGB(0,24,74);
static const COLORREF tirocomun=RGB(255,255,0);
static const COLORREF rayoverde=RGB(247,247,247);
static const COLORREF tiroblanco=RGB(255,255,247);
static inline bool istiro(const COLORREF pixel){
    return pixel==tirocomun   ||
           pixel==bolaroja    ||
           pixel==bolaazul    ||
           pixel==gotaazul    ||
           pixel==rayoverde   ||
           pixel==bolaverde   ||
           pixel==bolaverde2  ||
           pixel==tiroblanco  ||
           pixel==bolaverde3;
}

#define STARTY 320
#define ENDY 465
#define STARTX 140
#define ENDX 560
#define HEIGHT 60
#define WIDTH 36
#define TOPPER 25
#define SIDER 20

static inline BYTE *update(){
    static HDC hdc=NULL, hdcTemp;
    static BYTE* bitPointer;
    if(!hdc){
        hdc=GetDC(HWND_DESKTOP);
        hdcTemp = CreateCompatibleDC(hdc);
        BITMAPINFO bitmap;
        bitmap.bmiHeader.biSize = sizeof(bitmap.bmiHeader);
        bitmap.bmiHeader.biWidth = ENDX-STARTX;
        bitmap.bmiHeader.biHeight = STARTY-ENDY;
        bitmap.bmiHeader.biPlanes = 1;
        bitmap.bmiHeader.biBitCount = 32;
        bitmap.bmiHeader.biCompression = BI_RGB;
        bitmap.bmiHeader.biSizeImage = bitmap.bmiHeader.biWidth * 4 * bitmap.bmiHeader.biHeight;
        bitmap.bmiHeader.biClrUsed = 0;
        bitmap.bmiHeader.biClrImportant = 0;
        HBITMAP hBitmap2 = CreateDIBSection(hdcTemp, &bitmap, DIB_RGB_COLORS, (void**)(&bitPointer), NULL, 0);
        SelectObject(hdcTemp, hBitmap2);
    }
    BitBlt(hdcTemp, 0, 0, ENDX-STARTX, ENDY-STARTY, hdc, STARTX, STARTY, SRCCOPY);
    return bitPointer;
}

static inline pto findspace(){
    static int frow[ENDX][ENDY];
    static int islati[ENDX][ENDY];
    static int islati2[ENDX][ENDY];
    BYTE *p=update();
    for(int y=STARTY; y<ENDY; y++){
        int cont=0;
        for(int x=STARTX; x<ENDX; x++){
            p+=4;
            COLORREF pix=RGB(p[2], p[1], p[0]);
            if(pix==latita) islati[x][y]=x;
            else islati[x][y]=islati[x-1][y]>max(0, x-WIDTH)? islati[x-1][y] : 0;
            frow[x][y] = istiro(pix)? cont=0 : ++cont;
        }
    }
    int q=0;
    for(int x=STARTX; x<ENDX; x++){
        int cont=0;
        for(int y=STARTY; y<ENDY; y++){
            if(frow[x][y]>=WIDTH+SIDER) cont++;
            else cont=0;
            if(islati[x][y]) islati2[x][y]=y;
            else islati2[x][y]=islati2[x][y-1]>max(0, x-HEIGHT)? islati2[x][y-1] :0;
            if(cont>=TOPPER+HEIGHT){
                q++;
                if(islati2[x-SIDER/2][y])
                    return pto(x-(WIDTH+SIDER)/2, y-HEIGHT/2);
            }
        }
    }
    if(!q) return pto(-1, -1);
    q=rand()%q;
    for(int x=STARTX; x<ENDX; x++){
        int cont=0;
        for(int y=STARTY; y<ENDY; y++){
            if(frow[x][y]>=WIDTH+SIDER) cont++;
            else cont=0;
            if(cont>=TOPPER+HEIGHT && !q--){
                return pto(x-(WIDTH+SIDER)/2, y-HEIGHT/2);
            }
        }
    }
    return pto(-1, -1);
}

#define INTERVAL 2
int main(){
    srand(time(NULL));
    while(1){
        while(!(GetKeyState(VK_F6)>>7&1)) Sleep(10);
        while(GetKeyState(VK_F6)>>7&1) Sleep(10);
        for(unsigned iter=0;; iter++){
            if(iter%INTERVAL==0){
                if(GetKeyState(VK_F6)>>7&1){
                    mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
                    Sleep(10);
                    mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
                    while(GetKeyState(VK_F6)>>7&1) Sleep(10);
                    break;
                }
                mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
            }
            else if(iter%INTERVAL==INTERVAL/2)
                mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
            pto p=findspace();
            if(p.x!=-1) SetCursorPos(p.x, p.y), Sleep(10);
        }
    }
    return 0;
}

