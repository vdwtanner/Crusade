
function onEnterTrigger(context, trigger, collider)
	DisableMovementInput(context);
	DisableAbilityInput(context);
	PauseBGM(context);
	PlaySound(context, "player_hit.adpcm");
	
	MoveTo(GetPlayerCharacter(context), 11, 14, 1.0);
	Yield(trigger, 1.0);
	
	DisplayTextbox(context, "Mysterious Voice", "Let us explore the level! Oh what sights we might see!");
	CameraMoveTo(context, 33, 4, 4.0);
	CameraZoomTo(context, 2, 4.0);
	PlaySound(context, "tut_1.adpcm");
	Yield(trigger, 4.6);
	DisplayTextbox(context, "Mysterious Voice", "Oh look! A ladder. How convenient.");
	sound = PlaySound(context, "tut_2.adpcm");
	YieldUntilInput(trigger);
	StopSound(sound);
	
	CameraMoveTo(context, 39, 20, 1.0);
	Yield(trigger, 1.0);
	DisplayTextbox(context, "Mysterious Voice", "My. What a view. Wait. Whats that over there!");
	sound = PlaySound(context, "tut_3.adpcm");
	YieldUntilInput(trigger);
	StopSound(sound);
	
	CameraMoveTo(context, 71, 16, 1.0);
	CameraZoomTo(context, 1, 1.0);
	Yield(trigger, 1.0);
	DisplayTextbox(context, "Mysterious Voice", "Oh my. This church looks rather run down. Hurry and go spruce it up!");
	sound = PlaySound(context, "tut_4.adpcm");
	YieldUntilInput(trigger);
	StopSound(sound);
	
	x, y = GetPosition(collider);
	CameraMoveTo(context, x + .2, y, 1.0);
	Yield(trigger, 1.0);
	
	
	CameraFollowPlayer(context);
	EnableMovementInput(context);
	EnableAbilityInput(context);
	DeactivateTrigger(trigger);
	
	SwitchBGM(context, "Massive_Coronary.adpcm");
	Yield(trigger, 1.0);
	PlaySound(context, "tut_5.adpcm");
end


function onExitTrigger(context, trigger, collider)
	
end