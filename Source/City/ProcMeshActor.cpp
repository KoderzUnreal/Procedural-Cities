#pragma once

#include "City.h"
#include "ProcMeshActor.h"
#include "polypartition.h"

// Sets default values


unsigned int AProcMeshActor::workersWorking{ 0 };
AProcMeshActor::AProcMeshActor()
{
	PrimaryActorTick.bCanEverTick = true;
	exteriorMesh = CreateDefaultSubobject<URuntimeMeshComponentStatic>(TEXT("exteriorMesh"));
	sndExteriorMesh = CreateDefaultSubobject<URuntimeMeshComponentStatic>(TEXT("sndExteriorMesh"));
	interiorMesh = CreateDefaultSubobject<URuntimeMeshComponentStatic>(TEXT("interiorMesh"));
	windowMesh = CreateDefaultSubobject<URuntimeMeshComponentStatic>(TEXT("windowMesh"));
	windowFrameMesh = CreateDefaultSubobject<URuntimeMeshComponentStatic>(TEXT("windowFrameMesh"));
	occlusionWindowMesh = CreateDefaultSubobject<URuntimeMeshComponentStatic>(TEXT("occlusionWindowMesh"));
	floorMesh = CreateDefaultSubobject<URuntimeMeshComponentStatic>(TEXT("floorMesh"));
	roofMesh = CreateDefaultSubobject<URuntimeMeshComponentStatic>(TEXT("roofMesh"));
	greenMesh = CreateDefaultSubobject<URuntimeMeshComponentStatic>(TEXT("greenMesh"));
	concreteMesh = CreateDefaultSubobject<URuntimeMeshComponentStatic>(TEXT("concreteMesh"));
	roadMiddleMesh = CreateDefaultSubobject<URuntimeMeshComponentStatic>(TEXT("roadMiddleMesh"));
	asphaltMesh = CreateDefaultSubobject<URuntimeMeshComponentStatic>(TEXT("asphaltMesh"));
	SetActorTickEnabled(false);

}

AProcMeshActor::~AProcMeshActor()
{
	//exteriorMesh->DestroyComponent();
	//sndExteriorMesh->DestroyComponent();
	//interiorMesh->DestroyComponent();
	//windowMesh->DestroyComponent();
	//windowFrameMesh->DestroyComponent();
	//occlusionWindowMesh->DestroyComponent();
	//floorMesh->DestroyComponent();
	//roofMesh->DestroyComponent();
	//greenMesh->DestroyComponent();
	//concreteMesh->DestroyComponent();
	//roadMiddleMesh->DestroyComponent();
	//asphaltMesh->DestroyComponent();

}




bool AProcMeshActor::buildPolygons(TArray<FPolygon> &pols, FVector offset, URuntimeMeshComponentStatic* mesh, UMaterialInterface *mat) {
	if (mesh->GetLastSectionId(0) != INDEX_NONE || pols.Num() == 0) {
		return false;
	}

	TArray<FVector> vertices;
	TArray<int32> triangles;
	TArray<FVector2D> UV;
	TArray<FVector> normals;

	TArray<FColor> vertexColors;
	TArray<FRuntimeMeshTangent> tangents;

	int current = 0;
	for (FPolygon &pol : pols) {

		if (pol.points.Num() < 3)
			continue;
		// local coordinates are found by getting the coordinates of points on the plane which they span up
		FVector e1 = pol.points[1] - pol.points[0];
		e1.Normalize();
		FVector n = pol.normal.Size() < 1.0f ? FVector::CrossProduct(e1, pol.points[pol.points.Num() - 1] - pol.points[0]) : pol.normal;
		FVector e2 = FVector::CrossProduct(e1, n);
		e2.Normalize();


		FVector origin = pol.points[0]; 

		std::list<TPPLPoly> inTriangles;

		TPPLPoly poly;
		poly.Init(pol.points.Num());
		for (int i = 0; i < pol.points.Num(); i++) {
			FVector point = pol.points[i];
			float y = FVector::DotProduct(e1, point - origin);
			float x = FVector::DotProduct(e2, point - origin);
			UV.Add(FVector2D(x*texScaleMultiplier, y*texScaleMultiplier));
			TPPLPoint newP{ x, y, current + i };
			poly[i] = newP;
			vertices.Add(point);
			normals.Add(-n);

		}
		//exteriorMesh->clear
		TPPLPartition part;
		poly.SetOrientation(TPPL_CCW);
		int res = part.Triangulate_EC(&poly, &inTriangles);

		for (auto i : inTriangles) {
			triangles.Add(i[0].id);
			triangles.Add(i[1].id);
			triangles.Add(i[2].id);
		}
		current += pol.points.Num();
	}




	mesh->SetupMaterialSlot(0, TEXT("MeshMat"), mat);
	mesh->CreateSectionFromComponents(0, 0, 0, vertices, triangles, normals, UV, vertexColors, tangents, ERuntimeMeshUpdateFrequency::Infrequent, proceduralMeshesCollision);


	return true;
}



