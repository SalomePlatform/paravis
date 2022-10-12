// Copyright (C) 2022  CEA/DEN, EDF R&D
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

#ifndef vtkFileSeriesGroupReader_h
#define vtkFileSeriesGroupReader_h

#include "vtkMetaReader.h"

#include <memory>

struct vtkFileSeriesGroupReaderInternals;

class VTK_EXPORT vtkFileSeriesGroupReader : public vtkMetaReader
{
public:
  static vtkFileSeriesGroupReader* New();
  vtkTypeMacro(vtkFileSeriesGroupReader, vtkMetaReader);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /**
   * CanReadFile is forwarded to the internal reader if it supports it.
   */
  virtual int CanReadFile(const char* filename);

  /**
   * Adds names of files to be read. The files are read in the order
   * they are added.
   */
  virtual void AddFileName(const char* fname);

  /**
   * Remove all file names.
   */
  virtual void RemoveAllFileNames();

  /**
   * Returns the number of file names added by AddFileName.
   */
  virtual unsigned int GetNumberOfFileNames();

  /**
   * Returns the name of a file with index idx.
   */
  virtual const char* GetFileName(unsigned int idx);

protected:
  vtkFileSeriesGroupReader();
  ~vtkFileSeriesGroupReader() override;

  /**
   * Add/Remove filenames without changing the MTime.
   */
  void RemoveAllFileNamesInternal();
  void AddFileNameInternal(const char*);

  int RequestInformation(vtkInformation* request, vtkInformationVector** inputVector, vtkInformationVector* outputVector);
  int RequestData(vtkInformation* vtkNotUsed(request), vtkInformationVector** inputVector, vtkInformationVector* outputVector);
  int FillOutputPortInformation(int vtkNotUsed(port), vtkInformation* info);

private:
  vtkFileSeriesGroupReader(const vtkFileSeriesGroupReader&) = delete;
  void operator=(const vtkFileSeriesGroupReader&) = delete;

  std::unique_ptr<vtkFileSeriesGroupReaderInternals> Internals;
};

#endif
