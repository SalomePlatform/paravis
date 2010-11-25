#include "vtkMedQuadratureDefinition.h"

#include "vtkObjectFactory.h"

#include "vtkMedUtilities.h"
#include "vtkMedSetGet.h"
#include "vtkMedString.h"

#include "med.h"

vtkCxxRevisionMacro(vtkMedQuadratureDefinition, "$Revision$")
vtkStandardNewMacro(vtkMedQuadratureDefinition)

vtkMedQuadratureDefinition::vtkMedQuadratureDefinition()
{
	this->Geometry = MED_NONE;
	this->NumberOfQuadraturePoint = 0;
	this->Weights = NULL;
	this->PointLocalCoordinates = NULL;
	this->QuadraturePointLocalCoordinates = NULL;
	this->ShapeFunction = NULL;
	this->Name = vtkMedString::New();
	this->Name->SetSize(MED_TAILLE_NOM);
	this->MedIndex = -1;
}

vtkMedQuadratureDefinition::~vtkMedQuadratureDefinition()
{
	this->Name->Delete();
	this->ClearResources();
}

void vtkMedQuadratureDefinition::ClearResources()
{
	if (this->Weights)
		delete[] this->Weights;
	this->Weights = NULL;

	if (this->PointLocalCoordinates)
		delete[] this->PointLocalCoordinates;
	this->PointLocalCoordinates = NULL;

	if (this->QuadraturePointLocalCoordinates)
		delete[] this->QuadraturePointLocalCoordinates;
	this->QuadraturePointLocalCoordinates = NULL;

	if (this->ShapeFunction)
		delete[] this->ShapeFunction;
	this->ShapeFunction = NULL;
}

int vtkMedQuadratureDefinition::IsLoaded()
{
	return this->Weights != NULL && this->PointLocalCoordinates != NULL
			&& this->QuadraturePointLocalCoordinates != NULL && this->ShapeFunction
			!= NULL;
}

void vtkMedQuadratureDefinition::SecureResources()
{
	this->ClearResources();

	this->ShapeFunction = new double[this->GetSizeOfShapeFunction()];
	this->Weights = new double[this->GetSizeOfWeights()];
	this->QuadraturePointLocalCoordinates
			= new double[this->GetSizeOfQuadraturePointLocalCoordinates()];
	this->PointLocalCoordinates
			= new double[this->GetSizeOfPointLocalCoordinates()];
}

// compute the size of the coordinates array as follow
// (type_geo%100) give the number of node from the cell geo type
// (type_geo/100) give the dimension from the cell geo type
// example:
//  	MED_HEXA20=320
//		(type_geo%100) = 20 nodes
//		(type_geo/100) = 3
//		Total size = 20*3 = 60

int vtkMedQuadratureDefinition::GetSizeOfWeights()
{
	return this->NumberOfQuadraturePoint;
}

int vtkMedQuadratureDefinition::GetSizeOfPointLocalCoordinates()
{
	return vtkMedUtilities::GetNumberOfPoint(this->Geometry)
			* vtkMedUtilities::GetDimension(this->Geometry);
}

int vtkMedQuadratureDefinition::GetSizeOfQuadraturePointLocalCoordinates()
{
	return this->NumberOfQuadraturePoint * vtkMedUtilities::GetDimension(
			this->Geometry);
}

int vtkMedQuadratureDefinition::GetSizeOfShapeFunction()
{
	return this->NumberOfQuadraturePoint * vtkMedUtilities::GetNumberOfPoint(
			this->Geometry);
}

void vtkMedQuadratureDefinition::BuildShapeFunction()
{
	switch (this->Geometry)
	{
	case MED_POINT1:
		return BuildPoint1();
	case MED_SEG2:
		return BuildSeg2();
	case MED_SEG3:
		return BuildSeg3();
	case MED_TRIA3:
		return BuildTria3();
	case MED_TRIA6:
		return BuildTria6();
	case MED_QUAD4:
		return BuildQuad4();
	case MED_QUAD8:
		return BuildQuad8();
	case MED_HEXA8:
		return BuildHexa8();
	case MED_HEXA20:
		return BuildHexa20();
	case MED_TETRA4:
		return BuildTetra4();
	case MED_TETRA10:
		return BuildTetra10();
	case MED_PENTA6:
		return BuildPenta6();
	case MED_PENTA15:
		return BuildPenta15();
	case MED_PYRA5:
		return BuildPyra5();
	case MED_PYRA13:
		return BuildPyra13();
	default:
		vtkErrorMacro("ERROR in vtkMedQuadratureDefinition::BuildShapeFunction => Cell geometry not supported !!! "
				<< this->Geometry )
		return;
	}
}

