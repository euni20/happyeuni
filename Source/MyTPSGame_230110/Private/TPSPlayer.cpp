// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSPlayer.h"
#include <GameFramework/SpringArmComponent.h>
#include <Camera/CameraComponent.h>
#include <GameFramework/CharacterMovementComponent.h>
#include "BulletActor.h"
#include "Blueprint/UserWidget.h"
#include <Kismet/GameplayStatics.h>

// Sets default values
ATPSPlayer::ATPSPlayer() //�������Լ�
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//sudo code - �ǻ��ڵ�:�ѱ۷� ������ ���ϰ� �ڵ带 §��.=>�� �ൿ�� �˰����̶�� �Ѵ�. ������ ����� �ϳ��� �ذ��Ѵ�.
	//�ܰ��� �������� ���ϰ�ʹ�. �ܰ��� mesh�� �����Ѵ�.
	//1.�ܰ��� �ش��ϴ� ������ �о����ʹ�.
	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Characters/Mannequins/Meshes/SKM_Quinn.SKM_Quinn'"));

	//2.�о���°Ϳ� �����ߴٸ�
	if (tempMesh.Succeeded())
	{
		//3.Mesh�� �����ϰ� �ʹ�.
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
		//4.Transform�� �����ϰ� �ʹ�.
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));
	}

	// ��������, ī�޶� ������Ʈ�� �����ϰ� �ʹ�.
	springArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("springArmComp"));

	cameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("cameraComp"));

	//���������� ��Ʈ�� ���̰�
	springArmComp->SetupAttachment(RootComponent);
	//ī�޸��� ������ �Ͽ� ���̰� �ʹ�.
	cameraComp->SetupAttachment(springArmComp);

	springArmComp->SetRelativeLocation(FVector(0, 50, 100));
	springArmComp->TargetArmLength = 250;

	//�Է°��� ȸ���� �ݿ��ϰ� �ʹ�.
	bUseControllerRotationYaw = true;
	springArmComp->bUsePawnControlRotation = true;
	cameraComp->bUsePawnControlRotation = true;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	//�Ϲ����� ������Ʈ�� ����� �ʹ�.
	gunMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("gunMeshComp"));
	gunMeshComp->SetupAttachment(GetMesh());


	//�Ϲ����� ������ �о ������Ʈ�� �ְ�ʹ�.
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
	//UI�� �����ϰ� �ʹ�.
	crosshairUI = CreateWidget(GetWorld(), crosshairFactory);
	sniperUI = CreateWidget(GetWorld(), sniperFactory);
	//CrosshariUI�� ȭ�鿡 ǥ���ϰ� �ʹ�.
	crosshairUI->AddToViewport();

	ChooseGun(GRENADE_GUN);
	//1.�¾ �� cui�� ���̰� �ϰ�ʹ�.
	//2.�������۰��ϴ� ZoomIn�� �ϸ� cui X, sui o
	//3. �������۰��ϴ� ZoomOut�� �ϸ� cui o, sui X
	//4. �⺻���� �����ϸ� cui o, sui x

}

// Called every frame
void ATPSPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//direction �������� �̵��ϰ� �ʹ�.
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
	//���� �⺻���̶��
	if(bChooseGrenadeGun)
	{
		//FTimerHandle ������ ���� �޾ƾ� ���߿� ȯ���Ҽ��ִ°�ó��.
		GetWorldTimerManager().SetTimer(fireTimerHandle, this, &ATPSPlayer::DoFire, fireInterval, true);

		DoFire();
	}



	//�׷����ʴٸ�(�������۰�)
	else
	{
		FHitResult hitInfo;
		FVector start = cameraComp->GetComponentLocation(); //ī�޶��� ������ǥ
		FVector end = start + cameraComp->GetForwardVector()*100000;
		FCollisionQueryParams params;
		params.AddIgnoredActor(this);

		//UKismetSystemLibrary::BoxTrace �ڽ��� ����� �ִ� ���� ��Ȯ�ؾ��ϴϱ�
// 		FCollisionObject objParams;
// 		objParams.AddObjectTypesToQuery(ECollisionChannel::)
// 		GetWorld()->LineTraceSingleByObjectType()

		//�ٶ󺸰�ʹ�.
		bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, start, end, ECollisionChannel::ECC_Visibility, params);
		
		//���࿡ �ε��� ���� �ִٸ�
		if (true == bHit)
		{
			//��ȣ�ۿ��� �ϰ�ʹ�.

			//�ε��� ���� �����̺�Ʈ�� ǥ���ϰ�ʹ�.
			FTransform trans(hitInfo.ImpactPoint);
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletImpactFactory, trans );

			auto hitComp = hitInfo.GetComponent();
			// �ε��� ��ü�� �����ۿ��� �ϰ��ִٸ�
			if (hitComp && hitComp->IsSimulatingPhysics())
			{
			// ���� ���ϰ� �ʹ�.
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
	//�÷��̾� 1M��
	FTransform t = gunMeshComp->GetSocketTransform(TEXT("FirePosition"));
	//t.SetRotation(FQuat(GetControlRotation()));

	GetWorld()->SpawnActor<ABulletActor>(bulletFactory, t);
	
}

void ATPSPlayer::ChooseGun(bool bGrendade)
{
	//���� �ٲٱ� ���� �������۰��̰� �ٲٷ��� ���� ��ź�̸�
	if (false == bChooseGrenadeGun && true == bGrendade) {
	// FOV�� 90, cui 0 sui X
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
	//���� ��ź�̸� �ٷ� ����
	if (true == bChooseGrenadeGun)
	{
		return;
	}

	//���� �������۰��̶�� = ��ź���� �ƴ϶��
	//Ȯ�� FOV 30
	cameraComp->SetFieldOfView(30);
	//crosshair�� �Ⱥ��̰� �ϰ�, Ȯ����� ���̰� �ϰ�ʹ�.
	crosshairUI->RemoveFromParent();
	sniperUI->AddToViewport();
	
}

void ATPSPlayer::OnActionZoomOut()
	{
	//���� ��ź�̸� �ٷ� ����
	if (true == bChooseGrenadeGun){
		return;
	}

	//Ȯ�� FOV 90
	cameraComp->SetFieldOfView(90);
	//crosshair�� ���̰� �ϰ�, Ȯ����� �Ⱥ��̰� �ϰ�ʹ�.
	crosshairUI->AddToViewport();
	sniperUI->RemoveFromParent();

	
}

