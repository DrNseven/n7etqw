#pragma once
#include <Windows.h>
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_opengl3.h"
#include "Imgui/imgui_impl_win32.h"
#include "Imgui/imgui_internal.h"
#include "MinHook/include/MinHook.h"

#pragma warning(disable:4244) //disable warning C4244

//misc
int countnum = -1;

//globals
bool showmenu = false;
bool initonce = false;

//bool GDF;

//item states
int wallhack = 1;
int chams = 1;
int esp = 1;
int aimbot = 1;
int aimfov = 8;
int aimkey = 0;
int aimsens = 1;
int aimheight = 80;							//aim height value
bool autoshoot = 0;							//autoshoot
int as_xhairdst = 2;						//autoshoot activates below this crosshair distance

DWORD Daimkey = VK_SHIFT;		//aimkey
bool IsPressed = false;
bool targetfound = false;
//int preaim = 0;				//preaim to not aim behind

//w2s matrix
float MViewMX[16];
float ProjMX[16];
float Window[3];

//viewport
int Viewport[4];
float ViewportWidth;
float ViewportHeight;
float ScreenCenterX;
float ScreenCenterY;

DWORD gametick = timeGetTime();
float asdelay = 60.0f;

unsigned int gTexture;//or GLuint
unsigned int g2Texture;
unsigned int rTexture;
unsigned int r2Texture;
//GLuint textures[2];

//GLenum mcap;

void DisableDepthTest()
{
    //glDisable(GL_DEPTH_TEST); 
    
    _asm
    {
        push GL_BLEND
        sub dword ptr ss : [esp] , GL_BLEND - GL_DEPTH_TEST
        push offset DDT_RetAddr
        push dword ptr[glDisable]
        ret
    }
    _asm DDT_RetAddr:
    
}

void EnableDepthTest()
{
    //glEnable(GL_DEPTH_TEST);
    
    _asm
    {
        push GL_BLEND
        sub dword ptr ss : [esp] , GL_BLEND - GL_DEPTH_TEST
        push offset EDT_RetAddr
        push dword ptr[glEnable]
        ret
    }
    _asm EDT_RetAddr:
    
}

#include <string>
#include <fstream>
using namespace std;

// getdir & log
char dlldir[320];
char* GetDirFile(char* name)
{
    static char pldir[320];
    strcpy_s(pldir, dlldir);
    strcat_s(pldir, name);
    return pldir;
}

//log
void Log(const char* fmt, ...)
{
    if (!fmt)	return;

    char		text[4096];
    va_list		ap;
    va_start(ap, fmt);
    vsprintf_s(text, fmt, ap);
    va_end(ap);

    ofstream logfile((PCHAR)"log.txt", ios::app);
    if (logfile.is_open() && text)	logfile << text << endl;
    logfile.close();
}

void SaveCfg()
{
    ofstream fout;
    fout.open(GetDirFile((PCHAR)"n7et.ini"), ios::trunc);
    fout << "Wallhack " << wallhack << endl;
    fout << "Chams " << chams << endl;
    fout << "Esp " << esp << endl;
    fout << "Aimbot " << aimbot << endl;
    fout << "Aimkey " << aimkey << endl;
    fout << "Aimfov " << aimfov << endl;
    fout << "Aimheight " << aimheight << endl;
    fout << "Autoshoot " << autoshoot << endl;
    fout << "AsXhairdst " << as_xhairdst << endl;
    fout.close();
}

void LoadCfg()
{
    ifstream fin;
    string Word = "";
    fin.open(GetDirFile((PCHAR)"n7et.ini"), ifstream::in);
    fin >> Word >> wallhack;
    fin >> Word >> chams;
    fin >> Word >> esp;
    fin >> Word >> aimbot;
    fin >> Word >> aimkey;
    fin >> Word >> aimfov;
    fin >> Word >> aimheight;
    fin >> Word >> autoshoot;
    fin >> Word >> as_xhairdst;
    fin.close();
}

//void Color3ub(int r, int g, int b)//ET
//{
    //glDisableClientState(GL_COLOR_ARRAY);
    //glColor3ub(r, g, b);
    //glEnable(GL_COLOR_MATERIAL);
//}

//char* lpCurrentShader = NULL;

//aim stuff
typedef struct
{
    float x, y, z;
} tPointf;
std::vector<tPointf> AimPoint;

//esp
typedef struct
{
    float x, y, z;
} ePointf;
std::vector<ePointf> EspPoint;

