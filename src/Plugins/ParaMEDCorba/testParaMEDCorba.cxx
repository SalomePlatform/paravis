#include "vtkUnstructuredGrid.h"
#include "vtkParaMEDCorbaSource.h"

int main( int argc, char* argv[] )
{
  const char* fname = argc > 1 ? argv[1] : "data.iorp";

  vtkParaMEDCorbaSource* efr = vtkParaMEDCorbaSource::New();
  //efr->SetFileName( (char *)fname );
  efr->Update();
  //
  vtkDataObject* ef = efr->GetOutputDataObject(0);
  //
  efr->Delete();
  return 0;
}
