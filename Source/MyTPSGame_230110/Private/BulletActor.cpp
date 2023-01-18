// Fill out your copyright notice in the Description page of Project Settings.


#include "BulletActor.h"
#include <Components/SphereComponent.h>
#include <GameFramework/ProjectileMovementComponent.h>
#include "../MyTPSGame_230110.h"

// Sets default values
ABulletActor::ABulletActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//충돌체, 외관, 이동컴포넌트
	sphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("sphereComp"));
	SetRootComponent(sphereComp);
	sphereComp->SetSphereRadius(13);
	sphereComp->SetCollisionProfileName(TEXT("BlockAll"));

	meshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("meshComp"));
	meshComp->SetupAttachment(RootComponent);
	meshComp->SetRelativeScale3D(FVector(0.25f));
	meshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	movementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("movementComp"));

	movementComp->SetUpdatedComponent(sphereComp);
	movementComp->InitialSpeed = 5000;
	movementComp->MaxSpeed = 5000;
	movementComp->bShouldBounce = true;
	movementComp->Bounciness = 0.5f;

}

// Called when the game starts or when spawned
void ABulletActor::BeginPlay()
{
	Super::BeginPlay();

	//이렇게 하는 방법이 있고 SetLifeSpan(2); 함수로 하는방법이 있다.
	//이건 헤더파일에가서 함수를 만들어야함.
	//FTimerHandle dieTimerHandle;
	//GetWorld()->GetTimerManager().SetTimer(dieTimerHandle, this, &ABulletActor::OnDie, 2);
	//handle 제어한다는 뜻 컨트롤할때는 핸들이라는단어를 많이 씀.
	//Lambda함수
	// 특이사항 캡쳐를 한다.
	//auto myPlus = [캡처]함수의매개변수->구현
	//int number = 1; auto myprint = [number]()->void{int b =number;}
	// 	auto myPlus = [this](int a, int b)->int {return a + b; };
	// 	PRINT_LOG(TEXT("%d"), myPlus(10, 20));


	FTimerHandle dieTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(dieTimerHandle, FTimerDelegate::CreateLambda(
		[this]()->void{
			this->Destroy(); 
		}
	), 2, false);
}

// Called every frame
void ABulletActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABulletActor::OnDie()
{
	Destroy();
}

