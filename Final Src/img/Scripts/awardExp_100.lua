function onEnterTrigger(context, trigger, collider)
	AwardExp(collider, 100);
	DeactivateTrigger(trigger);
end