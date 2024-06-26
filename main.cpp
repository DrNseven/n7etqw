//n7etqw
//release x86, ctrl + b to compile
//net_allowcheats 1
//noclip
//pm_thirdperson 1
//pm_thirdpersonrange 1400 for logging models

#include <Windows.h>
#include <gl/GL.h>
//#include "GLU32/GLU.h"
#include <vector>

#pragma comment(lib,"opengl32.lib")
//#pragma comment(lib, "GLU32/GLU32.lib") 
#pragma comment(lib,"winmm.lib")

#include "main.h"


typedef HRESULT (WINAPI* wglSwapBuffers_t)(HDC hdc);
HRESULT WINAPI Hook_wglSwapBuffers(HDC hdc);
wglSwapBuffers_t Original_wglSwapBuffers;

typedef HRESULT(WINAPI* glDrawElements_t)(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices);
HRESULT WINAPI Hook_glDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices);
glDrawElements_t Original_glDrawElements;

typedef HRESULT(WINAPI* glBindTexture_t)(GLenum target,GLuint texture);
HRESULT WINAPI Hook_glBindTexture(GLenum target, GLuint texture);
glBindTexture_t Original_glBindTexture;

typedef HRESULT(WINAPI* glVertexPointer_t)(GLint size, GLenum type, GLsizei stride, const void* pointer);
HRESULT WINAPI Hook_glVertexPointer(GLint size, GLenum type, GLsizei stride, const void* pointer);
glVertexPointer_t Original_glVertexPointer;

typedef HRESULT(WINAPI* glEnable_t)(GLenum cap);
HRESULT WINAPI Hook_glEnable(GLenum cap);
glEnable_t Original_glEnable;

typedef HRESULT(WINAPI* glDisable_t)(GLenum cap);
HRESULT WINAPI Hook_glDisable(GLenum cap);
glDisable_t Original_glDisable;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI Hook_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

bool mResetImgui = false;

LRESULT WINAPI Hook_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_ACTIVATE:;
        if (wParam == WA_INACTIVE)
        {
            //Log("inactive");
            mResetImgui = true;
        }
        break;
    }

    if (showmenu && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
        return true;

    return CallWindowProc(Original_WndProc, hWnd, uMsg, wParam, lParam);
}

void InitImGui()
{
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsClassic();

    ImGui_ImplWin32_Init(hWnd);
    ImGui_ImplOpenGL3_Init();
    Original_WndProc = (WNDPROC)SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)Hook_WndProc);
}

