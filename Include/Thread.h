#pragma once
#include <windows.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "Define.h"
#include "Mesh.h"

typedef struct {
	uint8_t type;
	uint32_t taskIndex;
}Msg;

typedef struct {
	Quad* taskArr;
	uint32_t* taskCount;
}Task;

typedef struct {
	Quad* resultArr;
	uint32_t* resultCount;
}Result;

typedef struct {
	DWORD id;
	HANDLE handle;
	bool isActive;
	uint8_t fromThreadMessage;
	Msg toThreadMessage;
	char* fileNameArr;
	uint32_t fileCount;
}Thread;

typedef struct {
	DWORD id;
	HANDLE handle;
	bool isActive;
	uint8_t fromThreadMessage;
	Msg toThreadMessage;
	Task task;
	Result result;

}MyThread;

