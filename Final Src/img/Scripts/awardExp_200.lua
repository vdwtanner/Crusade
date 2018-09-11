function onEnterTrigger(context, trigger, collider)
	AwardExp(collider, 200);
	DeactivateTrigger(trigger);
end