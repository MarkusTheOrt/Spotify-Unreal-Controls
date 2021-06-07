// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "SpotifyDevSettings.generated.h"

/**
 * 
 */
UCLASS(Config=Game)
class SPOTIFY_API USpotifyDevSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:

	// The Spotify Developer App Client ID
	UPROPERTY(Config, EditDefaultsOnly)
	FString ClientId;

	UPROPERTY(Config, EditDefaultsOnly)
	FString Callback = TEXT("http://localhost:3036");

	UPROPERTY(Config, EditDefaultsOnly)
	FString SaveSlotName = TEXT("SpotifyCredentials");

public:
	
	virtual FName GetContainerName() const override;
	virtual FName GetCategoryName() const override;
	virtual FName GetSectionName() const override;
	virtual FText GetSectionText() const override;
	virtual FText GetSectionDescription() const override;
};
