#include "containers.h"
#include <time.h> 

/// <summary>
/// 创建数组
/// </summary>
/// <param name="elementSize"></param>
/// <returns></returns>
Array ArrayCreate(uint32_t elementSize) {
	size_t count = 10;
	Array arr = { .data = (uint8_t*)malloc(elementSize * count),.dataTemp = (uint8_t*)malloc(elementSize * 1),.elementSize = elementSize,.length = 0,.size = count * elementSize };
	return arr;
}

/// <summary>
/// 数组释放
/// </summary>
/// <param name="arr"></param>
void ArrayRelease(Array* arr) {

	arr->length = 0;
	arr->size = 0;
	free(arr->data);
	arr->data = NULL;
}

/// <summary>
/// 数组扩容
/// </summary>
/// <param name="arr">源数组</param>
/// <returns>扩容后的数组</returns>
Array* ArrayResize(Array* arr) {
	uint8_t* newContainer = (uint8_t*)malloc(arr->size * 2);
	memcpy(newContainer, arr->data, arr->size);
	free(arr->data);
	arr->data = newContainer;
	arr->size = arr->size * 2;
	return arr;
}

/// <summary>
/// 数组打印
/// </summary>
/// <param name="arr"></param>
void PrintArray(Array* arr) {

	printf("\nContainer size:%d , length:%d, elementSize:%dB,useByte:%dB,totalByte:%dB\n", arr->size, arr->length, arr->elementSize, arr->length * arr->elementSize, arr->size * arr->elementSize);
	printf("Elements:");
	for (uint32_t i = 0; i < arr->length; i++)
	{
		printf("   ");
		uint8_t* element = arr->data + (i * arr->elementSize);
		for (uint32_t j = 0; j < arr->elementSize; j++) {
			printf("%x", element[j]);
		}
	}
}

/// <summary>
/// 在数组末尾新增元素
/// </summary>
/// <param name="arr">源数组</param>
/// <param name="element">新增的元素</param>
void ArrayPush(Array* arr, void* element) {
	if (((arr->length + 1) * arr->elementSize > arr->size)) {
		arr = ArrayResize(arr);
	}

	memcpy(arr->data + (arr->length * arr->elementSize), element, arr->elementSize);

	arr->length = arr->length + 1;
}

/// <summary>
/// 获取数组元素
/// </summary>
/// <param name="arr">源数组</param>
/// <param name="index">元素下标</param>
/// <returns></returns>
uint8_t* GetArrayElementByIndex(Array* arr, uint32_t index) {

	return arr->data + (index * arr->elementSize);
}

/// <summary>
/// 插入数组元素
/// </summary>
/// <param name="arr">源数组</param>
/// <param name="element">插入的元素</param>
/// <param name="index">插入的位置</param>
void ArrayInsert(Array* arr, void* element, uint32_t index) {
	if (((arr->length + 1) * arr->elementSize > arr->size)) {
		//元素占用总空间 > 容器总空间 需要扩容
		arr = ArrayResize(arr);
	}
	//从数组最后一个元素开始挪动
	for (uint32_t i = arr->length; i > index; i--)
	{
		memcpy(arr->data + (i * arr->elementSize), arr->data + ((i - 1) * arr->elementSize), arr->elementSize);
	}
	memcpy(arr->data + (index * arr->elementSize), element, arr->elementSize);
	arr->length = arr->length + 1;
	
}

/// <summary>
/// 删除数组元素
/// </summary>
/// <param name="arr">源数组</param>
/// <param name="index">删除的元素下标</param>
void ArrayDelete(Array* arr, uint32_t index) {

	for (uint32_t i = index; i < arr->length; i++) {

		memcpy(arr->data + (i * arr->elementSize), arr->data + ((i + 1) * arr->elementSize), arr->elementSize);
	}
	arr->length = arr->length - 1;
}

/// <summary>
/// 获取数组元素数量
/// </summary>
/// <param name="arr"></param>
/// <returns></returns>
uint32_t GetArrayElementCount(Array* arr) {

	return arr->length;
}

/// <summary>
/// 交换数组元素
/// </summary>
/// <param name="arr"></param>
/// <param name="index1"></param>
/// <param name="index2"></param>
void ArrayExchange(Array* arr, int32_t index1, int32_t index2) {
	//uint8_t* temp = (uint8_t*)malloc(sizeof(uint8_t) * arr->elementSize);
	uint8_t* temp = arr->dataTemp;
	memcpy(temp, arr->data + (index1 * arr->elementSize), arr->elementSize);
	memcpy(arr->data + (index1 * arr->elementSize), arr->data + (index2 * arr->elementSize), arr->elementSize);
	memcpy(arr->data + (index2 * arr->elementSize), temp, arr->elementSize);
	//free(temp);
}