void vtkMedQuadratureDefinition::BuildPoint1()
{
		ShapeFunction[0] =0;
}

void vtkMedQuadratureDefinition::BuildSeg2()
{

	int nnodes = this->Geometry % 100;
	int dim = 1; // 1D <=> (x)
	double x;
	//
	for (int qpIndex = 0; qpIndex < this->NumberOfQuadraturePoint; qpIndex++)
		{
		x = this->QuadraturePointLocalCoordinates[(qpIndex * dim) + 0];
		//                                                       Aster <=> MED
		ShapeFunction[(qpIndex * nnodes) + 0] = 0.5 * (1.0 - x); // 1 <=>  1
		ShapeFunction[(qpIndex * nnodes) + 1] = 0.5 * (1.0 + x); // 2 <=>  2
		}
	return;
}

void vtkMedQuadratureDefinition::BuildSeg3()
{

	int nnodes = this->Geometry % 100;
	int dim = 1; // 1D <=> (x)
	double x;
	//
	for (int qpIndex = 0; qpIndex < this->NumberOfQuadraturePoint; qpIndex++)
		{
		x = this->QuadraturePointLocalCoordinates[(qpIndex * dim) + 0];
		//                                                           Aster <=> MED
		ShapeFunction[(qpIndex * nnodes) + 0] = -0.5 * (1.0 - x) * x; //  1 <=>  1
		ShapeFunction[(qpIndex * nnodes) + 1] = 0.5 * (1.0 + x) * x; //   2 <=>  2
		ShapeFunction[(qpIndex * nnodes) + 2] = (1.0 + x) * (1.0 - x); // 3 <=>  3
		}
	return;
}

void vtkMedQuadratureDefinition::BuildTria3()
{

	int nnodes = this->Geometry % 100;
	int dim = 2; // 2D <=> (x,y)
	double x, y;
	//
	for (int qpIndex = 0; qpIndex < this->NumberOfQuadraturePoint; qpIndex++)
		{
		x = this->QuadraturePointLocalCoordinates[(qpIndex * dim) + 0];
		y = this->QuadraturePointLocalCoordinates[(qpIndex * dim) + 1];
		//                                                 Aster <=> MED
		ShapeFunction[(qpIndex * nnodes) + 0] = 1 - x - y; // 1 <=>  1
		ShapeFunction[(qpIndex * nnodes) + 1] = x; //     2 <=>  2
		ShapeFunction[(qpIndex * nnodes) + 2] = y; //     3 <=>  3
		//
		//std::cout << "Shape function TRIA 3: (" << qpIndex << ") " << ShapeFunction[(qpIndex*nnodes)+ 0] << "\t" << ShapeFunction[(qpIndex*nnodes)+ 1] << "\t" << ShapeFunction[(qpIndex*nnodes)+ 2] << std::endl;
		}
	return;
}

void vtkMedQuadratureDefinition::BuildTria6()
{

	int nnodes = this->Geometry % 100;
	int dim = 2; // 2D <=> (x,y)
	double x, y;
	//
	for (int qpIndex = 0; qpIndex < this->NumberOfQuadraturePoint; qpIndex++)
		{
		x = this->QuadraturePointLocalCoordinates[(qpIndex * dim) + 0];
		y = this->QuadraturePointLocalCoordinates[(qpIndex * dim) + 1];
		//                                                                        Aster <=> MED
		ShapeFunction[(qpIndex * nnodes) + 0] = -(1.0 - x - y) * (1.0 - 2.0 * (1
				- x - y)); // 1 <=>  1
		ShapeFunction[(qpIndex * nnodes) + 1] = -x * (1.0 - 2.0 * x); //               2 <=>  2
		ShapeFunction[(qpIndex * nnodes) + 2] = -y * (1.0 - 2.0 * y); //               3 <=>  3
		ShapeFunction[(qpIndex * nnodes) + 3] = 4.0 * x * (1.0 - x - y); //              4 <=>  4
		ShapeFunction[(qpIndex * nnodes) + 4] = 4.0 * x * y; //                      5 <=>  5
		ShapeFunction[(qpIndex * nnodes) + 5] = 4.0 * y * (1.0 - x - y); //              6 <=>  6
		//
		//std::cout << "Shape function TRIA 6: (" << qpIndex << ") " << ShapeFunction[(qpIndex*nnodes)+ 0] << "\t" << ShapeFunction[(qpIndex*nnodes)+ 1] << "\t" << ShapeFunction[(qpIndex*nnodes)+ 2]<< "\t" << ShapeFunction[(qpIndex*nnodes)+ 3] << "\t" << ShapeFunction[(qpIndex*nnodes)+ 4]<< "\t" << ShapeFunction[(qpIndex*nnodes)+ 5] << std::endl;
		}
	return;
}

