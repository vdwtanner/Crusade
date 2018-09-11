
precondition = "ENEMIES_CONTAINED_DEAD"

function onEnterTrigger(context, trigger, collider)
	DisableMovementInput(context);
	DisableAbilityInput(context);
	PauseAI(context);
	
	player = GetPlayerCharacter(context);

	MoveTo(player, 16, 5, 4.0);
	Yield(trigger, 4.0);
	
	DisplayTextbox(context, "Crusader", "By Harventus! What manner of evil is this?");
	YieldUntilInput(trigger);
	DisplayTextbox(context, "Crusader", "Wait. They couldn't have... a portal to the 6 hells themselves?");
	YieldUntilInput(trigger);
	DisplayTextbox(context, "Crusader", "I must find some way to stem the tide of evil coming forth from this portal.");
	YieldUntilInput(trigger);
	
	-- SEBASTIAN SPAWN
	EmitBlockParticles(context, 13, 10, 0, 0, 48);
	sebast = CreateEnemy(context, 6, 12.5, 10);
	
	Yield(trigger, 1.0);
	
	CameraMoveTo(context, 13, 10, .3);
	CameraZoomTo(context, 1.5, .3);
	MoveTo(player, 14, 5, .1);
	DisplayTextbox(context, "Sebastian", "Haha! You're too late meat can!");
	YieldUntilInput(trigger);
	CameraMoveTo(context, 16, 5, .3);
	DisplayTextbox(context, "Crusader", "Sebastian! Where have you taken Daisy!?");
	YieldUntilInput(trigger);
	CameraMoveTo(context, 13, 10, .3);
	DisplayTextbox(context, "Sebastian", "She's already gone fool, delivered on a silver platter to Master Leonard!");
	YieldUntilInput(trigger);
	CameraMoveTo(context, 16, 5, .3);
	DisplayTextbox(context, "Crusader", "Master Leonard, the gluttonous goat devil? Daisy's time is running out. I must make haste.");
	YieldUntilInput(trigger);
	CameraMoveTo(context, 13, 10, .3);
	DisplayTextbox(context, "Sebastian", "Your quest ends here meat can! Die!");
	YieldUntilInput(trigger);
	SwitchBGM(context, "Plans_in_Motion.adpcm");
	x, y = GetPosition(player);
	CameraMoveTo(context, x, y, .3);
	CameraZoomTo(context, 1.0, .3);
	Yield(trigger, 0.3);
	CameraFollowPlayer(context);
	Yield(trigger, 0.5);

	
	DamageTile(context, 200, 12, 9);
	DamageTile(context, 200, 13, 9);
	
	EnableMovementInput(context);
	EnableAbilityInput(context);
	ResumeAI(context);
	
	YieldUntilDead(trigger, sebast);
	Yield(trigger, 0.5);
	EmitBlockParticles(context, 9, 5, 0, 0, 48);
	sebast = CreateEnemy(context, 6, 9, 5);
	EnableNoCollide(sebast);
	
	DisableMovementInput(context);
	DisableAbilityInput(context);
	PauseAI(context);
	
	Yield(trigger, 1.0);
	DisplayTextbox(context, "Sebastian", "Uugh. It seems I've been bested. But no need to worry oh plated one. I've brought a friend for you.");
	YieldUntilInput(trigger);
	Yield(trigger, .5);
	
	sx, sy = GetPosition(sebast);
	EmitBlockParticles(context, sx, sy, 0, 0, 48);
	DestroyEnemy(sebast);
	
	Yield(trigger, .5);
	
	-- OGRE SPAWN
	EmitBlockParticles(context, 3, 4, 0, 1.0, 48);
	Yield(trigger, .1);
	EmitBlockParticles(context, 5, 6, 0, 1.0, 48);
	Yield(trigger, .1);
	EmitBlockParticles(context, 1, 8, 0, 1.0, 48);
	Yield(trigger, .1);
	EmitBlockParticles(context, 4, 4, 0, 1.0, 48);
	Yield(trigger, .1);
	EmitBlockParticles(context, 4, 8, 0, 1.0, 48);
	Yield(trigger, .1);
	EmitBlockParticles(context, 5, 6, 0, 1.0, 48);
	Yield(trigger, .1);
	
	CameraMoveTo(context, 3, 6, 1.0);
	DisplayTextbox(context, "Crusader", "Something is coming through the portal!");
	YieldUntilInput(trigger);
	
	EmitBlockParticles(context, 2, 7, 0, 1.0, 48);
	EmitBlockParticles(context, 3, 7, 0, 1.0, 48);
	EmitBlockParticles(context, 4, 7, 0, 1.0, 48);
	
	EmitBlockParticles(context, 2, 6, 0, 1.0, 48);
	EmitBlockParticles(context, 3, 6, 0, 1.0, 48);
	EmitBlockParticles(context, 4, 6, 0, 1.0, 48);
	
	EmitBlockParticles(context, 2, 5, 0, 1.0, 48);
	EmitBlockParticles(context, 3, 5, 0, 1.0, 48);
	EmitBlockParticles(context, 4, 5, 0, 1.0, 48);
	
	Yield(trigger, .1);
	ogre = CreateEnemy(context, 4, 2.5, 5.1);
	PlaySound(context, "Ogre_Angry_1.adpcm");
	
	Yield(trigger, .4);
	DamageTile(context, 200, 5, 7);
	DamageTile(context, 200, 5, 6);
	DamageTile(context, 200, 5, 5);
	
	EnableMovementInput(context);
	EnableAbilityInput(context);
	ResumeAI(context);
	
	YieldUntilDead(trigger, ogre);

	Yield(trigger, 1.0);
	
	PauseAI(context);
	DisplayTextbox(context, "Crusader", "*huff*");
	YieldUntilInput(trigger);
	DisplayTextbox(context, "Crusader", "What a monster. I haven't fought like that in a while.");
	YieldUntilInput(trigger);
	DisplayTextbox(context, "Crusader", "Regardless, Daisy still needs my help. It is time to venture into the pit.");
	CameraMoveTo(context, 3, 6, 5.0);
	CameraZoomTo(context, 1.6, 5.0);
	Yield(trigger, 7.0);
	EndLevel(context);
end


function onExitTrigger(context, trigger, collider)
	
end