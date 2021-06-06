// Fill out your copyright notice in the Description page of Project Settings.


#include "SpotifyService.h"
#include "Spotify.h"
#include "SHA256.h"
#include "Common/TcpSocketBuilder.h"
#include "GenericPlatform/GenericPlatformHttp.h"
#include "Interfaces/IHttpResponse.h"
#include "Kismet/KismetSystemLibrary.h"

void USpotifyService::BeginAuthorization()
{
	ServerSocket = CreateServerSocket();

	TSharedRef<FInternetAddr> RemoteAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
}

FSocket* USpotifyService::CreateServerSocket()
{
	const FIPv4Endpoint Endpoint(FIPv4Address::InternalLoopback, Port);
	FSocket* NewSock = FTcpSocketBuilder(TEXT("HttpServer"))
		.AsReusable()
		.Listening(8)
		.BoundToEndpoint(Endpoint)
		.WithReceiveBufferSize(2 * 1024 * 1024)
		.Build();
	return NewSock;
}

void USpotifyService::TCPListener()
{
	if(!ServerSocket) return;
	auto Sockets = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
	const TSharedRef<FInternetAddr> RemoteAddress = Sockets->CreateInternetAddr();

	bool bPending;
	ServerSocket->HasPendingConnection(bPending);
	if(bPending)
	{
		//if(!bPending) return;
		if(ConnectionSocket)
		{
			ConnectionSocket->Close();
			Sockets->DestroySocket(ConnectionSocket);
		}

		ConnectionSocket = ServerSocket->Accept(*RemoteAddress, TEXT("ServerConnection"));
		if(ConnectionSocket != nullptr)
		{
			RemoteConnectionEndpoint = FIPv4Endpoint(RemoteAddress);
			bListening = true;
		}
	}
	
}

void USpotifyService::ConnectionListener()
{
	if(!bListening || !ConnectionSocket) return;

	TArray<uint8> ReceivedData;
	uint32 Size;
	
	while(ConnectionSocket->HasPendingData(Size))
	{
		ReceivedData.Init(0, FMath::Min(Size, 65507u));
		int32 ReadData = 0;
		ConnectionSocket->Recv(ReceivedData.GetData(), ReceivedData.Num(), ReadData);
	}

	ReceivedData.Add(0);
	const FString Request = UTF8_TO_TCHAR(ReceivedData.GetData());
	if(!Request.IsEmpty())
		RetrieveAuthKey(Request);

	const FString Response = TEXT("HTTP/1.1 200 OK\r\n\
		Cache-Control: no-cache, private\n\r\
		Server: Unreal-Socket-Server\n\r\n\r\
		<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\r\n<html><head>\r\n<title>Success!</title>\
		\r\n</head><body>\r<h1>Success!</h1>\n<p>You can close this document now!</p></body></html>");

	const TCHAR* ResponseData = *Response;
	const int32 ResponseSize = FCString::Strlen(ResponseData);
	int32 ResponseSent = 0;
	ConnectionSocket->Send((uint8*)TCHAR_TO_UTF8(ResponseData), ResponseSize, ResponseSent);
	ConnectionSocket->Shutdown(ESocketShutdownMode::ReadWrite);
	bListening = false;
}

void USpotifyService::RetrieveAuthKey(FString HttpResponse)
{
	const FRegexPattern AuthCodeRegex(TEXT("/?code=([\\d\\w-_]+)"));
	const FRegexPattern ErrorCodeRegex(TEXT("/?error=([\\d\\w-_]+)"));
	FRegexMatcher AuthMatcher(AuthCodeRegex, HttpResponse);
	FRegexMatcher ErrorMatcher(ErrorCodeRegex, HttpResponse);
	if(AuthMatcher.FindNext())
	{
		AuthKey = AuthMatcher.GetCaptureGroup(1);
		UE_LOG(LogSpotify, Warning, TEXT("%s"), *AuthKey);
		bListening = false;
		ServerSocket->Close();
		RequestRefreshKey();
	}
	if(ErrorMatcher.FindNext())
	{
		UE_LOG(LogSpotify, Error, TEXT("Error Authenticating with Spotify: %s"), *ErrorMatcher.GetCaptureGroup(1));
	}
	
}

void USpotifyService::RequestRefreshKey()
{
	if(!Http) return;

	auto Request = Http->CreateRequest();
	Request->SetURL("https://accounts.spotify.com/api/token");
	Request->SetVerb("POST");
	const FString Body = FString::Printf(TEXT("grant_type=authorization_code&code=%s&redirect_uri=%s:%d&client_id=%s&code_verifier=%s"),
		*AuthKey, *RedirectURL, Port, *ClientKey, *Verify);
	Request->SetContentAsString(Body);
	Request->SetHeader("Content-Type", "application/x-www-form-urlencoded;charset=UTF-8");
	Request->OnProcessRequestComplete().BindUObject(this, &USpotifyService::ReceiveRefreshKey);
	Request->ProcessRequest();
}

void USpotifyService::RequestPlaybackInformation()
{
}

void USpotifyService::RequestPlay()
{
}

void USpotifyService::RequestPause()
{
}

void USpotifyService::ReceiveRefreshKey(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if(!bWasSuccessful) return;

	if( Response->GetResponseCode() >= 200 && Response->GetResponseCode() < 300)
	{
		UE_LOG(LogSpotify, Warning, TEXT("REQ: %s"), *Response->GetContentAsString());
		return;
	}
	UE_LOG(LogSpotify, Error, TEXT("RES: %s"), *Response->GetContentAsString());
}


void USpotifyService::ReceivePlaybackInformation(FHttpRequestPtr Request, FHttpResponsePtr Response,
	bool bWasSuccessful)
{
}

void USpotifyService::ReceivePlay(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
}

void USpotifyService::ReceivePause(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
}

void USpotifyService::OnError(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
}


void USpotifyService::Tick(float DeltaTime)
{
	TCPListener();
	ConnectionListener();
}

bool USpotifyService::ShouldCreateSubsystem(UObject* Outer) const
{
	return Super::ShouldCreateSubsystem(Outer);
}

void USpotifyService::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// Just a bunch of characters.
	const FString RandomChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

	// Create a random string for the Secret-keyless authentication.
	
	for (int i = 0; i < 64; i++)
	{
		Verify += RandomChars.GetCharArray()[FMath::RandRange(0, RandomChars.Len() - 1)];
	}
	// Create and Base64 encode the challenge.
	Challenge = FBase64::Encode(sha256(Verify));
	Challenge = Challenge.Replace(TEXT("+"), TEXT("-"))
		.Replace(TEXT("/"), TEXT("_"))
		.Replace(TEXT(" "), TEXT(""));
	Challenge.RemoveFromEnd("=");
	Http = &FModuleManager::LoadModuleChecked<FHttpModule>("Http").Get();
	UKismetSystemLibrary::LaunchURL(FString::Printf(TEXT("https://accounts.spotify.com/authorize?response_type=code&client_id=%s&redirect_uri=%s:%d&scope=user-modify-playback-state&code_challenge=%s&code_challenge_method=S256"),
		*ClientKey, *RedirectURL, Port, *Challenge));
	BeginAuthorization();
}

void USpotifyService::Deinitialize()
{
	if(ConnectionSocket)
	{
		ConnectionSocket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ConnectionSocket);
	}
	if(ServerSocket)
	{
		ServerSocket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ServerSocket);
	}
	Super::Deinitialize();
}
