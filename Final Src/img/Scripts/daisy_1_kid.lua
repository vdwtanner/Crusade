
function onEnterTrigger(context, trigger, collider)
	DisableMovementInput(context);
	DisableAbilityInput(context);

	player = GetPlayerCharacter(context);
	
	sheep = CreateEnemy(context, 1, 67, 15);
	EmitBlockParticles(context, 67, 15, 0, 5, 47);
	--create shepherd at 73, 15
	shepherd = CreateEnemy(context, 5, 73, 15);

	
	MoveTo(player, 65, 15, 1.0);
	Yield(trigger, 1.0);
	
	CameraMoveTo(context, 72, 15, 1.0);
	CameraZoomTo(context, 2, 1.0);
	-- move shepherd to 72, 15
	MoveTo(shepherd, 72, 15, 1.0);
	Yield(trigger, 1.0);
	
	DisplayTextbox(context, "Crusader", "Boy! Get out of here! I'll handle these beasts!");
	YieldUntilInput(trigger);
	PlaySound(context, "kid_golly.adpcm");
	DisplayTextbox(context, "Boy", "Thanks Mister! I'm outta here!");
	YieldUntilInput(trigger);
	
	MoveTo(shepherd, 53, 15, 3.5);
	EnableNoCollide(shepherd);
	ResumeAI(context);
	
	x, y = GetPosition(player);
	Yield(trigger, 2.0);

	
	EnableMovementInput(context);
	EnableAbilityInput(context);
	DeactivateTrigger(trigger);
	
	Teleport(shepherd, 51, 14);
	DestroyEnemy(shepherd);
	--MoveTo(shepherd, 52, 14, .5);
	CameraMoveTo(context, x, y, 1.0);
	CameraZoomTo(context, 1, 1.0);
	Yield(trigger, 2.0);
	CameraFollowPlayer(context);
end


function onExitTrigger(context, trigger, collider)
	
end