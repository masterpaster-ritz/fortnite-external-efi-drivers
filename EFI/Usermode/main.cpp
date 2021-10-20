#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <math.h>
#include <TlHelp32.h>
#include <Psapi.h>
#include <tchar.h>
#include <winioctl.h>
#include <d3d9.h>
#include "C:\Users\mab82\Downloads\EFI\Usermode\D3XD\d3dx9.h"
#include <dwmapi.h>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dwmapi.lib")

#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_dx9.h"
#include "Imgui/imgui_impl_win32.h"

#include "utils.hpp"
#include <iostream>

#include <tlhelp32.h>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <vector>
#include <iomanip>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <string>
#include<iostream> 
#include<fstream>
#include <Windows.h>
#include <stdio.h>
#include "zStr.h"

// Blatant fortnite paste with the cruz's modified driver of sam tulach's efi driver
// No particular reason for doing this, just thought i will cause was bored and didnt see any fortnite sources utlizing this driver
// Hmu on discord (Chase.#1803) if you need any help :)

IDirect3D9Ex* p_Object = NULL;
IDirect3DDevice9Ex* p_Device = NULL;
D3DPRESENT_PARAMETERS p_Params = { NULL };

HWND MyWnd = NULL;
HWND GameWnd = NULL;
RECT GameRect = { NULL };
MSG Message = { NULL };

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
void CleanD3D();

// Hmu on discord (Chase.#1803) if you need any help :)