void vtkMedQuadratureDefinition::BuildTria7()
{

	int nnodes = this->Geometry % 100;
	int dim = 2; // 2D <=> (x,y)
	double x, y;
	//
	for (int qpIndex = 0; qpIndex < this->NumberOfQuadraturePoint; qpIndex++)
		{
		x = this->QuadraturePointLocalCoordinates[(qpIndex * dim) + 0];
		y = this->QuadraturePointLocalCoordinates[(qpIndex * dim) + 1];
		//                                                                                             Aster <=> MED
		ShapeFunction[(qpIndex * nnodes) + 0] = 1.0 - 3.0 * (x + y) + 2.0 * (x * x
				+ y * y) + 7.0 * x * y - 3.0 * x * y * (x + y); // 1 <=>  1
		ShapeFunction[(qpIndex * nnodes) + 1] = x * (-1.0 + 2.0 * x + 3.0 * y - 3.0
				* y * (x + y)); //                  2 <=>  2
		ShapeFunction[(qpIndex * nnodes) + 2] = y * (-1.0 + 2.0 * x + 3.0 * y - 3.0
				* x * (x + y)); //                  3 <=>  3
		ShapeFunction[(qpIndex * nnodes) + 3] = 4.0 * x * (1.0 - x - 4.0 * y + 3.0
				* y * (x + y)); //                   4 <=>  4
		ShapeFunction[(qpIndex * nnodes) + 4] = 4.0 * x * y
				* (-2.0 + 3.0 * (x + y)); //                          5 <=>  5
		ShapeFunction[(qpIndex * nnodes) + 5] = 4.0 * y * (1 - 4.0 * x - y + 3.0
				* x * (x + y)); //                     6 <=>  6
		ShapeFunction[(qpIndex * nnodes) + 6] = 27.0 * x * y * (1.0 - x - y); //                                7 <=>  7
		//
		//std::cout << "Shape function TRIA 7: (" << qpIndex << ") " << ShapeFunction[(qpIndex*nnodes)+ 0] << "\t" << ShapeFunction[(qpIndex*nnodes)+ 1] << "\t" << ShapeFunction[(qpIndex*nnodes)+ 2]<< "\t" << ShapeFunction[(qpIndex*nnodes)+ 3] << "\t" << ShapeFunction[(qpIndex*nnodes)+ 4]<< "\t" << ShapeFunction[(qpIndex*nnodes)+ 5]<< "\t" << ShapeFunction[(qpIndex*nnodes)+ 6] << std::endl;

		}
	return;
}

void vtkMedQuadratureDefinition::BuildQuad4()
{

	int nnodes = this->Geometry % 100;
	int dim = 2; // 2D <=> (x,y)
	double x, y;
	//
	for (int qpIndex = 0; qpIndex < this->NumberOfQuadraturePoint; qpIndex++)
		{
		x = this->QuadraturePointLocalCoordinates[(qpIndex * dim) + 0];
		y = this->QuadraturePointLocalCoordinates[(qpIndex * dim) + 1];
		//                                                             Aster <=> MED
		ShapeFunction[(qpIndex * nnodes) + 0] = (1 - x) * (1 - y) * 0.250; // 1 <=>  1
		ShapeFunction[(qpIndex * nnodes) + 1] = (1 + x) * (1 - y) * 0.250; // 2 <=>  2
		ShapeFunction[(qpIndex * nnodes) + 2] = (1 + x) * (1 + y) * 0.250; // 3 <=>  3
		ShapeFunction[(qpIndex * nnodes) + 3] = (1 - x) * (1 + y) * 0.250; // 4 <=>  4
		//
		/*std::cout << "Shape function QUAD 4: (" << qpIndex << ") "
		 << ShapeFunction[(qpIndex * nnodes) + 0] << "\t"
		 << ShapeFunction[(qpIndex * nnodes) + 1] << "\t"
		 << ShapeFunction[(qpIndex * nnodes) + 2] << "\t"
		 << ShapeFunction[(qpIndex * nnodes) + 3] << std::endl;*/

		}
	return;
}

