4.2 内存管理
4.2.1 内存分配和释放
cudaErrot_t cudaMalloc(void **devPtr, size_t count)
这个函数在设备上分配了count字节的全局内存，并用devPtr指针返回该内存的地址。
所分配的内存支持任何变量类型，包括整型、浮点类型变量、布尔类型等。
如果cudaMalloc函数执行失败则返回cudaErrorMemoryAllocation。
在已分配的全局内存中的值不会被清除，需用从主机上传输的数据来填充所分配的全局内存，或用cudaError_t cudaMemset(void *devPtr, int value, size_t count)初始化，这个函数用存储在变量value中的值来填充从设备内存地址devPtr处开始的count字节。
一旦不再使用已分配的全局内存，用cudaError_t cudaFree(void *devPtr)释放该内存空间。
该函数释放了devPtr指向的全局内存，该内存必须在此前使用一个设备分配函数来进行分配。否则，它将返回一个错误cudaErrorInvalidDevicePointer。如果地址空间已经被释放，那么cudaFree也返回一个错误。
设备内存的分配和释放操作成本较高，所以应用程序应重利用设备内存，以减少对整体性能的影响。
4.2.2 内存传输
从主机向设备传输数据
cudaError_t cudaMemcpy(void *dst, const void *src, size_t count, enum cudaMemcpyKind kind)
这个函数从内存位置src复制了count字节到内存位置dst。
变量kind有cudaMemcpyHostToHost, cudaMemcpyHostToDevice, cudaMemcpyDeviceToHost, cudaMemcpyDeviceToDevice。
memTransfer.cu
HtoD 9.8485ms
DtoH 9.7895ms
从主机到设备的数据传输用HtoD来标记，从设备到主机用DtoH来标记。
GPU芯片和板载GDDR5 GPU内存之间的理论峰值带宽非常高。
CPU和GPU之间通过PCIe Gen2总线相连，这种连接的理论带宽要低的多。
CUDA编程的一个基本原则应是尽可能地减少主机与设备之间的传输。
4.2.3 固定内存
分配的主机内存默认是pageable（可分页），GPU不能在可分页主机内存上安全地访问数据。
当从可分页主机内存传输数据到设备内存时，CUDA驱动程序首先分配临时页面锁定的或固定的主机内存，将主机源数据复制到固定内存中，然后从固定内存传输数据给设备内存。
直接分配固定主机内存cudaError_t cudaMallocHost(void **devPtr, size_t count)
这个函数分配了count字节的主机内存，这些内存是页面锁定的并且对设备来说是可访问的。
由于固定内存能被设备直接访问，所以它能用比可分页内存高得多的带宽进行读写。
分配过多的固定内存可能会降低主机系统的性能，因为它减少了用于存储虚拟内存数据的可分页内存的数量。
固定主机内存必须用cudaFreeHost(void *ptr)来释放。
pinMemTransfer.cu
HtoD 9.7735ms
DtoH 9.7290ms
与可分页内存相比，固定内存的分配和释放成本更高，但是它为大规模数据传输提供了更高的传输吞吐量。
固定内存获得的加速取决于设备计算能力。
将许多小的传输批处理为一个更大的传输能提高性能，它减少了单位传输消耗。
4.2.4 零拷贝内存
通常，主机不能直接访问设备变量，同时设备也不能之间访问主机变量。主机和设备都可以访问零拷贝内存。
使用零拷贝内存来共享主机和设备间的数据时，必须同步主机和设备间的内存访问。
零拷贝内存是固定（不可分页）内存，该内存映射到设备地址空间中。
cudaError_t cudaHostAlloc(void **pHost, size_t count, unsigned int flags)
这个函数分配了count字节的主机内存，该内存是页面锁定的且设备可访问的。
必须用cudaFreeHost函数释放。
flags可取
cudaHostAllocDefault使cudaHostAlloc函数的行为与cudaMallocHost函数一致。
cudaHostAllocPortable可以返回能被所有CUDA上下文使用的固定内存，而不仅是执行内存分配的那一个。
cudaHostAllocWriteCombined返回写结合内存，该内存可以在某些系统配置上通过PCIe 总线上更快地传输，但它在大多数主机上不能被有效地读取。
cudaHostAllocMapped标志返回，可以实现主机写入和设备读取被映射到设备地址空间中的主机内存。
cudaError_t cudaHostGetDevicePointer(void **pDevice, void *pHost, unsigned int flags)
该函数返回了一个在pDevice中的设备指针，该指针可以在设备上被引用以访问映射得到的固定主机内存。
sumArrayZerocopy.cu
DtoH 3.9040us
HtoD 3.3920us
