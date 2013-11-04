/*

adbrs.cpp

ADB Recovery Screenshot: Takes screenshot of device's screen when in recovery mode (can also take when it's not!)

|| Written by: Suyash Srijan
|| suyashsrijan@outlook.com
|| Tested on: HTC One w/ 4.3

|| You are free to use the code anywhere you want but this comment block must not be removed

*/

#include <stdio.h>
#include <iostream>
#include <tchar.h>
#include <windows.h>
#include <sstream>

int width = 1088; // Default width + 8 (stride)
int height = 1920; // Default height

// Convert integer to string type using stringstream
std::string str2int(int num) {
	std::stringstream ss;
	ss << num;
	return ss.str();
}

void killAdb() {
	// Kill ADB, just in case...
	// ---------------------------

	std::wcout << "Killing adb server and rerouting standard output stream to NULL\n";
	STARTUPINFO startupinfo;
	PROCESS_INFORMATION processinfo;
	LPTSTR szCmdLine = _tcsdup(TEXT("adb.exe kill-server -d >nul 2>&1"));
	ZeroMemory(&startupinfo, sizeof(startupinfo));
	startupinfo.cb = sizeof(startupinfo);
	ZeroMemory(&processinfo, sizeof(processinfo));
	HRESULT rv = CreateProcess(NULL, szCmdLine, NULL, NULL, FALSE, 0, NULL, NULL, &startupinfo, &processinfo);
	signed int _rv = static_cast<int>(rv);
	if (rv == 0) {
		// Something bad happened
		std::wcout << "\n\nCall failed. (Error: " << GetLastError() << ")\n";
		std::getwchar();
	}
	else {
		// We're done!
		std::wcout << "Done!\n";
	}
}

void cpyFrameBuffer() {
	// Copy window-system-provided framebuffer to current folder
	// ----------------------------------------------------------

	std::wcout << "Copying framebuffer device (fb0) from phone to current folder\n";
	STARTUPINFO startupinfo;
	PROCESS_INFORMATION processinfo;
	LPTSTR szCmdLine = _tcsdup(TEXT("adb.exe pull /dev/graphics/fb0 fb0"));
	ZeroMemory(&startupinfo, sizeof(startupinfo));
	startupinfo.cb = sizeof(startupinfo);
	ZeroMemory(&processinfo, sizeof(processinfo));
	HRESULT rv = CreateProcess(NULL, szCmdLine, NULL, NULL, FALSE, 0, NULL, NULL, &startupinfo, &processinfo);
	signed int _rv = static_cast<int>(rv);
	if (rv == 0) {
		// Something bad happened
		std::wcout << "\n\nCall failed. (Error: " << GetLastError() << ")\n";
		std::getwchar();
	}
	else {
		// We're done!
		std::wcout << "Done!\n";
	}
}

void framebufferToPNG(int w, int h) {
	// Convert the first frame from the framebuffer to PNG
	// ---------------------------------------------------------------------------------------------
	// If you get a weird output (with glitches) then you are passing the wrong [framebuffer] width.

	//  +-------------------------+----+
	//	|                         |    |
	//	|                         |    |
	//	| <--------res in x-----> |    | = display resolution (x)
	//	|                         |    |
	//	|                         |    |
	//	| <------- line_length-------->| = length of a line in bytes
	//	|                         |    |
	//	|                         |    |
	//	+-------------------------+----+
	//	                      ^      ^
	//	                      |      |
	//	  screen display------+      +----> stride

	// Your width needs to be yres + stride (stride = line_length - yres)
	// To get line_length, you need to call FBIOGET_FSCREENINFO, like this:
	// adb shell ioctl -rl 50 /dev/graphics/fb0 17922
	// and then extract the values

	std::wcout << "Converting fb frame to image file\n";
	STARTUPINFO startupinfo;
	PROCESS_INFORMATION processinfo;
	std::string cmdp1 = "ffmpeg -vframes 1 -f rawvideo -pix_fmt rgba -s";
	std::string cmdp2 = "-i fb0 screenshot.png";
	std::string fcmd;
	fcmd = cmdp1.append(" " + str2int(w)).append("x" + str2int(h)).append(" ").append(cmdp2);
	LPTSTR szCmdLine = _tcsdup(TEXT(fcmd.c_str()));
	ZeroMemory(&startupinfo, sizeof(startupinfo));
	startupinfo.cb = sizeof(startupinfo);
	ZeroMemory(&processinfo, sizeof(processinfo));
	HRESULT rv = CreateProcess(NULL, szCmdLine, NULL, NULL, FALSE, 0, NULL, NULL, &startupinfo,&processinfo);
	signed int _rv = static_cast<int>(rv);
	if (rv == 0) {
		// Something bad happened
		std::wcout << "\n\nCall failed. (Error: " << GetLastError() << ")\n";
		std::getwchar();
	}
	else {
		// We're done!
		std::wcout << "Done!\n";

	}


}



int _tmain(int argc, _TCHAR* argv[])
{
	// Check if user has specified an output resolution. If not, then fall back to default
	if (lstrcmpi(argv[1], _T("--resolution")) == 0) {
		width = _tstoi(argv[2]);
		height = _tstoi(argv[3]);
	}

	// Firing up the screenshot procedures!
	// TODO: Remove stupid sleep calls and switch to something better, like WaitForSingleObject
	std::wcout << "***** || ADB Recovery Screenshot || *****\n";
	std::wcout << "-----------------------------------------------------------------------------------------\n\n";
	killAdb(); // Kill ADB server!
	Sleep(5000); // Wait for 5000ms (overkill)
	cpyFrameBuffer(); // Copy framebuffer from device!
	Sleep(20000); // Wait for 20000ms (a bit overkill)
	framebufferToPNG(width, height); // Extract frame and save!
	Sleep(5000); // Wait for 5000ms
	return 0; // Exit
}
