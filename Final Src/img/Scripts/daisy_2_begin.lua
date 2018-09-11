function onEnterTrigger(context, trigger, collider)
	PauseBGM(context);
	DisableMovementInput(context);
	DisableAbilityInput(context);
	
	player = GetPlayerCharacter(context);
	sebastian = CreateEnemy(context, 6, 24, 7);
	MoveTo(sebastian, 23, 7, 1);
	
	Yield(trigger, 1.0);
	PauseAI(context);
	MoveTo(player, 15, 10, 2);
	Yield(trigger, 1.5);
	CameraMoveTo(context, 23, 10, 1.0);
	DisplayTextbox(context, "Crusader", "There's the Red Cloak! I shall smite him in the name of Harventus!");
	YieldUntilInput(trigger);
	DisplayTextbox(context, "Red Cloak", "Give up now, Meat Can! You'll never catch me, for I am the all powerful Sebastian the Red!");
	YieldUntilInput(trigger);
	PlaySound(context, "Crusader_laugh.adpcm");
	DisplayTextbox(context, "Crusader", "Ha! With Harventus all things are possible! Say your prayers heathen!");
	YieldUntilInput(trigger);
	DisplayTextbox(context, "Sebastian the Red", "I don't have time to play with you today, Meat Can.");
	YieldUntilInput(trigger);
	DisplayTextbox(context, "Sebastian the Red", "RISE my minions, RISE!");
	YieldUntilInput(trigger);
	PlaySound(context, "Bellow from Hell.adpcm");
	skelly1 = CreateEnemy(context, 2, 22, 7);
	EmitBlockParticles(context, 22, 7, 0, 5, 47);
	skelly2 = CreateEnemy(context, 2, 21, 7);
	EmitBlockParticles(context, 21, 7, 0, 5, 47);
	DisplayTextbox(context, "Sebastian the Red", "Have a magical day, Meat Can! Hahahahaha!!");
	ResumeAI(context);
	MoveTo(sebastian, 44, 7, 2.5);
	YieldUntilInput(trigger);
	x, y = GetPosition(player);
	CameraMoveTo(context, x, y, 1.0);
	Yield(trigger, 1.0);
	DisplayTextbox(context, "Crusader", "En guard!");
	DestroyEnemy(sebastian);
	YieldUntilInput(trigger);
	
	CameraFollowPlayer(context);
	EnableMovementInput(context);
	EnableAbilityInput(context);
	DeactivateTrigger(trigger);
	SwitchBGM(context, "Massive_Coronary.adpcm");
end


function onExitTrigger(context, trigger, collider)
	
end