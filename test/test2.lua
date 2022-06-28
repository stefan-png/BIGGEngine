-- engine uses one single lua_State per script, so global objects shared between scriptComponents.

x = "okay i pull up";    -- shared among all ScriptComponents with name 'test2'

function BIGGEngine.Init()
    BIGGEngine.log("calling BIGGEngine.Init in script '%s'.",BIGGEngine.ScriptName);
end

function BIGGEngine.MouseButton(button, action, mods)
    if action == BIGGEngine.ActionEnum.Press then
        if type(x) ~= "number" then
          x = 0.9
        end
        x = x + 1.1
        BIGGEngine.log("mouse button %d pressed in script '%s'. x = %.1f", button, BIGGEngine.ScriptName, x);
    end
    return false
end
