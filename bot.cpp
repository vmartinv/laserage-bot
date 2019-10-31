#include <windows.h>
#include <iostream>
#include <ctime>
using namespace std;

struct pto{
    int x,y;
    pto(int x,int y):x(x),y(y){}
};

static const int pause_key = VK_F6;
static const COLORREF power_up=RGB(231,0,49);
static const COLORREF red_ball=RGB(255,247,247);
static const COLORREF blue_ball=RGB(247,255,255);
static const COLORREF green_ball=RGB(156,255,49);
static const COLORREF green_ball2=RGB(123,231,0);
static const COLORREF green_ball3=RGB(140,255,8);
static const COLORREF blue_drop=RGB(0,24,74);
static const COLORREF normal_bullet=RGB(255,255,0);
static const COLORREF green_ray=RGB(247,247,247);
static const COLORREF white_bullet=RGB(255,255,247);
static inline bool is_shoot(const COLORREF pixel){
    return pixel==normal_bullet   ||
           pixel==red_ball    ||
           pixel==blue_ball    ||
           pixel==blue_drop    ||
           pixel==green_ray   ||
           pixel==green_ball   ||
           pixel==green_ball2  ||
           pixel==white_bullet  ||
           pixel==green_ball3;
}

#define STARTY 320
#define ENDY 465
#define STARTX 140
#define ENDX 560
#define HEIGHT 60
#define WIDTH 36
#define TOPPER 25
#define SIDER 20

static inline BYTE *get_window_pixels(HWND current_window){
    static HDC hdc=NULL, hdcTemp;
    static BYTE* bitPointer;
	if (!hdc) {
		hdc = GetDC(current_window);
		hdcTemp = CreateCompatibleDC(hdc);
		BITMAPINFO bitmap;
		bitmap.bmiHeader.biSize = sizeof(bitmap.bmiHeader);
		bitmap.bmiHeader.biWidth = ENDX - STARTX;
		bitmap.bmiHeader.biHeight = STARTY - ENDY;
		bitmap.bmiHeader.biPlanes = 1;
		bitmap.bmiHeader.biBitCount = 32;
		bitmap.bmiHeader.biCompression = BI_RGB;
		bitmap.bmiHeader.biSizeImage = bitmap.bmiHeader.biWidth * 4 * bitmap.bmiHeader.biHeight;
		bitmap.bmiHeader.biClrUsed = 0;
		bitmap.bmiHeader.biClrImportant = 0;
		HBITMAP hBitmap2 = CreateDIBSection(hdcTemp, &bitmap, DIB_RGB_COLORS, (void**)(&bitPointer), NULL, 0);
		if (hBitmap2 != NULL) {
			SelectObject(hdcTemp, hBitmap2);
		}
	}
    BitBlt(hdcTemp, 0, 0, ENDX-STARTX, ENDY-STARTY, hdc, STARTX, STARTY, SRCCOPY);
    return bitPointer;
}

static inline pto find_free_space(HWND game_window){
    static int frow[ENDX][ENDY];
    static int islati[ENDX][ENDY];
    static int islati2[ENDX][ENDY];
    BYTE *p=get_window_pixels(game_window);
    for(int y=STARTY; y<ENDY; y++){
        int cont=0;
        for(int x=STARTX; x<ENDX; x++){
            p+=4;
            COLORREF pix=RGB(p[2], p[1], p[0]);
            if(pix==power_up) islati[x][y]=x;
            else islati[x][y]=islati[x-1][y]>max(0, x-WIDTH)? islati[x-1][y] : 0;
            frow[x][y] = is_shoot(pix)? cont=0 : ++cont;
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
    srand((unsigned int)time(NULL));
    while(1){
        while(!(GetKeyState(pause_key)>>7&1)) Sleep(10);
        while(GetKeyState(pause_key)>>7&1) Sleep(10);
		HWND current_window = GetForegroundWindow();
		RECT window_pos;
		if (!GetWindowRect(current_window, &window_pos))
			return 1;
		for (unsigned iter = 0;; iter++) {
			int width = window_pos.right - window_pos.left;
			int height = window_pos.bottom - window_pos.top;
			if (iter % INTERVAL == 0) {
				if (GetKeyState(pause_key) >> 7 & 1) {
					mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
					Sleep(10);
					mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
					while (GetKeyState(VK_F6) >> 7 & 1) Sleep(10);
					break;
				}
				mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
			}
			else if (iter % INTERVAL == INTERVAL / 2) {
				mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
			}
			pto p = find_free_space(current_window);
			if (p.x != -1) {
				SetCursorPos(window_pos.left + p.x, window_pos.top + p.y);
			}
			Sleep(10);
		}
    }
    return 0;
}

