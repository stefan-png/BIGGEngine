-- engine uses one single lua_State per script, so global objects shared between scriptComponents.

x = "okay i pull up";    -- shared among all ScriptComponents with name 'test2'

function BIGGEngine.Init()
    BIGGEngine.log("calling BIGGEngine.Init in script '%s'.",BIGGEngine.ScriptName);
end

function BIGGEngine.MouseButton(button, action, mods)
    if action == BIGGEngine.ActionEnum.Press then
        BIGGEngine.log("transform.pos is %s mousepos is %s and windowsize is %s", this.transform.position, BIGGEngine.getMousePosition(), BIGGEngine.getWindowSize());
        BIGGEngine.setClipboardString(BIGGEngine.ScriptName);
    end
    return false
end
