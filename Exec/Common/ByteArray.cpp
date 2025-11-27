#include "ByteArray.h"
#include"platform.h"
#ifdef VLD_DEBUG
#include "vld.h"
#endif
using namespace std;
using namespace SEF::COMMON;

ByteArray::ByteArray() 
{

}
ByteArray::ByteArray(const ByteArray& right) 
{
	if (right.max_size > 0) 
	{
		byteMalloc(right.max_size);
		if (right.m_size > 0) 
		{
			m_size = right.m_size;
			memcpy(m_bytes, right.m_bytes, right.m_size);
		}
	}
}
ByteArray::ByteArray(SEF_Size_t size) 
{
	if (max_size < size) 
	{
		byteFree();
		byteMalloc(size);
		max_size = size;
	}
	m_size = size;
}
ByteArray::ByteArray(const SEF_String& v)
{
	if (max_size < v.size())
	{
		byteFree();
		byteMalloc(v.size()+1);
	}
	memcpy(m_bytes, v.data(), v.size());
	m_size = v.size()+1;
}
ByteArray::ByteArray(const SEF_Uchar* buf, SEF_Size_t size) 
{
	if (max_size < size) 
	{
		byteFree();
		byteMalloc(size);
	}
	memcpy(m_bytes, buf, size);
	m_size = size;
}
ByteArray::ByteArray(const SEF_Char* buf, SEF_Size_t size) 
{
	if (max_size < size) 
	{
		byteFree();
		byteMalloc(size);
	}
	memcpy(m_bytes, buf, size);
	m_size = size;
}
bool SEF::COMMON::ByteArray::appendBytes(const ByteArray& array)
{
	if (array.m_size == 0) 
	{
		return false;
	}
	if (max_size - m_size < array.m_size) 
	{
		return false;
	}
	memcpy(m_bytes + m_size, array.m_bytes, array.m_size);
	m_size += array.m_size;
	return true;
}
bool ByteArray::appendBytes(const SEF_Void* buf, SEF_Size_t size) 
{
	if (size == 0) 
	{
		return false;
	}
	if (max_size - m_size < size) 
	{
		return false;
	}
	memcpy(m_bytes + m_size, buf, size);
	m_size += size;
	return true;
}
void ByteArray::setBytes(const SEF_Void* buf, SEF_Size_t size)
{
	if (max_size < size) 
	{
		byteFree();
		byteMalloc(size);
	}
	memcpy(m_bytes, buf, size);
	m_size = size;
}
void ByteArray::setBytes(const SEF_Uchar* buf, SEF_Size_t size) 
{
	if (max_size < size) 
	{
		byteFree();
		byteMalloc(size);
	}
	memcpy(m_bytes, buf, size);
	m_size = size;
}
void ByteArray::setBytes(const SEF_Uchar* buf, SEF_Size_t size, const SEF_Uchar* buf2, SEF_Size_t size2) 
{
	if (max_size > size + size2) 
	{
		memset(m_bytes, 0, m_size);
		m_size = size + size2;
	}
	else 
	{
		byteFree();
		m_size = size + size2;
		byteMalloc(m_size);
	}
	memcpy(m_bytes, buf, size);
	memcpy(m_bytes + size, buf2, size2);

}
void ByteArray::appendBytes(const SEF_Uchar* buf, SEF_Size_t size, const SEF_Uchar* buf2, SEF_Size_t size2) 
{
	if (max_size - m_size < size + size2) 
	{
		byteExternMalloc(size + size2 + m_size);
	}
	memcpy(m_bytes + m_size, buf, size);
	m_size += size;
	memcpy(m_bytes + m_size, buf2, size2);
	m_size += size2;
}
ByteArray& ByteArray::operator=(const ByteArray& right)
{
	if (this != &right) 
	{
		if (m_bytes && m_size < right.m_size) 
		{
			byteFree();
			m_bytes = nullptr;
			m_size = 0;
		}
		if (right.m_bytes != nullptr) 
		{
			SEF_Size_t t_size = 0;
			const SEF_Uchar* t_bytes = right.getBytes(t_size);
			byteMalloc(t_size);
			memcpy(m_bytes, t_bytes, t_size);
			m_size = t_size;
		}
	}
	return *this;
}

ByteArray& ByteArray::operator=(const ByteArray* right)
{
	if (this != right)
	{
		if (m_bytes && m_size < right->m_size)
		{
			byteFree();
			m_bytes = nullptr;
			m_size = 0;
		}
		if (right->m_bytes != nullptr)
		{
			SEF_Size_t t_size = 0;
			const SEF_Uchar* t_bytes = right->getBytes(t_size);
			byteMalloc(t_size);
			memcpy(m_bytes, t_bytes, t_size);
			m_size = t_size;
		}
	}
	return *this;
}

ByteArray ByteArray::operator+(const ByteArray& right) 
{
	ByteArray array;
	SEF_Size_t t_size = m_size + right.m_size;
	if (t_size > 0) 
	{
		array.byteMalloc(t_size);
		if (m_size > 0) 
		{
			memcpy(array.m_bytes, m_bytes, m_size);
			array.m_size += m_size;
		}
		if (right.m_size > 0) 
		{
			memcpy(array.m_bytes + m_size, right.m_bytes, right.m_size);
			array.m_size += right.m_size;
		}
	}
	return array;
}

ByteArray::~ByteArray() 
{
	if (m_bytes) 
	{
		byteFree();
	}
}
SEF_Uchar* ByteArray::reMalloc(SEF_Size_t size) 
{
	if (max_size < size) 
	{
		if (m_bytes != nullptr) 
		{
			byteFree();
		}
		byteMalloc(size);
		memset(m_bytes, 0, size);
		max_size = size;
	}
	else
	{
		memset(m_bytes, 0, m_size);
	}
	return m_bytes;
}
SEF_Uchar* ByteArray::getBytes(SEF_Size_t& size) const 
{
	size = m_size;
	return m_bytes;
}
SEF_Uchar* ByteArray::getBytes() const
{
	return m_bytes;
}
SEF_Uint32 ByteArray::getSize() const
{
	return m_size;
}
bool  ByteArray::setSize(SEF_Uint32 size)
{
	if (max_size < size) 
	{
		return false;
	}
	m_size = size;
	return true;
}
void ByteArray::byteFree() 
{
	if (m_bytes != nullptr) 
	{
		free(m_bytes);
		m_bytes = nullptr;
		m_size = 0;
		max_size = 0;
	}
}
void ByteArray::byteMalloc(SEF_Size_t size) 
{
	m_bytes = (SEF_Uchar*)malloc(size);
	memset(m_bytes, 0x00, size);
	max_size = size;
}
void ByteArray::byteExternMalloc(SEF_Size_t size) 
{
	SEF_Size_t oSize = m_size;
	SEF_Uchar* t_bytes = (SEF_Uchar*)malloc(size);
	if (m_size > 0)
	{
		memcpy(t_bytes, m_bytes, m_size);
	}
	byteFree();
	m_size = oSize;
	max_size = size;
	m_bytes = t_bytes;
}

SEF_String SEF::COMMON::ByteArray::toString()const
{
	SEF_Uint32 size = 0;
	SEF_Uchar* data = getBytes(size);
	if (data == nullptr)
	{
		return SEF_String();
	}
	SEF_String res(reinterpret_cast <char*>(data), size);
	return res;
}