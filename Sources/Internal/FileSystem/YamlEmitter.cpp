/*==================================================================================
    Copyright (c) 2008, binaryzebra
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    * Neither the name of the binaryzebra nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE binaryzebra AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL binaryzebra BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=====================================================================================*/

#include "YamlEmitter.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/YamlNode.h"
#include "yaml/yaml.h"

namespace DAVA 
{
static const int32 IDENTATION_SPACES_COUNT = 4;

static yaml_scalar_style_t GetYamlScalarStyle(YamlNode::eStringRepresentation representation)
{
    switch(representation)
    {
    case YamlNode::SR_DOUBLE_QUOTED_REPRESENTATION : return YAML_DOUBLE_QUOTED_SCALAR_STYLE;
    case YamlNode::SR_PLAIN_REPRESENTATION : return YAML_PLAIN_SCALAR_STYLE;
    default: break;
    }
    return YAML_ANY_SCALAR_STYLE;
}

static yaml_sequence_style_t GetYamlSequenceStyle(YamlNode::eArrayRepresentation representation)
{
    switch(representation)
    {
    case YamlNode::AR_BLOCK_REPRESENTATION : return YAML_BLOCK_SEQUENCE_STYLE;
    case YamlNode::AR_FLOW_REPRESENTATION : return YAML_FLOW_SEQUENCE_STYLE;
    default: break;
    }
    return YAML_ANY_SEQUENCE_STYLE;
}

static yaml_mapping_style_t GetYamlMappingStyle(YamlNode::eMapRepresentation representation)
{
    switch(representation)
    {
    case YamlNode::MR_BLOCK_REPRESENTATION : return YAML_BLOCK_MAPPING_STYLE;
    case YamlNode::MR_FLOW_REPRESENTATION : return YAML_FLOW_MAPPING_STYLE;
    default: break;
    }
    return YAML_ANY_MAPPING_STYLE;
}

class OrderedYamlNode
{
public:
    OrderedYamlNode(const String& nodeName, const YamlNode* node)
        : yamlNodeName(nodeName)
        , yamlNodeValue(node)
    {}

    const String &GetNodeName() const  {return yamlNodeName;};
    const YamlNode *GetNodeValue() const {return yamlNodeValue;};

