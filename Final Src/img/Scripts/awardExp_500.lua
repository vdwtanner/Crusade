function onEnterTrigger(context, trigger, collider)
	AwardExp(collider, 500);
	DeactivateTrigger(trigger);
end