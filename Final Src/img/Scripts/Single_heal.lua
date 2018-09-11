function onEnterTrigger(context, trigger, collider)
	Heal(collider, 25);
	DeactivateTrigger(trigger);
end