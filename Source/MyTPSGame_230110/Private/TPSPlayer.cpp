// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSPlayer.h"
#include <GameFramework/SpringArmComponent.h>
#include <Camera/CameraComponent.h>
#include <GameFramework/CharacterMovementComponent.h>
#include "BulletActor.h"
#include "Blueprint/UserWidget.h"
#include <Kismet/GameplayStatics.h>

// Sets default values
ATPSPlayer::ATPSPlayer() //생성자함수
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//sudo code - 의사코드:한글로 순서를 정하고 코드를 짠다.=>이 행동을 알고리즘이라고 한다. 순서를 만들고 하나씩 해결한다.
	//외관을 수동으로 정하고싶다. 외관은 mesh가 관리한다.
	//1.외관에 해당하는 에셋을 읽어오고싶다.
	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Characters/Mannequins/Meshes/SKM_Quinn.SKM_Quinn'"));

	//2.읽어오는것에 성공했다면
	if (tempMesh.Succeeded())
	{
		//3.Mesh에 적용하고 싶다.
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
		//4.Transform을 수정하고 싶다.
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));
	}

	// 스프링암, 카메라 컴포넌트를 생성하고 싶다.
	springArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("springArmComp"));

	cameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("cameraComp"));

	//스프링암을 루트에 붙이고
	springArmComp->SetupAttachment(RootComponent);
	//카메리는 스프링 암에 붙이고 싶다.
	cameraComp->SetupAttachment(springArmComp);

	springArmComp->SetRelativeLocation(FVector(0, 50, 100));
	springArmComp->TargetArmLength = 250;

	//입력값을 회전에 반영하고 싶다.
	bUseControllerRotationYaw = true;
	springArmComp->bUsePawnControlRotation = true;
	cameraComp->bUsePawnControlRotation = true;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	//일반총의 컴포넌트를 만들고 싶다.
	gunMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("gunMeshComp"));
	gunMeshComp->SetupAttachment(GetMesh());


	//일반총의 에셋을 읽어서 컴포넌트에 넣고싶다.
	ConstructorHelpers::FObjectFinder<USkeletalMesh>tempGunMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/FPWeapon/Mesh/SK_FPGun.SK_FPGun'"));

	if (tempGunMesh.Succeeded())
	{
		gunMeshComp->SetSkeletalMesh(tempGunMesh.Object);
		gunMeshComp->SetRelativeLocationAndRotation(FVector(0, 50, 130),FRotator(0, 0, 0));

	}

	sniperMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("sniperMeshComp"));

	sniperMeshComp->SetupAttachment(GetMesh());

	ConstructorHelpers::FObjectFinder<UStaticMesh> tempSniper(TEXT("/Script/Engine.StaticMesh'/Game/SniperGun/sniper1.sniper1'"));

	if (tempSniper.Succeeded())
	{
		sniperMeshComp->SetStaticMesh(tempSniper.Object);
		sniperMeshComp->SetRelativeLocationAndRotation(FVector(0, 60, 140), FRotator());
		sniperMeshComp->SetRelativeScale3D(FVector(0.15f));

	}

}


// Called when the game starts or when spawned
void ATPSPlayer::BeginPlay()
{
	Super::BeginPlay();
	//UI를 생성하고 싶다.
	crosshairUI = CreateWidget(GetWorld(), crosshairFactory);
	sniperUI = CreateWidget(GetWorld(), sniperFactory);
	//CrosshariUI를 화면에 표기하고 싶다.
	crosshairUI->AddToViewport();

	ChooseGun(GRENADE_GUN);
	//1.태어날 때 cui를 보이게 하고싶다.
	//2.스나이퍼건일대 ZoomIn을 하면 cui X, sui o
	//3. 스나이퍼건일대 ZoomOut을 하면 cui o, sui X
	//4. 기본총을 선택하면 cui o, sui x

}

// Called every frame
void ATPSPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//direction 방향으로 이동하고 싶다.
	//P=P0+vt
	//SetActorLocation(GetActorLocation() + direction * walkSpeed * DeltaTime);
	FTransform trans(GetControlRotation());
	FVector resultDirection = trans.TransformVector(direction);
	
	resultDirection.Z = 0;
	resultDirection.Normalize();

	AddMovementInput(resultDirection);
	direction = FVector::ZeroVector;
}

// Called to bind functionality to input
void ATPSPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("Horizontal"), this, &ATPSPlayer::OnAxisHorizontal);

	PlayerInputComponent->BindAxis(TEXT("Vertical"), this, &ATPSPlayer::OnAxisVertical);

	PlayerInputComponent->BindAxis(TEXT("Look Up"), this, &ATPSPlayer::OnAxisLookup);

	PlayerInputComponent->BindAxis(TEXT("Turn Right"), this, &ATPSPlayer::OnAxisTurnRight);

	PlayerInputComponent->BindAction(TEXT("jump"), IE_Pressed, this, &ATPSPlayer::OnActionJump);

	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &ATPSPlayer::OnActionFirepressed);

	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Released, this, &ATPSPlayer::OnActionFireReleased);

	PlayerInputComponent->BindAction(TEXT("GrenadeGun"), IE_Pressed, this, &ATPSPlayer::OnActionGrenade);

	PlayerInputComponent->BindAction(TEXT("SniperGun"), IE_Pressed, this, &ATPSPlayer::OnActionSniper);

	PlayerInputComponent->BindAction(TEXT("Zoom"), IE_Pressed, this, &ATPSPlayer::OnActionZoomIn);

	PlayerInputComponent->BindAction(TEXT("Zoom"), IE_Released, this, &ATPSPlayer::OnActionZoomOut);
}


