function BIGGEngine.Callbacks.Init()
    print("[lua] calling BIGGEngine.Callbacks.Init in script "..BIGGEngine.ScriptName);
    mylib.subscribeMouseButton(5)

    BIGGEngine.Persistent.x = 5
    BIGGEngine.Persistent.y = -10
end

function BIGGEngine.Callbacks.MouseButton(button, action, mods)
    if button == BIGGEngine.MouseButtonEnum.Left and action == BIGGEngine.ActionEnum.Press then
        print("name is "..BIGGEngine.ScriptName)
    elseif button == BIGGEngine.MouseButtonEnum.Right and action == BIGGEngine.ActionEnum.Press then
        print("Persistent.x is"..tostring(BIGGEngine.Persistent.x))
    elseif button == BIGGEngine.MouseButtonEnum.Middle and action == BIGGEngine.ActionEnum.Press then
        print("Persistent.y is"..tostring(BIGGEngine.Persistent.y))
    end

    return false;
end