void vtkMedQuadratureDefinition::BuildQuad8()
{

	int nnodes = this->Geometry % 100;
	int dim = 2; // 3D <=> (x,y,z)
	double x, y;
	//
	for (int qpIndex = 0; qpIndex < this->NumberOfQuadraturePoint; qpIndex++)
		{
		x = this->QuadraturePointLocalCoordinates[(qpIndex * dim) + 0];
		y = this->QuadraturePointLocalCoordinates[(qpIndex * dim) + 1];
		//                                                                           Aster <=> MED
		ShapeFunction[(qpIndex * nnodes) + 0] = (1.0 - x) * (1.0 - y) * (-1.0 - x
				- y) * 0.25; // 1 <=>  1
		ShapeFunction[(qpIndex * nnodes) + 1] = (1.0 + x) * (1.0 - y) * (-1.0 + x
				- y) * 0.25; // 2 <=>  2
		ShapeFunction[(qpIndex * nnodes) + 2] = (1.0 + x) * (1.0 + y) * (-1.0 + x
				+ y) * 0.25; // 3 <=>  3
		ShapeFunction[(qpIndex * nnodes) + 3] = (1.0 - x) * (1.0 + y) * (-1.0 - x
				+ y) * 0.25; // 4 <=>  4
		ShapeFunction[(qpIndex * nnodes) + 4] = (1 - x * x) * (1 - y) * 0.5; //               5 <=>  5
		ShapeFunction[(qpIndex * nnodes) + 5] = (1.0 + x) * (1.0 - y * y) * 0.5; //           6 <=>  6
		ShapeFunction[(qpIndex * nnodes) + 6] = (1.0 - x * x) * (1.0 + y) * 0.5; //           7 <=>  7
		ShapeFunction[(qpIndex * nnodes) + 7] = (1.0 - x) * (1.0 - y * y) * 0.5; //           8 <=>  8
		//
		//std::cout << "Shape function QUAD 8: (" << qpIndex << ") " << ShapeFunction[(qpIndex*nnodes)+ 0] << "\t" << ShapeFunction[(qpIndex*nnodes)+ 1] << "\t" << ShapeFunction[(qpIndex*nnodes)+ 2]<< "\t" << ShapeFunction[(qpIndex*nnodes)+ 3] << std::endl;

		}
	return;
}

void vtkMedQuadratureDefinition::BuildPenta6()
{

	int nnodes = this->Geometry % 100;
	int dim = this->Geometry / 100; // 3D <=> (x,y,z)
	double x, y, z;
	//
	//ReferenceEntity->Print();
	//
	for (int qpIndex = 0; qpIndex < this->NumberOfQuadraturePoint; qpIndex++)
		{
		x = this->QuadraturePointLocalCoordinates[(qpIndex * dim) + 0];
		y = this->QuadraturePointLocalCoordinates[(qpIndex * dim) + 1];
		z = this->QuadraturePointLocalCoordinates[(qpIndex * dim) + 2];
		//                                                            Aster <=> MED
		ShapeFunction[(qpIndex * nnodes) + 0] = 0.5 * y * (1 - x); //       1 <=> 1
		ShapeFunction[(qpIndex * nnodes) + 2] = 0.5 * z * (1 - x); //       2 <=> 3
		ShapeFunction[(qpIndex * nnodes) + 1] = 0.5 * (1 - y - z) * (1 - x); // 3 <=> 2
		ShapeFunction[(qpIndex * nnodes) + 3] = 0.5 * y * (1 + x); //       4 <=> 4
		ShapeFunction[(qpIndex * nnodes) + 5] = 0.5 * z * (1 + x); //       5 <=> 6
		ShapeFunction[(qpIndex * nnodes) + 4] = 0.5 * (1 - y - z) * (1 + x); // 6 <=> 5
		//
		//std::cout << "Shape function Penta 6: (" << qpIndex << ") " << ShapeFunction[(qpIndex*nnodes)+ 0] << "\t" << ShapeFunction[(qpIndex*nnodes)+ 1] << "\t" << ShapeFunction[(qpIndex*nnodes)+ 2]<< "\t" << ShapeFunction[(qpIndex*nnodes)+ 3]<< "\t" << ShapeFunction[(qpIndex*nnodes)+ 4]<< "\t" << ShapeFunction[(qpIndex*nnodes)+ 5] << std::endl;

		}

	return;
}

