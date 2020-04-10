#include "AWSS3ClientObject.h"
#include "AWSS3PrivatePCH.h"


#include "Runtime/Engine/Classes/Engine/Engine.h"

#include "LatentActions.h"

#include "aws/core/utils/Outcome.h"
#include "aws/core/client/AWSError.h"
#include "aws/core/client/AWSClient.h"
#include "aws/s3/model/ListObjectsRequest.h"
#include "aws/s3/model/GetObjectRequest.h"
#include "Kismet/GameplayStatics.h"
#include "CoreMinimal.h"
#include "Misc/FileHelper.h"


#include "string"


TMap<FString, FListObjectReturned*> UAWSS3ClientObject::BucketToClientObject = TMap<FString, FListObjectReturned*>();
TMap<FString, FObjectReturned*> UAWSS3ClientObject::BucketKeyToObject = TMap<FString, FObjectReturned*>();

UAWSS3ClientObject *UAWSS3ClientObject::CreateS3ClientObject(const FCredentials credentials, const FClientConfiguration clientConfiguration){
	UAWSS3ClientObject *S3Client = NewObject<UAWSS3ClientObject>();
	
	

    S3Client->awsS3Client = new Aws::S3::S3Client(AWSSerializer::Credentials(credentials), AWSSerializer::ClientConfiguration(clientConfiguration));

    return S3Client;
}


FString UAWSS3ClientObject::GetString(){
	/*TSet<FString> demoset = TSet<FString>();
	demoset.Add(FString("Hello"));
	demoset.Add(FString("World"));
	demoset.Add(FString("Go"));
	demoset.Add(FString("Heels"));
	
	auto rv = AWSSerializer::TSettoAwsVector<Aws::String, FString>(demoset, AWSSerializer::PFStoPAS);

	for (auto elem : rv) {
		UE_LOG(LogTemp, Display, TEXT("Value in aws set: %s"), *FString(elem.c_str()));
	}*/

	return FString("Hello World");
}

FString* bucketToFString(Aws::S3::Model::Bucket* bucket) {
	auto rv = new FString(bucket->GetName().c_str());
	
	return rv;
}



TArray<FString> UAWSS3ClientObject::GetBucketNames(){
	
    auto outcome = this->awsS3Client->ListBuckets();
	TArray<FString> buckets;
    if(outcome.IsSuccess()){
        Aws::Vector<Aws::S3::Model::Bucket> list = outcome.GetResult().GetBuckets();
			
		buckets = AWSSerializer::AwsVectortoTArray<Aws::S3::Model::Bucket, FString>(list, bucketToFString);
    }

	return buckets;
}

TArray<FString> UAWSS3ClientObject::ListObjects(FString bucket_name) {
	TArray<FString> objects;

	Aws::S3::Model::ListObjectsRequest *request = new Aws::S3::Model::ListObjectsRequest();
	
	request->SetBucket(TCHAR_TO_UTF8(*bucket_name));

	auto outcome = this->awsS3Client->ListObjects( *request );

	if (outcome.IsSuccess()) {
		Aws::Vector<Aws::S3::Model::Object> list = outcome.GetResult().GetContents();

		for (Aws::S3::Model::Object obj : list) {
			objects.Add( FString(obj.GetKey().c_str()) );
		}
	}

	return objects;
}


bool UAWSS3ClientObject::ListObjectsAsync(FString bucket_name) {
	UE_LOG(LogTemp, Display, TEXT("Start of the function"));
	Aws::S3::Model::ListObjectsRequest *request = new Aws::S3::Model::ListObjectsRequest();
	request->SetBucket(TCHAR_TO_UTF8(*bucket_name));
	UE_LOG(LogTemp, Display, TEXT("Made an aws model using .so file information"));

	auto context = Aws::MakeShared<Aws::Client::AsyncCallerContext>("ListObjectsAllocationTag");
	context->SetUUID(TCHAR_TO_UTF8(*bucket_name));

	if (!UAWSS3ClientObject::BucketToClientObject.Contains(bucket_name)) {
		UAWSS3ClientObject::BucketToClientObject.Add(bucket_name, &(this->OnListObjectsReturned));
		this->awsS3Client->ListObjectsAsync(*request, UAWSS3ClientObject::ListObjectsAsync_Handler, context);
		return true;
	}

	return false;
			
}

bool UAWSS3ClientObject::GetObject(FString bucket, FString key, FString destination) {
	Aws::S3::Model::GetObjectRequest req;
	req.SetBucket(TCHAR_TO_UTF8(*bucket));
	req.SetKey(TCHAR_TO_UTF8(*key));

	auto res = this->awsS3Client->GetObject(req);
	if (res.IsSuccess()) {
		auto &retrieved_file = res.GetResultWithOwnership().GetBody();

		TArray<uint8> buffer;

		int64 content_length = res.GetResultWithOwnership().GetContentLength();
		int64 cnt = 0;

		UE_LOG(LogTemp, Display, TEXT("there should be %d bytes form s3"), res.GetResultWithOwnership().GetContentLength());
		
		while (cnt < content_length) {
			//Find more efficient way because TArray add is not great but works
			buffer.Add(retrieved_file.get());
			cnt++;
		}

		FFileHelper::SaveArrayToFile(buffer, *destination);
		UE_LOG(LogTemp, Display, TEXT("inside the function"));
		return true;
	}

	return false;
}

