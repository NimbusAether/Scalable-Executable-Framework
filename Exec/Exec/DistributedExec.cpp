#include "DistributedExec.h"
#include "DistributedKernelFactory.h"

using SEF::Exec::DistributedExec;

DistributedExec::DistributedExec() 
{

}

DistributedExec::~DistributedExec() 
{

}

Kernel* DistributedExec::createExecKernel() 
{
	KernelFactory* kernelFactory = new DistributedKernelFactory();
	kernel = &kernelFactory->createKernel();
	return kernel;
}

vector<Element*> DistributedExec::createExecElementsByConfig(const string& configString)
{
	return elements;
}

void DistributedExec::RunExec()
{

}