void vtkMedQuadratureDefinition::BuildPenta15()
{

	int nnodes = this->Geometry % 100;
	int dim = this->Geometry / 100; // 3D <=> (x,y,z)
	double x, y, z;
	//
	//ReferenceEntity->Print();
	//
	for (int qpIndex = 0; qpIndex < this->NumberOfQuadraturePoint; qpIndex++)
		{
		x = this->QuadraturePointLocalCoordinates[(qpIndex * dim) + 0];
		y = this->QuadraturePointLocalCoordinates[(qpIndex * dim) + 1];
		z = this->QuadraturePointLocalCoordinates[(qpIndex * dim) + 2]; // Aster <=> MED
		ShapeFunction[(qpIndex * nnodes) + 0] = y * (1 - x) * (2* y - 2 - x) * 0.5; //                            1 <=>  1
		ShapeFunction[(qpIndex * nnodes) + 2] = z * (1 - x) * (2* z - 2 - x) * 0.5; //	                          2 <=>  3
		ShapeFunction[(qpIndex * nnodes) + 1] = (x - 1) * (1 - y - z) * (x + 2* y
				+ 2* z ) * 0.5; //                    3 <=>  2
		ShapeFunction[(qpIndex * nnodes) + 3] = y * (1 + x) * (2* y - 2 + x) * 0.5; //	                          4 <=>  4
		ShapeFunction[(qpIndex * nnodes) + 5] = z * (1 + x) * (2* z - 2 + x) * 0.5; //                            5 <=>  6
		ShapeFunction[(qpIndex * nnodes) + 4] = (-x - 1) * (1 - y - z) * (-x + 2* y
				+ 2* z ) * 0.5; //                  6 <=>  5
		ShapeFunction[(qpIndex * nnodes) + 8] = 2.0 * y * z * (1 - x); //                                    7 <=>  9
		ShapeFunction[(qpIndex * nnodes) + 7] = 2.0 * z * (1 - y - z) * (1 - x); //                              8 <=>  8
		ShapeFunction[(qpIndex * nnodes) + 6] = 2.0 * y * (1 - y - z) * (1 - x); //                              9 <=>  7
		ShapeFunction[(qpIndex * nnodes) + 12] = y * (1 - x * x); //                                      10 <=> 13
		ShapeFunction[(qpIndex * nnodes) + 14] = z * (1 - x * x); //                                      11 <=> 15
		ShapeFunction[(qpIndex * nnodes) + 13] = (1 - y - z) * (1 - x * x); //                                12 <=> 14
		ShapeFunction[(qpIndex * nnodes) + 11] = 2.0 * y * z * (1 + x); //                                   13 <=> 12
		ShapeFunction[(qpIndex * nnodes) + 10] = 2.0 * z * (1 - y - z) * (1 + x); //                            14 <=> 11
		ShapeFunction[(qpIndex * nnodes) + 9] = 2.0 * y * (1 - y - z) * (1 + x); //                            15 <=> 10
		}

	return;
}

void vtkMedQuadratureDefinition::BuildHexa8()
{

	int nnodes = this->Geometry % 100;
	int dim = this->Geometry / 100; // 3D <=> (x,y,z)
	double x, y, z;
	//
	//ReferenceEntity->Print();
	//
	for (int qpIndex = 0; qpIndex < this->NumberOfQuadraturePoint; qpIndex++)
		{
		x = this->QuadraturePointLocalCoordinates[(qpIndex * dim) + 0];
		y = this->QuadraturePointLocalCoordinates[(qpIndex * dim) + 1];
		z = this->QuadraturePointLocalCoordinates[(qpIndex * dim) + 2];
		//                                                                       Aster <=> MED
		ShapeFunction[(qpIndex * nnodes) + 0] = 0.125 * (1 - x) * (1 - y) * (1 - z); //     1 <=> 1
		ShapeFunction[(qpIndex * nnodes) + 1] = 0.125 * (1 + x) * (1 - y) * (1 - z); //     2 <=> 2
		ShapeFunction[(qpIndex * nnodes) + 2] = 0.125 * (1 + x) * (1 + y) * (1 - z); //     3 <=> 3
		ShapeFunction[(qpIndex * nnodes) + 3] = 0.125 * (1 - x) * (1 + y) * (1 - z); //     4 <=> 4
		ShapeFunction[(qpIndex * nnodes) + 4] = 0.125 * (1 - x) * (1 - y) * (1 + z); //     5 <=> 5
		ShapeFunction[(qpIndex * nnodes) + 5] = 0.125 * (1 + x) * (1 - y) * (1 + z); //     6 <=> 6
		ShapeFunction[(qpIndex * nnodes) + 6] = 0.125 * (1 + x) * (1 + y) * (1 + z); //     7 <=> 7
		ShapeFunction[(qpIndex * nnodes) + 7] = 0.125 * (1 - x) * (1 + y) * (1 + z); //     8 <=> 8
		}

	return;
}

