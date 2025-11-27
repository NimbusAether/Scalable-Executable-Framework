#include "OneShotExec.h"
#include "OneShotKernelFactory.h"

using SEF::Exec::OneShotExec;
using namespace SEF::Config;

OneShotExec::OneShotExec() 
{
}

OneShotExec::~OneShotExec() 
{

}

Kernel* OneShotExec::createExecKernel() 
{
	KernelFactory* kernelFactory = new OneShotKernelFactory();
	kernel = &kernelFactory->createKernel();
	return kernel;
}

vector<Element*> OneShotExec::createExecElementsByConfig(string configString) 
{
	return elements;
}

void OneShotExec::RunExec() 
{

}
