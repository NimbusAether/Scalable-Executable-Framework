#pragma once
#include "base.h"
#include <deque>
#include <vector>
#include <set>
//#include <iostream>
//#include <stdexcept>
#include "ByteArray.h"
//using namespace std;

namespace SEF
{
   // Forward declarations
    namespace COMMON
    {
      /** Represents a parameter that is part of a Request. */
      class UDM_COMMON_API Parameter
      {
      public:
          static void CountAdd();
          static void CountDel();
          /** Constructor. */
          Parameter(const Parameter& right);
          Parameter(const COMMON::ValueType& type = COMMON::ValueType::VALUE_TYPE_NONE);
          Parameter(const COMMON::ValueType& type, const COMMON::ByteArray &bytes);
          Parameter(const COMMON::SEF_String& arg);
          Parameter(const COMMON::SEF_Bool& arg);
          Parameter(const COMMON::SEF_Double& arg);
          Parameter(const COMMON::SEF_Float& arg);
          Parameter(const COMMON::SEF_Long& arg);
          Parameter(const COMMON::SEF_Int64& arg);
          Parameter(const COMMON::SEF_Uint64& arg);
          Parameter(const COMMON::SEF_Uint32& arg);
          Parameter(const COMMON::SEF_Int32& arg);
          Parameter(const COMMON::SEF_Uint16& arg);
          Parameter(const COMMON::SEF_Int16& arg);
          Parameter(const COMMON::SEF_Uchar& arg);
          Parameter(const COMMON::SEF_Char& arg);
          Parameter(const COMMON::SEF_Uchar* arg, COMMON::SEF_Size_t size);
          Parameter(const COMMON::SEF_Char* arg, COMMON::SEF_Size_t size);
          Parameter(const COMMON::SEF_Uchar* arg, COMMON::SEF_Size_t size, COMMON::ValueType type);

          /** Encodes a paramater to an XDR buffer
           * @param buffer The XDR buffer to use for encoding.
           */
          virtual void serialize(COMMON::ByteArray& buffer);

          /** Decodes a paramater from the buffer to the value.
           * @param buffer The XDR buffer to use for decoding.
           */
          virtual void deserialize(COMMON::ByteArray& buffer);
          virtual COMMON::SEF_Size_t deserialize(COMMON::SEF_Uchar *buffer, COMMON::SEF_Size_t size);

          virtual COMMON::SEF_Size_t GetSerializeSize();


          COMMON::SEF_String getString()const;
          COMMON::SEF_Bool getBool()const;
          COMMON::SEF_Double getDouble()const;
          COMMON::SEF_Float getFloat()const;
          COMMON::SEF_Long getLong()const;
          COMMON::SEF_Int64 getInt64()const;
          COMMON::SEF_Uint64 getUint64()const;
          COMMON::SEF_Uint32 getUint32()const;
          COMMON::SEF_Int32 getInt32()const;
          COMMON::SEF_Uint16 getUint16()const;
          COMMON::SEF_Int16 getInt16()const;
          COMMON::SEF_Uchar getUchar()const;
          COMMON::SEF_Char getChar()const;
          COMMON::SEF_String toString()const;


          Parameter& operator=(const Parameter& right);
          ~Parameter();
         /** The parameter type. */
         COMMON::ValueType     m_type = COMMON::ValueType::VALUE_TYPE_NONE;
         /** The data stored in the parameter. */
         COMMON::ByteArray m_bytes;
      };
         
      /** A class containing information about a request. */
      class UDM_COMMON_API Request
      {
      public:
         Request();
         /** Constructor specifying name. 
         * @param name The name of the request.
         */
         Request(const COMMON::SEF_String& name);
         /** Copy constructor. 
         * @param copy The request to copy.
         */
         Request(const Request& copy);
         /** Common use constructor to specify name and single Bool arguement. 
         * @param name The name of the request.
         * @param arg0 A Boolean argument to add to the request.
         */
         Request(COMMON::SEF_String name, COMMON::SEF_Bool arg0);
         /** Common use constructor to specify name and single Int64 arguement. 
         * @param name The name of the request.
         * @param arg0 An Int64 argument to add to the request.
         */
         Request(COMMON::SEF_String name, COMMON::SEF_Int64 arg0);
         /** Common use constructor to specify name and single Float64 arguement. 
         * @param name The name of the request.
         * @param arg0 A Float64 argument to add to the request.
         */
         Request(COMMON::SEF_String name, COMMON::SEF_Float arg0);
         /** Common use constructor to specify name and single String arguement.
         * @param name The name of the request.
         * @param arg0 A String argument to add to the request.
         */
         Request(COMMON::SEF_String name, COMMON::SEF_String arg0);
         /** Destructor. */
         virtual ~Request();
         /** Clear all parameters in the request. */
         void clear();
         /** Assignment operator. 
         * @return A reference to this request.
         * @param right The request to copy.
         */
         Request & operator=(const Request& right);
               