HRESULT WINAPI Hook_wglSwapBuffers(HDC hdc)
{
    if(mResetImgui)
    {
        //Log("mResetImgui");
        InitWindow();
        bImGuiInit = false;
        mResetImgui = false;
    }

    if (!bImGuiInit)
    {
        InitImGui();
        LoadCfg();
        bImGuiInit = true;
    }

    //get viewport
    glGetIntegerv(GL_VIEWPORT, Viewport);
    //Viewport[2] == width
    //Viewport[3] == height

    //get imgui displaysize
    ImGuiIO io = ImGui::GetIO();
    ViewportWidth = io.DisplaySize.x;
    ViewportHeight = io.DisplaySize.y;
    ScreenCenterX = ViewportWidth / 2.0f;
    ScreenCenterY = ViewportHeight / 2.0f;
    

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    if (GetAsyncKeyState(VK_INSERT) & 1) {
        SaveCfg(); //save settings
        showmenu = !showmenu;
    }


    if (showmenu) {

        ImGui::Begin("ETQW Multihack", &showmenu);

        ImGui::SetWindowSize({ 300, 300 }, ImGuiCond_Once);
        ImVec4 Bgcol = ImColor(0.0f, 0.4f, 0.28f, 0.8f); //bg color
        ImGui::PushStyleColor(ImGuiCol_WindowBg, Bgcol);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.2f, 0.2f, 1.8f)); //frame color
        //ImGui::PushStyleColor(?, ImVec4(0.2f, 1.0f, 0.2f, 1.0f));//which is the currently selected one

        ImGui::SliderInt("Wallhack", &wallhack, 0, 1);
        ImGui::SliderInt("Chams", &chams, 0, 1);
        ImGui::SliderInt("Esp", &esp, 0, 2);
        ImGui::SliderInt("Aimbot", &aimbot, 0, 2);
        ImGui::SliderInt("Aimsens", &aimsens, 0, 10);
        ImGui::Text("Aimkey");
        const char* aimkey_Options[] = { "Shift", "Right Mouse", "Left Mouse", "Middle Mouse", "Ctrl", "Alt", "Capslock", "Space", "X", "C", "V" };
        ImGui::SameLine();
        ImGui::Combo("##AimKey", (int*)&aimkey, aimkey_Options, IM_ARRAYSIZE(aimkey_Options));
        ImGui::SliderInt("Aimfov", &aimfov, 0, 10);
        ImGui::SliderInt("Aimheight", &aimheight, 0, 100);
        ImGui::Checkbox("Autoshoot", &autoshoot);
        ImGui::SliderInt("As xhair dst", &as_xhairdst, 0, 10);
        //ImGui::SliderInt("countnum", &countnum, 0, 100);

        ImGui::Text("Use arrow keys & space");
        ImGui::Text("to navigate");

        ImGui::End();
    }

    
    if (esp == 1 ||esp == 2)
    {
        ImGui::Begin("Transparent", reinterpret_cast<bool*>(true), ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBackground);
        ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
        ImGui::SetWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y), ImGuiCond_Always);
        //ImGui::SetWindowSize(ImVec2(Viewport[2], Viewport[3]), ImGuiCond_Always);

        if (EspPoint.size() != NULL)
        {
            for (unsigned int i = 0; i < EspPoint.size(); i++)
            {
                //if (EspPoint[i].x > 1 && EspPoint[i].y > 1 && EspPoint[i].z > 1 && EspPoint[i].x < Viewport[2] && EspPoint[i].y < Viewport[3])
                if (EspPoint[i].x > 1 && EspPoint[i].y > 1 && /*EspPoint[i].z > 1 && */EspPoint[i].x < ViewportWidth && EspPoint[i].y < ViewportHeight)
                {
                    //text esp
                    //if(-EspPoint[i].z > 0)
                    //ImGui::GetWindowDrawList()->AddText(ImGui::GetFont(), ImGui::GetFontSize(), ImVec2(EspPoint[i].x, EspPoint[i].y), ImColor(255, 255, 255, 255), "Model", 0, 0.0f, 0); //draw text

                    //distance esp
                    if(-EspPoint[i].z > 0)
                    ImGui::GetWindowDrawList()->AddText(ImGui::GetFont(), ImGui::GetFontSize(), ImVec2(EspPoint[i].x, EspPoint[i].y), ImColor(255, 255, 0, 255), VariableText("%.f", -EspPoint[i].z), 0, 0.0f, 0); //draw variable

                    //line esp
                    if (-EspPoint[i].z > 0)
                    ImGui::GetWindowDrawList()->AddLine({ EspPoint[i].x, EspPoint[i].y }, ImVec2(io.DisplaySize.x / 2, io.DisplaySize.y / 2 * (0)), IM_COL32(255, 255, 255, 255), 1.0f);//0up, 10middle, 20down

                    //draw circle
                    if (esp == 1 && -EspPoint[i].z > 0)
                    ImGui::GetWindowDrawList()->AddCircle(ImVec2(EspPoint[i].x, EspPoint[i].y), 10000 / -EspPoint[i].z, IM_COL32(255, 0, 0, 255), 12, 2.0f); //scale with distance
                    else
                    ImGui::GetWindowDrawList()->AddCircle(ImVec2(EspPoint[i].x, EspPoint[i].y), 10000 / -EspPoint[i].z, IM_COL32(0, 255, 0, 255), 12, 2.0f); //scale with distance
                }
            }
        }
        ImGui::End();
    }
    EspPoint.clear();

    //"Shift", "Right Mouse", "Left Mouse", "Middle Mouse", "Ctrl", "Alt", "Capslock", "Space", "X", "C", "V"
    if (aimkey == 0) Daimkey = VK_SHIFT;
    if (aimkey == 1) Daimkey = VK_RBUTTON;
    if (aimkey == 2) Daimkey = VK_LBUTTON;
    if (aimkey == 3) Daimkey = VK_MBUTTON;
    if (aimkey == 4) Daimkey = VK_CONTROL;
    if (aimkey == 5) Daimkey = VK_MENU;
    if (aimkey == 6) Daimkey = VK_CAPITAL;
    if (aimkey == 7) Daimkey = VK_SPACE;
    if (aimkey == 8) Daimkey = 0x58; //X
    if (aimkey == 9) Daimkey = 0x43; //C
    if (aimkey == 10) Daimkey = 0x56; //V

    //aimbot part 2
    if (aimbot == 1 || aimbot == 2) //&& GetAsyncKeyState(aimkey)) //& 0x8000)
    {
        unsigned int pointNum = -1;
        unsigned int i = 0;
        float Closest = -0xFFFFFF;
        float Closest2 = 0xFFFFFF;
        float CenterX = (float)Viewport[2] / 2;
        float CenterY = (float)Viewport[3] / 2;

        if (AimPoint.size() != NULL)
        {
            for (i = 0; i < AimPoint.size(); i++)
            {
                //test
                //drawMessage(AimPoint[i].x, AimPoint[i].y, 1.0, "Model");
                //drawMessage(AimPoint[pointNum].x, AimPoint[pointNum].y, 0.8, "Iterations this frame: %d (1st %.2f %.2f %.2f)", AimPoint.size(), AimPoint[0].x, AimPoint[0].y, AimPoint[0].z);

                float xdif = CenterX - AimPoint[i].x;
                float ydif = CenterY - AimPoint[i].y;

                float hyp = sqrt((xdif * xdif) + (ydif * ydif));

                if (hyp < Closest2)
                {
                    Closest2 = hyp;
                    pointNum = i;
                }
            }
        }

        if (pointNum != -1)
        {
            //gl aim
            //if (aimmode == 1)
                //AimPoint[pointNum].y = Viewport[3] - AimPoint[pointNum].y; //inverted mouse
            //else
                AimPoint[pointNum].y = AimPoint[pointNum].y; //normal mouse

            double DistX = (double)AimPoint[pointNum].x - CenterX;
            double DistY = (double)AimPoint[pointNum].y - CenterY;

            //drawMessage(AimPoint[pointNum].x, AimPoint[pointNum].y, 1.0, "aiming at");

            DistX /= aimsens;
            DistY /= aimsens;

            if (GetAsyncKeyState(Daimkey) & 0x8000)
            {
                    mouse_event(MOUSEEVENTF_MOVE, (DWORD)DistX, (DWORD)DistY, NULL, NULL);
            }

            //autoshoot on
            if (!GetAsyncKeyState(VK_LBUTTON)) //manual override, if player attacks manually don't interfere
                if (autoshoot == 1 && GetAsyncKeyState(Daimkey) & 0x8000)
                {
                    if (!IsPressed)
                    {
                        IsPressed = true;
                        mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
                    }
                }

        }
    }
    AimPoint.clear();

    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (autoshoot == 1 && IsPressed)
    {
        if (timeGetTime() - gametick > asdelay) //
        {
            //Log("timeGetTime() - gametick == %d", timeGetTime() - gametick);
            IsPressed = false;
            mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
            gametick = timeGetTime();
        }
    }

    return Original_wglSwapBuffers(hdc);
}

