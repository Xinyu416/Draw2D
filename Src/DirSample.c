#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include "DirSample.h"

int ListDir(const char* dir, char* files)
{
	WIN32_FIND_DATAA ffd;  // 使用ANSI版本
	LARGE_INTEGER filesize;
	char szDir[MAX_PATH];
	HANDLE hFind = INVALID_HANDLE_VALUE;
	DWORD dwError = 0;

	uint32_t index = 0;
	// 检查路径是否以反斜杠结尾
	size_t len = strlen(dir);
	if (len > 0 && dir[len - 1] == '\\') {
		snprintf(szDir, MAX_PATH, "%s*", dir);
	}
	else {
		snprintf(szDir, MAX_PATH, "%s\\*", dir);
	}

	printf("搜索路径: %s\n", szDir);

	hFind = FindFirstFileA(szDir, &ffd);
	if (INVALID_HANDLE_VALUE == hFind)
	{
		dwError = GetLastError();
		printf("FindFirstFile失败，错误码: %d\n", dwError);

		// 具体错误信息
		switch (dwError) {
		case 2: printf("错误: 系统找不到指定的文件\n"); break;
		case 3: printf("错误: 系统找不到指定的路径\n"); break;
		case 5: printf("错误: 拒绝访问\n"); break;
		case 32: printf("错误: 进程无法访问文件\n"); break;
		default: printf("未知错误\n");
		}
		return dwError;
	}

	// 列出文件
	do
	{
		if (strcmp(ffd.cFileName, ".") == 0 || strcmp(ffd.cFileName, "..") == 0) {
			continue; // 跳过.和..
		}

		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			//printf("  %s   <DIR>\n", ffd.cFileName);
		}
		else
		{
			filesize.LowPart = ffd.nFileSizeLow;
			filesize.HighPart = ffd.nFileSizeHigh;
			//printf("  %s   %lld bytes\n", ffd.cFileName, filesize.QuadPart);
			strcpy(files + (index * MAX_PATH), ffd.cFileName);
			index++;
		}
	} while (FindNextFileA(hFind, &ffd) != 0);

	dwError = GetLastError();
	if (dwError != ERROR_NO_MORE_FILES)
	{
		printf("遍历文件时出错: %d\n", dwError);
	}

	FindClose(hFind);
	return 0;  // 成功返回0
}

uint32_t _getFilesNum(const char* dir) {
	WIN32_FIND_DATAA ffd;  // 使用ANSI版本
	LARGE_INTEGER filesize;
	char szDir[MAX_PATH];
	HANDLE hFind = INVALID_HANDLE_VALUE;
	DWORD dwError = 0;
	uint32_t filesNum = 0;

	// 检查路径是否以反斜杠结尾
	size_t len = strlen(dir);
	if (len > 0 && dir[len - 1] == '\\') {
		snprintf(szDir, MAX_PATH, "%s*", dir);
	}
	else {
		snprintf(szDir, MAX_PATH, "%s\\*", dir);
	}

	hFind = FindFirstFileA(szDir, &ffd);
	if (INVALID_HANDLE_VALUE == hFind)
	{
		dwError = GetLastError();
		printf("FindFirstFile失败，错误码: %d\n", dwError);

		// 具体错误信息
		switch (dwError) {
		case 2: printf("错误: 系统找不到指定的文件\n"); break;
		case 3: printf("错误: 系统找不到指定的路径\n"); break;
		case 5: printf("错误: 拒绝访问\n"); break;
		case 32: printf("错误: 进程无法访问文件\n"); break;
		default: printf("未知错误\n");
		}
		return dwError;
	}

	// 列出文件
	do
	{
		if (strcmp(ffd.cFileName, ".") == 0 || strcmp(ffd.cFileName, "..") == 0) {
			continue; // 跳过.和..
		}

		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			//printf("  %s   <DIR>\n", ffd.cFileName);
		}
		else
		{
			filesize.LowPart = ffd.nFileSizeLow;
			filesize.HighPart = ffd.nFileSizeHigh;
			filesNum++;
			//printf("  %s   %lld bytes\n", ffd.cFileName, filesize.QuadPart);
		}
	} while (FindNextFileA(hFind, &ffd) != 0);

	dwError = GetLastError();
	if (dwError != ERROR_NO_MORE_FILES)
	{
		printf("遍历文件时出错: %d\n", dwError);
	}

	printf("_getFilesNum::%d\n", filesNum);

	FindClose(hFind);
	return filesNum;
}