void vtkMedQuadratureDefinition::BuildHexa20()
{

	int nnodes = this->Geometry % 100;
	int dim = this->Geometry / 100; // 3D <=> (x,y,z)
	double x, y, z;
	//
	for (int qpIndex = 0; qpIndex < this->NumberOfQuadraturePoint; qpIndex++)
		{
		x = this->QuadraturePointLocalCoordinates[(qpIndex * dim) + 0];
		y = this->QuadraturePointLocalCoordinates[(qpIndex * dim) + 1];
		z = this->QuadraturePointLocalCoordinates[(qpIndex * dim) + 2]; // Aster <=> MED
		ShapeFunction[(qpIndex * nnodes) + 0] = 0.125 * (1 - x) * (1 - y) * (1 - z)
				* (-2 - x - y - z); //       1 <=>  1
		ShapeFunction[(qpIndex * nnodes) + 3] = 0.125 * (1 + x) * (1 - y) * (1 - z)
				* (-2 + x - y - z); //       2 <=>  4
		ShapeFunction[(qpIndex * nnodes) + 2] = 0.125 * (1 + x) * (1 + y) * (1 - z)
				* (-2 + x + y - z); //       3 <=>  3
		ShapeFunction[(qpIndex * nnodes) + 1] = 0.125 * (1 - x) * (1 + y) * (1 - z)
				* (-2 - x + y - z); //       4 <=>  2
		ShapeFunction[(qpIndex * nnodes) + 4] = 0.125 * (1 - x) * (1 - y) * (1 + z)
				* (-2 - x - y + z); //       5 <=>  5
		ShapeFunction[(qpIndex * nnodes) + 7] = 0.125 * (1 + x) * (1 - y) * (1 + z)
				* (-2 + x - y + z); //       6 <=>  8
		ShapeFunction[(qpIndex * nnodes) + 6] = 0.125 * (1 + x) * (1 + y) * (1 + z)
				* (-2 + x + y + z); //       7 <=>  7
		ShapeFunction[(qpIndex * nnodes) + 5] = 0.125 * (1 - x) * (1 + y) * (1 + z)
				* (-2 - x + y + z); //       8 <=>  6
		ShapeFunction[(qpIndex * nnodes) + 11] = 0.25 * (1 - (x * x)) * (1 - y)
				* (1 - z); //                 9 <=> 12
		ShapeFunction[(qpIndex * nnodes) + 10] = 0.25 * (1 - (y * y)) * (1 + x)
				* (1 - z); //                10 <=> 11
		ShapeFunction[(qpIndex * nnodes) + 9] = 0.25 * (1 - (x * x)) * (1 + y) * (1
				- z); //                11 <=> 10
		ShapeFunction[(qpIndex * nnodes) + 8] = 0.25 * (1 - (y * y)) * (1 - x) * (1
				- z); //                12 <=>  9
		ShapeFunction[(qpIndex * nnodes) + 16] = 0.25 * (1 - (z * z)) * (1 - x)
				* (1 - y); //                13 <=> 17
		ShapeFunction[(qpIndex * nnodes) + 19] = 0.25 * (1 - (z * z)) * (1 + x)
				* (1 - y); //                14 <=> 20
		ShapeFunction[(qpIndex * nnodes) + 18] = 0.25 * (1 - (z * z)) * (1 + x)
				* (1 + y); //                15 <=> 19
		ShapeFunction[(qpIndex * nnodes) + 17] = 0.25 * (1 - (z * z)) * (1 - x)
				* (1 + y); //                16 <=> 18
		ShapeFunction[(qpIndex * nnodes) + 15] = 0.25 * (1 - (x * x)) * (1 - y)
				* (1 + z); //                17 <=> 16
		ShapeFunction[(qpIndex * nnodes) + 14] = 0.25 * (1 - (y * y)) * (1 + x)
				* (1 + z); //                18 <=> 15
		ShapeFunction[(qpIndex * nnodes) + 13] = 0.25 * (1 - (x * x)) * (1 + y)
				* (1 + z); //                19 <=> 14
		ShapeFunction[(qpIndex * nnodes) + 12] = 0.25 * (1 - (y * y)) * (1 - x)
				* (1 + z); //                20 <=> 13
		}
	return;
}

void vtkMedQuadratureDefinition::BuildTetra4()
{

	int nnodes = this->Geometry % 100;
	int dim = this->Geometry / 100; // 3D <=> (x,y,z)
	double x, y, z;
	//
	for (int qpIndex = 0; qpIndex < this->NumberOfQuadraturePoint; qpIndex++)
		{
		x = this->QuadraturePointLocalCoordinates[(qpIndex * dim) + 0];
		y = this->QuadraturePointLocalCoordinates[(qpIndex * dim) + 1];
		z = this->QuadraturePointLocalCoordinates[(qpIndex * dim) + 2];
		//                                                   Aster <=> MED
		ShapeFunction[(qpIndex * nnodes) + 0] = y; //       1 <=>  1
		ShapeFunction[(qpIndex * nnodes) + 1] = z; //       2 <=>  2
		ShapeFunction[(qpIndex * nnodes) + 2] = 1 - x - y - z; // 3 <=>  3
		ShapeFunction[(qpIndex * nnodes) + 3] = x; //       4 <=>  4
		}
	return;
}

