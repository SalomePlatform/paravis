#include "vtkActor.h"
#include "vtkCamera.h"
#include "vtkProperty.h"
#include "vtkCompositePolyDataMapper.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkCylinderSource.h"
#include "vtkMedReader.h"
#include "vtkDataObject.h"
#include "vtkDataSetAttributes.h"
#include "vtkPolyDataNormals.h"
#include "vtkMedReader.h"

#include "vtkTestUtilities.h"
#include "vtkRegressionTestImage.h"

int main(int argc, char *argv[])
{
  vtkMedReader* reader = vtkMedReader::New();
  reader->SetFileName(argv[1]);
  reader->Update();

  vtkCompositePolyDataMapper *mapper = vtkCompositePolyDataMapper::New();
  mapper->SetInputConnection(reader->GetOutputPort());

  vtkActor *actor = vtkActor::New();
  actor->SetMapper(mapper);

  vtkRenderer *renderer = vtkRenderer::New();
  renderer->AddActor(actor);
  renderer->SetBackground(0.5, 0.5, 0.5);

  vtkRenderWindow *renWin = vtkRenderWindow::New();
  renWin->AddRenderer(renderer);

  vtkRenderWindowInteractor *interactor = vtkRenderWindowInteractor::New();
  interactor->SetRenderWindow(renWin);

  renWin->SetSize(400,400);
  renWin->Render();
  interactor->Initialize();
  renderer->ResetCamera();
  renWin->Render();
  renderer->ResetCamera();

  int retVal = vtkRegressionTestImageThreshold(renWin,18);
  if( retVal == vtkRegressionTester::DO_INTERACTOR)
    {
    interactor->Start();
    }

  reader->Delete();
  mapper->Delete();
  actor->Delete();
  renderer->Delete();
  renWin->Delete();
  interactor->Delete();

  return !retVal;
}