HRESULT DirectXInit(HWND hWnd) {
	if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &p_Object))) {
		exit(3);
	}

	ZeroMemory(&p_Params, sizeof(p_Params));
	p_Params.Windowed = TRUE;
	p_Params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	p_Params.hDeviceWindow = hWnd;
	p_Params.MultiSampleQuality = D3DMULTISAMPLE_NONE;
	p_Params.BackBufferFormat = D3DFMT_A8R8G8B8;
	p_Params.BackBufferWidth = Settings::Majors::Width;
	p_Params.BackBufferHeight = Settings::Majors::Height;
	p_Params.EnableAutoDepthStencil = TRUE;
	p_Params.AutoDepthStencilFormat = D3DFMT_D16;
	p_Params.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	if (FAILED(p_Object->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &p_Params, 0, &p_Device)))
	{
		p_Object->Release();
		exit(4);
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.IniFilename = nullptr; //crutial for not leaving the imgui.ini file

	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX9_Init(p_Device);

	ImGui::GetMouseCursor();
	ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
	ImGui::GetIO().WantCaptureMouse = Settings::Majors::menuIsOpen;
	ImGui::GetIO().MouseDrawCursor = Settings::Majors::menuIsOpen;

	p_Object->Release();
	return S_OK;
}
// Hmu on discord (Chase.#1803) if you need any help :)
void SetupWindow()
{
	GameWnd = FindWindowW(NULL, TEXT("Fortnite  "));

	if (GameWnd)
	{
		GetClientRect(GameWnd, &GameRect);

		POINT xy = { 0 };

		ClientToScreen(GameWnd, &xy);

		GameRect.left = xy.x;
		GameRect.top = xy.y;

		Settings::Majors::Width = GameRect.right;
		Settings::Majors::Height = GameRect.bottom;
	}
	else {
		exit(2);
	}

	WNDCLASSEX overlayWindowClass;
	ZeroMemory(&overlayWindowClass, sizeof(WNDCLASSEX));
	overlayWindowClass.cbClsExtra = NULL;
	overlayWindowClass.cbWndExtra = NULL;
	overlayWindowClass.cbSize = sizeof(WNDCLASSEX);
	overlayWindowClass.style = CS_HREDRAW | CS_VREDRAW;
	overlayWindowClass.lpfnWndProc = WinProc;
	overlayWindowClass.hInstance = NULL;
	overlayWindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	overlayWindowClass.hIcon = LoadIcon(0, IDI_APPLICATION);
	overlayWindowClass.hIconSm = LoadIcon(0, IDI_APPLICATION);
	overlayWindowClass.hbrBackground = (HBRUSH)RGB(0, 0, 0);
	overlayWindowClass.lpszClassName = L"Ass";
	overlayWindowClass.lpszMenuName = L"Ass";
	RegisterClassEx(&overlayWindowClass);

	MyWnd = CreateWindowEx(NULL, L"Ass", L"Ass", WS_POPUP | WS_VISIBLE, GameRect.left, GameRect.top, Settings::Majors::Width, Settings::Majors::Height, NULL, NULL, NULL, NULL);

	MARGINS margin = { GameRect.left, GameRect.top, Settings::Majors::Width, Settings::Majors::Height };
	DwmExtendFrameIntoClientArea(MyWnd, &margin);

	SetWindowLong(MyWnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW);

	ShowWindow(MyWnd, SW_SHOW);
	UpdateWindow(MyWnd);
}
// Hmu on discord (Chase.#1803) if you need any help :)
Vector3 Camera(unsigned __int64 RootComponent)
{
	unsigned __int64 PtrPitch;
	Vector3 Camera;

	auto pitch = Driver::read<uintptr_t>(pid, Settings::Majors::LocalPlayer + 0xb0);
	Camera.x = Driver::read<float>(pid, RootComponent + 0x12C);
	Camera.y = Driver::read<float>(pid, pitch + 0x678);

	float test = asin(Camera.y);
	float degrees = test * (180.0 / M_PI);

	Camera.y = degrees;

	if (Camera.x < 0)
		Camera.x = 360 + Camera.x;

	return Camera;
}
// Hmu on discord (Chase.#1803) if you need any help :)
void actorLoop() {
	std::vector<ActorStruct> actorStructVector;

	uintptr_t uWorld = Driver::read<uintptr_t>(pid, BaseAddr + 0x9864730); //update
	if (!uWorld) {
		return;
	}

	uintptr_t PersistentLevel = Driver::read<uintptr_t>(pid, uWorld + 0x30);
	if (!PersistentLevel) {
		return;
	}

	uintptr_t OwningGameInstance = Driver::read<uintptr_t>(pid, uWorld + 0x188); //updated
	if (!OwningGameInstance) {
		return;
	}

	uintptr_t LocalPlayers = Driver::read<uintptr_t>(pid, OwningGameInstance + 0x38);
	if (!LocalPlayers) {
		return;
	}

	Settings::Majors::LocalPlayer = Driver::read<uintptr_t>(pid, LocalPlayers);
	if (!Settings::Majors::LocalPlayer) {
		return;
	}

	uintptr_t LocalPlayerController = Driver::read<uintptr_t>(pid, Settings::Majors::LocalPlayer + 0x30);
	if (!LocalPlayerController) {
		return;
	}

	Settings::Majors::LocalPawn = Driver::read<uintptr_t>(pid, LocalPlayerController + 0x2A0);
	if (!Settings::Majors::LocalPawn) {
		return;
	}
	else {
		Settings::Majors::LocalPawnRootComponent = Driver::read<uintptr_t>(pid, Settings::Majors::LocalPawn + 0x130);
		Settings::Majors::LocalPlayerRelativeLocation = Driver::read<Vector3>(pid, Settings::Majors::LocalPawnRootComponent + 0x11C);

		Settings::Majors::LocalPlayerID = Driver::read<int>(pid, Settings::Majors::LocalPawn + 0x18);
	}

	uint64_t localplayerstate = Driver::read<uint64_t>(pid, Settings::Majors::LocalPawn + 0x240);
	int LocalTeam = Driver::read<int>(pid, localplayerstate + 0xED8); //updated
	Vector3 Localcam = Camera(Settings::Majors::LocalPawnRootComponent);

	for (int index = 0; index < Driver::read<int>(pid, PersistentLevel + (0x98 + sizeof(uintptr_t))); index++)
	{
		uintptr_t PersistentLevelActors = Driver::read<uintptr_t>(pid, PersistentLevel + 0x98);
		if (!PersistentLevelActors) {
			return;
		}

		uintptr_t CurrentActor = Driver::read<uintptr_t>(pid, PersistentLevelActors + (index * sizeof(uintptr_t)));
		if (!CurrentActor) {
			continue;
		}

		uintptr_t CurrentActorMesh = Driver::read<uintptr_t>(pid, CurrentActor + 0x280);
		if (!CurrentActorMesh) {
			continue;
		}

		int CurrentActorID = Driver::read<int>(pid, CurrentActor + 0x18);
		if (!CurrentActorID) {
			continue;
		}

		bool bSpotted = Driver::read<bool>(pid, CurrentActor + 0x542);
		if (!bSpotted) {
			continue;
		}

		if (CurrentActorID != 0 && ((CurrentActorID == Settings::Majors::LocalPlayerID) || (bSpotted != 0 && Settings::Majors::CorrectbSpotted == bSpotted))) {
			Settings::Majors::CorrectbSpotted = bSpotted;

			ActorStruct Actor{ };
			Actor.pObjPointer = CurrentActor;
			Actor.ID = CurrentActorID;
			Actor.Mesh = CurrentActorMesh;

			actorStructVector.push_back(Actor);
		}
		for (const ActorStruct& ActorStruct : actorStructVector)
		{
			if (ActorStruct.pObjPointer == Settings::Majors::LocalPawn) {
				continue;
			}
			uint64_t playerstate = Driver::read<uint64_t>(pid, ActorStruct.pObjPointer + 0x240);
			int TeamIndex = Driver::read<int>(pid, playerstate + 0xED8);
		}
	}

	if (actorStructVector.empty()) {
		return;
	}

	bool bValidEnemyInArea = false;
	float ClosestActorDistance = FLT_MAX;
	Vector3 ClosestActorMouseAimbotPosition = Vector3(0.0f, 0.0f, 0.0f);
	float distance;

	for (const ActorStruct& ActorStruct : actorStructVector)
	{
		if (ActorStruct.pObjPointer == Settings::Majors::LocalPawn) {
			continue;
		}

		uintptr_t RootComponent = Driver::read<uintptr_t>(pid, ActorStruct.pObjPointer + 0x130);
		if (!RootComponent) {
			continue;
		}

		uint64_t playerstate = Driver::read<uint64_t>(pid, ActorStruct.pObjPointer + 0x240);
		int TeamIndex = Driver::read<int>(pid, playerstate + 0xED0);

		Vector3 vHeadBone = GetBoneWithRotation(ActorStruct.Mesh, 66);
		Vector3 vRootBone = GetBoneWithRotation(ActorStruct.Mesh, 0);

		Vector3 vHeadBoneOut = ProjectWorldToScreen(Vector3(vHeadBone.x, vHeadBone.y, vHeadBone.z + 20), Vector3(Localcam.y, Localcam.x, Localcam.z));
		Vector3 vRootBoneOut = ProjectWorldToScreen(vRootBone, Vector3(Localcam.y, Localcam.x, Localcam.z));

		Vector3 RootPos = GetBoneWithRotation(ActorStruct.Mesh, select_hitbox());
		Vector3 selection;
		
		float BoxHeight = vHeadBoneOut.y - vRootBoneOut.y;

		if (BoxHeight < 0)
				BoxHeight = BoxHeight * (-1.f);
		float BoxWidth = BoxHeight * 0.35;


		Vector3 RelativeInternalLocation = Driver::read<Vector3>(pid, RootComponent + 0x11C);
		if (!RelativeInternalLocation.x && !RelativeInternalLocation.y) {
			continue;
		}

		Vector3 RelativeScreenLocation = ProjectWorldToScreen(RelativeInternalLocation, Vector3(Localcam.y, Localcam.x, Localcam.z));
		if (!RelativeScreenLocation.x && !RelativeScreenLocation.y) {
			continue;
		}

		distance = Settings::Majors::LocalPlayerRelativeLocation.Distance(RelativeInternalLocation) / 100.f;

		if (TeamIndex != LocalTeam) {

			if (distance <= Settings::EspDistance) {

				if (Settings::Prediction) {

					Vector3 Velocity = Driver::read<Vector3>(pid, ActorStruct.pObjPointer + 0x140);
					auto Result = CalculatePrediction(RootPos, Velocity, distance, 275.0f);
					selection = ProjectWorldToScreen(Result, Vector3(Localcam.y, Localcam.x, Localcam.z));
				}

				float ScreenLocationX = selection.x - Settings::Majors::ScreenCenterX, ScreenLocationY = selection.y - Settings::Majors::ScreenCenterY;
				float ActorDistance = std::sqrtf(ScreenLocationX * ScreenLocationX + ScreenLocationY * ScreenLocationY);

				if (Settings::Box)
				{
					DrawCornerBox(vRootBoneOut.x - (BoxWidth / 2), vHeadBoneOut.y, BoxWidth, BoxHeight, 1.9f, &ESPColor);
				}

				if (ActorDistance < ClosestActorDistance && ActorDistance < Settings::AimbotFOVValue) {
					ClosestActorDistance = ActorDistance;
					ClosestActorMouseAimbotPosition = Vector3(ScreenLocationX, ScreenLocationY, 0.0f);
					bValidEnemyInArea = true;
				}
			}
		}
	}

	if (Settings::Aimbot && bValidEnemyInArea && GetAsyncKeyState(hotkeys::aimkey)) {
		float PlayerLocationX = ClosestActorMouseAimbotPosition.x /= Settings::AimbotSmoothingValue, PlayerLocationY = ClosestActorMouseAimbotPosition.y /= Settings::AimbotSmoothingValue;

		if (!PlayerLocationX || !PlayerLocationY) {
			return;
		}

		mouse_event(MOUSEEVENTF_MOVE, PlayerLocationX, PlayerLocationY, 0, 0);
	}
}
// Hmu on discord (Chase.#1803) if you need any help :)
void renderLoopCall() {

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	RECT rect = { 0 };

	if (GetWindowRect(GameWnd, &rect))
	{
		Settings::Majors::Width = rect.right - rect.left;
		Settings::Majors::Height = rect.bottom - rect.top;
	}

	Settings::Majors::ScreenCenterX = (Settings::Majors::Width / 2.0f), Settings::Majors::ScreenCenterY = (Settings::Majors::Height / 2.0f);

	if (GetAsyncKeyState(VK_INSERT) & 1) {
		Settings::Majors::menuIsOpen = !Settings::Majors::menuIsOpen;
	}

	actorLoop();

	static int tabb = 1;

	if (Settings::Majors::menuIsOpen)
	{
		if (ImGui::Begin(z(" Fortnite "), 0, ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_::ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings))
			ImGui::SetNextWindowSize(ImVec2({ 300, 650 }), ImGuiSetCond_FirstUseEver); {
				ImGui::SetWindowSize(ImVec2(320, 250));
				ImGui::Checkbox(z("Enable Box Esp"), &Settings::Box);
				ImGui::Checkbox(z("Enable Aimbot"), &Settings::Aimbot);
				ImGui::Text(z("Aimbot Key: ")); ImGui::SameLine(90.f);
				HotkeyButton(hotkeys::aimkey, ChangeKey, keystatus);
				ImGui::SliderFloat(z("Aimbot Fov"), &Settings::AimbotFOVValue, 20, 500);
				ImGui::SliderFloat(z("Smoothness"), &Settings::AimbotSmoothingValue, 1, 30);
				ImGui::Text(z("Aimbot Bone: "));
				ImGui::SameLine();
				ImGui::Combo(z("##BONES"), &Settings::hitbox, Hitbox, IM_ARRAYSIZE(Hitbox));

			ImGui::End();
		}
	}
	else {
	}

	ImGui::EndFrame();

	p_Device->SetRenderState(D3DRS_ZENABLE, false);
	p_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	p_Device->SetRenderState(D3DRS_SCISSORTESTENABLE, false);
	p_Device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);

	if (p_Device->BeginScene() >= 0)
	{
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		p_Device->EndScene();
	}

	HRESULT result = p_Device->Present(NULL, NULL, NULL, NULL);

	if (result == D3DERR_DEVICELOST && p_Device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
	{
		ImGui_ImplDX9_InvalidateDeviceObjects();
		p_Device->Reset(&p_Params);
		ImGui_ImplDX9_CreateDeviceObjects();
	}
}
// Hmu on discord (Chase.#1803) if you need any help :)
WPARAM MainLoop()
{
	static RECT old_rc;
	ZeroMemory(&Message, sizeof(MSG));

	while (Message.message != WM_QUIT)
	{
		if (PeekMessage(&Message, MyWnd, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}

		HWND hwndActive = GetForegroundWindow();

		if (hwndActive == GameWnd) {
			HWND hwndTest = GetWindow(hwndActive, GW_HWNDPREV);

			SetWindowPos(MyWnd, hwndTest, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}

		RECT rc;
		POINT xy;

		ZeroMemory(&rc, sizeof(RECT));
		ZeroMemory(&xy, sizeof(POINT));
		GetClientRect(GameWnd, &rc);
		ClientToScreen(GameWnd, &xy);
		rc.left = xy.x;
		rc.top = xy.y;

		ImGuiIO& io = ImGui::GetIO();
		io.ImeWindowHandle = GameWnd;
		io.DeltaTime = 1.0f / 60.0f;

		POINT p;
		GetCursorPos(&p);
		io.MousePos.x = p.x - xy.x;
		io.MousePos.y = p.y - xy.y;

		if (GetAsyncKeyState(VK_LBUTTON)) {
			io.MouseDown[0] = true;
			io.MouseClicked[0] = true;

			io.MouseClickedPos[0].x = io.MousePos.x;
			io.MouseClickedPos[0].x = io.MousePos.y;
		}
		else {
			io.MouseDown[0] = false;
		}

		SetWindowPos(GameWnd, MyWnd, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

		renderLoopCall();
	}

	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CleanD3D();
	DestroyWindow(MyWnd);

	return Message.wParam;
}
// Hmu on discord (Chase.#1803) if you need any help :)
LRESULT CALLBACK WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, Message, wParam, lParam)) {
		return true;
	}

	switch (Message)
	{
	case WM_DESTROY:
		CleanD3D();
		PostQuitMessage(0);
		exit(4);
		break;
	case WM_SIZE:
		if (p_Device != NULL && wParam != SIZE_MINIMIZED)
		{
			ImGui_ImplDX9_InvalidateDeviceObjects();
			p_Params.BackBufferWidth = LOWORD(lParam);
			p_Params.BackBufferHeight = HIWORD(lParam);
			HRESULT hr = p_Device->Reset(&p_Params);

			if (hr == D3DERR_INVALIDCALL) {
				IM_ASSERT(0);
			}

			ImGui_ImplDX9_CreateDeviceObjects();
		}
		break;
	default:
		return DefWindowProc(hWnd, Message, wParam, lParam);
		break;
	}

	return 0;
}
// Hmu on discord (Chase.#1803) if you need any help :)
void CleanD3D() {
	if (p_Device != NULL)
	{
		p_Device->EndScene();
		p_Device->Release();
	}

	if (p_Object != NULL)
	{
		p_Object->Release();
	}
}
// Hmu on discord (Chase.#1803) if you need any help :)
bool CheckDriverStatus() {
	int icheck = 29;
	NTSTATUS status = 0;
	Unprotect(Driver::GetBaseAddress);
	uintptr_t BaseAddr = Driver::GetBaseAddress(Driver::currentProcessId);
	if (BaseAddr == 0) {
		return false;
	}
	Protect(Driver::GetBaseAddress);

	int checked = Driver::read<int>(Driver::currentProcessId, (uintptr_t)&icheck, &status);
	if (checked != icheck) {
		return false;
	}

	return true;
}
// Hmu on discord (Chase.#1803) if you need any help :)
DWORD GetProcessIdByName(wchar_t* name) {
	Protect(_ReturnAddress());

	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (Process32First(snapshot, &entry) == TRUE) {
		while (Process32Next(snapshot, &entry) == TRUE) {
			if (_wcsicmp(entry.szExeFile, name) == 0) {
				Unprotect(_ReturnAddress());
				return entry.th32ProcessID;
			}
		}
	}

	CloseHandle(snapshot);
	Unprotect(_ReturnAddress());
	return 0;
}
// Hmu on discord (Chase.#1803) if you need any help :)
uintptr_t milliseconds_now() {
	static LARGE_INTEGER s_frequency;
	static BOOL s_use_qpc = QueryPerformanceFrequency(&s_frequency);
	if (s_use_qpc) {
		LARGE_INTEGER now;
		QueryPerformanceCounter(&now);
		return (1000LL * now.QuadPart) / s_frequency.QuadPart;
	}
	else {
		return GetTickCount();
	}
}
// Hmu on discord (Chase.#1803) if you need any help :)
void LoadProtectedFunctions() {

	uintptr_t t = milliseconds_now();
	BYTE xorkey = 0x0;
	for (DWORD i = 0; i < 8; i++) {
		xorkey = ((BYTE*)&t)[i];
		if (xorkey > 0x3 && xorkey < 0xf0) {
			break;
		}
	}
	if (xorkey <= 0x3 || xorkey >= 0xf0) {
		xorkey = 0x56;
	}

	addFunc({ LoadProtectedFunctions, (uintptr_t)CheckDriverStatus - (uintptr_t)LoadProtectedFunctions - 0x3, xorkey, false });
	addFunc({ CheckDriverStatus, (uintptr_t)GetProcessIdByName - (uintptr_t)CheckDriverStatus - 0x3, xorkey, false });
	addFunc({ GetProcessIdByName, (uintptr_t)milliseconds_now - (uintptr_t)GetProcessIdByName - 0x3, xorkey, false });

	addFunc({ Driver::SendCommand, (uintptr_t)Driver::GetBaseAddress - (uintptr_t)Driver::SendCommand - 0x3, xorkey, false });
	addFunc({ Driver::GetBaseAddress, (uintptr_t)Driver::copy_memory - (uintptr_t)Driver::GetBaseAddress - 0x3, xorkey, false });
	addFunc({ Driver::copy_memory, (uintptr_t)GetKernelModuleExport - (uintptr_t)Driver::copy_memory - 0x3, xorkey, false });
	addFunc({ GetKernelModuleExport, (uintptr_t)GetKernelModuleAddress - (uintptr_t)GetKernelModuleExport - 0x3, xorkey, false });
	addFunc({ GetKernelModuleAddress, (uintptr_t)Driver::initialize - (uintptr_t)GetKernelModuleAddress - 0x3, xorkey, false });
	addFunc({ Driver::initialize, (uintptr_t)Driver::read_memory - (uintptr_t)Driver::initialize - 0x3, xorkey, false });
	//addFunc({ Driver::read_memory, (uintptr_t)Driver::write_memory - (uintptr_t)Driver::read_memory - 0x3, xorkey, false });
	//addFunc({ Driver::write_memory, (uintptr_t)getEntity - (uintptr_t)Driver::write_memory - 0x3, xorkey, false });

	for (size_t i = 0; i < funcCount; i++) {
		if (functions[i].address != LoadProtectedFunctions)
			Protect(functions[i].address);
	}
	Unprotect(_ReturnAddress());
}
// Hmu on discord (Chase.#1803) if you need any help :)
bool verify_game() {

	wchar_t name[] = { 'F', 'o', 'r', 't', 'n', 'i', 't', 'e', 'C', 'l', 'i', 'e', 'n', 't', '-', 'W', 'i' , 'n' , '6', '4', '-' , 'S' , 'h', 'i', 'p', 'p', 'i', 'n', 'g', '.','e', 'x', 'e' , 0 };
	//FortniteClient-Win64-Shipping
	Unprotect(GetProcessIdByName);
	pid = GetProcessIdByName(name);
	Protect(GetProcessIdByName);
	memset(name, 0, sizeof(name));

	Unprotect(Driver::GetBaseAddress);
	BaseAddr = Driver::GetBaseAddress(pid);
	Protect(Driver::GetBaseAddress);

	if (BaseAddr != 0) {

		GamePid = pid;
		GameBaseAddress = BaseAddr;


		GamePid = 0;
		GameBaseAddress = 0;

	}
	//ProtectedSleep(2000);

	return true;
}
// Hmu on discord (Chase.#1803) if you need any help :)
int main()
{
	LoadProtectedFunctions();
	Protect(LoadProtectedFunctions);
	Unprotect(Driver::initialize);
	Unprotect(CheckDriverStatus);

	if (!Driver::initialize() || !CheckDriverStatus()) {
		wchar_t VarName[] = { 'F','a','s','t','B','o','o','t','O','p','t','i','o','n','\0' };
		UNICODE_STRING FVariableName = UNICODE_STRING();
		FVariableName.Buffer = VarName;
		FVariableName.Length = 28;
		FVariableName.MaximumLength = 30;
		myNtSetSystemEnvironmentValueEx(
			&FVariableName,
			&DummyGuid,
			0,
			0,
			ATTRIBUTES);
		memset(VarName, 0, sizeof(VarName));
		Beep(600, 1000);
		printf(z("Driver not Loaded"));
		ProtectedSleep(3000);
		exit(1);
		return 1;
	}
	Protect(Driver::initialize);
	Protect(CheckDriverStatus);
	Sleep(1000);
	SetupWindow();
	DirectXInit(MyWnd);
	verify_game();
	std::cout << "" << std::endl;

	Beep(500, 500);
	while (TRUE)
	{
		MainLoop();
	}

	Sleep(10000);
	return 0;
}