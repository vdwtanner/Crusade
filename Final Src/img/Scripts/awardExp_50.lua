function onEnterTrigger(context, trigger, collider)
	AwardExp(collider, 50);
	DeactivateTrigger(trigger);
end