// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HttpModule.h"
#include "Interfaces/IPv4/IPv4Endpoint.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SpotifyService.generated.h"

UENUM()
enum class ESpotifyURLType : uint8
{
	AuthCode,
	AccessKey,
	Playback,
	PlayResume,
	Pause,
	Next,
	Previous,
	Seek,
	Repeat,
	Volume,
	Shuffle
};

static TMap<ESpotifyURLType, FString> SpotifyUrls = {
	{ ESpotifyURLType::AuthCode,	TEXT("https://accounts.spotify.com/authorize?response_type=code&client_id=%s&redirect_uri=%s:%d&scope=%s&state=%s&code_challenge_method=S256") },
	{ ESpotifyURLType::AccessKey,		TEXT("https://accounts.spotify.com/api/token") },
	{ ESpotifyURLType::Playback ,	TEXT("https://api.spotify.com/v1/me/player") },
	{ ESpotifyURLType::PlayResume,	TEXT("https://api.spotify.com/v1/me/player/play") },
	{ ESpotifyURLType::Pause,		TEXT("https://api.spotify.com/v1/me/player/pause") },
	{ ESpotifyURLType::Next,		TEXT("https://api.spotify.com/v1/me/player/next") },
	{ ESpotifyURLType::Previous,	TEXT("https://api.spotify.com/v1/me/player/previous") },
	{ ESpotifyURLType::Seek,		TEXT("https://api.spotify.com/v1/me/player/seek") },
	{ ESpotifyURLType::Repeat,		TEXT("https://api.spotify.com/v1/me/player/repeat") },
	{ ESpotifyURLType::Volume,		TEXT("https://api.spotify.com/v1/me/player/volume") },
	{ ESpotifyURLType::Shuffle,	TEXT("https://api.spotify.com/v1/me/player/shuffle") }
};

/**
 * This Class handles the Spotify API
 * It has the same lifetime as a Game Instance (meaning it will persist between worlds)
 */
UCLASS()
class SPOTIFY_API USpotifyService : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

protected:

	// Your applications public key.
	UPROPERTY()
	FString ClientKey = "43bf2bdff42d4bc8877fdd9d2ff77717";

	// Where a user should be redirected to after approving.
	UPROPERTY()
	FString RedirectURL = "http://localhost";

	// The Port (443 for https, 80 for http) - default 3036 for no particular reason.
	UPROPERTY()
	uint16 Port = 3036;

	// The Retrieved Auth key.
	UPROPERTY()
	FString AuthKey;

	// Access Key (the Key used for executing API calls).
	UPROPERTY()
	FString AccessKey;

	// This key wont run out, it is used to refresh the Access key once it runs out.
	UPROPERTY()
	FString RefreshKey;

	// For the Proof-Key-Challenge-Exchange (PKCE).
	UPROPERTY()
	FString Verify;

	// The Auto-Generated Challenge for PKCE.
	UPROPERTY()
	FString Challenge;

	FHttpModule* Http;

	// This Socket listens for incoming connection on localhost:Port.
	FSocket* ServerSocket;

	// This Socket exchanges Data.
	FSocket* ConnectionSocket;

	// @TODO: Remove.
	FIPv4Endpoint RemoteConnectionEndpoint;

	// Whether or not we should listen on a pending connection.
	bool bListening;
	
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

#pragma endregion

#pragma region API Requests
	/////////////////////////////////////////
	// API Requests
	
	// Requests a new Refresh Key.
	void RequestRefreshKey();
	
	// Requests Information about Playback. (Title, Artist, Duration, Progression, Volume etc...)
	void RequestPlaybackInformation();

	// Request the player to start or resume playback.
	void RequestPlay();

	// Request the player to pause playback.
	void RequestPause();
	
	/////////////////////////////////////////
	// API Responses

	// Received the Refresh Key.
	void ReceiveRefreshKey(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	// When Playback info is received.
	void ReceivePlaybackInformation(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	// Whether the playback started or not.
	void ReceivePlay(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	// Whether the playback is paused or not.
	void ReceivePause(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

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