    bool operator < (const OrderedYamlNode& right) const
    {
//         bool result = false;
//         if(yamlNodeValue->GetType() == right.yamlNodeValue->GetType())
//         {
//             switch(yamlNodeValue->GetType())
//             {
//             case YamlNode::TYPE_STRING:
//                 {
//                     result = yamlNodeValue->AsString() < right.yamlNodeValue->AsString();
//                 }
//                 break;
//             case YamlNode::TYPE_ARRAY :
//                 {
//                     result = yamlNodeValue->typeRepresentation.arrayStyle < right.yamlNodeValue->typeRepresentation.arrayStyle;
//                 }
//                 break;
//             case YamlNode::TYPE_MAP   :
//                 {
//                     const YamlNode* leftIndexNode = yamlNodeValue->Get(YamlNode::SAVE_INDEX_NAME);
//                     const YamlNode* rightIndexNode = right.yamlNodeValue->Get(YamlNode::SAVE_INDEX_NAME);
// 
//                     if (leftIndexNode && rightIndexNode)
//                     {
//                         result = leftIndexNode->AsInt() < rightIndexNode->AsInt()
//                     }
//                     else
//                     {
//                         result = yamlNodeValue->typeRepresentation.mapStyle < right.yamlNodeValue->typeRepresentation.mapStyle; 
//                     }
//                 }
//                 break;
//             }
//             return result;
//         }

        const YamlNode* leftIndexNode = yamlNodeValue->Get(YamlNode::SAVE_INDEX_NAME);
        const YamlNode* rightIndexNode = right.yamlNodeValue->Get(YamlNode::SAVE_INDEX_NAME);

        if (!leftIndexNode || !rightIndexNode)
        {
            return false;
        }

        return leftIndexNode->AsInt() < rightIndexNode->AsInt();
    }

private:
    String yamlNodeName;
    const YamlNode *yamlNodeValue;
};

int write_handler(void *ext, unsigned char *buffer, size_t size)
{
    File *yamlFile = static_cast<File *>(ext);

    uint32 prevFileSize = yamlFile->GetSize();
    uint32 bytesWritten = yamlFile->Write(buffer, size);

    bool error = (yamlFile->GetSize() != prevFileSize + bytesWritten);
    return error ? 0 : 1;
}

DAVA::YamlEmitter::~YamlEmitter()
{
}

DAVA::YamlEmitter::YamlEmitter()
{
}

bool YamlEmitter::SaveToYamlFile(const FilePath &outFileName, const YamlNode *node, uint32 attr)
{
    ScopedPtr<YamlEmitter> emitter(new YamlEmitter());
    return emitter->Emit(node, outFileName, attr);
}

bool YamlEmitter::Emit(const YamlNode * node, const FilePath & outFileName, uint32 attr)
{
    ScopedPtr<File> yamlFile( File::Create(outFileName, attr) );
    if ((File*)yamlFile == NULL)
    {
        Logger::Error("[YamlEmitter::Emit] Can't create file: %s for output", outFileName.GetAbsolutePathname().c_str());
        return false;
    }

    return Emit(node, yamlFile);
}

bool YamlEmitter::Emit(const YamlNode * node, File *outFile)
{
    yaml_emitter_t emitter;

    DVASSERT(yaml_emitter_initialize(&emitter));
    yaml_emitter_set_encoding(&emitter, YAML_UTF8_ENCODING);
    yaml_emitter_set_unicode(&emitter, 1);
    yaml_emitter_set_width(&emitter, -1);
    yaml_emitter_set_indent(&emitter, IDENTATION_SPACES_COUNT);
    yaml_emitter_set_output(&emitter, &write_handler, outFile);

    do 
    {
        if (!EmitStreamStart(&emitter)) break;

        if (!EmitDocumentStart(&emitter)) break;

        if (!EmitYamlNode(&emitter, node)) break;

        if (!EmitDocumentEnd(&emitter)) break;

        if (!EmitStreamEnd(&emitter)) break;
    } while (0);

    if (YAML_NO_ERROR != emitter.error)
    {
        switch (emitter.error)
        {
        case YAML_MEMORY_ERROR:
            Logger::Error("[YamlEmitter::Emit] Memory error: Not enough memory for emitting");
            break;

        case YAML_WRITER_ERROR:
            Logger::Error("[YamlEmitter::Emit] Writer error: %s", emitter.problem);
            break;

        case YAML_EMITTER_ERROR:
            Logger::Error("[YamlEmitter::Emit] Emitter error: %s", emitter.problem);
            break;

        default:
            /* Couldn't happen. */
            Logger::Error("[YamlEmitter::Emit] Internal error\n");
            break;
        }
    }

    DVASSERT(yaml_emitter_flush(&emitter));
    yaml_emitter_delete(&emitter);

    return true;
}

void YamlEmitter::OrderMapYamlNode(const MultiMap<String, YamlNode*>& mapNodes, Vector<OrderedYamlNode> &sortedChildren ) const
{
    sortedChildren.reserve( mapNodes.size() );

    for (MultiMap<String, YamlNode*>::const_iterator t = mapNodes.begin(); t != mapNodes.end(); ++t)
    {
        // Only the nodes of type Map are expected here.
        if (t->second->GetType() == YamlNode::TYPE_MAP)
            continue;

        sortedChildren.push_back(OrderedYamlNode(t->first, t->second));
    }

//     for (MultiMap<String, YamlNode*>::const_iterator t = mapNodes.begin(); t != mapNodes.end(); ++t)
//     {
//         // Only the nodes of type Map are expected here.
//         if (t->second->GetType() != YamlNode::TYPE_STRING)
//             continue;
// 
//         sortedChildren.push_back(OrderedYamlNode(t->first, t->second));
//     }
// 
//     for (MultiMap<String, YamlNode*>::const_iterator t = mapNodes.begin(); t != mapNodes.end(); ++t)
//     {
//         // Only the nodes of type Map are expected here.
//         if (t->second->GetType() != YamlNode::TYPE_ARRAY)
//             continue;
// 
//         sortedChildren.push_back(OrderedYamlNode(t->first, t->second));
//     }


    uint32 lastNotMapItemIndex = sortedChildren.size();
    // Order the map nodes by the "Save index".
    for (MultiMap<String, YamlNode*>::const_iterator t = mapNodes.begin(); t != mapNodes.end(); ++t)
    {
        // Only the nodes of type Map are expected here.
        if (t->second->GetType() != YamlNode::TYPE_MAP)
            continue;

        sortedChildren.push_back(OrderedYamlNode(t->first, t->second));
    }

    std::sort(sortedChildren.begin() + lastNotMapItemIndex, sortedChildren.end());
}

bool YamlEmitter::EmitYamlNode( yaml_emitter_t * emitter, const YamlNode * node )
{
    switch( node->GetType() )
    {
    case YamlNode::TYPE_STRING:
        {
            if (!EmitScalar(emitter, node->AsString(), GetYamlScalarStyle(node->representation.stringStyle)))
                return false;
        }
        break;
    case YamlNode::TYPE_ARRAY:
        {
            if (!EmitSequenceStart(emitter, GetYamlSequenceStyle(node->representation.arrayStyle)))
                return false;

            const Vector<YamlNode*> &sequence = node->AsVector();
            Vector<YamlNode*>::const_iterator iter = sequence.begin();
            for( ; iter != sequence.end(); ++iter )
            {
                if (!EmitYamlNode(emitter, (*iter)))
                    return false;
            }

            if (!EmitSequenceEnd(emitter))
                return false;
        }
        break;
    case YamlNode::TYPE_MAP:
        {
            if (!EmitMappingStart(emitter, GetYamlMappingStyle(node->representation.mapStyle.value)))
                return false;

            const MultiMap<String, YamlNode*> &mapNodes = node->AsMap();
            // Order the map nodes by the "Save Index" and write.
            Vector<OrderedYamlNode> sortedMapNodes;
            OrderMapYamlNode(mapNodes, sortedMapNodes);
            Vector<OrderedYamlNode>::const_iterator iter = sortedMapNodes.begin();
            for ( ;iter != sortedMapNodes.end(); ++iter )
            {
                if (iter->GetNodeName() == YamlNode::SAVE_INDEX_NAME)
                    continue;
                if (!EmitScalar(emitter, iter->GetNodeName(), GetYamlScalarStyle(node->representation.mapStyle.key)))
                    return false;
                if (!EmitYamlNode(emitter, iter->GetNodeValue()))
                    return false;
            }

            if (!EmitMappingEnd(emitter))
                return false;
        }
        break;
    }

    return true;
}

bool YamlEmitter::EmitStreamStart( yaml_emitter_t * emitter )
{
    yaml_event_t event;
    if (!yaml_stream_start_event_initialize(&event, YAML_UTF8_ENCODING) ||
        !yaml_emitter_emit(emitter, &event))
        return false;

    return true;
}

bool YamlEmitter::EmitStreamEnd(yaml_emitter_t * emitter)
{
    yaml_event_t event;
    if (!yaml_stream_end_event_initialize(&event) ||
        !yaml_emitter_emit(emitter, &event))
        return false;

    return true;
}

bool YamlEmitter::EmitDocumentStart(yaml_emitter_t * emitter)
{
    yaml_event_t event;
    if (!yaml_document_start_event_initialize(&event, NULL, NULL, NULL, 1) ||
        !yaml_emitter_emit(emitter, &event))
        return false;

    return true;
}

bool YamlEmitter::EmitDocumentEnd(yaml_emitter_t * emitter)
{
    yaml_event_t event;
    if (!yaml_document_end_event_initialize(&event, 1) ||
        !yaml_emitter_emit(emitter, &event))
        return false;

    return true;
}

bool YamlEmitter::EmitSequenceStart(yaml_emitter_t * emitter, int32 sequenceStyle)
{
    yaml_event_t event;
    if (!yaml_sequence_start_event_initialize(&event, NULL, NULL, 0, (yaml_sequence_style_t)sequenceStyle) ||
        !yaml_emitter_emit(emitter, &event))
        return false;

    return true;
}

bool YamlEmitter::EmitSequenceEnd(yaml_emitter_t * emitter)
{
    yaml_event_t event;
    if (!yaml_sequence_end_event_initialize(&event) ||
        !yaml_emitter_emit(emitter, &event))
        return false;

    return true;
}

bool YamlEmitter::EmitMappingStart(yaml_emitter_t * emitter, int32 mappingStyle)
{
    yaml_event_t event;
    if (!yaml_mapping_start_event_initialize(&event, NULL, NULL, 0, (yaml_mapping_style_t)mappingStyle) ||
        !yaml_emitter_emit(emitter, &event))
        return false;

    return true;
}

bool YamlEmitter::EmitMappingEnd(yaml_emitter_t * emitter)
{
    yaml_event_t event;
    if (!yaml_mapping_end_event_initialize(&event) ||
        !yaml_emitter_emit(emitter, &event))
        return false;

    return true;
}

bool YamlEmitter::EmitScalar(yaml_emitter_t * emitter, const String &value, int32 scalarStyle)
{
    yaml_event_t event;
    if (!yaml_scalar_event_initialize(&event, NULL, (yaml_char_t *)YAML_DEFAULT_SCALAR_TAG, (yaml_char_t *)value.c_str(), -1, 1, 1, (yaml_scalar_style_t)scalarStyle) ||
        !yaml_emitter_emit(emitter, &event))
        return false;

    return true;
}

}