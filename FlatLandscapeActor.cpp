#include "FlatLandscapeActor.h"
#include "LandscapeEdit.h"
#include "Engine/World.h"
#include "Engine/Texture2D.h"

AFlatLandscapeActor::AFlatLandscapeActor()
{
    PrimaryActorTick.bCanEverTick = true;

    // Set default values
    Width = 128;
    Height = 128;
    Scale = 100.0f;
    ComponentSizeQuads = 63;
    NumSubsections = 1;
    SubsectionSizeQuads = ComponentSizeQuads;
    FlatHeight = 32768; // Midpoint for flat terrain

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
    NoiseGenerator = CreateDefaultSubobject<UFastNoiseWrapper>(TEXT("NoiseGenerator"));
    HeightmapTexture = nullptr;
}

void AFlatLandscapeActor::BeginPlay()
{
    Super::BeginPlay();
    ValidateParameters();
    InitializeLandscape();
    CreateHeightmapTexture();
}

void AFlatLandscapeActor::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
    ValidateParameters();
    UpdateNoiseSettings();
    InitializeLandscape();
    CreateHeightmapTexture();
}

void AFlatLandscapeActor::InitializeLandscape()
{
    if (!GetWorld())
    {
        UE_LOG(LogTemp, Error, TEXT("World is null."));
        return;
    }

    // Clear any existing landscape
    ClearLandscape();

    // Create a Landscape Actor
    Landscape = GetWorld()->SpawnActor<ALandscape>(GetActorLocation(), GetActorRotation());
    if (!Landscape)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create Landscape Actor"));
        return;
    }

    // Settings
    Landscape->GetRootComponent()->SetMobility(EComponentMobility::Static);

    // Set landscape properties
    Landscape->ComponentSizeQuads = ComponentSizeQuads;
    Landscape->SubsectionSizeQuads = SubsectionSizeQuads;
    Landscape->NumSubsections = NumSubsections;

    // Generate a new GUID
    Landscape->SetLandscapeGuid(FGuid::NewGuid());

    // Generate heightmap data
    GenerateHeightmap();

    // Setup heightmap for landscape
    TMap<FGuid, TArray<uint16>> HeightmapDataPerLayers;
    TMap<FGuid, TArray<FLandscapeImportLayerInfo>> MaterialLayerDataPerLayer;

    // Add our heightmap data
    HeightmapDataPerLayers.Add(FGuid(), HeightmapData);
    MaterialLayerDataPerLayer.Add(FGuid(), TArray<FLandscapeImportLayerInfo>());

    // Import the landscape
    Landscape->Import(Landscape->GetLandscapeGuid(), 0, 0, Width - 1, Height - 1,
                      Landscape->NumSubsections, Landscape->SubsectionSizeQuads,
                      HeightmapDataPerLayers, TEXT("NONE"), MaterialLayerDataPerLayer,
                      ELandscapeImportAlphamapType::Additive);

    // Set landscape transform
    Landscape->SetActorLocation(GetActorLocation());
    Landscape->SetActorScale3D(FVector(Scale, Scale, Scale));

    // Create landscape info
    Landscape->CreateLandscapeInfo();

    // Set actor label
    Landscape->SetActorLabel(GetName());

    UE_LOG(LogTemp, Log, TEXT("Landscape initialization completed successfully."));
}

void AFlatLandscapeActor::GenerateHeightmap()
{
    HeightmapData.Init(0, Width * Height);
    for (int32 Y = 0; Y < Height; ++Y)
    {
        for (int32 X = 0; X < Width; ++X)
        {
            float baseNoise = NoiseGenerator->GetNoise2D(X * NoiseScale, Y * NoiseScale);
            
            float finalHeight = FlatHeight + baseNoise * NoiseIntensity;

            int32 HeightValue = FMath::RoundToInt(finalHeight);
            HeightValue = FMath::Clamp(HeightValue, 0, 65535);  // Ensure it's within valid range
            HeightmapData[Y * Width + X] = HeightValue;
        }
    }
}

