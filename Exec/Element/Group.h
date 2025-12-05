#pragma once
#include "Element.h"

using namespace SEF::Element;
using std::vector;
namespace SEF {
	namespace Element {
		class UDM_EXPORT_API Group
		{
		public:
			Group();
			~Group();
			string GetGroupName();
			std::vector<Element*> GetElements();
			void SetGroupName(const string& groupName);
			bool AddElementToGroup(Element* element);
		protected:
			string group_name;
			vector<Element*> elements;
		};
	}
}