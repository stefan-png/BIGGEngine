function BIGGEngine.Callbacks.Init()
    print("[lua] calling BIGGEngine.Callbacks.Init!");
    mylib.subscribe(5)

    BIGGEngine.Persistent.x = 5
    BIGGEngine.Persistent.y = -10
end

function BIGGEngine.Callbacks.MouseButton(button, action, mods)
    if button == BIGGEngine.MouseButtonEnum.Left and action == BIGGEngine.ActionEnum.Press then
        print("name is "..BIGGEngine.ScriptName)
    elseif button == BIGGEngine.MouseButtonEnum.Right and action == BIGGEngine.ActionEnum.Press then
        print("right mouse button pressed!")
    elseif button == BIGGEngine.MouseButtonEnum.Middle and action == BIGGEngine.ActionEnum.Press then
        print("middle mouse button pressed!")
    end

    return false;
end