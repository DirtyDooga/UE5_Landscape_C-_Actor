#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Landscape.h"
#include "LandscapeComponent.h"
#include "FastNoiseWrapper.h"
#include "Engine/Texture2D.h"
#include "FlatLandscapeActor.generated.h"

UCLASS()
class HOUSE_OF_PALAIOLOGOS_API AFlatLandscapeActor : public AActor
{
    GENERATED_BODY()

public:    
    AFlatLandscapeActor();

protected:
    virtual void BeginPlay() override;

public:    
    virtual void Tick(float DeltaTime) override;
    virtual void OnConstruction(const FTransform& Transform) override;

#if WITH_EDITOR
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

    // Landscape properties
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Landscape")
    int32 Width;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Landscape")
    int32 Height;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Landscape")
    float Scale;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Landscape")
    int32 ComponentSizeQuads;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Landscape")
    int32 NumSubsections;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Landscape")
    int32 SubsectionSizeQuads;

    // Noise properties

    // Texture generated from heightmap
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Noise")
    UTexture2D* HeightmapTexture;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise")
    UFastNoiseWrapper* NoiseGenerator;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise")
    float NoiseScale = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise")
    float NoiseIntensity = 1000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise")
    int32 Seed = 1337;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise")
    EFastNoise_NoiseType NoiseType = EFastNoise_NoiseType::Perlin;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise")
    float Frequency = 0.01f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise")
    EFastNoise_Interp Interp = EFastNoise_Interp::Linear;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise")
    EFastNoise_FractalType FractalType = EFastNoise_FractalType::FBM;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise")
    int32 Octaves = 3;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise")
    float Lacunarity = 2.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise")
    float Gain = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise")
    EFastNoise_CellularDistanceFunction CellularDistanceFunction = EFastNoise_CellularDistanceFunction::Euclidean;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise")
    EFastNoise_CellularReturnType CellularReturnType = EFastNoise_CellularReturnType::CellValue;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise")
    float CellularJitterModifier = 1.0f;

private:
    UPROPERTY()
    ALandscape* Landscape;

    UPROPERTY()
    TArray<uint16> HeightmapData;

    UPROPERTY()
    uint16 FlatHeight;

    void InitializeLandscape();

    void ValidateParameters();

    void ClearLandscape();

    void UpdateNoiseSettings();

    void GenerateHeightmap();

    void CreateHeightmapTexture();
};
