#include "VMManager.h"

MemManager CreateMemManager(uint32_t memSize) {
	MemManager memManager = { .memSize = memSize,.data = (uint8_t*)malloc(sizeof(uint8_t) * memSize) };
	memManager.staticIndex = 0;
	memManager.dynamicIndex = 0;
	return memManager;
}

uint8_t* PushToStaticBlock(MemManager memM, uint32_t elementSize) {
	if (!GetHasPlace(memM, elementSize))
	{
		return NULL;
	}
	uint8_t fill = _getAliagnOffset(memM.staticIndex);
	memM.staticIndex = memM.staticIndex + fill;
	uint8_t* data = memM.data + memM.staticIndex;
	//index偏移
	memM.staticIndex += elementSize;
	return data;
}

uint8_t* PushToDynamicBlock(MemManager memM, uint32_t elementSize) {
	if (!GetHasPlace(memM, elementSize))
	{
		return NULL;
	}
	uint8_t fill = _getAliagnOffset(memM.dynamicIndex);
	//index偏移
	memM.dynamicIndex = memM.dynamicIndex + fill + elementSize;
	uint32_t index = memM.memSize - 1 - memM.dynamicIndex;
	//转为从前往后的index
	uint8_t* data = memM.data + index;
	return data;
}

uint8_t _getAliagnOffset(uint32_t index) {
	//求字节对齐（最大字节数）
	uint8_t maxByte = 4;
	uint32_t count = index + 1;
	uint8_t mod = count % maxByte;
	uint8_t offset = (mod == 0) ? 0 : (maxByte - mod);
	return offset;
}

bool GetHasPlace(MemManager memM, uint32_t elementSize) {
	uint32_t count = memM.staticIndex + memM.dynamicIndex + 2 + elementSize;
	if (count < memM.memSize)
	{
		return true;
	}
	return false;
}










































VMManager CreateManager(uint32_t totalSize, uint32_t staticDataSize, uint32_t dynamicDataSize) {
	VMManager vmM = { .totalSize = totalSize,.data = (uint8_t*)malloc(sizeof(uint8_t) * totalSize),.staticCount = 0,.dynamicCount = 0 };
	//设置首元素地址
	vmM.staticData = vmM.data;
	vmM.dynamicData = vmM.data + (totalSize - dynamicDataSize);
	return vmM;
}

void SetDynamicData(VMManager* vmM, void* dynamicElement) {
	if (!GetHasPlace_Dynamic(vmM))
	{
		//空间不足
		printf("SetDynamicData::NoPlace!\n");
		return;
	}
	vmM->dynamicCount = vmM->dynamicCount + 1;
	uint32_t dynamicDataLen = vmM->dynamicCount * vmM->dynamicDataSize;
	uint32_t offset = vmM->totalSize - dynamicDataLen;
	vmM->dynamicData = vmM->data + offset;
	memcpy(vmM->dynamicData, dynamicElement, vmM->dynamicDataSize);
}

void SetStaticData(VMManager* vmM, void* staticElement) {
	if (!GetHasPlace_Static(vmM))
	{
		return;
	}
	uint32_t offset = (vmM->staticCount - 1) * vmM->staticDataSize;
	memcpy(vmM->staticData + offset, staticElement, vmM->staticDataSize);
}

uint8_t* GetDynamicData(VMManager* vmM, uint32_t index) {
	if (!vmM->dynamicCount > 0)
	{
		return NULL;
	}
	uint32_t offset = vmM->dynamicCount - 1 - index;
	uint8_t* data = vmM->dynamicData + offset;
	return data;
}

uint8_t* GetStaticData(VMManager* vmM, uint32_t index) {
	if (!vmM->staticCount > 0)
	{
		return NULL;
	}
	uint8_t* data = vmM->staticData + (vmM->staticDataSize * index);
	return data;
}

bool GetHasPlace_Dynamic(VMManager* vmM) {
	//判断下一个元素位置是否足够
	uint32_t dynamicNextCount = vmM->dynamicCount + 1;

	uint32_t dataSize = vmM->dynamicDataSize * dynamicNextCount + vmM->staticCount * vmM->staticDataSize;
	if (dataSize < vmM->totalSize)
	{
		//有空间可以存放数据
		return true;
	}
	//没空间放数据
	return false;
}

bool GetHasPlace_Static(VMManager* vmM) {
	uint32_t staticNextCount = vmM->staticCount + 1;
	uint32_t dataSize = vmM->staticDataSize * staticNextCount + vmM->dynamicCount * vmM->dynamicDataSize;
	if (dataSize < vmM->totalSize)
	{
		//有空间可以存放数据
		return true;
	}
	//没空间放数据
	return false;
}

void ClearDynamicData(VMManager* vmM) {
	vmM->dynamicCount = 0;
}

void ClearStaticData(VMManager* vmM) {
	vmM->staticCount = 0;
}

void PrintManager(VMManager* vmM) {
	printf("vmM:data:%p,staticData:%p,dynamicData:%p,staticCount:%d,dynamicCount:%d,totalSize:%d\n", vmM->data, vmM->staticData, vmM->dynamicData, vmM->staticCount, vmM->dynamicCount, vmM->totalSize);
}
