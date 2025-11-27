#include "Group.h"

using SEF::Element::Group;

Group::Group() 
{
	group_name = "";
}

Group::~Group() 
{

}

string Group::GetGroupName() 
{
	return group_name;
}

vector<Element*> Group::GetElements() 
{
	return elements;
}

void Group::SetGroupName(const string &groupName)
{
	group_name = groupName;
}

bool Group::AddElementToGroup(Element* element) 
{
	try 
	{
		elements.push_back(element);
		return true;
	}
	catch (std::exception) 
	{
		return false;
	}

}