void ATPSPlayer::OnAxisHorizontal(float value)
{
	direction.Y = value;
}


void ATPSPlayer::OnAxisVertical(float value)
{
	direction.X = value;
}

void ATPSPlayer::OnAxisLookup(float value)
{
	//Pitch
	AddControllerPitchInput(value);
}

void ATPSPlayer::OnAxisTurnRight(float value)
{
	//Yaw
	AddControllerYawInput(value);
}

void ATPSPlayer::OnActionJump()
{
	Jump();
}

void ATPSPlayer::OnActionFirepressed()
{
	//만약 기본총이라면
	if(bChooseGrenadeGun)
	{
		//FTimerHandle 영수증 부터 받아야 나중에 환불할수있는것처럼.
		GetWorldTimerManager().SetTimer(fireTimerHandle, this, &ATPSPlayer::DoFire, fireInterval, true);

		DoFire();
	}



	//그렇지않다면(스나이퍼건)
	else
	{
		FHitResult hitInfo;
		FVector start = cameraComp->GetComponentLocation(); //카메라의 월드좌표
		FVector end = start + cameraComp->GetForwardVector()*100000;
		FCollisionQueryParams params;
		params.AddIgnoredActor(this);

		//UKismetSystemLibrary::BoxTrace 박스로 쏠수도 있다 점은 정확해야하니까
// 		FCollisionObject objParams;
// 		objParams.AddObjectTypesToQuery(ECollisionChannel::)
// 		GetWorld()->LineTraceSingleByObjectType()

		//바라보고싶다.
		bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, start, end, ECollisionChannel::ECC_Visibility, params);
		
		//만약에 부딪힌 것이 있다면
		if (true == bHit)
		{
			//상호작용을 하고싶다.

			//부딪힌 곳에 폭팔이벤트를 표시하고싶다.
			FTransform trans(hitInfo.ImpactPoint);
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletImpactFactory, trans );

			auto hitComp = hitInfo.GetComponent();
			// 부딪힌 물체가 물리작용을 하고있다면
			if (hitComp && hitComp->IsSimulatingPhysics())
			{
			// 힘을 가하고 싶다.
				FVector forceDir = (hitInfo.TraceEnd -hitInfo.TraceStart).GetSafeNormal();
				
				FVector force = forceDir * 10000000 + hitComp->GetMass();
				hitComp->AddForce(force);
			}
		}
	}
}


void ATPSPlayer::OnActionFireReleased()
{
	GetWorldTimerManager().ClearTimer(fireTimerHandle);
}

void ATPSPlayer::DoFire()
{
	//플레이어 1M앞
	FTransform t = gunMeshComp->GetSocketTransform(TEXT("FirePosition"));
	//t.SetRotation(FQuat(GetControlRotation()));

	GetWorld()->SpawnActor<ABulletActor>(bulletFactory, t);
	
}

void ATPSPlayer::ChooseGun(bool bGrendade)
{
	//만약 바꾸기 전이 스나이퍼건이고 바꾸려는 것이 유탄이면
	if (false == bChooseGrenadeGun && true == bGrendade) {
	// FOV를 90, cui 0 sui X
		cameraComp->SetFieldOfView(90);
		crosshairUI->AddToViewport();
		sniperUI->RemoveFromParent();
	}
	bChooseGrenadeGun = bGrendade;
	gunMeshComp->SetVisibility(bGrendade);
	sniperMeshComp->SetVisibility(!bGrendade);
}

void ATPSPlayer::OnActionGrenade()
{
	ChooseGun(true);
}

void ATPSPlayer::OnActionSniper()
{
	ChooseGun(false);
}

void ATPSPlayer::OnActionZoomIn()
{
	//만약 유탄이면 바로 종료
	if (true == bChooseGrenadeGun)
	{
		return;
	}

	//만약 스나이퍼건이라면 = 유탄총이 아니라면
	//확대 FOV 30
	cameraComp->SetFieldOfView(30);
	//crosshair를 안보이게 하고, 확대경을 보이게 하고싶다.
	crosshairUI->RemoveFromParent();
	sniperUI->AddToViewport();
	
}

void ATPSPlayer::OnActionZoomOut()
	{
	//만약 유탄이면 바로 종료
	if (true == bChooseGrenadeGun){
		return;
	}

	//확대 FOV 90
	cameraComp->SetFieldOfView(90);
	//crosshair를 보이게 하고, 확대경을 안보이게 하고싶다.
	crosshairUI->AddToViewport();
	sniperUI->RemoveFromParent();

	
}

