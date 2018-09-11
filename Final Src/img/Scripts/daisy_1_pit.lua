
function onEnterTrigger(context, trigger, collider)
	DisableMovementInput(context);
	DisableAbilityInput(context);

	player = GetPlayerCharacter(context);
	MoveTo(player, 52, 14, .5);
	Yield(trigger, .5);
	
	CameraMoveTo(context, 56, 10, 1.0);
	Yield(trigger, 1.0);
	
	DisplayTextbox(context, "Crusader", "This pit goes for as far as the eye can see...");
	YieldUntilInput(trigger);
	DisplayTextbox(context, "Crusader", "I could probably get out if I fell in, but it might hurt a bit.");
	YieldUntilInput(trigger);
	DisplayTextbox(context, "Crusader", "I should continue. That child needs my help!");
	YieldUntilInput(trigger);
	
	x, y = GetPosition(player);
	CameraMoveTo(context, x, y, .5);
	Yield(trigger, .5);
	
	CameraFollowPlayer(context);
	EnableMovementInput(context);
	EnableAbilityInput(context);
	DeactivateTrigger(trigger);
end


function onExitTrigger(context, trigger, collider)
	
end