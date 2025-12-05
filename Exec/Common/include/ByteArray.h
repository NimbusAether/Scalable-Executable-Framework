#pragma once
#include <string>
#include <mutex>
#include <cstdint>
#include"base.h"


namespace SEF
{
	namespace COMMON
	{
	class UDM_COMMON_API ByteArray {
		public:
			ByteArray();
			ByteArray(const ByteArray& right);
			ByteArray(SEF_Size_t size);
			ByteArray(const SEF_String&v);
			ByteArray(const SEF_Uchar* buf, SEF_Size_t size);
			ByteArray(const SEF_Char* buf, SEF_Size_t size);
			bool appendBytes(const ByteArray &array);
			bool appendBytes(const SEF_Void* buf, SEF_Size_t size);
			void setBytes(const SEF_Void* buf, SEF_Size_t size);
			void setBytes(const SEF_Uchar* buf, SEF_Size_t size);
			void setBytes(const SEF_Uchar* buf, SEF_Size_t size, const SEF_Uchar* buf2, SEF_Size_t size2);
			void appendBytes(const SEF_Uchar* buf, SEF_Size_t size, const SEF_Uchar* buf2, SEF_Size_t size2);
			ByteArray& operator=(const ByteArray& right);
			ByteArray& operator=(const ByteArray* right);
			ByteArray operator+(const ByteArray& right);
			SEF_String toString()const;

			virtual ~ByteArray();
			SEF_Uchar* reMalloc(SEF_Size_t size);
			SEF_Uchar* getBytes(SEF_Size_t& size) const;
			SEF_Uchar* getBytes() const;
			SEF_Uint32 getSize()const;
			bool setSize(SEF_Uint32 size);
		private:
			void byteFree();
			void byteMalloc(SEF_Size_t size);
			void byteExternMalloc(SEF_Size_t size);
			SEF_Uchar* m_bytes = nullptr;
			SEF_Size_t	m_size = 0;
			SEF_Size_t	max_size = 0;
		};
		namespace Threading{
			typedef std::mutex Mutex;
		}


	}
}