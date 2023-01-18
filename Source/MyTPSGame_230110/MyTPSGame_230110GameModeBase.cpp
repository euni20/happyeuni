// Copyright Epic Games, Inc. All Rights Reserved.


#include "MyTPSGame_230110GameModeBase.h"
#include "MyTPSGame_230110.h"

AMyTPSGame_230110GameModeBase::AMyTPSGame_230110GameModeBase()
{
	//로그를 출력 Hello World
	//UE_LOG(LogTemp, Warning, TEXT("Hello World"));
	//UE_LOG(LogTemp, Warning, TEXT("%s"), *CALLINFO);

	PRINT_LOG(TEXT("%s %s"), TEXT("Hello"), TEXT("World"));
}
