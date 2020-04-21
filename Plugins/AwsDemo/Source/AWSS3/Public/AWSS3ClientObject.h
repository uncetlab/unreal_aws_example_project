#pragma once

#include "CoreMinimal.h"


#include "AWSBase/Public/Utils/ClientConfiguration.h"
#include "AWSBase/Public/Utils/Credentials.h"
#include "AWSBase/Public/Utils/Serializer.h"
#include "AWSBase/Public/Utils/SerializerTemplatesImplementations.h"

#include "Misc/FileHelper.h"

#include "Utils/S3ObjectResult.h"

#include "aws/s3/model/GetObjectRequest.h"
#include "aws/s3/model/ListObjectsRequest.h"
#include "aws/s3/S3Client.h"

#include "AWSS3ClientObject.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FObjectReturned, FS3ObjectResult, result);

UCLASS(BlueprintType)
class UAWSS3ClientObject: public UObject {
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
		FObjectReturned OnObjectReturned;

	UFUNCTION(BlueprintCallable, Category = "S3 Client")
		static UAWSS3ClientObject*
		CreateS3ClientObject(const FCredentials credentials, const FClientConfiguration clientConfiguration);

	UFUNCTION(BlueprintCallable, Category = "S3 Client")
		bool
		GetObject(FString bucket, FString key, FString destination);


	UFUNCTION(BlueprintCallable, Category = "S3 Client")
		FString
		GetObjectAsync(FString bucket, FString key, FString destination);

	static TMap<FString, FObjectReturned*> UUIDToObject;
private:
	Aws::S3::S3Client* awsS3Client;

	static void GetObjectAsync_Handler(const Aws::S3::S3Client* client,
		const Aws::S3::Model::GetObjectRequest& request,
		Aws::S3::Model::GetObjectOutcome outcome,
		const std::shared_ptr<const Aws::Client::AsyncCallerContext>& context);
};