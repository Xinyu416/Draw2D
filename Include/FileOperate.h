#include <windows.h>

inline void getDirFileNames(const char* dir, char* outFileNames)
{
	WIN32_FIND_DATAA ffd;
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
	printf("dir:%s\n", szDir);
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
		}
		else
		{
			strcpy(outFileNames + (index * MAX_PATH), dir);
			strcpy(outFileNames + (index * MAX_PATH + len), "\\");
			strcpy(outFileNames + (index * MAX_PATH + (len + 1)), ffd.cFileName);
			index++;
		}
	} while (FindNextFileA(hFind, &ffd) != 0);

	dwError = GetLastError();
	if (dwError != ERROR_NO_MORE_FILES)
	{
		printf("遍历文件时出错: %d\n", dwError);
	}

	FindClose(hFind);
	return 0;
}

inline uint32_t getDirFileCount(const char* dir) {
	WIN32_FIND_DATAA ffd;
	LARGE_INTEGER filesize;
	char szDir[MAX_PATH];
	HANDLE hFind = INVALID_HANDLE_VALUE;
	DWORD dwError = 0;
	uint32_t outFilesNum = 0;

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
		}
		else
		{
			outFilesNum++;
		}
	} while (FindNextFileA(hFind, &ffd) != 0);

	dwError = GetLastError();
	if (dwError != ERROR_NO_MORE_FILES)
	{
		printf("遍历文件时出错: %d\n", dwError);
	}

	FindClose(hFind);
	return outFilesNum;
}