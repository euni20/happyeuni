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

	//�浹ü, �ܰ�, �̵�������Ʈ
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

	//�̷��� �ϴ� ����� �ְ� SetLifeSpan(2); �Լ��� �ϴ¹���� �ִ�.
	//�̰� ������Ͽ����� �Լ��� ��������.
	//FTimerHandle dieTimerHandle;
	//GetWorld()->GetTimerManager().SetTimer(dieTimerHandle, this, &ABulletActor::OnDie, 2);
	//handle �����Ѵٴ� �� ��Ʈ���Ҷ��� �ڵ��̶�´ܾ ���� ��.
	//Lambda�Լ�
	// Ư�̻��� ĸ�ĸ� �Ѵ�.
	//auto myPlus = [ĸó]�Լ��ǸŰ�����->����
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