bool UAWSS3ClientObject::GetObjectAsync(FString bucket, FString key, FString destination) {
	Aws::S3::Model::GetObjectRequest req;
	req.SetBucket(TCHAR_TO_UTF8(*bucket));
	req.SetKey(TCHAR_TO_UTF8(*key));

	FString uuid = bucket.Append(":").Append(key);

	auto context = Aws::MakeShared<Aws::Client::AsyncCallerContext>("GetObjectAllocationTag");
	context->SetUUID(TCHAR_TO_UTF8(*uuid));
	
	if (!UAWSS3ClientObject::BucketKeyToObject.Contains(uuid)) {
		UAWSS3ClientObject::BucketKeyToObject.Add(uuid, &(this->OnObjectReturned) );
		this->awsS3Client->GetObjectAsync(req, UAWSS3ClientObject::GetObjectAsync_Handler, context);
		return true;
	}

	return false;
}


void UAWSS3ClientObject::ListObjectsAsync_Handler(const Aws::S3::S3Client *client,
	const Aws::S3::Model::ListObjectsRequest &request,
	const Aws::S3::Model::ListObjectsOutcome &outcome,
	const std::shared_ptr<const Aws::Client::AsyncCallerContext>& context) {

	TArray<FString> objects;

	if (outcome.IsSuccess()) {
		Aws::Vector<Aws::S3::Model::Object> list = outcome.GetResult().GetContents();

		//UE_LOG(LogTemp, Display, TEXT("<<<<<<<<<<<<< Objects in %s >>>>>>>>>>>>>>"), *FString(context->GetUUID().c_str()) );


		for (Aws::S3::Model::Object obj : list) {
			FString obj_name = FString(obj.GetKey().c_str());
			objects.Add(obj_name);
			//UE_LOG(LogTemp, Display, TEXT("%s"), *obj_name );
		}

		//UE_LOG(LogTemp, Display, TEXT("<<<<<<<<<<<<<<< Finished >>>>>>>>>>>>>>>>>"), *FString(context->GetUUID().c_str()));
	}
	FString uuid = FString(context->GetUUID().c_str());
	if(UAWSS3ClientObject::BucketToClientObject.Contains(uuid)) {
		FListObjectReturned *delegate1 = *UAWSS3ClientObject::BucketToClientObject.Find(uuid);

		FS3ListObjectsResult result;

		result.bucketname = uuid;
		result.objs = objects;

		delegate1->Broadcast(result);
		UAWSS3ClientObject::BucketToClientObject.Remove(uuid);
		UE_LOG(LogTemp, Display, TEXT("Found the delegate and called broadvcast"));
	}

}

void UAWSS3ClientObject::GetObjectAsync_Handler(const Aws::S3::S3Client *client,
	const Aws::S3::Model::GetObjectRequest &request,
	Aws::S3::Model::GetObjectOutcome outcome,
	const std::shared_ptr<const Aws::Client::AsyncCallerContext>& context) {


	FString uuid = FString(context->GetUUID().c_str());
	FString bucket;
	FString key;

	FString basefile = FString("H:/tmp/").TrimStartAndEnd(); //TODO change this to a const based on platform

	uuid.Split(":", &bucket, &key);

	
	if (outcome.IsSuccess()) {
	
		auto &retrieved_file = outcome.GetResultWithOwnership().GetBody();
		
		
		TArray<uint8> buffer;

		int64 content_length = outcome.GetResultWithOwnership().GetContentLength();
		int64 cnt = 0;

		UE_LOG(LogTemp, Display, TEXT("there should be %d bytes from s3"), content_length);

		while (cnt < content_length) {
			//Find more efficient way because TArray add is not great but works
			buffer.Add(retrieved_file.get());
			cnt++;
		}

		FString file_loc = basefile.Append(key);
		FS3ObjectResult result;

		result.BucketName = bucket;
		result.FileLocation = file_loc;
		result.S3Key = key;

		

		FFileHelper::SaveArrayToFile(buffer, *file_loc );
		UE_LOG(LogTemp, Display, TEXT("Saved file"));

		
		 if (UAWSS3ClientObject::BucketKeyToObject.Contains(uuid)) {
			FObjectReturned *delegate1 = *UAWSS3ClientObject::BucketKeyToObject.Find(uuid);
			delegate1->Broadcast(result);
			UAWSS3ClientObject::BucketKeyToObject.Remove(uuid);
			
		 }
		
	} 

}




