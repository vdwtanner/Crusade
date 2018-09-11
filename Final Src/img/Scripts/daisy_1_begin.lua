
function onEnterTrigger(context, trigger, collider)
	DisableMovementInput(context);
	DisableAbilityInput(context);
	PauseAI(context);
	player = GetPlayerCharacter(context);
	Yield(trigger, 1.0);
	MoveTo(player, 6, 13, 1.5);
	Yield(trigger, 1.5);
	DisplayTextbox(context, "Crusader", "*Yawn*");
	YieldUntilInput(trigger);
	DisplayTextbox(context, "Crusader", "Well, time for another day of guard duty.");
	YieldUntilInput(trigger);
	DisplayTextbox(context, "Crusader", "Better get over to the tower...");
	CameraMoveTo(context, 33, 14, 2.0);
	YieldUntilInput(trigger);
	x, y = GetPosition(player);
	CameraMoveTo(context, x, y, 1.0);
	Yield(trigger, 1.0);
	DisplayTextbox(context, "Crusader", "How do I walk again? Ah right the arrow keys.");
	YieldUntilInput(trigger);
	
	CameraFollowPlayer(context);
	EnableMovementInput(context);
	EnableAbilityInput(context);
	DeactivateTrigger(trigger);
end


function onExitTrigger(context, trigger, collider)
	
end