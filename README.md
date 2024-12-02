# Sample for checking memory usage of TensorRT Inference

## How to check the memory usage 
This sample is based on the original trtexec sample which is available in Drive OS SDK 6.0.5.0, class memoryConsumption was added to check the memory consumption by calculating the difference of pages_free in "/proc/vm/stats" and CPUMemUsed using mallinfo() before and after some API calls.
- Checking "pages_free" item in "/proc/vm/stats" could get the current MemAvailable.
- Checking space in use by calling [mallinfo()]{https://www.qnx.com/developers/docs/7.1/#com.qnx.doc.neutrino.lib_ref/topic/m/mallinfo.html} could get the CPUMemUsed for the current process
Then we got
- Total_Consumed_Mem = Pre_MemAvailable - Post_MemAvailable
- CPU_Consumed_Mem = Pre_CPUMemUsed - Post_CPUMemUsed 
- Device_Consumed_Mem = Total_Consumed_Mem - CPU_Consumed_Mem

Note that Device_Consumed_Mem is the sum of GPU and other NV hardware engine like VIC/PVA Memory Usage

You may check the diff with
```
$ git log
$ git show HEAD
```

## Step to run this sample

### 1. Generate the TensorRT Engine, run below cmd line 
```
	# ./trtexec --onnx=ResNet50.onnx  --fp16 --int8 --saveEngine=./ResNet50GPU.engine 
```

### 2. Compile the trtexec on x86 host 
```
        $ export SDP_BASE=Path_to_qnx_toolchain/
	$ export QNX_HOST=$SDP_BASE/host/linux/x86_64
	$ export QNX_TARGET=$SDP_BASE/target/qnx7/
	$ export QNX_TOOLCHAIN=$SDP_BASE
	$ export QNX_VERSION=7.1.0
	$ export CUDA_INSTALL_DIR=/usr/local/cuda-safe-11.4
	$ export PATH=$QNX_HOST/usr/bin/:$PATH
	$ export HOST_COMPILER=$QNX_HOST/usr/bin/aarch64-unknown-nto-qnx7.1.0-g++
	$ make TARGET=qnx clean
	$ make TARGET=qnx 
```

### 4. Check the logs of memConsumption
For this test with ResNet50GPU.engine, got below prints:
```
# ./trtexec --loadEngine=../resnet50.bin  
&&&& RUNNING TensorRT.trtexec [TensorRT v8412] # ./trtexec --loadEngine=../resnet50.bin
----------------Start to narrow down the mem usage for engine creation---------------
pages_free=0x6469f7 (25.102GB)
CpuMemUsed is 33 MB

----------------InferRuntime Creation---------------
pages_free=0x626168 (24.594GB)
CpuMemUsed is 307 MB
Consumed mem size during [InferRuntime Creation]: 520 MB	CPU Mem: 274 MB	GPU Mem: 246 MB	 

----------------Engine deserialization---------------
pages_free=0x625509 (24.583GB)
CpuMemUsed is 311 MB
Consumed mem size during [Engine deserialization]: 12 MB	CPU Mem: 4 MB	GPU Mem: 8 MB	 
Consumed mem size in total: 532 MB	CPU Mem in total: 278 MB	GPU Mem in total: 254 MB	 

----------------Start to narrow down the mem usage after engine creation ---------------
pages_free=0x625509 (24.583GB)
CpuMemUsed is 311 MB

----------------ExecutionContext Creation---------------
pages_free=0x6254bb (24.582GB)
CpuMemUsed is 312 MB
Consumed mem size during [ExecutionContext Creation]: 1 MB	CPU Mem: 1 MB	GPU Mem: 0 MB	 

----------------Bindings buffer filling---------------
pages_free=0x6252b1 (24.580GB)
CpuMemUsed is 312 MB
Consumed mem size during [Bindings buffer filling]: 2 MB	CPU Mem: 0 MB	GPU Mem: 2 MB	 

----------------Run inferences after setupInference---------------
pages_free=0x62524a (24.580GB)
CpuMemUsed is 312 MB
Consumed mem size during [Run inferences after setupInference]: 0 MB	CPU Mem: 0 MB	GPU Mem: 0 MB	 
Consumed mem size in total: 3 MB	CPU Mem in total: 1 MB	GPU Mem in total: 2 MB	 

&&&& PASSED TensorRT.trtexec [TensorRT v8412] # ./trtexec --loadEngine=../resnet50.bin```
```
From these logs, we could get the memory consumoption of 
	- Breakdown the memory usage of setup inference:
		- creating InferRuntime: 520 MB 
		- deserilizing engine: 12 MB 
		- creating ExecutionContext: 1 MB
		- filling bindings buffers: 2 MB
	- Inferences: 0 MB

So most of memory (520 MB) was consumed by the first time of InferRuntime Creating. 
Note that the creation of second InferRuntime in same process will reuse the initialized CUDA/cuDNN/cuBLAS context, so it won't cost much memory as the first creation.