void AFlatLandscapeActor::ClearLandscape()
{
    if (Landscape)
    {
        Landscape->Destroy();
        Landscape = nullptr;
    }
}

void AFlatLandscapeActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AFlatLandscapeActor::ValidateParameters()
{
    if (Width <= 0)
    {
        Width = 128;
        UE_LOG(LogTemp, Warning, TEXT("Width must be positive. Resetting to default value of 128."));
    }
    if (Height <= 0)
    {
        Height = 128;
        UE_LOG(LogTemp, Warning, TEXT("Height must be positive. Resetting to default value of 128."));
    }
    if (Scale <= 0.0f)
    {
        Scale = 100.0f;
        UE_LOG(LogTemp, Warning, TEXT("Scale must be positive. Resetting to default value of 100.0f."));
    }
    if (ComponentSizeQuads <= 0)
    {
        ComponentSizeQuads = 63;
        UE_LOG(LogTemp, Warning, TEXT("ComponentSizeQuads must be positive. Resetting to default value of 63."));
    }
    if (NumSubsections <= 0)
    {
        NumSubsections = 1;
        UE_LOG(LogTemp, Warning, TEXT("NumSubsections must be positive. Resetting to default value of 1."));
    }
    if (SubsectionSizeQuads <= 0)
    {
        SubsectionSizeQuads = ComponentSizeQuads;
        UE_LOG(LogTemp, Warning, TEXT("SubsectionSizeQuads must be positive. Resetting to default value of ComponentSizeQuads."));
    }
    if (!NoiseGenerator)
    {
        NoiseGenerator = NewObject<UFastNoiseWrapper>(this);
        UE_LOG(LogTemp, Warning, TEXT("NoiseGenerator was null. Created a new one."));
    }
}

void AFlatLandscapeActor::UpdateNoiseSettings()
{
    if (NoiseGenerator)
    {
        NoiseGenerator->SetupFastNoise(NoiseType, Seed, Frequency);
        NoiseGenerator->SetOctaves(Octaves);
        NoiseGenerator->SetLacunarity(Lacunarity);
        NoiseGenerator->SetGain(Gain);
        NoiseGenerator->SetFractalType(FractalType);
        NoiseGenerator->SetDistanceFunction(CellularDistanceFunction);
        NoiseGenerator->SetReturnType(CellularReturnType);
        NoiseGenerator->SetCellularJitter(CellularJitterModifier);
        NoiseGenerator->SetInterpolation(Interp);
    }
}

void AFlatLandscapeActor::CreateHeightmapTexture()
{
    if (HeightmapData.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("HeightmapData is empty."));
        return;
    }

    // Create a new Texture2D
    HeightmapTexture = UTexture2D::CreateTransient(Width, Height, PF_B8G8R8A8);
    if (!HeightmapTexture)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create texture."));
        return;
    }

    // Lock the texture for editing
    FTexture2DMipMap& Mip = HeightmapTexture->GetPlatformData()->Mips[0];
    void* TextureData = Mip.BulkData.Lock(LOCK_READ_WRITE);
    FColor* FormattedData = static_cast<FColor*>(TextureData);

    // Fill the texture with heightmap data
    for (int32 Y = 0; Y < Height; ++Y)
    {
        for (int32 X = 0; X < Width; ++X)
        {
            uint16 HeightValue = HeightmapData[Y * Width + X];
            uint8 PixelValue = HeightValue >> 8;  // Simplified mapping for visualization
            FormattedData[Y * Width + X] = FColor(PixelValue, PixelValue, PixelValue, 255);
        }
    }

    // Unlock the texture
    Mip.BulkData.Unlock();

    // Update the texture to apply changes
    HeightmapTexture->UpdateResource();

    // Mark the texture as dirty to be saved properly
    HeightmapTexture->MarkPackageDirty();

    UE_LOG(LogTemp, Log, TEXT("Heightmap texture created successfully."));
}

#if WITH_EDITOR
void AFlatLandscapeActor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    // Update noise generator settings
    UpdateNoiseSettings();

    // Reinitialize the landscape when properties change in the editor
    InitializeLandscape();
    CreateHeightmapTexture();
}
#endif
