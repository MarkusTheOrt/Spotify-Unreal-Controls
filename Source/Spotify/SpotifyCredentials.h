// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SpotifyCredentials.generated.h"

/**
 * 
 */
UCLASS()
class SPOTIFY_API USpotifyCredentials : public USaveGame
{
	GENERATED_BODY()
public:
	UPROPERTY(SaveGame)
	FString Verify;

	UPROPERTY(SaveGame)
	FString Challenge;

	UPROPERTY(SaveGame)
	FString RefreshKey;

	void SetValues(FString InVerify, FString InChallenge, FString InRefreshKey);
	
};
