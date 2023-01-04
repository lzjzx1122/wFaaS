/*
    This file used to define the native function hooked by host_interface.h .
*/

#include <cstdint>
#include <vector>
#include <wasm_exec_env.h>
#include <wasm_export.h>

std::vector<int> argLengths;
std::vector<uint8_t*> argCollection;
uint8_t* resultBuffer;
//todo:return size

static int read_input_native(wasm_exec_env_t exec_env , int32_t argIndex, uint8_t* inBuffer, int32_t inLength)
{
    if(argIndex >= argLengths.size()) throw "[Host_Iface_Func] Arg index out of bound";
    if(inLength == 0) return argLengths[argIndex];
    if(inLength != argLengths[argIndex]) throw "[Host_Iface_Func] Wrong arg length";
    uint8_t* tmpPtr = argCollection[argIndex];
    std::copy(tmpPtr, tmpPtr+inLength, inBuffer);
    return inLength;
}

static NativeSymbol ns[] = {
    {
        "_Z10read_inputiPhi", 	    // the name of WASM function name
     	(void *)read_input_native,    // the native function pointer
        "(i*~)i"		           // the function prototype signature
    }
};