void vtkMedQuadratureDefinition::BuildTetra10()
{

	int nnodes = this->Geometry % 100;
	int dim = this->Geometry / 100; // 3D <=> (x,y,z)
	double x, y, z;
	//
	for (int qpIndex = 0; qpIndex < this->NumberOfQuadraturePoint; qpIndex++)
		{
		x = this->QuadraturePointLocalCoordinates[(qpIndex * dim) + 0];
		y = this->QuadraturePointLocalCoordinates[(qpIndex * dim) + 1];
		z = this->QuadraturePointLocalCoordinates[(qpIndex * dim) + 2]; // Aster <=> MED
		ShapeFunction[(qpIndex * nnodes) + 0] = y * (2* y - 1); //                                      1 <=>  1
		ShapeFunction[(qpIndex * nnodes) + 2] = z * (2* z - 1); //                                      2 <=>  3
		ShapeFunction[(qpIndex * nnodes) + 1] = (1 - x - y - z) * (1 - 2* x - 2* y
				- 2* z ); //                        3 <=>  2
		ShapeFunction[(qpIndex * nnodes) + 3] = x * (2* x - 1); //                                      4 <=>  4
		ShapeFunction[(qpIndex * nnodes) + 6] = 4* y * z; //                                            5 <=>  7
		ShapeFunction[(qpIndex * nnodes) + 5] = 4* z * (1 - x - y - z); //                                    6 <=>  6
		ShapeFunction[(qpIndex * nnodes) + 4] = 4* y * (1 - x - y - z); //                                    7 <=>  5
		ShapeFunction[(qpIndex * nnodes) + 7] = 4* x * y; //                                            8 <=>  8
		ShapeFunction[(qpIndex * nnodes) + 9] = 4* x * z; //                                            9 <=> 10
		ShapeFunction[(qpIndex * nnodes) + 8] = 4* x * (1 - x - y - z); //                                   10 <=>  9
		}
	return;
}

void vtkMedQuadratureDefinition::BuildPyra5()
{

	int nnodes = this->Geometry % 100;
	int dim = this->Geometry / 100; // 3D <=> (x,y,z)
	double x, y, z;
	//
	for (int qpIndex = 0; qpIndex < this->NumberOfQuadraturePoint; qpIndex++)
		{
    x = this->QuadraturePointLocalCoordinates[(qpIndex * dim) + 0];
    y = this->QuadraturePointLocalCoordinates[(qpIndex * dim) + 1];
    z = this->QuadraturePointLocalCoordinates[(qpIndex * dim) + 2];
		//                                                                                   Aster <=> MED
    ShapeFunction[(qpIndex * nnodes) + 0] = (-x + y + z - 1.0) * (-x - y + z - 1.0) / (1.0 - z) * 0.25; //  1 <=>  1
    ShapeFunction[(qpIndex * nnodes) + 3] = (-x - y + z - 1.0) * (x - y + z - 1.0) / (1.0 - z) * 0.25; //   2 <=>  4
    ShapeFunction[(qpIndex * nnodes) + 2] = (x + y + z - 1.0) * (x - y + z - 1.0) / (1.0 - z) * 0.25; //    3 <=>  3
    ShapeFunction[(qpIndex * nnodes) + 1] = (x + y + z - 1.0) * (-x + y + z - 1.0) / (1.0 - z) * 0.25; //   4 <=>  2
    ShapeFunction[(qpIndex * nnodes) + 4] = z; //                                      5 <=>  5
		}
	return;
}

