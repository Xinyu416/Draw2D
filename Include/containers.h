#pragma once

#ifndef __CONTAINERS__
#define __CONTAINERS__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

//typedef bool (*CompareFunc)(void*, void*);


typedef struct array {
	uint8_t* data;//数组元素
	uint8_t elementSize;//数组元素所占字节
	uint8_t* dataTemp;//数据交换临时空间
	uint8_t block[7];
	uint32_t length;//数组元素长度
	uint32_t size;//数组容器长度
}Array;

typedef struct {
	uint32_t maxSize;
	uint8_t* pop;
	uint8_t* push;
	uint8_t* data;
}Queue;

typedef struct testContainer {
	size_t number0;
	size_t number1;
	size_t number2;
	size_t number3;
	size_t number4;
	size_t number5;
	size_t number6;
	size_t number7;
	size_t number8;
	size_t number9;
}TestContainer;

Array ArrayCreate(uint32_t elementSize);

void ArrayRelease(Array* arr);

Array* ArrayResize(Array* arr);

void PrintArray(Array* arr);

void ArrayPush(Array* arr, void* element);

void* GetArrayElementByIndex(Array* arr, uint32_t index);

void ArrayInsert(Array* arr, void* element, uint32_t index);

/// <summary>
/// 数组元素删除
/// </summary>
/// <param name="arr"></param>
/// <param name="index"></param>
void ArrayDelete(Array* arr, uint32_t index);

/// <summary>
/// 获取数组元素个数
/// </summary>
/// <param name="arr"></param>
/// <returns></returns>
uint32_t GetArrayElementCount(Array* arr);

void ArrayExchange(Array* arr, int32_t index1, int32_t index2);

int32_t ArrayPartition(Array* arr, int32_t startIndex, int32_t endIndex, bool (*compareFunc)(void*, void*));

void ArrayQuickSort(Array* arr, int32_t startIndex, int32_t endIndex, bool (*compareFunc)(void*, void*));

/// <summary>
/// 数组元素交换
/// </summary>
/// <param name="arr"></param>
/// <param name="index1"></param>
/// <param name="index2"></param>
void Exchange(uint32_t* arr, int32_t index1, int32_t index2);

/// <summary>
/// 数组分区
/// </summary>
/// <param name="arr"></param>
/// <param name="p"></param>
/// <param name="r"></param>
/// <returns></returns>
int32_t Partition(uint32_t* arr, int32_t p, int32_t r);

/// <summary>
/// 快速排序算法
/// </summary>
void QuickSort(uint32_t* arr, int32_t p, int32_t r);


#endif