/// <summary>
/// 数组分区
/// </summary>
/// <param name="arr"></param>
/// <param name="startIndex"></param>
/// <param name="endIndex"></param>
/// <param name="compareFunc"></param>
/// <returns></returns>
int32_t ArrayPartition(Array* arr, int32_t startIndex, int32_t endIndex, bool (*compareFunc)(void*, void*)) {
	//数组最后一个元素 基准值
	uint8_t* x = arr->data + (endIndex * arr->elementSize);
	uint8_t* valj;
	int32_t i = startIndex - 1;
	for (int32_t j = startIndex; j < endIndex; j++)
	{
		valj = arr->data + (j * arr->elementSize);
		if (compareFunc(valj, x)) {

			i++;
			ArrayExchange(arr, i, j);
		}
	}
	i++;
	ArrayExchange(arr, i, endIndex);
	return i;
}

/// <summary>
/// 数组快速排序
/// </summary>
/// <param name="arr"></param>
/// <param name="startIndex"></param>
/// <param name="endIndex"></param>
/// <param name="compareFunc"></param>
void ArrayQuickSort(Array* arr, int32_t startIndex, int32_t endIndex, bool (*compareFunc)(void*, void*)) {

	if (startIndex >= endIndex)return;
	int32_t q = ArrayPartition(arr, startIndex, endIndex, compareFunc);
	ArrayQuickSort(arr, startIndex, (q - 1), compareFunc);
	ArrayQuickSort(arr, (q + 1), endIndex, compareFunc);
}

/// <summary>
/// 普通数据类型的交换
/// </summary>
/// <param name="arr"></param>
/// <param name="index1"></param>
/// <param name="index2"></param>
void Exchange(uint32_t* arr, int32_t index1, int32_t index2) {
	uint32_t temp = arr[index1];
	arr[index1] = arr[index2];
	arr[index2] = temp;
}

/// <summary>
/// 普通数组数据类型分区
/// </summary>
/// <param name="arr"></param>
/// <param name="p"></param>
/// <param name="r"></param>
/// <returns></returns>
int32_t Partition(uint32_t* arr, int32_t p, int32_t r) {

	int32_t x = arr[r];
	int32_t i = p - 1;
	for (int32_t j = p; j < r; j++)
	{
		if (arr[j] <= x) {

			i++;
			Exchange(arr, i, j);
		}
	}

	i++;
	Exchange(arr, i, r);
	return i;
}

/// <summary>
/// 快速排序算法
/// </summary>
/// <param name="arr"></param>
/// <param name="p"></param>
/// <param name="r"></param>
void QuickSort(uint32_t* arr, int32_t p, int32_t r) {

	if (p >= r)return;
	int32_t q = Partition(arr, p, r);
	QuickSort(arr, p, (q - 1));
	QuickSort(arr, (q + 1), r);
}

void TestQuickSort() {
	//快速排序一般数据类型测试
	//uint32_t testArr[] = { 8,6,7,4,1,3,9,5,2 };
	//uint32_t* testArr = (uint32_t*)malloc(sizeof(uint32_t) * 8);
	clock_t t = clock();
	uint32_t* testArr = (uint32_t*)malloc(sizeof(uint32_t) * 10000000);
	testArr[0] = 2;
	testArr[1] = 8;
	testArr[2] = 7;
	testArr[3] = 1;
	testArr[4] = 3;
	testArr[5] = 5;
	testArr[6] = 6;
	testArr[7] = 4;
	for (size_t i = 0; i < 10000000; i++)
	{
		testArr[i] = rand();
	}
	int32_t p = 0;
	int32_t r = 10000000 - 1;

	/*数组创建计时*/
	t = clock() - t;
	printf("\n ArrayCreate Time: %d clicks (%f seconds).\n", t, ((float)t) / CLOCKS_PER_SEC);
	QuickSort(testArr, p, r);

	t = clock() - t;
	printf("\n ArraySort Time: %d clicks (%f seconds).\n", t, ((float)t) / CLOCKS_PER_SEC);
	printf("\n");
	for (size_t k = 0; k < 10000000; k++)
	{
		printf("%d,", testArr[k]);
	}
	free(testArr);
}

