// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "BaseLibrary.h"
#include "RuntimeMeshComponentStatic.h"

#include "ProcMeshActor.generated.h"

UCLASS()
class CITY_API AProcMeshActor : public AActor
{
	GENERATED_BODY()
	
public:

	static unsigned int workersWorking;
	GenerationMode generationMode;
	unsigned int maxThreads = 10000;
	// Sets default values for this actor's properties
	AProcMeshActor();
	~AProcMeshActor();

	UFUNCTION(BlueprintCallable, Category = "Generation")
	bool buildMaterialPolygons(TArray<FMaterialPolygon> pols, FVector offset);

	bool clearMeshes(bool fullReplacement);

	UFUNCTION(BlueprintCallable, Category = "Settings")
		void init(GenerationMode generationMode_in) {
		generationMode = generationMode_in;
		switch (generationMode) {
		case GenerationMode::complete: maxThreads = 1000; break;
		case GenerationMode::procedural_aggressive: maxThreads = 1000; break;
		case GenerationMode::procedural_relaxed: maxThreads = 1; break;
		}
	}
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = appearance, meta = (AllowPrivateAccess = "true"))
		UMaterial* exteriorMat;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = appearance, meta = (AllowPrivateAccess = "true"))
		UMaterial* sndExteriorMat;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = appearance, meta = (AllowPrivateAccess = "true"))
		UMaterial* windowMat;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = appearance, meta = (AllowPrivateAccess = "true"))
		UMaterial* windowFrameMat;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = appearance, meta = (AllowPrivateAccess = "true"))
		UMaterial* occlusionWindowMat;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = appearance, meta = (AllowPrivateAccess = "true"))
		UMaterial* interiorMat;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = appearance, meta = (AllowPrivateAccess = "true"))
		UMaterial* floorMat;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = appearance, meta = (AllowPrivateAccess = "true"))
		UMaterial* roofMat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = appearance, meta = (AllowPrivateAccess = "true"))
		UMaterial* greenMat;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = appearance, meta = (AllowPrivateAccess = "true"))
		UMaterial* concreteMat;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = appearance, meta = (AllowPrivateAccess = "true"))
		UMaterial* roadMiddleMat;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = appearance, meta = (AllowPrivateAccess = "true"))
		UMaterial* asphaltMat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = appearance, meta = (AllowPrivateAccess = "true"))
		float texScaleMultiplier = 1.0f;

	//TArray<
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	bool buildPolygons(TArray<FPolygon> &pols, FVector offset, URuntimeMeshComponentStatic* mesh, UMaterialInterface *mat);


	bool wantsToWork = false;
	bool isWorking = false;
	int currentlyWorkingArray = 0;


	UPROPERTY(VisibleAnywhere, Category = Meshes)
	URuntimeMeshComponentStatic* exteriorMesh;
	UPROPERTY(VisibleAnywhere, Category = Meshes)
	URuntimeMeshComponentStatic* interiorMesh;
	UPROPERTY(VisibleAnywhere, Category = Meshes)
	URuntimeMeshComponentStatic* windowMesh;
	UPROPERTY(VisibleAnywhere, Category = Meshes)
	URuntimeMeshComponentStatic* occlusionWindowMesh;
	UPROPERTY(VisibleAnywhere, Category = Meshes)
	URuntimeMeshComponentStatic* windowFrameMesh;
	UPROPERTY(VisibleAnywhere, Category = Meshes)
	URuntimeMeshComponentStatic* sndExteriorMesh;
	UPROPERTY(VisibleAnywhere, Category = Meshes)
	URuntimeMeshComponentStatic* floorMesh;
	UPROPERTY(VisibleAnywhere, Category = Meshes)
	URuntimeMeshComponentStatic* roofMesh;

	UPROPERTY(VisibleAnywhere, Category = Meshes)
	URuntimeMeshComponentStatic * greenMesh;
	UPROPERTY(VisibleAnywhere, Category = Meshes)
	URuntimeMeshComponentStatic * concreteMesh;
	UPROPERTY(VisibleAnywhere, Category = Meshes)
	URuntimeMeshComponentStatic * roadMiddleMesh;
	UPROPERTY(VisibleAnywhere, Category = Meshes)
	URuntimeMeshComponentStatic * asphaltMesh;

	TArray<URuntimeMeshComponentStatic*> components;
	TArray<UMaterialInterface*> materials;
	TArray<TArray<FPolygon>> polygons;

	int currIndex = 1;
	
	
};
