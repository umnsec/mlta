#ifndef _KACONFIG_H
#define _KACONFIG_H


#include "llvm/Support/FileSystem.h"
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <fstream>
#include <map>
#include "Common.h"

using namespace std;
using namespace llvm;

//
// Configurations
//

//#define DEBUG_MLTA

extern int ENABLE_MLTA;
#define SOUND_MODE 1
#define UNROLL_LOOP_ONCE 1

#define MAX_TYPE_LAYER 10
//#define MLTA_FIELD_INSENSITIVE
//#define MAP_CALLER_TO_CALLEE 1

#define MAP_DECLARATION_FUNCTION
#define PRINT_ICALL_TARGET
//#define PRINT_SOURCE_LINE
// Paths of sources
#define LINUX_SOURCE_KASE "/home/kjlu/projects/kernels/linux-5.1"


#endif
