// Fill out your copyright notice in the Description page of Project Settings.


#include "SpotifyDevSettings.h"

FName USpotifyDevSettings::GetContainerName() const
{
	return TEXT("Project");
}

FName USpotifyDevSettings::GetCategoryName() const
{
	return NAME_Game;
}

FName USpotifyDevSettings::GetSectionName() const
{
	return TEXT("Spotify");
}

FText USpotifyDevSettings::GetSectionText() const
{
	return NSLOCTEXT("Spotify", "Section", "Spotify");
}

FText USpotifyDevSettings::GetSectionDescription() const
{
	return NSLOCTEXT("Spotify", "Description", "Change your Spotify Game Setup.");
}
