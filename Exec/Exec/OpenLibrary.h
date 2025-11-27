#pragma once
#include "ElementFactory.h"


using SEF::Element::ElementFactory;
namespace SEF {
	namespace Exec {
		class DllInfo;
		class  OpenLibrary
		{
		public:
			OpenLibrary();
			~OpenLibrary();
			ElementFactory* LoaderFactory(const char* dllName);
		private:
			DllInfo* m_info = nullptr;				
		};
	}
}
