// Copyright (C) 2022-2024  CEA, EDF
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "vtkFileSeriesGroupReader.h"

#include <vtkCollection.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkMultiProcessController.h>
#include <vtkObjectFactory.h>
#include <vtkSmartPointer.h>
#include <vtkStreamingDemandDrivenPipeline.h>

#include "vtkMEDReader.h"

#include <vector>
#include <string>

vtkStandardNewMacro(vtkFileSeriesGroupReader);

//=============================================================================
struct vtkFileSeriesGroupReaderInternals
{
  std::vector<std::string> FileNames;
  vtkNew<vtkCollection> ReaderCollection;
};

//=============================================================================
vtkFileSeriesGroupReader::vtkFileSeriesGroupReader()
  : Internals(new vtkFileSeriesGroupReaderInternals())
{
  this->SetNumberOfInputPorts(0);
  this->SetNumberOfOutputPorts(1);
}

//-----------------------------------------------------------------------------
vtkFileSeriesGroupReader::~vtkFileSeriesGroupReader() = default;

//----------------------------------------------------------------------------
void vtkFileSeriesGroupReader::AddFileName(const char* name)
{
  // Make sure the reader always has a filename set
  this->ReaderSetFileName(name);

  this->AddFileNameInternal(name);
  this->Modified();
}

//----------------------------------------------------------------------------
void vtkFileSeriesGroupReader::RemoveAllFileNames()
{
  this->RemoveAllFileNamesInternal();
  this->Modified();
}

//----------------------------------------------------------------------------
void vtkFileSeriesGroupReader::RemoveAllFileNamesInternal()
{
  this->Internals->FileNames.clear();
}

//----------------------------------------------------------------------------
void vtkFileSeriesGroupReader::AddFileNameInternal(const char* name)
{
  this->Internals->FileNames.emplace_back(name);
}

//----------------------------------------------------------------------------
unsigned int vtkFileSeriesGroupReader::GetNumberOfFileNames()
{
  return static_cast<unsigned int>(this->Internals->FileNames.size());
}

//----------------------------------------------------------------------------
const char* vtkFileSeriesGroupReader::GetFileName(unsigned int idx)
{
  if (idx >= this->Internals->FileNames.size())
  {
    return nullptr;
  }
  return this->Internals->FileNames[idx].c_str();
}

//----------------------------------------------------------------------------
int vtkFileSeriesGroupReader::CanReadFile(const char* filename)
{
  if (!this->Reader)
  {
    return 0;
  }

  return this->ReaderCanReadFile(filename);
}

//----------------------------------------------------------------------------
int vtkFileSeriesGroupReader::RequestInformation(
  vtkInformation* request, vtkInformationVector** inputVector, vtkInformationVector* outputVector)
{
  return this->Reader->ProcessRequest(request, inputVector, outputVector);
}

