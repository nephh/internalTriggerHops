#include <Windows.h>
#include <iostream>

#define KeyDOWN 0x8000

struct offsets
{
	DWORD dwLocalPlayer = 0xC618AC;
	DWORD dwForceAttack = 0x307FD44;
	DWORD dwEntityList = 0x4C3E674;
	DWORD m_iCrosshairId = 0xB2DC;
	DWORD m_iTeamNum = 0xF0;
	DWORD m_iHealth = 0xFC;
	DWORD m_fFlags = 0x100;
	DWORD dwForceJump = 0x50E13BC;
	DWORD m_vecVelocity = 0x110;
}	offset;

struct variables
{
	DWORD localPlayer;
	DWORD gameModule;
	DWORD entity;
	int eTeam;
	int eHealth;
	int myTeam;
	int crosshair;
	BYTE flag;
}	val;

struct vector
{
	float x, y, z;
};

void shoot()
{
	if (GetAsyncKeyState(VK_XBUTTON1) & KeyDOWN)
	{
		*(int*)(val.gameModule + offset.dwForceAttack) = 5;
		Sleep(20);
		*(int*)(val.gameModule + offset.dwForceAttack) = 4;
	}
}

bool checkTBot()
{
	val.crosshair = *(int*)(val.localPlayer + offset.m_iCrosshairId);
	if (val.crosshair != 0 && val.crosshair < 64)
	{
		val.entity = *(DWORD*)(val.gameModule + offset.dwEntityList + ((val.crosshair - 1) * 0x10));
		val.eTeam = *(int*)(val.entity + offset.m_iTeamNum);
		val.eHealth = *(int*)(val.entity + offset.m_iHealth);

		if (val.eTeam != val.myTeam && val.eHealth > 0)
			return true;
		else
			return false;
	}
	return 0;
}

void handleTBot()
{
	if (checkTBot())
		shoot();
}

void bHop()
{
	if (GetAsyncKeyState(VK_SPACE) && val.flag & (1 << 0))
		*(DWORD*)(val.gameModule + offset.dwForceJump) = 6;
	Sleep(20);
}

bool isPlayerMoving()
{
	vector playerVel = *(vector*)(val.localPlayer + offset.m_vecVelocity);
	int vel = playerVel.x + playerVel.y + playerVel.z;
	if (vel != 0)
		return true;
	else
		return false;
}

void handleBHop()
{
	if (isPlayerMoving())
		bHop();
}

int main()
{
	bool toggleTBot = false, toggleBHop = false;

	val.gameModule = (DWORD)GetModuleHandle("client_panorama.dll");
	val.localPlayer = *(DWORD*)(val.gameModule + offset.dwLocalPlayer);

	if (val.localPlayer == NULL)
		while (val.localPlayer == NULL)
			val.localPlayer = *(DWORD*)(val.gameModule + offset.dwLocalPlayer);

	while (true)
	{
		if (GetAsyncKeyState(VK_F2) & 1)
		{
			val.myTeam = *(int*)(val.localPlayer + offset.m_iTeamNum);
			toggleTBot = !toggleTBot;
		}
		
		if (GetAsyncKeyState(VK_F1) & KeyDOWN)
		{
			val.flag = *(BYTE*)(val.localPlayer + offset.m_fFlags);
			toggleBHop = !toggleBHop;
		}

		if (toggleTBot)
			handleTBot();

		if (toggleBHop)
			handleBHop();
	}
};

BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH) {
		DisableThreadLibraryCalls(hModule);
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)main, NULL, NULL, NULL);
	}
	return TRUE;
}