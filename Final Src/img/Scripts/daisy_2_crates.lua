function onEnterTrigger(context, trigger, collider)
	DisableMovementInput(context);
	DisableAbilityInput(context);
	PauseAI(context);
	player = GetPlayerCharacter(context);
	DisplayTextbox(context, "Crusader", "What's that over there?");
	YieldUntilInput(trigger);
	CameraMoveTo(context, 122, 10, 1.0);
	Yield(trigger, 1.75);
	x, y = GetPosition(player);
	CameraMoveTo(context, x, y, 1.0);
	Yield(trigger, 1.0);
	DisplayTextbox(context, "Crusader", "I wonder what is in those crates...");
	YieldUntilInput(trigger);
	
	CameraFollowPlayer(context);
	EnableMovementInput(context);
	EnableAbilityInput(context);
	ResumeAI(context);
	DeactivateTrigger(trigger);
end


function onExitTrigger(context, trigger, collider)
	
end