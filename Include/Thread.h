#pragma once
#include <windows.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct {
	uint8_t type;
	uint32_t taskIndex;
}Msg;

typedef struct {
	DWORD id;
	HANDLE handle;
	bool isActive;
	uint8_t fromThreadMessage;
	Msg toThreadMessage;
	char* fileNameArr;
	uint32_t fileNum;
}Thread;

