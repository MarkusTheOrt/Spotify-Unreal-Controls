// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h" 
#include "HttpModule.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SpotifyService.generated.h"

// Params: Song Name, Artists, Album Name, Volume, Progress, Duration, isPlaying
DECLARE_DYNAMIC_MULTICAST_DELEGATE_SevenParams(FOnReceivePlaybackDataDelegate, FString, SongName, const TArray<FString>&,
	Artists, FString, AlbumName, int, Volume, int, Progress, int, Duration, bool, isPlaying);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlaybackAdvancedDelegate, int, Duration, int, Progress);

/**
 * This Class handles the Spotify API
 * It has the same lifetime as a Game Instance (meaning it will persist between worlds)
 */
UCLASS()
class SPOTIFY_API USpotifyService : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

protected:
	
	void SaveToSlot();

	bool LoadCredentials();

	UPROPERTY(Transient)
	FString SaveSlotName;
	
	// Your applications public key.
	UPROPERTY(Transient)
	FString ClientKey;

	// Where a user should be redirected to after approving.
	UPROPERTY(Transient)
	FString RedirectURL;

	// The Retrieved Auth key.
	UPROPERTY(Transient)
	FString AuthKey;

	// Access Key (the Key used for executing API calls).
	UPROPERTY(Transient)
	FString AccessKey;

	// This key wont run out, it is used to refresh the Access key once it runs out.
	UPROPERTY(Transient)
	FString RefreshKey;

	// For the Proof-Key-Challenge-Exchange (PKCE).
	UPROPERTY(Transient)
	FString Verify;

	// The Auto-Generated Challenge for PKCE.
	UPROPERTY(Transient)
	FString Challenge;

	UPROPERTY(Transient)
	FDateTime AccessKeyExpiration;
	
	FHttpModule* Http;

	// This Socket listens for incoming connection on localhost:Port.
	FSocket* ServerSocket;

	// This Socket exchanges Data.
	FSocket* ConnectionSocket;

	// Whether or not we should listen on a pending connection.
	bool bListening;

	FTimerHandle AccessKeyExpireTimerHandle;

	FTimerHandle PlaybackInfoTimerHandle;

	// Only call big update whenever song ID changes.
	UPROPERTY(Transient)
	FString SongId;

public:

	UPROPERTY(BlueprintAssignable)
	FOnReceivePlaybackDataDelegate OnReceivePlaybackDataDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnPlaybackAdvancedDelegate OnPlaybackAdvancedDelegate;
	
protected:

#pragma region Authentication
	
	// Start Auth Procedure.
	UFUNCTION()
	void BeginAuthorization();

	// Creates the Server Socket.
	FSocket* CreateServerSocket();

	// Listening for incoming connections.
	void TCPListener();

	// Exchange Data between connections.
	void ConnectionListener();

	// Filter Auth key from HTTP Request.
	void RetrieveAuthKey(FString HttpResponse);

	void RefreshAccessKey();

#pragma endregion

#pragma region API Requests
	/////////////////////////////////////////
	// API Requests
	
	// Requests a new Refresh Key.
	void RequestRefreshKey();
	
	// Requests Information about Playback. (Title, Artist, Duration, Progression, Volume etc...)
	void RequestPlaybackInformation();

	// Request the player to start or resume playback.
	UFUNCTION(BlueprintCallable)
	void RequestPlay();

	void PlaybackRequest(const FString& Url, const FString& Verb);

	void PlaybackRequest(const FString& Url, const FString& Verb, const FString& Body);

	// Request the player to pause playback.
	UFUNCTION(BlueprintCallable)
	void RequestPause();

	UFUNCTION(BlueprintCallable)
	void RequestNext();

	UFUNCTION(BlueprintCallable)
	void RequestPrev();

	UFUNCTION(BlueprintCallable)
	void Seek(int TimeInSeconds);

	UFUNCTION(BlueprintCallable)
	void SetVolume(float Val);
	
	/////////////////////////////////////////
	// API Responses

	// Received the Refresh Key.
	void ReceiveRefreshKey(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	// When Playback info is received.
	void ReceivePlaybackInformation(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	// Whether the playback started or not.
	void ReceivePlay(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	// Handle common error messages.
	void OnError(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

#pragma endregion
	
public:

	// FTickableGameObject Interface
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override
	{
		return GetStatID();
	}
	// End of FTickableGameObject Interface

	// UGameInstanceSubsystem Interface
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// End of UGameInstanceSubsystem Interface
	
};
