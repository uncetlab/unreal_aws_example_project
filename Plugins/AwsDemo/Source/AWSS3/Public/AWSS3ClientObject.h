#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/LatentActionManager.h"

#include "AWSBase/Public/Utils/ClientConfiguration.h"
#include "AWSBase/Public/Utils/Credentials.h"
#include "AWSBase/Public/Utils/Serializer.h"
#include "AWSBase/Public/Utils/SerializerTemplatesImplementations.h"

#include "Utils/GetObjectsResult.h"
#include "Utils/S3ObjectResult.h"
#include "AWSBaseModule.h"

#include "aws/s3/model/GetObjectRequest.h"
#include "aws/s3/model/ListObjectsRequest.h"
#include "aws/s3/S3Client.h"


#include "AWSS3ClientObject.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FListObjectReturned, FS3ListObjectsResult, bucketName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FObjectReturned, FS3ObjectResult, objectinfo);

UCLASS(BlueprintType)
class UAWSS3ClientObject: public UObject {
    GENERATED_BODY()

public:
	

	UPROPERTY(BlueprintAssignable)
	FListObjectReturned OnListObjectsReturned;

	UPROPERTY(BlueprintAssignable)
	FObjectReturned OnObjectReturned;


    UFUNCTION(BlueprintCallable, Category = "S3 Client")
    static UAWSS3ClientObject *
    CreateS3ClientObject(const FCredentials credentials, const FClientConfiguration clientConfiguration);

	UFUNCTION(BlueprintCallable, Category = "S3 Client")
	FString GetString();

    UFUNCTION(BlueprintCallable, Category = "S3 Client")
    TArray<FString>
    GetBucketNames();

	UFUNCTION(BlueprintCallable, Category = "S3 Client")
	TArray<FString>
	ListObjects(FString bucket_name);

	UFUNCTION(BlueprintCallable, Category = "S3 Client")
	bool
	ListObjectsAsync(FString bucket_name);

	UFUNCTION(BlueprintCallable, Category = "S3 Client")
	bool	
	GetObject(FString bucket, FString key, FString destination);

	UFUNCTION(BlueprintCallable, Category = "S3 Client")
	bool
	GetObjectAsync(FString bucket, FString key, FString destination);

	//These have to be maps to the delegates because the async handling functions have to be static, but the delegates are instance properties
	//This also allows us to use multiple clients 
	static TMap<FString, FListObjectReturned*> BucketToClientObject;
	static TMap<FString, FObjectReturned*> BucketKeyToObject;

	

private:

	Aws::S3::S3Client* awsS3Client;

	static void ListObjectsAsync_Handler(const Aws::S3::S3Client *client,
		const Aws::S3::Model::ListObjectsRequest &request,
		const Aws::S3::Model::ListObjectsOutcome &outcome,
		const std::shared_ptr<const Aws::Client::AsyncCallerContext>& context);

	static void GetObjectAsync_Handler(const Aws::S3::S3Client *client,
		const Aws::S3::Model::GetObjectRequest &request,
		Aws::S3::Model::GetObjectOutcome outcome,
		const std::shared_ptr<const Aws::Client::AsyncCallerContext>& context);
};