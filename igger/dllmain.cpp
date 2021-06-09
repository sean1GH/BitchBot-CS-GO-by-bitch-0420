#include <iostream> // Include basic c++ library
#include <Windows.h> // Include basic windows library
#include "csgo.h" // Include header file where static offsets are located

using namespace hazedumper; // Use hazedumper namespace for quicker access
using namespace signatures; // Use signatures namespace for quicker access
using namespace netvars; // Use netvars namespace for quicker access
using namespace std; // Use std namespace for quicker access 

void Bhop(DWORD client, DWORD local) // Bhop function
{
	DWORD flags = *(DWORD*)(local + m_fFlags); // Define flags as localplayer added to flags offset
	if (GetAsyncKeyState(VK_SPACE) && flags & (1 << 0)) // Check to see if space is pressed and localplayer meets requirements
		*(DWORD*)(client + dwForceJump) = 6; // Write to forcejump to send a jump to client
}

float rgbColor(float val) // rgbColor function
{
	return val / 255.f; // Return our color in rgb form
}

void Glow(DWORD client, DWORD local) // Glow function
{
	DWORD glowManager = *(DWORD*)(client + dwGlowObjectManager); // Define our glow manager
	for (int i = 1; i < 64; i++) // Loop through each entity
	{
		DWORD entity = *(DWORD*)(client + dwEntityList + i * 0x10); // Define a variable for entity
		DWORD localTeam = *(DWORD*)(local + m_iTeamNum); // Define local team 
		if (entity) // Check for entity
		{
			DWORD entityTeam = *(DWORD*)(entity + m_iTeamNum); // Define entity's team 
			DWORD entityGlow = *(DWORD*)(entity + m_iGlowIndex); // Define entity's glow index
			if (entityTeam != localTeam) // Check to see if entity is not on local player's team
			{
				*(float*)(glowManager + entityGlow * 0x38 + 0x4) = rgbColor(255); // Set glow's red value
				*(float*)(glowManager + entityGlow * 0x38 + 0x8) = rgbColor(0); // Set glow's green value
				*(float*)(glowManager + entityGlow * 0x38 + 0xC) = rgbColor(0); // Set glow's blue value
				*(float*)(glowManager + entityGlow * 0x38 + 0x10) = rgbColor(225); // Set glow's alpha value
				*(int*)(glowManager + entityGlow * 0x38 + 0x24) = 1; // Active glow
			}
			else if (entityTeam == localTeam) // Check to see if entity is on local player's team
			{
				*(float*)(glowManager + entityGlow * 0x38 + 0x4) = rgbColor(0); // Set glow's red value
				*(float*)(glowManager + entityGlow * 0x38 + 0x8) = rgbColor(255); // Set glow's green value
				*(float*)(glowManager + entityGlow * 0x38 + 0xC) = rgbColor(0); // Set glow's blue value
				*(float*)(glowManager + entityGlow * 0x38 + 0x10) = rgbColor(225); // Set glow's alpha value
				*(int*)(glowManager + entityGlow * 0x38 + 0x24) = 1; // Active glow
			}
		}
	}
}

void CreateConsole() // CreateConsole function
{
	AllocConsole(); // Create the console
	FILE * f; // Define the file variable
	freopen_s(&f, "CONOUT$", "w", stdout); // Allow writing to console
}

DWORD APIENTRY MainThread(HMODULE hModule) // MainThread function
{
	CreateConsole(); // Call CreateConsole function
	DWORD client = (DWORD)GetModuleHandle("client.dll"); // Define client.dll base module handle
	DWORD localPlayer = *(DWORD*)(client + dwLocalPlayer); // Define localplayer

	cout << "attached to game\n"; // Signal that dll has attached to game
	Beep(500, 250); // Make beep to notify of dll attaching

	if (localPlayer == NULL) // Check to see if localplayer is not valid
		while (localPlayer == NULL) // Create loop where the condition is the localplayer is not valid
			localPlayer = *(DWORD*)(client + dwLocalPlayer); // Keep defining localplayer until valid

	cout << "running cheat loop\n"; // Signal that it is about run main cheat loop
	while (!GetAsyncKeyState(VK_END)) // Run main cheat loop until end key is pressed
	{
		Bhop(client, localPlayer); // Call Bhop function
		Glow(client, localPlayer); // Call Glow function
	}

	FreeConsole(); // Deattach console from game
	FreeLibraryAndExitThread(hModule, 0); // ExitThread for unloading
	return 1; // Return 1 standard in main thread
}

BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) // DLLMain function
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH) // Case for call
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)MainThread, hModule, NULL, NULL); // Create main cheat thread
		
    return TRUE; // Return true standard in dllmain
}

