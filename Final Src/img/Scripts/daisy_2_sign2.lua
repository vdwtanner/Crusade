function onEnterTrigger(context, trigger, collider)
	DisableMovementInput(context);
	DisableAbilityInput(context);
	PauseAI(context);
	player = GetPlayerCharacter(context);
	DisplayTextbox(context, "Sign", "You are now entering Red Cloak territory. NO MEAT CANS ALLOWED!!");
	YieldUntilInput(trigger);
	DisplayTextbox(context, "Crusader", "...well that's not very nice of them! I guess I'll just have to teach them some manners while I'm here.");
	YieldUntilInput(trigger);
	DisplayTextbox(context, "Crusader", "I'll just take a swig of this health potion I found in Uncle Joe's Lumberjack Outfitter and then I'll be off!");
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