//w2s
void ObjectToWindow(float MViewMX[16], float ProjMX[16], int Viewport[4], float WindowOut[3])
{
    float x2 = ProjMX[0] * MViewMX[12] + ProjMX[4] * MViewMX[13] + ProjMX[8] * MViewMX[14] + ProjMX[12];
    float y2 = ProjMX[1] * MViewMX[12] + ProjMX[5] * MViewMX[13] + ProjMX[9] * MViewMX[14] + ProjMX[13] + aimheight;
    float z2 = ProjMX[2] * MViewMX[12] + ProjMX[6] * MViewMX[13] + ProjMX[10] * MViewMX[14] + ProjMX[14];
    float w2 = ProjMX[3] * MViewMX[12] + ProjMX[7] * MViewMX[13] + ProjMX[11] * MViewMX[14] + ProjMX[15];

    WindowOut[0] = (float)(Viewport[0] + Viewport[2]) * 0.5f * (x2 / w2 + 1.0f);
    WindowOut[1] = (float)(Viewport[1] + Viewport[3]) * 0.5f * (y2 / w2 + 1.0f);
    WindowOut[2] = (float)0.5f * (z2 / w2 + 1.0f);

    WindowOut[1] = (float)(Viewport[3] - WindowOut[1]);
}

/*
int glhProjectf(float objx, float objy, float objz, float* modelview, float* projection, int* viewport, float* windowCoordinate) {
    //Transformation vectors
    float fTempo[8];
    //Modelview transform
    fTempo[0] = modelview[0] * objx + modelview[4] * objy + modelview[8] * objz + modelview[12]; //w is always 1
    fTempo[1] = modelview[1] * objx + modelview[5] * objy + modelview[9] * objz + modelview[13];
    fTempo[2] = modelview[2] * objx + modelview[6] * objy + modelview[10] * objz + modelview[14];
    fTempo[3] = modelview[3] * objx + modelview[7] * objy + modelview[11] * objz + modelview[15];
    //Projection transform, the final row of projection matrix is always [0 0 -1 0]
    //so we optimize for that.
    fTempo[4] = projection[0] * fTempo[0] + projection[4] * fTempo[1] + projection[8] * fTempo[2] + projection[12] * fTempo[3];
    fTempo[5] = projection[1] * fTempo[0] + projection[5] * fTempo[1] + projection[9] * fTempo[2] + projection[13] * fTempo[3];
    fTempo[6] = projection[2] * fTempo[0] + projection[6] * fTempo[1] + projection[10] * fTempo[2] + projection[14] * fTempo[3];
    fTempo[7] = -fTempo[2];
    //The result normalizes between -1 and 1
    if (fTempo[7] == 0.0) //The w value
        return 0;
    fTempo[7] = 1.0 / fTempo[7];
    //Perspective division
    fTempo[4] *= fTempo[7];
    fTempo[5] *= fTempo[7];
    fTempo[6] *= fTempo[7];
    //Window coordinates
    //Map x, y to range 0-1
    windowCoordinate[0] = (fTempo[4] * 0.5 + 0.5) * viewport[2] + viewport[0];
    windowCoordinate[1] = (fTempo[5] * 0.5 + 0.5) * viewport[3] + viewport[1];
    //This is only correct when glDepthRange(0.0, 1.0)
    windowCoordinate[2] = (1.0 + fTempo[6]) * 0.5; //Between 0 and 1
    return 1;
}
*/

const char* VariableText(const char* format, ...) {
    va_list argptr;
    va_start(argptr, format);

    char buffer[2048];
    vsprintf_s(buffer, format, argptr); // It should use vsnprintf but meh doesn't matter here

    va_end(argptr);

    return buffer;
}

# define M_PI 3.14159265358979323846
void drawSphere(double r, int lats, int longs) {
    int i, j;
    for (i = 0; i <= lats; i++) {
        double lat0 = M_PI * (-0.5 + (double)(i - 1) / lats);
        double z0 = sin(lat0);
        double zr0 = cos(lat0);

        double lat1 = M_PI * (-0.5 + (double)i / lats);
        double z1 = sin(lat1);
        double zr1 = cos(lat1);

        glBegin(GL_QUAD_STRIP);
        for (j = 0; j <= longs; j++) {
            double lng = 2 * M_PI * (double)(j - 1) / longs;
            double x = cos(lng);
            double y = sin(lng);

            glNormal3f(x * zr0, y * zr0, z0);
            glVertex3f(r * x * zr0, r * y * zr0, r * z0);
            glNormal3f(x * zr1, y * zr1, z1);
            glVertex3f(r * x * zr1, r * y * zr1, r * z1);
        }
        glEnd();
    }
}

static bool CALLBACK EnumHwndCallback(HWND hWnd, LPARAM lParam)
{
    const auto isMainWindow = [hWnd]() {
        return GetWindow(hWnd, GW_OWNER) == nullptr && IsWindowVisible(hWnd);
        };

    DWORD dwProcessId = 0;
    GetWindowThreadProcessId(hWnd, &dwProcessId);

    if (GetCurrentProcessId() != dwProcessId || !isMainWindow() || hWnd == GetConsoleWindow())
        return true;

    *(HWND*)lParam = hWnd;

    return false;
}

HWND        hWnd;
PCHAR       FontPath;
WNDPROC Original_WndProc;
bool bImGuiInit = false;

void InitWindow()
{
    SetWindowLongPtr(hWnd, GWL_WNDPROC, (LONG_PTR)Original_WndProc); //Reset WndProc

    do
    {
        ::EnumWindows((WNDENUMPROC)EnumHwndCallback, (LPARAM)&hWnd);
        Sleep(200);
    } while (hWnd == NULL);
}