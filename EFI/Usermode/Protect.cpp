#include "Protect.h"

int funcCount = 0;
ProtectedFunction functions[50];
// Hmu on discord (Chase.#1803) if you need any help :)
void unsafe_unprotect(int index) {
	//XOR((BYTE*)functions[index].address, functions[index].size, functions[index].lastXor);
}
// Hmu on discord (Chase.#1803) if you need any help :)
void nextLastXor(int index) {
	BYTE xorByte = functions[index].lastXor;
	if (xorByte > 0xf3) {
		xorByte = 0x5;
	}
	xorByte += 0x01;
	functions[index].lastXor = xorByte;
}
// Hmu on discord (Chase.#1803) if you need any help :)
void unsafe_protect(int index) {
	nextLastXor(index);
	unsafe_unprotect(index);
}
// Hmu on discord (Chase.#1803) if you need any help :)
int GetFunctionIndex(void* FunctionAddress) {
	for (int i = 0; i < funcCount; i++) {
		if ((uintptr_t)functions[i].address <= (uintptr_t)FunctionAddress &&
			(uintptr_t)functions[i].address + functions[i].size >= (uintptr_t)FunctionAddress) {
			return i;
		}
	}
	return -1;
}
// Hmu on discord (Chase.#1803) if you need any help :)
void Unprotect(void* FunctionAddress) {
	int function = GetFunctionIndex(FunctionAddress);

	if (function > -1 && functions[function].crypted == true) {
		unsafe_unprotect(function);
		functions[function].crypted = false;
	}
}
// Hmu on discord (Chase.#1803) if you need any help :)
void Protect(void* FunctionAddress) {
	int function = GetFunctionIndex(FunctionAddress);
	if (function > -1 && functions[function].crypted == false) {
		unsafe_protect(function);
		functions[function].crypted = true;
	}
}
// Hmu on discord (Chase.#1803) if you need any help :)
void ProtectedSleep(int ms) {
	Protect(_ReturnAddress());
	Sleep(ms);
	Unprotect(_ReturnAddress());
}
// Hmu on discord (Chase.#1803) if you need any help :)
void addFunc(ProtectedFunction func) {
	functions[funcCount] = func;
	funcCount++;
}
// Hmu on discord (Chase.#1803) if you need any help :)
void XOR(BYTE* data, size_t size, BYTE XOR_KEY) {
	for (size_t i = 0; i < size; i++) {
		data[i] = data[i] ^ XOR_KEY;
	}
}