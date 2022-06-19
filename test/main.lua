function BIGGEngine.Init()
    BIGGEngine.log("[lua] calling BIGGEngine.Init in script '%s'.",BIGGEngine.ScriptName);

    BIGGEngine.x = 5
    BIGGEngine.y = -10
end

function BIGGEngine.MouseButton(button, action, mods)
    if action == BIGGEngine.ActionEnum.Press then
        BIGGEngine.x = BIGGEngine.x + 1
        BIGGEngine.log("mouse button %d pressed in script '%s'. x = %d", button, BIGGEngine.ScriptName, BIGGEngine.x);
    end
    return false
end