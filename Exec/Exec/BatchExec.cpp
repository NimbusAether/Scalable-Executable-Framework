#include "BatchExec.h"
#include "BatchKernelFactory.h"

using SEF::Exec::BatchExec;

BatchExec::BatchExec() 
{

}

BatchExec::~BatchExec() 
{

}

Kernel* BatchExec::createExecKernel() 
{
	KernelFactory* kernelFactory = new BatchKernelFactory();
	kernel = &kernelFactory->createKernel();
	return kernel;
}

vector<Element*> BatchExec::createExecElementsByConfig(string configString) 
{
	return elements;
}

void BatchExec::RunExec() 
{

}