void vtkMedQuadratureDefinition::BuildPyra13()
{

	int nnodes = this->Geometry % 100;
	int dim = this->Geometry / 100; // 3D <=> (x,y,z)
	double x, y, z;
	//
	for (int qpIndex = 0; qpIndex < this->NumberOfQuadraturePoint; qpIndex++)
		{
		x = this->QuadraturePointLocalCoordinates[(qpIndex * dim) + 0];
		y = this->QuadraturePointLocalCoordinates[(qpIndex * dim) + 1];
		z = this->QuadraturePointLocalCoordinates[(qpIndex * dim) + 2];
		//                                                                                              Aster <=> MED
		ShapeFunction[(qpIndex * nnodes) + 0] = (-x + y + z - 1.0) * (-x - y + z
				- 1.0) * (x - 0.5) / (1.0 - z) * 0.5; //      1 <=>  1
		ShapeFunction[(qpIndex * nnodes) + 3] = (-x - y + z - 1.0) * (x - y + z
				- 1.0) * (y - 0.5) / (1.0 - z) * 0.5; //       2 <=>  4
		ShapeFunction[(qpIndex * nnodes) + 2] = (x - y + z - 1.0) * (x + y + z
				- 1.0) * (-x - 0.5) / (1.0 - z) * 0.5; //       3 <=>  3
		ShapeFunction[(qpIndex * nnodes) + 1] = (x + y + z - 1.0) * (-x + y + z
				- 1.0) * (-y - 0.5) / (1.0 - z) * 0.5; //      4 <=>  2
		ShapeFunction[(qpIndex * nnodes) + 4] = 2.0 * z * (z - 0.5); //                                      5 <=>  5
		ShapeFunction[(qpIndex * nnodes) + 8] = -(-x + y + z - 1.0) * (-x - y + z
				- 1.0) * (x - y + z - 1.0) / (1.0 - z) * 0.5; // 6 <=>  9
		ShapeFunction[(qpIndex * nnodes) + 7] = -(-x - y + z - 1.0) * (x - y + z
				- 1.0) * (x + y + z - 1.0) / (1.0 - z) * 0.5; //  7 <=>  8
		ShapeFunction[(qpIndex * nnodes) + 6] = -(x - y + z - 1.0) * (x + y + z
				- 1.0) * (-x + y + z - 1.0) / (1.0 - z) * 0.5; //  8 <=>  7
		ShapeFunction[(qpIndex * nnodes) + 5] = -(x + y + z - 1.0) * (-x + y + z
				- 1.0) * (-x - y + z - 1.0) / (1.0 - z) * 0.5; // 9 <=>  6
		ShapeFunction[(qpIndex * nnodes) + 9] = z * (-x + y + z - 1.0) * (-x - y
				+ z - 1.0) / (1.0 - z); //               10 <=> 10
		ShapeFunction[(qpIndex * nnodes) + 12] = z * (-x - y + z - 1.0) * (x - y
				+ z - 1.0) / (1.0 - z); //               11 <=> 13
		ShapeFunction[(qpIndex * nnodes) + 11] = z * (x - y + z - 1.0) * (x + y + z
				- 1.0) / (1.0 - z); //                12 <=> 12
		ShapeFunction[(qpIndex * nnodes) + 10] = z * (x + y + z - 1.0) * (-x + y
				+ z - 1.0) / (1.0 - z); //               13 <=> 11
		}
	return;
}

void vtkMedQuadratureDefinition::BuildCenter(med_geometrie_element geometry)
{
	this->Geometry = geometry;
	this->NumberOfQuadraturePoint = 1;
	this->SecureResources();
	int npts = vtkMedUtilities::GetNumberOfPoint(this->Geometry);
  for (int i = 0; i < npts; i++)
    {
    ShapeFunction[i] = 1.0 / (double) npts;
    }
  this->Weights[0] = 1;

}

void vtkMedQuadratureDefinition::BuildELNO(med_geometrie_element geometry)
{
	this->Geometry = geometry;
	this->NumberOfQuadraturePoint = vtkMedUtilities::GetNumberOfPoint(geometry);
	this->SecureResources();
	int np2 = this->NumberOfQuadraturePoint * this->NumberOfQuadraturePoint;
	for (int i = 0; i < np2; i++)
		{
		this->ShapeFunction[i] = 0;
		}
	for (int i = 0; i < this->NumberOfQuadraturePoint; i++)
		{
		ShapeFunction[i + i * this->NumberOfQuadraturePoint] = 1.0;
		}
	double w = 1.0 / (double) this->NumberOfQuadraturePoint;
	for (int i = 0; i < this->NumberOfQuadraturePoint; i++)
		{
		this->Weights[i] = w;
		}
	//QuadraturePointLocalCoordinates and PointLocalCoordinates are unknown
}

void vtkMedQuadratureDefinition::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);
	PRINT_MED_STRING(os, indent, Name);
	PRINT_IVAR(os, indent, Geometry);
	PRINT_IVAR(os, indent, NumberOfQuadraturePoint);
	PRINT_IVAR(os, indent, MedIndex);
	PRINT_VECTOR(os, indent, Weights, this->GetSizeOfWeights());
	PRINT_VECTOR(os, indent, PointLocalCoordinates, this->GetSizeOfPointLocalCoordinates());
	PRINT_VECTOR(os, indent, QuadraturePointLocalCoordinates, this->GetSizeOfQuadraturePointLocalCoordinates());
	PRINT_VECTOR(os, indent, ShapeFunction, this->GetSizeOfShapeFunction());
}
