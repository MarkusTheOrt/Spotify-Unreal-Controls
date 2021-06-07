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

	// The Spotify Developer App Client ID (You'll Retrieve this from the Spotify Dev Console).
	UPROPERTY(Config, EditDefaultsOnly)
	FString ClientId;

	// The Callback on where to redirect (Needs to be the same as you entered in your spotify app settings.)
	UPROPERTY(Config, EditDefaultsOnly)
	FString Callback = TEXT("http://localhost:3036");

	// The Save game where it saves the Refresh Key.
	UPROPERTY(Config, EditDefaultsOnly)
	FString SaveSlotName = TEXT("SpotifyCredentials");

public:
	
	virtual FName GetContainerName() const override;
	virtual FName GetCategoryName() const override;
	virtual FName GetSectionName() const override;
	virtual FText GetSectionText() const override;
	virtual FText GetSectionDescription() const override;
};
