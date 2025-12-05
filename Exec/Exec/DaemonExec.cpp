#include "DaemonExec.h"
#include "DaemonKernelFactory.h"

using SEF::Exec::DaemonExec;

DaemonExec::DaemonExec() 
{

}

DaemonExec::~DaemonExec() 
{

}

Kernel* DaemonExec::createExecKernel()
{
	KernelFactory* kernelFactory = new DaemonKernelFactory();
	kernel = &kernelFactory->createKernel();
	return kernel;
}

vector<Element*> DaemonExec::createExecElementsByConfig(string configString) 
{
	return elements;
}

void DaemonExec::RunExec()
{

}