HRESULT WINAPI Hook_glDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices)
{
    if (initonce)
    {
        //glGenTextures(2, textures);
        glGenTextures(1, &gTexture);
        glGenTextures(1, &g2Texture);
        glGenTextures(1, &rTexture);
        glGenTextures(1, &r2Texture);
        initonce = false;
    }

    //models
    #define STROGG ((count == 9720||count == 9924||count == 6612||count == 9960||count == 8814 || count == 2097|| count == 1998|| count == 1986|| count == 1944)&&(indices==NULL))
    #define GDF ((count == 7812||count == 8268||count == 7806||count == 7788||count == 7218 || count == 1362||count == 1470||count == 1434||count == 2436)&&(indices==NULL))

    //if ((count == 7812 || count == 8268 || count == 7806 || count == 7788 || count == 7218 || count == 1362 || count == 1470 || count == 1434 || count == 2436) && (indices == NULL))
        //GDF = true;
    //else GDF = false;

    #define GDF_HEADS ((count == 3486||count == 396||count == 3378||count == 408||count == 3927||count == 396||count == 3957||count == 444||count == 2502||count == 534)&&(indices==NULL))
    //strogg heads are part of the body most of the time

    #define STROGG_VEHICLES ((count == 5082 || count == 6570 || count == 2253|| count == 15345 || count == 4740 || count == 10698 || count == 1812 || count == 11847 || count == 2889) && (indices == NULL))
    #define GDF_VEHICLES ((count == 7995 || count == 2715 || count == 13101 || count == 4530 || count == 3756 || count == 798 || count == 5949 || count == 2004 || count == 3396 || count == 1560 || count == 7800 || count == 3840 || count == 10320 || count == 3171) && (indices == NULL))

    //weapons (are not drawn if far away)
    //#define STROGG_WEAPONS ((count == 1068||count == 2454||count == 2673||count == 2835||count == 1269||count == 2694)&&(indices==NULL))
    //#define GDF_WEAPONS ((count == 1140||count == 2931||count == 2754||count == 2604||count == 1626||count == 2814)&&(indices==NULL))

    //triangle above heads
    //if (mcap == 2960 && countnum == count)//6 with 1 target, 12, 18, 36 with 6 targets

    
    if(wallhack==1||chams==1)
    {
        //behind walls
        if ((wallhack == 1) && (STROGG || GDF))
        {
            DisableDepthTest();
            //glDepthRange(0.0, 0.0);
        }

        if (chams == 1 && STROGG)
        {
            uint8_t texMagenta[16] = {
                255, 155, 0, 0,
                255, 155, 0, 0,
                255, 155, 0, 0,
                255, 155, 0, 0
            };
            glBindTexture(GL_TEXTURE_2D, rTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, texMagenta);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }
        
        if(chams == 1 && GDF)
        {
            uint8_t texCyan[16] = {
                0, 175, 255, 0,
                0, 175, 255, 0,
                0, 175, 255, 0,
                0, 175, 255, 0
            };
            glBindTexture(GL_TEXTURE_2D, gTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCyan);
            //glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, texCyan);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);// GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);// GL_NEAREST);
        }
        
        Original_glDrawElements(mode, count, type, indices);

        //infront of walls
        if ((wallhack == 1) && (STROGG || GDF))
        {
            EnableDepthTest(); 
            //glDepthRange(0.0, 1.0);
        }

        if (chams == 1 && STROGG)
        {
            uint8_t texRed[16] = {
                255, 0, 0, 0,
                255, 0, 0, 0,
                255, 0, 0, 0,
                255, 0, 0, 0
            };
            glBindTexture(GL_TEXTURE_2D, r2Texture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, texRed);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }

        if (chams == 1 && GDF)
        {
            //unsigned char texGreen[] = {
            uint8_t texGreen[16] = {
                0, 255, 0, 255,  // Green
                0, 255, 0, 255,  // Green
                0, 255, 0, 255,  // Green
                0, 255, 0, 255   // Green
            };

            glBindTexture(GL_TEXTURE_2D, g2Texture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, texGreen);
            //gluBuild2DMipmaps(GL_TEXTURE_2D, 3, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, texGreen);
            //glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, texGreen);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            //glBindTexture(GL_TEXTURE_2D, 0);
        }
   }


    //esp part 1
    if (
        (esp == 1 && STROGG) ||
        (esp == 2 && GDF)
        )
    {
        glGetFloatv(GL_MODELVIEW_MATRIX, MViewMX);
        glGetFloatv(GL_PROJECTION_MATRIX, ProjMX);

        ObjectToWindow(MViewMX, ProjMX, Viewport, Window); //w2s

        ePointf p = { Window[0], Window[1], MViewMX[14] };
        EspPoint.push_back(p);
    }

    //aimbot part 1
    if (
        (aimbot == 1 && (STROGG|| STROGG_VEHICLES)) ||//aim at strogg
        (aimbot == 2 && (GDF|| GDF_VEHICLES))//aim at gdf
        )
    {
        glGetFloatv(GL_MODELVIEW_MATRIX, MViewMX);
        glGetFloatv(GL_PROJECTION_MATRIX, ProjMX);

        ObjectToWindow(MViewMX, ProjMX, Viewport, Window); //w2s

        float ScrCenter[3];
        ScrCenter[0] = float(Viewport[2] / 2);
        ScrCenter[1] = float(Viewport[3] / 2);
        ScrCenter[2] = 0.0f;

        float radiusx = aimfov * (ScrCenter[0] / 100);
        float radiusy = aimfov * (ScrCenter[1] / 100);

        //aimfov
        if (Window[0] >= (ScrCenter[0] - radiusx) && Window[0] <= (ScrCenter[0] + radiusx) && Window[1] >= (ScrCenter[1] - radiusy) && Window[1] <= (ScrCenter[1] + radiusy))
        {
            tPointf p = { Window[0], Window[1], MViewMX[14] };
            AimPoint.push_back(p);
        }
    }


    //GDF Vehicles
    //GDF Platypus Speedboat count == 7995 || count == 2715
    //Bumblebee Helicopter count == 13101 || count == 4530
    //Husky Quad count == 3756 || count == 798
    //Armadillo Jeep count == 5949 || count == 2004
    //Trojan Armored Personnel Carrier count == 3396 || count == 1560
    //Titan Tank count == 7800 || count == 3840
    //Anansi Helicopter count == 10320 || count == 3171
    //Mobile Command Post (MCP)

    //Strogg Vehicles
    //Icarus Hover Pack count == 5082
    //Hog Truck count == 6570 || count == 2253
    //Desecrator Hovertank count == 15345 || count == 4740
    //Cyclops Heavy Walker count == 10698 || count == 1812
    //Tormentor Attack Craft count == 11847 || count == 2889

    /*
    if(STROGG_VEHICLES|| GDF_VEHICLES)
    {
        // save OpenGL state
        //glPushMatrix();
        //glPushAttrib(GL_ALL_ATTRIB_BITS);
       
        //DisableDepthTest();
        //glDepthFunc(GL_ALWAYS);
        //glDepthRange(0.0, 0.0);

        glEnable(GL_TEXTURE_GEN_S); //enable texture coordinate generation, to draw to any texture
        glEnable(GL_TEXTURE_GEN_T);

        //glColorMask(1, 0, 0, 1); //red
        drawSphere(10, 10, 10);

        glDisable(GL_TEXTURE_GEN_S);
        glDisable(GL_TEXTURE_GEN_T);

        //EnableDepthTest();
        //glDepthFunc(GL_LEQUAL);
        //glDepthRange(0.0, 1.0);

        // restore OpenGL state
        //glPopAttrib();
        //glPopMatrix();
    }
    */

    /*
    //hold down P key until a texture is erased, press END to log values of those textures
    if (GetAsyncKeyState('O') & 1) //-
        countnum--;
    if (GetAsyncKeyState('P') & 1) //+
        countnum++;
    if (GetAsyncKeyState(VK_MENU) && GetAsyncKeyState('9') & 1) //reset, set to -1
        countnum = -1;

    
    //log selected values
    if (countnum == count / 100)
        if (GetAsyncKeyState(VK_END) & 1) //press END to log to log.txt
            Log("count == %d", count);

    //erase selected
    if (countnum == count / 100)
    {
        return 0;
    }

    //hold down P key until a texture is erased, press END to log values of those textures
    if (GetAsyncKeyState('O') & 1) //-
        countnum--;
    if (GetAsyncKeyState('P') & 1) //+
        countnum++;
    if (GetAsyncKeyState(VK_MENU) && GetAsyncKeyState('9') & 1) //reset, set to -1
        countnum = -1;
    */
    return Original_glDrawElements(mode, count, type, indices);
}

