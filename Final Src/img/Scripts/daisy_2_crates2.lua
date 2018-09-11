function onEnterTrigger(context, trigger, collider)
	DisableMovementInput(context);
	DisableAbilityInput(context);
	PauseAI(context);
	player = GetPlayerCharacter(context);
	DisplayTextbox(context, "Crusader", "It looks like they stole these health potions from the Lumberjack Outfitter.");
	YieldUntilInput(trigger);
	DisplayTextbox(context, "Crusader", "I'll leave most of these for their rightful owner, but I think this one is going to be on the house.");
	YieldUntilInput(trigger);
	Heal(collider, 30);
	CameraFollowPlayer(context);
	EnableMovementInput(context);
	EnableAbilityInput(context);
	ResumeAI(context);
	DeactivateTrigger(trigger);
end


function onExitTrigger(context, trigger, collider)
	
end