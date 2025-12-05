#pragma once
#include <string>
#include "UdmMessage.h"

using namespace std;
using namespace SEF::Udm;
namespace SEF
{
	namespace Udm
	{
		template <typename T>
		class UdmJsonRefMsg:public UdmMsg
		{
		public:
			UdmJsonRefMsg(T& ref):m_ref(ref){};
			virtual~UdmJsonRefMsg() 
			{
				return;
			};
			T& getRef() 
			{ 
				return m_ref;
			};

			UdmJsonRefMsg& operator=(UdmJsonRefMsg& right) 
			{
				if (this != &right) 
				{
					m_ref = right.m_ref;
				}
				return *this;
			}

		private:
			T &m_ref;
		};
	}
}