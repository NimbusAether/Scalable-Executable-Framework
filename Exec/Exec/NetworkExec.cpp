#include "NetworkExec.h"
#include "NetworkKernelFactory.h"

using SEF::Exec::NetworkExec;

NetworkExec::NetworkExec() 
{

}

NetworkExec::~NetworkExec() 
{

}

Kernel* NetworkExec::createExecKernel() 
{
	KernelFactory* kernelFactory = new NetworkKernelFactory();
	kernel = &kernelFactory->createKernel();
	return kernel;
}

vector<Element*> NetworkExec::createExecElementsByConfig(const string& configString)
{
	return elements;
}

void NetworkExec::RunExec() 
{

}

