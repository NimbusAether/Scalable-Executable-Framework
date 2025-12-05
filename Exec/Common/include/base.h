#pragma once
#include <stdint.h>
#include <string>

#define UDM_EXPORT_API __declspec(dllexport)
#define UDM_IMPORT_API __declspec(dllimport)

#ifdef COMMON_LIB
#define UDM_COMMON_API UDM_EXPORT_API
#else
#define UDM_COMMON_API UDM_IMPORT_API
#endif

namespace SEF
{
	namespace Udm {
		enum class UdmType
		{
			None = -1,
			// IPC_Communication_Middleware
			SimpleSingleton = 0,
			ThreadSafetySingleton = 1,
			SharedMemory = 2,
			MessageQueue = 3,
			Semaphore = 4,
			// Net_Communication_Middleware
			SocketAPI = 5,
			ZeroMQ = 6,
			FastDDS = 7,
		};
		constexpr int portStep = 1;
	};

	namespace COMMON
	{
		enum class ValueType
		{
			VALUE_TYPE_NONE = 0,
			VALUE_TYPE_BOOL,
			VALUE_TYPE_CHAR,
			VALUE_TYPE_UCHAR,
			VALUE_TYPE_CHARS,
			VALUE_TYPE_UCHARS,
			VALUE_TYPE_INT,
			VALUE_TYPE_UINT,
			VALUE_TYPE_INT16,
			VALUE_TYPE_UINT16,
			VALUE_TYPE_INT32,
			VALUE_TYPE_UINT32,
			VALUE_TYPE_INT64,
			VALUE_TYPE_UINT64,
			VALUE_TYPE_LONG,
			VALUE_TYPE_FLOAT,
			VALUE_TYPE_DOUBLE,
			VALUE_TYPE_STRING,
			VALUE_TYPE_SUBSCRIBE,
			VALUE_TYPE_REQUEST,
			VALUE_TYPE_REQUESTQUEUE,
			VALUE_TYPE_QUERY_TOPICNODE,
			VALUE_TYPE_TOPICNODE,
		};

	#define SEF_TRUE true
	#define SEF_FALSE false


		typedef char			SEF_Char;
		typedef unsigned char	SEF_Uchar;
		typedef unsigned short	SEF_Ushort;
		typedef int16_t			SEF_Int16;
		typedef uint16_t		SEF_Uint16;
		typedef int32_t			SEF_Int32;
		typedef uint32_t		SEF_Uint32;
		typedef uint64_t		SEF_Uint64;
		typedef int64_t			SEF_Int64;
		typedef long			SEF_Long;
		typedef float			SEF_Float;
		typedef double			SEF_Double;
		typedef bool			SEF_Bool;
		typedef std::string		SEF_String;
		typedef SEF_Uint32		SEF_Size_t;
		typedef void			SEF_Void;
		typedef int				SEF_Int;
		typedef unsigned int	SEF_Uint;



		class UDM_COMMON_API SwapEndian {
		public:
			static SEF_Uint32 swap2local(SEF_Uchar* data, SEF_Uint32& result);
			static SEF_Uint32 swap2local(SEF_Uint32 data, SEF_Uint32& result);

			static SEF_Uint16 swap2local(SEF_Uchar* data, SEF_Uint16& result);
			static SEF_Uint16 swap2local(SEF_Uint16 data, SEF_Uint16& result);

			static  void swap2network(SEF_Uchar* data, SEF_Uint32 tmp);
			static  void swap2network(SEF_Uchar* data, SEF_Uint16  tmp);
		};
	}
}