
function onEnterTrigger(context, trigger, collider)
	EnablePlayerFlight(context);
end


function onExitTrigger(context, trigger, collider)
	DisablePlayerFlight(context);
end