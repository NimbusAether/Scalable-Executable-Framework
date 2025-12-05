#include "InteractiveExec.h"
#include "InteractiveKernelFactory.h"

using SEF::Exec::InteractiveExec;

InteractiveExec::InteractiveExec() 
{

}

InteractiveExec::~InteractiveExec() 
{

}

Kernel* InteractiveExec::createExecKernel() 
{
	KernelFactory* kernelFactory = new InteractiveKernelFactory();
	kernel = &kernelFactory->createKernel();
	return kernel;
}

vector<Element*> InteractiveExec::createExecElementsByConfig(string configString)
{
	return elements;
}

void InteractiveExec::RunExec() 
{

}
