# TypeDive: Multi-Layer Type Analysis (MLTA) for Refining Indirect-Call Targets 

This project includes a prototype implementation (TypeDive) of MLTA.
MLTA relies on an observation that function pointers are commonly
stored into objects whose types have a multi-layer type hierarchy;
before indirect calls, function pointers will be loaded from objects
with the same type hierarchy layer by layer.  By matching the
multi-layer types of function pointers and functions, MLTA can
dramatically refine indirect-call targets.  MLTA's approach is highly
scalable (e.g., finishing the analysis of the Linux kernel within
minutes) and does not have false negatives in principle. 


TypeDive has been tested with LLVM 15.0, O0 and O2 optimization
levels, and the Linux kernel. The finally results of TypeDive may
have a few false negatives. Observed causes include hacky code in
Linux (mainly the out-of-bound access from `container_of`), compiler
bugs, and false negatives from the baseline (function-type matching). 


## How to use TypeDive

### Build LLVM 
```sh 
	$ ./build-llvm.sh 
	# The tested LLVM is of commit e758b77161a7 
```

### Build TypeDive 
```sh 
	# Build the analysis pass 
	# First update Makefile to make sure the path to the built LLVM is correct
	$ make 
	# Now, you can find the executable, `kanalyzer`, in `build/lib/`
```
 
### Prepare LLVM bitcode files of OS kernels

* First build IRDumper. Before make, make sure the path to LLVM in
	`IRDumper/Makefile` is correct. It must be using the same LLVM used
	for building TypeDive
* See `irgen.py` for details on how to generate bitcode/IR

### Run TypeDive
```sh
	# To analyze a list of bitcode files, put the absolute paths of the bitcode files in a file, say "bc.list", then run:
	$ ./build/lib/kalalyzer @bc.list
	# Results will be printed out, or can you get the results in map `Ctx->Callees`.
```

### Configurations

* Config options can be found in `Config.h`
```sh
	# If precision is the priority, you can comment out `SOUND_MODE`
	# `SOURCE_CODE_PATH` should point to the source code 
```


## More details
* [The MLTA paper (CCS'19)](https://www-users.cse.umn.edu/~kjlu/papers/mlta.pdf)
```sh
@inproceedings{mlta-ccs19,
  title        = {{Where Does It Go? Refining Indirect-Call Targets with Multi-Layer Type Analysis}},
  author       = {Kangjie Lu and Hong Hu},
  booktitle    = {Proceedings of the 26th ACM Conference on Computer and Communications Security (CCS)},
  month        = November,
  year         = 2019,
  address      = {London, UK},
}
```
