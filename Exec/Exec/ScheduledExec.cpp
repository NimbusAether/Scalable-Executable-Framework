#include "ScheduledExec.h"
#include "ScheduledKernelFactory.h"

using SEF::Exec::ScheduledExec;

ScheduledExec::ScheduledExec() 
{

}

ScheduledExec::~ScheduledExec() 
{

}

Kernel* ScheduledExec::createExecKernel() 
{
	KernelFactory* kernelFactory = new ScheduledKernelFactory();
	kernel = &kernelFactory->createKernel();
	return kernel;
}

vector<Element*> ScheduledExec::createExecElementsByConfig(string configString) 
{
	return elements;
}

void ScheduledExec::RunExec() 
{

}
