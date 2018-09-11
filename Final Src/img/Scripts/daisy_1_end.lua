
precondition = "ENEMIES_CONTAINED_DEAD"

function onEnterTrigger(context, trigger, collider)
	DisableMovementInput(context);
	DisableAbilityInput(context);

	player = GetPlayerCharacter(context);

	
	MoveTo(player, 65, 15, 5.0);
	Yield(trigger, 5.0);
	
	CameraMoveTo(context, 72, 15, 1.0);
	CameraZoomTo(context, 2, 1.0);
	
	DisplayTextbox(context, "Crusader", "That seems to be the last of those corrupted sheep.");
	YieldUntilInput(trigger);
	DisplayTextbox(context, "??", "A young voice, you believe belonging to the boy from earlier, calls to you from afar.");
	YieldUntilInput(trigger);
	PlaySound(context, "kid_took_daisy.adpcm");
	DisplayTextbox(context, "Boy", "Mister! They... they took daisy!");
	YieldUntilInput(trigger);
	DisplayTextbox(context, "Boy", "The men in red took her Mister!");
	YieldUntilInput(trigger);
	DisplayTextbox(context, "Crusader", "Men in red? Does he mean... the red cloaks? What are they doing here?");
	YieldUntilInput(trigger);
	DisplayTextbox(context, "Crusader", "No matter, it seems another group of hertics have brought the wrong kind of attention to themselves.");
	YieldUntilInput(trigger);
	DisplayTextbox(context, "Crusader", "Time to do what I do best...");
	YieldUntilInput(trigger);
	DisplayTextbox(context, "Crusader", "Crusade!");
	
	AwardExp(player, 150);
	x, y = GetPosition(player);
	CameraZoomTo(context, .75, 5.0);
	Yield(trigger, 5.0);

	EndLevel(context);
end


function onExitTrigger(context, trigger, collider)
	
end