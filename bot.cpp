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
static const COLORREF red_ball = RGB(255, 247, 247);
static const COLORREF red_ball2= RGB(123, 24, 0);
static const COLORREF blue_ball=RGB(247,255,255);
static const COLORREF green_ball=RGB(156,255,49);
static const COLORREF green_ball2=RGB(123,231,0);
static const COLORREF green_ball3=RGB(140,255,8);
static const COLORREF blue_drop=RGB(0,24,74);
static const COLORREF normal_bullet=RGB(255,255,0);
static const COLORREF green_ray=RGB(247,247,247);
static const COLORREF white_bullet=RGB(255,255,247);
static inline bool is_shot(const COLORREF pixel){
    return pixel == normal_bullet   ||
		   pixel == red_ball		||
		   pixel == red_ball2		||
           pixel == blue_ball		||
           pixel == blue_drop		||
           pixel == green_ray		||
           pixel == green_ball   	||
           pixel == green_ball2	    ||
           pixel == white_bullet    ||
           pixel == green_ball3;
}

// We scan for bullets inside a rectangle
// limited by the points (STARTX, STARTY) and (ENDX, ENDY)
#define STARTY 330
#define ENDY 450
#define STARTX 150
#define ENDX 555

// Height and width of the ship
#define HEIGHT 60
#define WIDTH 36

// Extra spaces at the sides to avoid getting close to bullets
#define EXTRA_TOP 40
#define EXTRA_SIDES 25

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
    static int no_shot_hor[ENDX][ENDY];
    static int has_power_up_hor[ENDX][ENDY];
    static int has_power_up[ENDX][ENDY];
	BYTE* p = get_window_pixels(game_window);
	// this logic is too complicated
	// basically we scan by rows and by columns detecting power ups and bullets in linear time
    for(int y=STARTY; y<ENDY; y++){
        int cont=0;
        for(int x=STARTX; x<ENDX; x++){
            p+=4;
            COLORREF pix=RGB(p[2], p[1], p[0]);
            if(pix==power_up) has_power_up_hor[x][y]=x;
            else has_power_up_hor[x][y]=has_power_up_hor[x-1][y]>max(0, x-WIDTH)? has_power_up_hor[x-1][y] : 0;
            no_shot_hor[x][y] = is_shot(pix)? cont=0 : ++cont;
        }
    }
    int possible_places=0;
    for(int x=STARTX; x<ENDX; x++){
        int cont=0;
        for(int y=STARTY; y<ENDY; y++){
            if(no_shot_hor[x][y] >= WIDTH + EXTRA_SIDES) cont++;
            else cont=0;
            if(has_power_up_hor[x][y]) has_power_up[x][y]=y;
            else has_power_up[x][y]=has_power_up[x][y-1]>max(0, x-HEIGHT)? has_power_up[x][y-1] :0;
			if (cont >= EXTRA_TOP + HEIGHT) {
				possible_places++;
				if (has_power_up[x - (WIDTH + EXTRA_SIDES) / 2][y - HEIGHT / 2]) {
					return pto(x - (WIDTH + EXTRA_SIDES) / 2, y - HEIGHT / 2);
				}
			}
        }
    }
    if(!possible_places) return pto(-1, -1);
    possible_places=rand()%possible_places;
    for(int x=STARTX; x<ENDX; x++){
        int cont=0;
        for(int y=STARTY; y<ENDY; y++){
            if(no_shot_hor[x][y] >= WIDTH + EXTRA_SIDES) cont++;
            else cont=0;
            if(cont >= EXTRA_TOP+HEIGHT && !possible_places--){
                return pto(x - (WIDTH + EXTRA_SIDES)/2, y - HEIGHT / 2);
            }
        }
    }
    return pto(-1, -1);
}

bool is_paused_key_pressed() {
	return GetKeyState(pause_key) >> 7 & 1;
}

void print_help() {

	cout << "LaserAge bot." << endl;
	cout << "To use, start the game. Make sure the game window is active. Then, press F6 to start/pause the bot." << endl;
}

#define CLICK_INTERVAL 2
int main(){
	print_help();
    srand((unsigned int)time(NULL));
    while(1){
        while(!is_paused_key_pressed()) Sleep(100);
        while(is_paused_key_pressed()) Sleep(10);
		HWND current_window = GetForegroundWindow();
		RECT window_pos;
		if (!GetWindowRect(current_window, &window_pos))
			return 1;
		for (unsigned iter = 0;; iter++) {
			if (iter % CLICK_INTERVAL == 0) {
				if (is_paused_key_pressed()) {
					mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
					Sleep(10);
					mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
					while (is_paused_key_pressed()) Sleep(10);
					break;
				}
				mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
			}
			else if (iter % CLICK_INTERVAL == CLICK_INTERVAL / 2) {
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

