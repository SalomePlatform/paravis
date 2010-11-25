/*=========================================================================

 Program:   ParaView
 Module:    $RCSfile$

 Copyright (c) Kitware, Inc.
 All rights reserved.
 See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/
#include "vtkSMInformationKeyDomainHelper.h"
#include "vtkClientServerStream.h"
#include "vtkObjectFactory.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkCompositeDataIterator.h"
#include "vtkDataSetAttributes.h"
#include "vtkAbstractArray.h"
#include "vtkInformationIterator.h"
#include "vtkInformationKey.h"

vtkCxxRevisionMacro(vtkSMInformationKeyDomainHelper, "$Revision$")
;
vtkStandardNewMacro(vtkSMInformationKeyDomainHelper)
;

#include <vtkstd/map>
#include <vtkstd/string>
#include <vtkstd/set>
#include <vtkstd/utility>

typedef vtkstd::pair<vtkstd::string, vtkstd::string> Key;
typedef vtkstd::set<Key> KeySet;
typedef vtkstd::map<vtkstd::string, KeySet> FieldKeysMap;
typedef vtkstd::map<int, FieldKeysMap> TypedFieldKeysMap;

class vtkSMInformationKeyDomainHelperInternal
{
public:
  // For each field type
  // for each field name
  // store a set keys given by the pair location/name
  TypedFieldKeysMap InformationKeys;
};

//---------------------------------------------------------------------------
vtkSMInformationKeyDomainHelper::vtkSMInformationKeyDomainHelper()
{
  this->Internal = new vtkSMInformationKeyDomainHelperInternal();
  for(int type = 0; type < vtkDataObject::NUMBER_OF_ATTRIBUTE_TYPES; type++)
    {
    this->Internal->InformationKeys[type] = FieldKeysMap();
    }
}

//---------------------------------------------------------------------------
vtkSMInformationKeyDomainHelper::~vtkSMInformationKeyDomainHelper()
{
  delete this->Internal;
}

int vtkSMInformationKeyDomainHelper::GetNumberOfFields(int attributeType)
{
  if(this->Internal->InformationKeys.find(attributeType) == this->Internal->InformationKeys.end())
    return 0;
  return this->Internal->InformationKeys[attributeType].size();
}

const char* vtkSMInformationKeyDomainHelper::GetFieldName(int attributeType, int fieldId)
{
  if(this->Internal->InformationKeys.find(attributeType) == this->Internal->InformationKeys.end())
    return NULL;
  FieldKeysMap& fields = this->Internal->InformationKeys[attributeType];
  if(fieldId < 0 || fieldId >= fields.size())
    return NULL;

  FieldKeysMap::iterator it = fields.begin();
  for (int ii=0; ii<fieldId; ii++)
    it++;

  return it->first.c_str();
}

int vtkSMInformationKeyDomainHelper::InitializeFromStream(
    const vtkClientServerStream& stream)
{
  this->Internal->InformationKeys.clear();
  int arg = 0;
  int ntypes;
  stream.GetArgument(0, arg, &ntypes);
  arg++;
  for(int tid = 0; tid < ntypes; tid++)
    {
    int type;
    stream.GetArgument(0, arg, &type);
    arg++;
    this->Internal->InformationKeys[type] = FieldKeysMap();
    FieldKeysMap& fields = this->Internal->InformationKeys[type];
    int nfield;
    stream.GetArgument(0, arg, &nfield);
    arg++;
    for(int fid = 0; fid < nfield; fid++)
      {
      char* fieldName;
      stream.GetArgument(0, arg, &fieldName);
      arg++;
      vtkstd::string fnamestring = fieldName;
      fields[fnamestring] = KeySet();
      KeySet& keySet = fields[fnamestring];

      int nkeys;
      stream.GetArgument(0, arg, &nkeys);
      arg++;
      for(int kid = 0; kid < nkeys; kid++)
        {
        char* keyName;
        stream.GetArgument(0, arg, &keyName);
        arg++;
        vtkstd::string knamestring = keyName;

        char* locName;
        stream.GetArgument(0, arg, &locName);
        arg++;
        vtkstd::string locnamestring = locName;

        Key key(locnamestring, knamestring);
        keySet.insert(key);
        }
      }
    }
  return 1;
}

int vtkSMInformationKeyDomainHelper::CopyToStream(vtkDataObject* data,
    vtkClientServerStream& stream)
{
  vtkMultiBlockDataSet* mb = vtkMultiBlockDataSet::SafeDownCast(data);
  if(mb != NULL)
    {
    vtkCompositeDataIterator* it = mb->NewIterator();
    it->VisitOnlyLeavesOn();
    while(!it->IsDoneWithTraversal())
      {
      vtkDataObject* obj = it->GetCurrentDataObject();
      this->AppendLeaf(obj);
      it->GoToNextItem();
      }
    it->Delete();
    }
  else
    {
    this->AppendLeaf(data);
    }
  stream << vtkClientServerStream::Reply;
  stream << this->Internal->InformationKeys.size();
  TypedFieldKeysMap::iterator typeit = this->Internal->InformationKeys.begin();
  while(typeit != this->Internal->InformationKeys.end())
    {
    stream << typeit->first;
    FieldKeysMap& fields = typeit->second;
    stream << fields.size();
    FieldKeysMap::iterator fieldit = fields.begin();
    while(fieldit != fields.end())
      {
      stream << fieldit->first.c_str();
      KeySet& keySet = fieldit->second;
      stream << keySet.size();
      KeySet::iterator keyit = keySet.begin();
      while(keyit != keySet.end())
        {
        Key k = *keyit;
        stream << k.first.c_str() << k.second.c_str();
        keyit++;
        }
      fieldit++;
      }
    typeit++;
    }
  stream << vtkClientServerStream::End;
  return 1;
}

//----------------------------------------------------------------------------
void vtkSMInformationKeyDomainHelper::AppendLeaf(vtkDataObject* data)
{
  for(int type = 0; type < vtkDataObject::NUMBER_OF_ATTRIBUTE_TYPES; type++)
    {
    vtkDataSetAttributes* attr = data->GetAttributes(type);
    if(attr == NULL)
      continue;

    FieldKeysMap& fieldKeysMap = this->Internal->InformationKeys[type];

    for(int fid = 0; fid < attr->GetNumberOfArrays(); fid++)
      {
      vtkAbstractArray* array = attr->GetAbstractArray(fid);
      vtkstd::string arrayname = array->GetName();
      if(fieldKeysMap.find(arrayname) == fieldKeysMap.end())
        {
        fieldKeysMap[arrayname] = KeySet();
        }
      KeySet& keySet = fieldKeysMap[arrayname];
      if(array->HasInformation())
        {
        vtkInformation* info = array->GetInformation();
        vtkInformationIterator* it = vtkInformationIterator::New();
        it->SetInformation(info);
        it->InitTraversal();
        while(!it->IsDoneWithTraversal())
          {
          vtkInformationKey* key = it->GetCurrentKey();
          keySet.insert(Key(key->GetLocation(), key->GetName()));
          it->GoToNextItem();
          }
        it->Delete();
        }
      }
    }
}

int vtkSMInformationKeyDomainHelper::FieldHasKey(const char* field,
    const char* location, const char* name)
{
  TypedFieldKeysMap::iterator typeit = this->Internal->InformationKeys.begin();
  while(typeit != this->Internal->InformationKeys.end())
    {
    FieldKeysMap& fields = typeit->second;
    if(fields.find(field) == fields.end())
      continue;
    KeySet& keySet = fields[field];
    Key k = Key(location, name);
    if(keySet.find(k) != keySet.end())
      return 1;
    typeit++;
    }
  return 0;
}

//---------------------------------------------------------------------------
void vtkSMInformationKeyDomainHelper::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
