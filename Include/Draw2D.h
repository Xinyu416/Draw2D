#pragma once
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include "GameEngine.h"

void AppLoop(HWND hwnd);
void AppRelease();
HWND CreateRenderWindow(uint32_t width, uint32_t height);
void SendBufferToDisplay(HWND hwnd, BITMAPINFO bmi, uint8_t* frameBuffer);