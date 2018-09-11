
function onEnterTrigger(context, trigger, collider)
	DisableMovementInput(context);
	DisableAbilityInput(context);
	
	DisplayTextbox(context, "Crusader", "Hmm.");	
	sound = PlaySound(context, "tut_end_1.adpcm");
	YieldUntilInput(trigger);
	StopSound(sound);
	
	CameraMoveTo(context, 60, 20, 1.0);
	Yield(trigger, 1.5);
	
	CameraMoveTo(context, 60, 12, 1.0);
	Yield(trigger, 1.5);
	
	x, y = GetPosition(collider);
	CameraMoveTo(context, x + .2, y, 1.0);
	Yield(trigger, 1.0);
	
	DisplayTextbox(context, "Crusader", "There haven't been worshippers here for quite some time.");
	sound = PlaySound(context, "tut_end_2.adpcm");
	YieldUntilInput(trigger);
	StopSound(sound);
	DisplayTextbox(context, "Crusader", "No point in cleaning for a service that will never come.");
	sound = PlaySound(context, "tut_end_3.adpcm");
	YieldUntilInput(trigger);
	StopSound(sound);
	DisplayTextbox(context, "Crusader", "There should be another church over this hill. I should continue on.");
	sound = PlaySound(context, "tut_end_4.adpcm");
	YieldUntilInput(trigger);
	StopSound(sound);
	
	x, y = GetPosition(collider);
	CameraMoveTo(context, x + .2, 31, 5.0);
	Yield(trigger, 5.0);
	
	EnableMovementInput(context);
	EnableMovementInput(context);
	EndLevel(context);
end


function onExitTrigger(context, trigger, collider)
	
end