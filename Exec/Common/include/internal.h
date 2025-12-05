#pragma once
#include "TopicDefine.h"
namespace SEF
{
	namespace INTERNAL
	{
		enum class TreeItemType {
			TOPICQUERY_REQUEST = 0,
			TOPICQUERY_VARIABLE_SUB = 1,
			TOPICQUERY_VARIABLE_PUB = 2,
		};
		constexpr char* TOPIC_QUERY_REQUEST_TOPICNAME = "#gTopicRequestItems#";
		constexpr char* TOPIC_QUERY_VAR_SUB_TOPICNAME = "#gTopicVariableSubItems#";
		constexpr char* TOPIC_QUERY_VAR_PUB_TOPICNAME = "#gTopicVariablePubItems#";

		constexpr char ITEM_SPLIC_CHAR = '@';

	}
}
