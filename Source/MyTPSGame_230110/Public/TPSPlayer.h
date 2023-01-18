// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TPSPlayer.generated.h"

#define GRENADE_GUN true
#define SNIPER_GUN false

class USpringArmComponent; // ���漱��

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

	//�Ѿ˰����� ������
	//���콺 ���ʹ�ư�� ������ �Ѿ˰��忡�� �Ѿ��� ���� �ѱ���ġ�� ��ġ�ϰ�ʹ�.

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

	//�������忡�� ������ �����ϰ� �ʹ�. Crosshair, Sniper
	//���帣 ����������
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> crosshairFactory;

	UPROPERTY(EditAnywhere)
		TSubclassOf<class UUserWidget> sniperFactory;

	UPROPERTY() //�̰� ���־������ 
	class UUserWidget* crosshairUI;

	UPROPERTY()
	class UUserWidget* sniperUI;

	//1�� Ű�� 2��Ű������ (�ѱ�ü)�Է�ó���� �ϰ�ʹ�.
	void OnActionGrenade();
	void OnActionSniper();

	//Zoom�� �����ϰ�ʹ�. In/Out
	void OnActionZoomIn(); //Ȯ�� FOV 30
	void OnActionZoomOut();// ��� FOV 90

	//���Ȱ���
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