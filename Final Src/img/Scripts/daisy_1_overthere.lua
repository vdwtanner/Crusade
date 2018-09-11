
function onEnterTrigger(context, trigger, collider)
	DisableMovementInput(context);
	DisableAbilityInput(context);
	player = GetPlayerCharacter(context);
	
	PlaySound(context, "kid_help_1.adpcm");
	DisplayTextbox(context, "Crusader", "What's that noise?");
	YieldUntilInput(trigger);
	PlaySound(context, "kid_help_2.adpcm");
	DisplayTextbox(context, "Crusader", "Sounds like someone is in trouble...");
	YieldUntilInput(trigger);
	
	sheep = CreateEnemy(context, 1, 66, 15);
	-- create shepherd at 68, 15
	shepherd = CreateEnemy(context, 5, 68, 15);
	
	CameraMoveTo(context, 62, 14, 1.5);
	MoveTo(sheep, 65, 15, 1.5);
	Yield(trigger, 1.5);
	
	PlaySound(context, "kid_help_3.adpcm");
	DisplayTextbox(context, "Shepherd Boy", "Ah! Help!");
	YieldUntilInput(trigger);
	
	x, y = GetPosition(player);
	CameraMoveTo(context, x, y, 1.5);
	Yield(trigger, 1.5);
	
	DisplayTextbox(context, "Crusader", "By the light of Harventus, what manner of foul demon is that?!");
	YieldUntilInput(trigger);
	DisplayTextbox(context, "Crusader", "Regardless, that poor child is in danger! I must save him.");
	YieldUntilInput(trigger);
	
	--remove shepherd
	DestroyEnemy(sheep);
	DestroyEnemy(shepherd);
	CameraFollowPlayer(context);
	EnableMovementInput(context);
	EnableAbilityInput(context);
	DeactivateTrigger(trigger);
end


function onExitTrigger(context, trigger, collider)
	
end