// =============================================================================================== //

//DWORD Shader;
//bool shaderfound = false;
HRESULT WINAPI Hook_glBindTexture(GLenum target, GLuint texture)
{
    //get shader in quake engine games
    //__asm mov Shader, esi
    //if (Shader > 0x1000 && texture != NULL && Shader != NULL)
        //shaderfound = true;
    //else shaderfound = false;

    //et: 0x016BB418
    //char * lpCurrentShader = *((char **)0x016BB418);

    //lpCurrentShader = *((char**)0x016BB418);//get shader

    //model rec
    //if (strstr(lpCurrentShader, "models/players/temperate/axis/"))
        //team_axis = true; else team_axis = false;

    //if (strstr(lpCurrentShader, "models/players/temperate/allied/"))
        //team_allies = true; else team_allies = false;

   //log shader names
    //if (GetAsyncKeyState(VK_F10) < 0)
        //if (texture != NULL && Shader != NULL)
            //if (strstr((char*)Shader, "models"))
                //Log("Shader == %s\r", Shader);

    return Original_glBindTexture(target, texture);
}
// =============================================================================================== //

HRESULT WINAPI Hook_glVertexPointer(GLint size, GLenum type, GLsizei stride, const void* pointer)
{
    return Original_glVertexPointer(size, type, stride, pointer);
}

