#pragma once

#include "CoreMinimal.h"


#include "GetObjectsResult.generated.h"

USTRUCT(BlueprintType)
struct FS3ListObjectsResult {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AWS S3")
	FString bucketname;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AWS S3")
	TArray<FString> objs;

};
