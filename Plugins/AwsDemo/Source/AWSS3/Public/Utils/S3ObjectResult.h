#pragma once

#include "CoreMinimal.h"

#include "S3ObjectResult.generated.h"

USTRUCT(BlueprintType)
struct FS3ObjectResult {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AWS S3")
	FString Bucket;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AWS S3")
	FString FileLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AWS S3")
	FString Key;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AWS S3")
	bool wasSuccessful;
	
};