#include "AWSS3ClientObject.h"
#include "AWSS3PrivatePCH.h"

TMap<FString, FObjectReturned*> UAWSS3ClientObject::UUIDToObject = TMap<FString, FObjectReturned*>();

UAWSS3ClientObject* UAWSS3ClientObject::CreateS3ClientObject(const FCredentials credentials, const FClientConfiguration clientConfiguration) {
    UAWSS3ClientObject* S3Client = NewObject<UAWSS3ClientObject>();

    S3Client->awsS3Client = new Aws::S3::S3Client(AWSSerializer::Credentials(credentials), AWSSerializer::ClientConfiguration(clientConfiguration));

    return S3Client;
}

bool UAWSS3ClientObject::GetObject(FString bucket, FString key, FString destination) {
	Aws::S3::Model::GetObjectRequest req;
	//Fstring->AwsString AWSSerializer::FStoAS
	req.SetBucket(AWSSerializer::FStoAS(*bucket));
	req.SetKey(AWSSerializer::FStoAS(*key));

	auto res = this->awsS3Client->GetObject(req);
	if (res.IsSuccess()) {
		auto& retrieved_file = res.GetResultWithOwnership().GetBody();

		TArray<uint8> buffer;

		int64 content_length = res.GetResultWithOwnership().GetContentLength();
		int64 cnt = 0;

		UE_LOG(LogTemp, Display, TEXT("there should be %d bytes from s3"), res.GetResultWithOwnership().GetContentLength());

		while (cnt < content_length) {
			//Find more efficient way because TArray add is not dynamically allocated but works
			buffer.Add(retrieved_file.get());
			cnt++;
		}

		FFileHelper::SaveArrayToFile(buffer, *destination);
		return true;
	}
	return false;
}


FString UAWSS3ClientObject::GetObjectAsync(FString bucket, FString key, FString destination) {
	Aws::S3::Model::GetObjectRequest req;
	req.SetBucket(AWSSerializer::FStoAS(*bucket));
	req.SetKey(AWSSerializer::FStoAS(*key));

	FString uuid = bucket.Append(":").Append(key).Append(":").Append(destination);

	auto context = Aws::MakeShared<Aws::Client::AsyncCallerContext>("GetObjectAllocationTag");
	context->SetUUID(AWSSerializer::FStoAS(*uuid));
	if (!UAWSS3ClientObject::UUIDToObject.Contains(uuid)) {
		//if there is already a key in the map then we are waiting on the results of this call alreayd so dont make it twice
		UAWSS3ClientObject::UUIDToObject.Add(uuid, &(this->OnObjectReturned));
		this->awsS3Client->GetObjectAsync(req, UAWSS3ClientObject::GetObjectAsync_Handler, context);
	}
	return uuid;
}


void UAWSS3ClientObject::GetObjectAsync_Handler(const Aws::S3::S3Client* client,
	const Aws::S3::Model::GetObjectRequest& request,
	Aws::S3::Model::GetObjectOutcome outcome,
	const std::shared_ptr<const Aws::Client::AsyncCallerContext>& context) {


	FString uuid = FString(context->GetUUID().c_str());
	FString bucket;
	FString key;
	FString destination; //TODO change this to a const based on platform
	FString tmp;

	uuid.Split(":", &bucket, &tmp);
	tmp.Split(":", &key, &destination);

	FS3ObjectResult result;

	result.Bucket = bucket;
	result.FileLocation = destination;
	result.Key = key;
	result.wasSuccessful = false; //default to false but will be set to true in if statement below

	if (outcome.IsSuccess()) {

		auto& retrieved_file = outcome.GetResultWithOwnership().GetBody();


		TArray<uint8> buffer;

		int64 content_length = outcome.GetResultWithOwnership().GetContentLength();
		int64 cnt = 0;

		UE_LOG(LogTemp, Display, TEXT("there should be %d bytes from s3"), content_length);

		while (cnt < content_length) {
			//Find more efficient way because TArray add is not great but works
			buffer.Add(retrieved_file.get());
			cnt++;
		}

		FFileHelper::SaveArrayToFile(buffer, *destination);
		UE_LOG(LogTemp, Display, TEXT("Saved file at %s"), *destination);

		
		result.wasSuccessful = true;
	}

	if (UAWSS3ClientObject::UUIDToObject.Contains(uuid)) {
		FObjectReturned* delegate1 = *UAWSS3ClientObject::UUIDToObject.Find(uuid);
		delegate1->Broadcast(result);
		UAWSS3ClientObject::UUIDToObject.Remove(uuid);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Could not fine the uuid: %s in the map to delegates"), *uuid);
	}
}

