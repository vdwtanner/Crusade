
function onEnterTrigger(context, trigger, collider)
	ApplyImpulse(collider, 0, 100);
end


function onExitTrigger(context, trigger, collider)
	ApplyImpulse(collider, 0, 100);
end