/// <summary>
/// 比较自定义类型函数
/// </summary>
/// <param name="e1"></param>
/// <param name="e2"></param>
/// <returns></returns>
bool myCompareFuncUint16(void* e1, void* e2) {
	uint16_t e1val = *((uint16_t*)e1);
	uint16_t e2val = *((uint16_t*)e2);
	//printf("\ne1val：%d,e2val:%d\n", e1val, e2val);
	return e1val <= e2val;
}
bool myCompareSize_t(void* e1, void* e2) {
	size_t e1val = *((size_t*)e1);
	size_t  e2val = *((size_t*)e2);
	//printf("\ne1val：%d,e2val:%d\n", e1val, e2val);
	return e1val <= e2val;
}
bool myCompareTestContainer(void* e1, void* e2) {
	TestContainer e1val = *((TestContainer*)e1);
	TestContainer  e2val = *((TestContainer*)e2);
	//printf("\ne1val：%d,e2val:%d\n", e1val, e2val);
	return e1val.number0 <= e2val.number0;
}

void TestFuncPtr() {
	/*函数指针类型测试*/
	/*声明myFunction 函数变量*/
	bool (*myFunction)(void*, void*) = myCompareFuncUint16;
	/*通过函数变量形式调用函数*/
	myFunction(123, 567);
}

void TestCustomArray() {
	Array arr = ArrayCreate(2);
	PrintArray(&arr);
	uint16_t e0 = 0xddff;
	uint16_t e1 = 0xeeee;
	uint16_t e2 = 0xe59d;
	uint16_t e3 = 0xaaaa;
	uint16_t e4 = 0xbbbb;
	uint16_t e5 = 0xcccc;
	uint16_t e6 = 0xdddd;
	uint16_t e7 = 0xabcd;
	uint16_t e8 = 0xaabb;
	uint16_t e9 = 0xccdd;
	uint16_t e10 = 0xffff;
	ArrayPush(&arr, &e0);
	ArrayPush(&arr, &e1);
	ArrayPush(&arr, &e2);
	ArrayPush(&arr, &e3);
	ArrayPush(&arr, &e4);
	ArrayInsert(&arr, &e5, 0);
	ArrayInsert(&arr, &e6, 5);
	PrintArray(&arr);

	/*uint16_t* e3 = (uint16_t*)GetArrayElementByIndex(&arr,0);
	printf("e3:%x\n",*e3);*/

	uint16_t* insertElement1 = 0x3333;
	ArrayInsert(&arr, &insertElement1, 1);
	PrintArray(&arr);

	ArrayDelete(&arr, 1);
	PrintArray(&arr);

	uint16_t* insertElement2 = 0xffff;
	ArrayInsert(&arr, &insertElement2, 1);
	PrintArray(&arr);

	ArrayDelete(&arr, 2);
	PrintArray(&arr);

	/*ArrayExchange(&arr, 0, 1);
	PrintArray(&arr);*/

	/*用快速排序重排数组*/
	//ArrayPartition(&arr,0, GetArrayElementCount(&arr)-1, myCompareFunc);
	ArrayQuickSort(&arr, 0, GetArrayElementCount(&arr) - 1, myCompareFuncUint16);
	PrintArray(&arr);

	ArrayRelease(&arr);
}

void TestCustomArray2() {
	Array arr = ArrayCreate(sizeof(size_t));
	size_t randomNumber = 0;
	size_t e = 0;
	clock_t t = clock();
	for (size_t i = 0; i < 10000000; i++)
	{
		e = rand();
		ArrayPush(&arr, &e);
	}
	/*数组创建计时*/
	t = clock() - t;
	printf("\n ArrayCreate Time: %d clicks (%f seconds).\n", t, ((float)t) / CLOCKS_PER_SEC);
	ArrayQuickSort(&arr, 0, GetArrayElementCount(&arr) - 1, myCompareSize_t);

	t = clock() - t;
	printf("\nArrayQuickSort Time: %d clicks (%f seconds).\n", t, ((float)t) / CLOCKS_PER_SEC);
	ArrayRelease(&arr);
}


void TestCustomArrayTestContainer() {
	printf("sizeof(TestContainer):%d\n", sizeof(TestContainer));
	Array arr = ArrayCreate(sizeof(TestContainer));
	size_t randomNumber = 0;
	TestContainer e = { 0 };
	clock_t t = clock();
	for (size_t i = 0; i < 10000000; i++)
	{
		e.number0 = rand();
		ArrayPush(&arr, &e);
	}
	t = clock() - t;
	printf("\nIt took me %d clicks (%f seconds).\n", t, ((float)t) / CLOCKS_PER_SEC);

	ArrayQuickSort(&arr, 0, GetArrayElementCount(&arr) - 1, myCompareTestContainer);
	t = clock() - t;
	printf("\nIt took me %d clicks (%f seconds).\n", t, ((float)t) / CLOCKS_PER_SEC);
	ArrayRelease(&arr);
}