//----------------------------------------------------------------------------
int vtkFileSeriesGroupReader::RequestData(vtkInformation* vtkNotUsed(request),
  vtkInformationVector** vtkNotUsed(inputVector), vtkInformationVector* outputVector)
{
  auto output = vtkMultiBlockDataSet::GetData(outputVector, 0);
  unsigned int nBlock = this->GetNumberOfFileNames();
  output->SetNumberOfBlocks(nBlock);

  vtkInformation* info = outputVector->GetInformationObject(0);
  double time = info->Get(vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEP());

  vtkMultiProcessController *vmpc(vtkMultiProcessController::GetGlobalController());
  unsigned int iProc = vmpc ? vmpc->GetLocalProcessId() : 0;
  unsigned int nProc = vmpc ? vmpc->GetNumberOfProcesses() : 1;

  // Simple case, one file/bloc for n proc
  if (nBlock == 1)
  {
    // Make sure the information is up to date
    this->ReaderSetFileName(this->GetFileName(0));
    this->Reader->UpdateInformation();

    this->Reader->UpdateTimeStep(time);
    vtkDataObject* outputReader = vtkMultiBlockDataSet::SafeDownCast(this->Reader->GetOutputDataObject(0))->GetBlock(0);
    output->SetBlock(0, outputReader);

    // Copy the GAUSS_DATA info key
    vtkInformation* mInfo = this->Reader->GetOutputInformation(0);
    if (mInfo->Has(vtkMEDReader::GAUSS_DATA()))
    {
      info->CopyEntry(mInfo, vtkMEDReader::GAUSS_DATA());
    }
  }
  // N file/block read by m proc, with n <= m, means 0/1 file/block per proc
  else if (nBlock <= nProc && iProc < nBlock)
  {
    // Distribute in MEDReader only when reading a single file in a single block
    vtkMEDReader::SafeDownCast(this->Reader)->SetDistributeWithMPI(false);

    // Needed as the MEDReader do not support changing its filename
    // without reloading everything.
    this->ReaderSetFileName(this->GetFileName(iProc));
    vtkMEDReader::SafeDownCast(this->Reader)->ReloadInternals();
    this->Reader->UpdateInformation();

    this->Reader->UpdateTimeStep(time);
    vtkDataObject* outputReader = vtkMultiBlockDataSet::SafeDownCast(this->Reader->GetOutputDataObject(0))->GetBlock(0);
    output->SetBlock(iProc, outputReader);

    // Copy the GAUSS_DATA info key
    vtkInformation* mInfo = this->Reader->GetOutputInformation(0);
    if (mInfo->Has(vtkMEDReader::GAUSS_DATA()))
    {
      info->CopyEntry(mInfo, vtkMEDReader::GAUSS_DATA());
    }
  }
  // Multiple files/block per proc
  else
  {
    unsigned int nFiles = nBlock / nProc;
    unsigned int offFile = iProc * nFiles;
    unsigned int supFiles = nBlock % nProc;

    // Last proc handle remaining files/block
    if (iProc + 1 == nProc)
    {
      nFiles += supFiles;
    }

    vtkMEDReader* exposedReader = vtkMEDReader::SafeDownCast(this->Reader);
    this->Internals->ReaderCollection->RemoveAllItems();
    for (unsigned int i = 0; i < nFiles; i++)
    {
      // Create as many MEDReader as we need to avoid deep copy
      vtkNew<vtkMEDReader> localReader;
      this->Internals->ReaderCollection->AddItem(localReader.Get());

      for (int iField = 0; iField < exposedReader->GetNumberOfFieldsTreeArrays(); iField++)
      {
	const char* name = exposedReader->GetFieldsTreeArrayName(iField);
        localReader->SetFieldsStatus(name, exposedReader->GetFieldsTreeArrayStatus(name));
      }
      for (int iTimes = 0; iTimes < exposedReader->GetNumberOfTimesFlagsArrays(); iTimes++)
      {
	const char* name = exposedReader->GetTimesFlagsArrayName(iTimes);
        localReader->SetTimesFlagsStatus(name, exposedReader->GetTimesFlagsArrayStatus(name));
      }
      localReader->GenerateVectors(exposedReader->GetGenerateVect());
      localReader->ChangeMode(exposedReader->GetIsStdOrMode());
      localReader->GhostCellGeneratorCallForPara(exposedReader->GetGCGCP());
      localReader->GetRidOffDebugArrays(exposedReader->GetRemoveDebugArrays());

      // Configure the localReader for usage with the files
      localReader->SetFileName(this->GetFileName(i + offFile));
      localReader->SetDistributeWithMPI(false);
      localReader->UpdateInformation();
      localReader->UpdateTimeStep(time);

      vtkDataObject* outputReader = vtkMultiBlockDataSet::SafeDownCast(localReader->GetOutputDataObject(0))->GetBlock(0);
      output->SetBlock(i + offFile, outputReader);

      if (i == 0)
      {
        // Copy the GAUSS_DATA info key of the first filename
        vtkInformation* mInfo = localReader->GetOutputInformation(0);
        if (mInfo->Has(vtkMEDReader::GAUSS_DATA()))
        {
          info->CopyEntry(mInfo, vtkMEDReader::GAUSS_DATA());
        }
      }
    }
  }
  return 1;
}

//------------------------------------------------------------------------------
int vtkFileSeriesGroupReader::FillOutputPortInformation(
  int vtkNotUsed(port), vtkInformation* info)
{
  info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkMultiBlockDataSet");
  return 1;
}

//-----------------------------------------------------------------------------
void vtkFileSeriesGroupReader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  os << indent << "MetaFileName: " << (this->_MetaFileName ? this->_MetaFileName : "(none)")
     << endl;
}