bool AProcMeshActor::clearMeshes(bool fullReplacement) {
	if (fullReplacement) {
		exteriorMesh->RemoveAllSectionsForLOD(0);
		sndExteriorMesh->RemoveAllSectionsForLOD(0);
		greenMesh->RemoveAllSectionsForLOD(0);
		concreteMesh->RemoveAllSectionsForLOD(0);
		roofMesh->RemoveAllSectionsForLOD(0);
	}
	interiorMesh->RemoveAllSectionsForLOD(0);
	windowMesh->RemoveAllSectionsForLOD(0);
	windowFrameMesh->RemoveAllSectionsForLOD(0);
	occlusionWindowMesh->RemoveAllSectionsForLOD(0);
	floorMesh->RemoveAllSectionsForLOD(0);
	roadMiddleMesh->RemoveAllSectionsForLOD(0);
	asphaltMesh->RemoveAllSectionsForLOD(0);
	return true;
}


// divides the polygon into the different materials used by the house
bool AProcMeshActor::buildMaterialPolygons(TArray<FMaterialPolygon> pols, FVector offset) {
	if (isWorking) {
		clearMeshes(true);
		isWorking = false;
		workersWorking--;

		//return false;
	}

	TArray<FPolygon> exterior;
	TArray<FPolygon> exteriorSnd;

	TArray<FPolygon> interior;
	TArray<FPolygon> windows;
	TArray<FPolygon> windowFrames;

	TArray<FPolygon> occlusionWindows;
	TArray<FPolygon> floors;
	TArray<FPolygon> roofs;

	TArray<FPolygon> concrete;
	TArray<FPolygon> green;

	TArray<FPolygon> roadMiddle;
	TArray<FPolygon> asphalt;
	for (FMaterialPolygon &p : pols) {
		switch (p.type) {
		case PolygonType::exterior:
			exterior.Add(p);
			break;
		case PolygonType::exteriorSnd:
			exteriorSnd.Add(p);
			break;
		case PolygonType::interior:
			interior.Add(p);
			break;
		case PolygonType::window:
			windows.Add(p);
			break;
		case PolygonType::floor:
			floors.Add(p);
			break;
		case PolygonType::roof:
			roofs.Add(p);
			break;
		case PolygonType::occlusionWindow:
			occlusionWindows.Add(p);
			break;
		case PolygonType::windowFrame:
			windowFrames.Add(p);
			break;
		case PolygonType::concrete:
			concrete.Add(p);
			break;
		case PolygonType::green:
			green.Add(p);
			break;
		case PolygonType::roadMiddle:
			roadMiddle.Add(p);
			break;
		case PolygonType::asphalt:
			asphalt.Add(p);
			break;
		}

	}


	polygons.Empty();
	polygons.Add(exterior);
	polygons.Add(exteriorSnd);
	polygons.Add(interior);
	polygons.Add(windows);
	polygons.Add(windowFrames);
	polygons.Add(occlusionWindows);
	polygons.Add(floors);
	polygons.Add(roofs);
	polygons.Add(green);
	polygons.Add(concrete);
	polygons.Add(roadMiddle);
	polygons.Add(asphalt);

	components.Empty();
	components.Add(exteriorMesh);
	components.Add(sndExteriorMesh);
	components.Add(interiorMesh);
	components.Add(windowMesh);
	components.Add(windowFrameMesh);
	components.Add(occlusionWindowMesh);
	components.Add(floorMesh);
	components.Add(roofMesh);
	components.Add(greenMesh);
	components.Add(concreteMesh);
	components.Add(roadMiddleMesh);
	components.Add(asphaltMesh);

	materials.Empty();
	materials.Add(exteriorMat);
	materials.Add(sndExteriorMat);
	materials.Add(interiorMat);
	materials.Add(windowMat);
	materials.Add(windowFrameMat);
	materials.Add(occlusionWindowMat);
	materials.Add(floorMat);
	materials.Add(roofMat);
	materials.Add(greenMat);
	materials.Add(concreteMat);
	materials.Add(roadMiddleMat);
	materials.Add(asphaltMat);



	currentlyWorkingArray = 0;
	wantsToWork = true;
	SetActorTickEnabled(true);

	//isWorking = true;
	//buildPolygons(exterior, offset, exteriorMesh, exteriorMat);
	//buildPolygons(exteriorSnd, offset, sndExteriorMesh, sndExteriorMat);
	//buildPolygons(interior, offset, interiorMesh, interiorMat);
	//buildPolygons(windows, offset, windowMesh, windowMat);
	//buildPolygons(floors, offset, floorMesh, floorMat);
	//buildPolygons(roofs, offset, roofMesh, roofMat);
	//buildPolygons(occlusionWindows, offset, occlusionWindowMesh, occlusionWindowMat);
	//buildPolygons(windowFrames, offset, windowFrameMesh, windowFrameMat);
	//buildPolygons(concrete, offset, concreteMesh, concreteMat);
	//buildPolygons(green, offset, greenMesh, greenMat);
	//buildPolygons(roadMiddle, offset, roadMiddleMesh, roadMiddleMat);


	//if (a < 11) {
	//	//UE_LOG(LogTemp, Warning, TEXT("a: %i"), a);
	//	//Destroy();
	//	return false;
	//} 
	return true;


}


// Called when the game starts or when spawned
void AProcMeshActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProcMeshActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (wantsToWork && workersWorking < maxThreads) {
		workersWorking++;
		wantsToWork = false;
		isWorking = true;
	}

	if (isWorking) {
		TArray<FPolygon> &current = polygons[currentlyWorkingArray];
		buildPolygons(current, FVector(0, 0, 0), components[currentlyWorkingArray], materials[currentlyWorkingArray]);
		currentlyWorkingArray++;
		if (currentlyWorkingArray >= polygons.Num()) {
			isWorking = false;
			workersWorking--;
			SetActorTickEnabled(false);
		}
	}

}

