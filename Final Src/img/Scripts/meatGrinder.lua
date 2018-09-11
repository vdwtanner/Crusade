precondition = "ENEMIES_CONTAINED_DEAD"

function onEnterTrigger(context, trigger, collider)
	DisplayTextbox(context, "Announcer", "Well folks, looks like the crusader has done it! All of the villains are dead!");
	YieldUntilInput(trigger);
	EndLevel(context);
end

function onTriggerExit(context, trigger, collider)

end