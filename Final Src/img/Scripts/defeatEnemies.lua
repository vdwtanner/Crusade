
precondition = "ENEMIES_CONTAINED_DEAD"

function onEnterTrigger(context, trigger, collider)
	DisableMovementInput(context);
	DisableAbilityInput(context);
	
	DisplayTextbox(context, "Mysterious Voice", "Congrats, you've killed all the things!");	
	PlaySound(context, "player_hit.adpcm");
	YieldUntilInput(trigger);
	
	EnableMovementInput(context);
	EnableAbilityInput(context);
	
end


function onExitTrigger(context, trigger, collider)
	
end