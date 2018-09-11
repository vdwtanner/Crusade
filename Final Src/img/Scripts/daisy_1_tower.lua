
function onEnterTrigger(context, trigger, collider)
	DisableMovementInput(context);
	DisableAbilityInput(context);
	player = GetPlayerCharacter(context);
	CameraMoveTo(context, 36, 14, 1.0);
	Yield(trigger, 1.25);
	CameraMoveTo(context, 36, 19, 1.0);
	Yield(trigger, 1.5);
	x, y = GetPosition(player);
	CameraMoveTo(context, x, y, 1.0);
	Yield(trigger, 1.0);
	
	DisplayTextbox(context, "Crusader", "Hmm. A ladder. Quite a dangerous adversary.");
	YieldUntilInput(trigger);
	DisplayTextbox(context, "Crusader", "I believe I can climb these with the up and down arrows.");
	YieldUntilInput(trigger);
	
	CameraFollowPlayer(context);
	EnableMovementInput(context);
	EnableAbilityInput(context);
	DeactivateTrigger(trigger);
end


function onExitTrigger(context, trigger, collider)
	
end