         /** Returns the name of the request.
         * @return The name of the request.
         */
         const COMMON::SEF_String& getName() const;

         /** Sets the name of the request.
         * @param name The name of the request.
         */
         void setName(const COMMON::SEF_String& name);


         /** Gets the name of the request sender.
         * @return The name of the request sender.
         */
         const COMMON::SEF_String& getSender() const;
               
         /** Sets the name of the request sender.
         * @param name The name of the sender.
         */
         void setSender(const COMMON::SEF_String& name);

         /** Returns the number of parameters in the request.
         * @return The number of parameters in the request.
         */
         COMMON::SEF_Uint32 parameterCount() const;
               
         /** Returns the parameter with a given index.
         * Request argument(index) must be valid otherwise an exception is thrown.
         * @throws std::ranger_error() if index is out of range.
         * @param index The index of the parameter.
         */
         const Parameter getParameter(COMMON::SEF_Uint32 index) const;

         /** Returns the type of parameter with the given index.
         * Request argument(index) must be valid otherwise an exception is thrown.
         * @throws std::range_error() if index is out of range.
         * @return The type of parameter with the given index.
         * @param index The index of the parameter.
         */
         COMMON::ValueType getParameterType(COMMON::SEF_Uint32 index) const;

         /** Returns a parameter with the given index as a boolean value.
         * @throws std::runtime_error() if the parameter is the wrong type.
         * @throws std::range_error() if the index is out of range.
         * @return The parameter with the given index as a boolean value.
         * @param index The index of the parameter.
         */
         COMMON::SEF_Bool getBool(COMMON::SEF_Uint32 index) const;

         /** Returns a parameter with the given index as a Char value.
         * @throws std::runtime_error() if the parameter is the wrong type.
         * @throws std::range_error() if the index is out of range.
         * @return The parameter with the given index as a boolean value.
         * @param index The index of the parameter.
         */
         COMMON::SEF_Char getChar(COMMON::SEF_Uint32 index) const;

         /** Returns a parameter with the given index as a UChar value.
         * @throws std::runtime_error() if the parameter is the wrong type.
         * @throws std::range_error() if the index is out of range.
         * @return The parameter with the given index as a boolean value.
         * @param index The index of the parameter.
         */
         COMMON::SEF_Uchar getUChar(COMMON::SEF_Uint32 index) const;

         /** Returns a parameter with the given index as a UInt16 value.
         * @throws std::runtime_error() if the parameter is the wrong type.
         * @throws std::range_error() if the index is out of range.
         * @return The parameter with the given index as a boolean value.
         * @param index The index of the parameter.
         */
         COMMON::SEF_Uint16 getUInt16(COMMON::SEF_Uint32 index) const;

         /** Returns a parameter with the given index as a UInt32 value.
         * @throws std::runtime_error() if the parameter is the wrong type.
         * @throws std::range_error() if the index is out of range.
         * @return The parameter with the given index as a boolean value.
         * @param index The index of the parameter.
         */
         COMMON::SEF_Uint32 getUInt32(COMMON::SEF_Uint32 index) const;

         /** Returns a parameter with the given index as a UInt64 value.
         * @throws std::runtime_error() if the parameter is the wrong type.
         * @throws std::range_error() if the index is out of range.
         * @return The parameter with the given index as a boolean value.
         * @param index The index of the parameter.
         */
         COMMON::SEF_Uint64 getUInt64(COMMON::SEF_Uint32 index) const;

         /** Returns a parameter with the given index as a Int16 value.
         * @throws std::runtime_error() if the parameter is the wrong type.
         * @throws std::range_error() if the index is out of range.
         * @return The parameter with the given index as a boolean value.
         * @param index The index of the parameter.
         */
         COMMON::SEF_Int16 getInt16(COMMON::SEF_Uint32 index) const;

         /** Returns a parameter with the given index as an Int32 value.
         * @throws std::runtime_error() if the parameter is the wrong type.
         * @throws std::range_error() if the index is out of range.
         * @return The parameter with the given index as a boolean value.
         * @param index The index of the parameter.
         */
         COMMON::SEF_Int32 getInt32(COMMON::SEF_Uint32 index) const;

         /** Returns a parameter with the given index as an Int64 value.
         * @throws std::runtime_error() if the parameter is the wrong type.
         * @throws std::range_error() if the index is out of range.
         * @return The parameter with the given index as a boolean value.
         * @param index The index of the parameter.
         */
         COMMON::SEF_Int64 getInt64(COMMON::SEF_Uint32 index) const;

