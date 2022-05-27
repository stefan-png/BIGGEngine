require("BIGGEngine")


function mouseButtonCb(button, action, mods)
    if button == BIGGEngine.MouseButton.Left and action == BIGGEngine.Action.Press then
        print("left mouse button pressed!")
    elseif button == BIGGEngine.MouseButton.Right and action == BIGGEngine.Action.Press then
        print("right mouse button pressed!")
    elseif button == BIGGEngine.MouseButton.Middle and action == BIGGEngine.Action.Press then
        print("middle mouse button pressed!")
    end

    return true;
end