// =============================================================================================== //

HRESULT WINAPI Hook_glEnable(GLenum cap)
{
    //mcap = cap;

    return Original_glEnable(cap);
}

// =============================================================================================== //

HRESULT WINAPI Hook_glDisable(GLenum cap)
{
    //mcap = cap;

    return Original_glDisable(cap);
}

// =============================================================================================== //

void InitHook()
{
    //LPVOID lpTarget1 = ::GetProcAddress(::GetModuleHandle(L"gdi32.dll"), "SwapBuffers");//gdi
    LPVOID lpTarget1 = ::GetProcAddress(::GetModuleHandle(L"opengl32.dll"), "wglSwapBuffers");//opengl
    LPVOID lpTarget2 = ::GetProcAddress(::GetModuleHandle(L"opengl32.dll"), "glDrawElements");
    //LPVOID lpTarget3 = ::GetProcAddress(::GetModuleHandle(L"opengl32.dll"), "glBindTexture"); 
    //LPVOID lpTarget4 = ::GetProcAddress(::GetModuleHandle(L"opengl32.dll"), "glEnable");
    //LPVOID lpTarget5 = ::GetProcAddress(::GetModuleHandle(L"opengl32.dll"), "glVertexPointer");

    MH_Initialize();
    MH_CreateHook(lpTarget1, &Hook_wglSwapBuffers, (void**)&Original_wglSwapBuffers);
    MH_EnableHook(lpTarget1);
    MH_CreateHook(lpTarget2, &Hook_glDrawElements, (void**)&Original_glDrawElements);
    MH_EnableHook(lpTarget2);
    //MH_CreateHook(lpTarget3, &Hook_glBindTexture, (void**)&Original_glBindTexture);
    //MH_EnableHook(lpTarget3);
    //MH_CreateHook(lpTarget4, &Hook_glEnable, (void**)&Original_glEnable);
    //MH_EnableHook(lpTarget4);
    //MH_CreateHook(lpTarget5, &Hook_glVertexPointer, (void**)&Original_glVertexPointer);
    //MH_EnableHook(lpTarget5);
}

// =============================================================================================== //

DWORD WINAPI nStart(LPVOID lpParameter)
{
    InitWindow();
    InitHook();

    return 0;
}

// =============================================================================================== //

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
    switch (dwReason)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        //GetModuleFileName(hModule, dlldir, 512);
        GetModuleFileNameA(hModule, dlldir, 512);
        for (size_t i = strlen(dlldir); i > 0; i--) { if (dlldir[i] == '\\') { dlldir[i + 1] = 0; break; } }
        CreateThread(NULL, 0, nStart, hModule, 0, NULL);
        break;

    case DLL_PROCESS_DETACH:
        MH_Uninitialize();
        break;
    }

    return true;
}