         /** Returns a parameter with the given index as a Float32 value.
         * @throws std::runtime_error() if the parameter is the wrong type.
         * @throws std::range_error() if the index is out of range.
         * @return The parameter with the given index as a boolean value.
         * @param index The index of the parameter.
         */
         COMMON::SEF_Float getFloat32(COMMON::SEF_Uint32 index) const;

         COMMON::SEF_Double getDouble(COMMON::SEF_Uint32 index) const;


         /** Returns a parameter with the given index as a String value.
         * @throws std::runtime_error() if the parameter is the wrong type.
         * @throws std::range_error() if the index is out of range.
         * @return The parameter with the given index as a boolean value.
         * @param index The index of the parameter.
         */
         const COMMON::SEF_String getString(COMMON::SEF_Uint32 index) const;

         /** Returns a parameter with the given index as a ByteArray value.
         * @throws std::runtime_error() if the parameter is the wrong type.
         * @throws std::range_error() if the index is out of range.
         * @return The parameter with the given index as a boolean value.
         * @param index The index of the parameter.
         */
         const COMMON::ByteArray getByteArray(COMMON::SEF_Uint32 index) const;

         /** Add a boolean parameter to the request.
         * @param value The value of the parameter to add.
         */
         void add(const COMMON::SEF_Bool& value);

         /** Add a Char parameter to the request.
         * @param value The value of the parameter to add.
         */
         void add(const COMMON::SEF_Char& value);

         /** Add a UChar parameter to the request.
         * @param value The value of the parameter to add.
         */
         void add(const COMMON::SEF_Uchar& value);

         /** Add a UInt16 parameter to the request.
         * @param value The value of the parameter to add.
         */
         void add(const COMMON::SEF_Uint16& value);

         /** Add a UInt32 parameter to the request.
         * @param value The value of the parameter to add.
         */
         void add(const COMMON::SEF_Uint32& value);

         /** Add a UInt64 parameter to the request.
         * @param value The value of the parameter to add.
         */
         void add(const COMMON::SEF_Uint64& value);

         /** Add an Int16 parameter to the request.
         * @param value The value of the parameter to add.
         */
         void add(const COMMON::SEF_Int16& value);

         /** Add an Int32 parameter to the request.
         * @param value The value of the parameter to add.
         */
         void add(const COMMON::SEF_Int32& value);

         /** Add a Int64 parameter to the request.
         * @param value The value of the parameter to add.
         */
         void add(const COMMON::SEF_Int64& value);

         /** Add a Float32 parameter to the request.
         * @param value The value of the parameter to add.
         */
         void add(const COMMON::SEF_Float& value);

         void add(const COMMON::SEF_Double& value);

         /** Add a String parameter to the request.
         * @param value The value of the parameter to add.
         */
         void add(const COMMON::SEF_String& value);


         void add(const SEF_Char* value);

         /** Add a String parameter to the request.
         * Note, this will convert the Char* to a String.
         * You must use getString() to retreive the value.
         * @param value The value of the parameter to add.
         */
         void  add(const COMMON::SEF_Char* value, size_t  size);

         /** Add a SEF_Uchar parameter to the request.
         * @param value The value of the parameter to add.
         */
         void add(const COMMON::SEF_Uchar* value, size_t  size);

         /** Add a parameter as an array of bytes.
         * @param value The value of the parameter to add.
         * @param size The size of the value in bytes.
         * @param type The type of parameter to add.
         */
         void add(const COMMON::SEF_Uchar* value, COMMON::SEF_Size_t size,
            COMMON::ValueType type);

         /** Add a prepopulated parameter.
         * @param value The parameter to add.
         */
         void add(Parameter& value);

         /** request to string.
          */
         COMMON::SEF_String toString()const;

         /** Encodes a request to an XDR buffer
          * @param buffer The XDR buffer to use for encoding.
          */
         virtual void serialize(COMMON::ByteArray& buffer);

         /** Decodes a request from the buffer to the value.
          * @param buffer The XDR buffer to use for decoding.
          */
         virtual COMMON::SEF_Size_t deserialize(const COMMON::ByteArray& buffer);

         virtual COMMON::SEF_Size_t deserialize(const COMMON::SEF_Uchar* data, SEF_Size_t size);



         /** Sets the name of the request.
* @param name The name of the request.
*/
         void setMarker(const COMMON::SEF_Uint32 marker);

         COMMON::SEF_Uint32 getMarker();

         void setCseq(const COMMON::SEF_Uint16 cseq);

         COMMON::SEF_Uint16 getCseq();

         virtual COMMON::SEF_Size_t getSerializeSize()const;
      private:
         /** The name of the request. */
         COMMON::SEF_String    m_name;
         /** The name of the request sender. */
         COMMON::SEF_String    m_senderName;
         /** The parameters of the request. */
         std::vector<Parameter>      m_parameters; 

         COMMON::SEF_Uint32    m_marker = 0;
         COMMON::SEF_Uint16    m_cseq = 0;
         COMMON::SEF_Uint32    m_paramaterPredictSize = 0;

