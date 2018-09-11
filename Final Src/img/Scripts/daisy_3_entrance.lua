
function onEnterTrigger(context, trigger, collider)
	DisableMovementInput(context);
	DisableAbilityInput(context);
	PauseAI(context);

	player = GetPlayerCharacter(context);

	DisplayTextbox(context, "Crusader", "This seems to be the lair of those red cloaked fiends.");
	YieldUntilInput(trigger);
	DisplayTextbox(context, "Crusader", "Time to eradicate them from the face of the earth.");
	YieldUntilInput(trigger);
	
	EnableMovementInput(context);
	EnableAbilityInput(context);
	ResumeAI(context);
	
	DeactivateTrigger(trigger);
end


function onExitTrigger(context, trigger, collider)
	
end