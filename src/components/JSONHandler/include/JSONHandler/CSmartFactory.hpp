/**
 * @file CSmartFactory.hpp
 * @brief CSmartFactory header file.
 */
// Copyright (c) 2013, Ford Motor Company
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
//
// Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following
// disclaimer in the documentation and/or other materials provided with the
// distribution.
//
// Neither the name of the Ford Motor Company nor the names of its contributors
// may be used to endorse or promote products derived from this software
// without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 'A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#ifndef __CSMARTFACTORY_HPP__
#define __CSMARTFACTORY_HPP__

#include "SmartObjects/CSmartObject.hpp"
#include "SmartObjects/CSmartSchema.hpp"
#include <map>
#include <string>

namespace NsSmartDeviceLink
{
    namespace NsJSONHandler
    {
        /**
         * @brief String constants used by SmartFactory.
         */
        namespace strings
        {
            /**
             * @brief String constant for MSG_PARAMS.
             */
            extern const std::string S_MSG_PARAMS;

            /**
             * @brief String constant for PARAMS.
             */
            extern const std::string S_PARAMS;

            /**
             * @brief String constant for FUNCTION_ID.
             */
            extern const std::string S_FUNCTION_ID;

            /**
             * @brief String constant for MESSAGE_TYPE.
             */
            extern const std::string S_MESSAGE_TYPE;

            /**
             * @brief String constant for PROTOCOL_VERSION.
             */
            extern const std::string S_PROTOCOL_VERSION;

            /**
             * @brief String constant for PROTOCOL_TYPE.
             */
            extern const std::string S_PROTOCOL_TYPE;

            /**
             * @brief String constant for CORRELATION_ID.
             */
            extern const std::string S_CORRELATION_ID;
        }

        /**
         * @brief Smart Schema key.
         *
         * @tparam FunctionIdEnum Type of function ID enum.
         * @tparam MessageTypeEnum Type of messageType enum.
         */
        template <class FunctionIdEnum, class MessageTypeEnum>
        struct SmartSchemaKey
        {
            /**
             * @brief Value of function ID for the key.
             */
            FunctionIdEnum functionId;

            /**
             * @brief Value of messageType for the key.
             */
            MessageTypeEnum messageType;

            /**
             * @brief Constructor.
             *
             * @param functionIdParam Value of function ID.
             * @param messageTypeParam Value of message type.
             */
            SmartSchemaKey(FunctionIdEnum functionIdParam, MessageTypeEnum messageTypeParam);
        };

        /**
         * @brief Smart Factory.
         *
         * This class is used as base class for generated factories.
         * Clients should use methods of this class to access all
         * SmartSchema validation features.
         *
         * @tparam FunctionIdEnum Type of function ID enum.
         * @tparam MessageTypeEnum Type of messageType enum.
         */
        template <class FunctionIdEnum, class MessageTypeEnum>
        class CSmartFactory
        {
        public:

            /**
             * @brief Constructor.
             */
            CSmartFactory(void);

            /**
             * @brief Attach schema to the SmartObject.
             *
             * @param object SmartObject to attach schema for.
             *
             * @return True if operation was successful or false otherwise.
             */
            bool attachSchema(NsSmartDeviceLink::NsSmartObjects::CSmartObject& object);

        protected:

            /**
             * @brief Defines map of SmartSchemaKeys to the SmartSchems.
             */
            typedef std::map<SmartSchemaKey<FunctionIdEnum, MessageTypeEnum>, NsSmartDeviceLink::NsSmartObjects::CSmartSchema> SchemaMap;

            /**
             * @brief Map of all schemas for this factory.
             */
            SchemaMap mSchemas;
        };

        template <class FunctionIdEnum, class MessageTypeEnum>
        CSmartFactory<FunctionIdEnum, MessageTypeEnum>::CSmartFactory(void)
        : mSchemas()
        {
        }

        template <class FunctionIdEnum, class MessageTypeEnum>
        bool CSmartFactory<FunctionIdEnum, MessageTypeEnum>::attachSchema(NsSmartDeviceLink::NsSmartObjects::CSmartObject& object)
        {
            if(false == object.keyExists(strings::S_PARAMS)) return false;
            if(false == object[strings::S_PARAMS].keyExists(strings::S_MESSAGE_TYPE)) return false;
            if(false == object[strings::S_PARAMS].keyExists(strings::S_FUNCTION_ID)) return false;

            MessageTypeEnum msgtype((MessageTypeEnum)(int)object[strings::S_PARAMS][strings::S_MESSAGE_TYPE]);
            FunctionIdEnum fid((FunctionIdEnum)(int)object[strings::S_PARAMS][strings::S_FUNCTION_ID]);

            SmartSchemaKey<FunctionIdEnum, MessageTypeEnum> key(fid, msgtype);

            typename SchemaMap::iterator schemaIterator = mSchemas.find(key);

            if(schemaIterator == mSchemas.end())
            {
                // Schema was not found
                return false;
            }

            object.setSchema(schemaIterator->second);
            schemaIterator->second.applySchema(object);

            return true;
        }

        template <class FunctionIdEnum, class MessageTypeEnum>
        SmartSchemaKey<FunctionIdEnum, MessageTypeEnum>::SmartSchemaKey(FunctionIdEnum functionIdParam, MessageTypeEnum messageTypeParam)
        : functionId(functionIdParam)
        , messageType(messageTypeParam)
        {

        }


        template <class FunctionIdEnum, class MessageTypeEnum>
        bool operator<(const SmartSchemaKey< FunctionIdEnum, MessageTypeEnum >& l, const SmartSchemaKey< FunctionIdEnum, MessageTypeEnum >& r)
        {
            if (l.functionId < r.functionId) return true;
            if (l.functionId > r.functionId) return false;

            if (l.messageType < r.messageType) return true;
            if (l.messageType > r.messageType) return false;

            return false;
        }
    }
}
#endif //__CSMARTFACTORY_HPP__