         COMMON::ByteArray      m_byteArray;
      protected:
          static void CountAdd();
          static void CountDel();
      };

      /** A queue to handle requests. */
      class UDM_COMMON_API RequestQueue
      {
      public:
         //friend class GPATE::Udm::Snapshot;

         /** Constructor.
         * 
         */
          RequestQueue();

         /** Constructor.
         * @param name The name of the request queue.
         */
         RequestQueue(const COMMON::SEF_String& name);

         /** Copy constructor.
         * @param copy The request queue to copy.
         */
         RequestQueue(const RequestQueue& copy);

         RequestQueue(RequestQueue&& other)noexcept;


         /** Destructor. */
         virtual ~RequestQueue();

         /** Clear the queue. */
         void clear();
               
         /** Turns on/off request queue logging to the database.
         * @param state The new listen state.
         */
         void setListenState(const COMMON::SEF_Bool &state);

         /** Get the listen state.
         * @return The value of the listen state.
         */
         const COMMON::SEF_Bool& getListenState() const;

         /** Lock the queue. */
         void lock();

         /** Unlock the queue. */
         void unlock();

         /** Push a request to the back of the queue.
         * @param request The request to add to the queue.
         */
         void pushBack(std::shared_ptr<Request> request);

         /** Add a registered request name. The purpose of this method is to
         * publish the intention to handle a request with the given name.
         * Note, you do not have to register a request to be able to use
         * it. This just provides other clients with information about the
         * requests you intend to handle.
         * @param requestName The name of the request to register.
         */
         void addRegisteredRequest(const COMMON::SEF_String& requestName);

         /** Return A collection of the request names that have been registered.
         * @return A collection of the request names that have been registered.
         */
         std::set< COMMON::SEF_String > getRegisteredRequests();

         /** Returns true if the named request is registered.
         * @return True if the name is registered, false otherwise.
         * @param name The name of the request to query for.
         */
         COMMON::SEF_Bool isRequestRegistered(const COMMON::SEF_String& name);

         /** Pops a request off the front of the queue and returns it. 
         * @return The request at the front of the queue.
         * @note Call empty() to verify the queue is not empty before calling
         * this function otherwise this function's behavior is undefined.
         */
         const std::shared_ptr<Request> popFront();

         /** Returns a copy of the request at the front of the queue.
         * @return A copy of the request at the front of the queue.
         * @note Call empty() to verify the queue is not empty before calling
         * this function otherwise this function's behavior is undefined.
         */
         const std::shared_ptr<Request> peekFront() const;

         /** Pops a request off the front of the queue and stores it in the 
         * request paramater. 
         * @note Call empty() to verify the queue is not empty before calling
         * this function otherwise this function's behavior is undefined.
         * @param request The location to store the request.
         */
         bool popFront(std::shared_ptr<Request>& request);

         /** Stores a copy of the request at the front of the queue in the
         * request parameter.
         * @note Call empty() to verify the queue is not empty before calling 
         * this function otherwise this function's behavior is undefined.
         * @param request The location to store the request.
         */
         void peekFront(std::shared_ptr<Request>& request) const;

         /** Returns true if the queue is empty.
         * @return True if the queue is empty, false otherwise.
         */
         COMMON::SEF_Bool empty() const;

         /** Get the number of requests in the queue.
         * @return The number of requests in the queue.
         */
         COMMON::SEF_Uint32 getSize() const;

         /** Set the name of the queue.
         * @return The name of the queue.
         */
         void setName(const COMMON::SEF_String&name);

         /** Get the name of the queue.
         * @return The name of the queue.
         */
         const COMMON::SEF_String& getName() const;
               
         /** Print the queue contents to standard output. */
         SEF_String toString() const;
              

         /** Encodes a request queue to an XDR buffer
          * @param buffer The XDR buffer to use for encoding.
          */
         virtual void serialize(COMMON::ByteArray& buffer);

         /** Decodes a request queue from the buffer to the value.
          * @param buffer The XDR buffer to use for decoding.
          */
         virtual void deserialize(const COMMON::ByteArray& buffer);

      private:
         /** Type definition for the request queue. */
         typedef std::deque<std::shared_ptr<Request>> RequestDeque;

         /** The name of the request queue. */
         COMMON::SEF_String               m_name;
         /** The deque where the requests are stored. */
         RequestDeque                        m_queue;
         /** A set of request names that are registered. */
         std::set< COMMON::SEF_String >   m_registeredRequests;
         /** Mutex to protect the queue data. */
         COMMON::Threading::Mutex     m_mutex;
         /** The listen state of the queue. */
         COMMON::SEF_Bool            m_listenState;

         COMMON::SEF_Size_t        m_requestBytesSize = 0;
      };
   }
}

