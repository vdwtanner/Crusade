function onEnterTrigger(context, trigger, collider)
	DisableMovementInput(context);
	DisableAbilityInput(context);
	PauseAI(context);
	player = GetPlayerCharacter(context);
	DisplayTextbox(context, "Sign", "Uncle Joe's Lumberjack Outfitter");
	YieldUntilInput(trigger);
	DisplayTextbox(context, "Crusader", "I've heard that this place has really good prices! I wonder how the business is doing...");
	YieldUntilInput(trigger);
	PlaySound(context, "Bellow from Hell.adpcm");
	skelly1 = CreateEnemy(context, 2, 68, 17);
	EmitBlockParticles(context, 68, 17, 0, 5, 47);
	DisplayTextbox(context, "Crusader", "Something tells me they're in a bit of a slump right now.");
	YieldUntilInput(trigger);
	DisplayTextbox(context, "Crusader", "Time to clean the place up!");
	YieldUntilInput(trigger);
	
	CameraFollowPlayer(context);
	EnableMovementInput(context);
	EnableAbilityInput(context);
	ResumeAI(context);
	DeactivateTrigger(trigger);
end


function onExitTrigger(context, trigger, collider)
	
end