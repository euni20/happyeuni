// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TPSPlayer.generated.h"

#define GRENADE_GUN true
#define SNIPER_GUN false

class USpringArmComponent; // 전방선언

UCLASS()
class MYTPSGAME_230110_API ATPSPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATPSPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere)
	class USpringArmComponent* springArmComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UCameraComponent* cameraComp;

	void OnAxisHorizontal(float value);
	void OnAxisVertical(float value);
	void OnAxisLookup(float value);
	void OnAxisTurnRight(float value);
	void OnActionJump();
	void OnActionFirepressed();
	void OnActionFireReleased();

	void DoFire();

	//총알공장을 만들자
	//마우스 왼쪽버튼을 누르면 총알공장에서 총알을 만들어서 총구위치에 배치하고싶다.

	UPROPERTY(EditAnywhere)
	TSubclassOf<class ABulletActor> bulletFactory;

	FVector direction;
	float walkSpeed = 600;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class USkeletalMeshComponent* gunMeshComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UStaticMeshComponent* sniperMeshComp;

	UPROPERTY(EditAnywhere)
	float fireInterval = 0.5f;

	FTimerHandle fireTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bChooseGrenadeGun;
	//void ChooseGun(bool bGrendade);
	void ChooseGun(bool bGrenade);

	//위젯공장에서 위젯을 생성하고 싶다. Crosshair, Sniper
	//공장르 먼저만들자
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> crosshairFactory;

	UPROPERTY(EditAnywhere)
		TSubclassOf<class UUserWidget> sniperFactory;

	UPROPERTY() //이거 꼭넣어줘야함 
	class UUserWidget* crosshairUI;

	UPROPERTY()
	class UUserWidget* sniperUI;

	//1번 키와 2번키에대한 (총교체)입력처리를 하고싶다.
	void OnActionGrenade();
	void OnActionSniper();

	//Zoom을 구현하고싶다. In/Out
	void OnActionZoomIn(); //확대 FOV 30
	void OnActionZoomOut();// 축소 FOV 90

	//폭팔공장
	UPROPERTY(EditAnyWhere)
	class UParticleSystem* bulletImpactFactory;

	
};


/*int plus(int* a, int* b)
{
	*a = 200;
	return *a + *b;

}


void main()
{
	int aa = 10;
	int bb = 20;
	Plus(&aa, &bb);
	//Call by Pointer

}
*/