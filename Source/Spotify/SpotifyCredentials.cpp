// Fill out your copyright notice in the Description page of Project Settings.


#include "SpotifyCredentials.h"

void USpotifyCredentials::SetValues(FString InVerify, FString InChallenge, FString InRefreshKey)
{
	Verify = InVerify;
	Challenge = InChallenge;
	RefreshKey = InRefreshKey;
}
